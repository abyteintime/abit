#include "yarnbox/bytecode/dumper.hpp"

#include "fmt/format.h"

#include "abit/string.hpp"
#include "abit/templates.hpp"

#include "abit/ue/UObject.hpp"
#include "abit/ue/UObject/fmt.hpp"
#include "yarnbox/bytecode/encoding.hpp"

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

static bool
PrimitiveHasDisplay(primitive::Type prim)
{
	using namespace yarn::primitive;
	return !(prim == PEmpty || prim == PDebugInfo || prim == PSentinel);
}

static void
DumpPrimRec(
	const BytecodeTree& tree,
	BytecodeTree::Span span,
	Primitive prim,
	uint64_t value,
	size_t level,
	std::string& outString
)
{
	using namespace yarn::primitive;

	auto out = std::back_inserter(outString);
	if (PrimitiveHasDisplay(prim.type) && prim.type != PInsn) {
		AddIndent(level, outString);
		fmt::format_to(out, "{:03x}..{:03x} ", span.start, span.end);
	}

	switch (prim.type) {
		case PUnsupported:
			outString += "<unsupported>\n";
			break;

		case PEmpty:
		case PDebugInfo:
		case PSentinel:
			// Those exist in the node's data but there's no reason for us to dump them.
			break;

		case PU8:
		case PU16:
		case PU32:
		case PU64: {
			bool isBytecodeBogus = tree.IsBytecodeBogusAt(span.start);
			switch (static_cast<IntKind>(prim.arg)) {
				case KUnsigned:
					fmt::format_to(out, "i {}\n", value);
					break;

				case KSigned:
					switch (prim.type) {
						case PU8:
							fmt::format_to(out, "i(s8) {}\n", static_cast<int8_t>(value));
							break;
						case PU16:
							fmt::format_to(out, "i(s16) {}\n", static_cast<int16_t>(value));
							break;
						case PU32:
							fmt::format_to(out, "i(s32) {}\n", static_cast<int32_t>(value));
							break;
						default:
							// Everything else works like PS64.
							fmt::format_to(out, "i(s64) {}\n", static_cast<int64_t>(value));
							break;
					}
					break;

				case KFloat:
					fmt::format_to(out, "i(f32) {}\n", abit::BitCast<float>(value));
					break;

				case KPointer:
					fmt::format_to(out, "i(ptr) {}\n", reinterpret_cast<void*>(value));
					break;

				case KObject: {
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
					if (value == 0) {
						outString += "i(obj) None\n";
					} else if (!isBytecodeBogus && !isPointerBogus) {
						const UObject* object = reinterpret_cast<UObject*>(value);
						fmt::format_to(out, "i(obj) {:ip}\n", UObjectFmt{ object });
					} else {
						const void* ptr = reinterpret_cast<void*>(value);
						fmt::format_to(out, "i(obj) {:16} (bogus pointer)\n", ptr);
					}
					break;
				}

				case KOffsetAbs:
					fmt::format_to(out, "i(oabs) {:#x}\n", static_cast<uint16_t>(value));
					break;
				case KOffsetRel:
					fmt::format_to(out, "i(orel) {:#x}\n", static_cast<int16_t>(value));
					break;

				case KName: {
					FName name = abit::BitCast<FName>(value);
					if (!isBytecodeBogus && name.IsValid()) {
						FString nameFString = name.ToString();
						fmt::format_to(out, "i(name) '{}'\n", nameFString.ToString());
					} else {
						fmt::format_to(
							out, "i(name) <invalid {}_{}>\n", name.nameIndex, name.instanceIndex
						);
					}
				} break;
			}
			break;
		}

		case PAnsiString:
			fmt::format_to(out, "s \"{}\"\n", tree.strings[value]);
			break;
		case PWideString:
			fmt::format_to(out, "s L\"{}\"\n", abit::Narrow(tree.wideStrings[value]));
			break;

		case PInsn:
		case PPrimitiveCast:
			DumpNodeRec(tree, value, level, outString);
			break;

		case PInsns: {
			uint64_t count = tree.Data(value, 0);

			fmt::format_to(out, "@+ ({}) [\n", count);
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
		"{:03x}..{:03x} @ {} ({})",
		node.span.start,
		node.span.end,
		OpcodeToString(node.opcode),
		fmt::underlying(node.opcode)
	);
	if (tree.IsBytecodeBogusAt(node.span.start)) {
		outString += " (bogus)";
	}
	outString += '\n';

	const Rule& rule = encoding.Rule(node.opcode);
	for (size_t i = 0; i < rule.primsCount; ++i) {
		Primitive prim = rule.prims[i];
		uint64_t value = tree.Data(node.data, i);
		BytecodeTree::Span ip = tree.DataSpan(node.data, i);
		DumpPrimRec(tree, ip, prim, value, level + 1, outString);
	}
}

void
yarn::DumpNode(const BytecodeTree& tree, BytecodeTree::NodeIndex node, std::string& outString)
{
	DumpNodeRec(tree, node, 0, outString);
}
