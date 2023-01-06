#include "yarnbox/bytecode/dumper.hpp"

#include "fmt/format.h"

#include "abit/string.hpp"

#include "yarnbox/bytecode/encoding.hpp"
#include "yarnbox/ue/UObject.hpp"
#include "yarnbox/ue/UObject/fmt.hpp"

using namespace yarn;
using namespace ue;

static void
AddIndent(size_t level, std::string& outString)
{
	for (size_t i = 0; i < level; ++i) {
		outString += "  ";
	}
}

static void
DumpNodeRec(
	const BytecodeTree& tree,
	BytecodeTree::NodeIndex nodeIndex,
	size_t level,
	std::string& outString
);

static void
DumpPrimRec(
	const BytecodeTree& tree,
	size_t ip,
	Primitive prim,
	uint64_t value,
	size_t level,
	std::string& outString
)
{
	using namespace yarn::primitive;

	switch (prim.type) {
		case PUnsupported:
			AddIndent(level, outString);
			outString += "<unsupported>\n";
			break;

		case PEmpty:
		case PDebugInfo:
		case PSentinel:
			break;

		case PU8:
		case PU16:
		case PU32:
		case PU64:
			AddIndent(level, outString);
			switch (static_cast<IntKind>(prim.arg)) {
				case KDefault:
					fmt::format_to(std::back_inserter(outString), "i {}\n", value);
					break;
				case KPointer:
					fmt::format_to(
						std::back_inserter(outString), "i(ptr) {}\n", reinterpret_cast<void*>(value)
					);
					break;
				case KObject: {
					bool isBytecodeBogus = tree.IsBytecodeBogusAt(ip);
					// This is a giant hack which exploits the fact that valid pointers on x86
					// usually start with the bits 00007F due to how the x86 virtual address space
					// is laid out:
					//
					// https://en.wikipedia.org/wiki/X86-64#Virtual_address_space_details
					// > Many operating systems (including, but not limited to, the Windows NT
					// > family) take the higher-addressed half of the address space (named kernel
					// > space) for themselves and leave the lower-addressed half (user space) for
					// > application code, user mode stacks, heaps, and other data regions.
					//
					// This makes it possible for us to identify when a pointer is disassembled
					// correctly most of the times. I haven't had this crash on me yet, so I
					// suppose it is an okay way of checking for pointer validity, even if
					// a bit hacky. I mean, the Boehm GC has to be able to find pointers somehow
					// too, right?
					bool isPointerBogus = (0xFFFFFF00'00000000 & value) != 0x00007F00'00000000;
					if (!isBytecodeBogus && !isPointerBogus) {
						const UObject* object = reinterpret_cast<UObject*>(value);
						fmt::format_to(
							std::back_inserter(outString), "i(obj) {:ip}\n", UObjectFmt{ object }
						);
					} else {
						const void* ptr = reinterpret_cast<void*>(value);
						fmt::format_to(
							std::back_inserter(outString), "i(obj) {:16} (bogus pointer)\n", ptr
						);
					}
					break;
				}
				case KOffset:
					fmt::format_to(
						std::back_inserter(outString),
						"i(offset) {:#x}\n",
						static_cast<int16_t>(value)
					);
					break;
			}
			break;

		case PAnsiString:
			AddIndent(level, outString);
			fmt::format_to(std::back_inserter(outString), "s \"{}\"\n", tree.strings[value]);
			break;

		case PInsn:
			DumpNodeRec(tree, value, level, outString);
			break;

		case PInsns: {
			uint64_t count = tree.Data(value, 0);

			AddIndent(level, outString);
			fmt::format_to(std::back_inserter(outString), "@+ ({}) [\n", count);
			for (size_t i = 0; i < count; ++i) {
				BytecodeTree::NodeIndex insn = tree.Data(value, 1 + i);
				DumpNodeRec(tree, insn, level + 1, outString);
			}

			AddIndent(level, outString);
			outString += "]\n";
			break;
		}
	}
}

static void
DumpNodeRec(
	const BytecodeTree& tree,
	BytecodeTree::NodeIndex nodeIndex,
	size_t level,
	std::string& outString
)
{
	AddIndent(level, outString);

	const BytecodeTree::Node& node = tree.nodes[nodeIndex];
	fmt::format_to(
		std::back_inserter(outString),
		"@ {:04x} | {} ({})",
		node.ip,
		OpcodeToString(node.opcode),
		fmt::underlying(node.opcode)
	);
	if (tree.IsBytecodeBogusAt(node.ip)) {
		outString += " (bogus)";
	}
	outString += '\n';

	const Rule& rule = encoding.Rule(node.opcode);
	if (!rule.useData) {
		uint64_t value = node.data.u64;
		DumpPrimRec(tree, node.ip, rule.base, value, level + 1, outString);
	} else {
		size_t indexInData = 0;
		for (size_t datum = 0; datum < rule.dataCount; ++datum) {
			Primitive prim = rule.data[datum];
			uint64_t value = tree.Data(node.data.u64, indexInData);
			DumpPrimRec(tree, node.ip, prim, value, level + 1, outString);
			indexInData += primitive::HasDataInBytecodeTree(prim.type);
		}
	}
}

void
yarn::DumpNode(const BytecodeTree& tree, BytecodeTree::NodeIndex node, std::string& outString)
{
	DumpNodeRec(tree, node, 0, outString);
}
