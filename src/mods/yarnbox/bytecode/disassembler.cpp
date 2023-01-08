#include "yarnbox/bytecode/disassembler.hpp"

#include <algorithm>
#include <optional>

#include "abit/loader/logging.hpp"

using namespace yarn;

using NodeIndex = BytecodeTree::NodeIndex;
using DataIndex = BytecodeTree::DataIndex;
using Node = BytecodeTree::Node;

Disassembler::Disassembler(const uint8_t* bytecode, size_t length, BytecodeTree& outTree)
	: bytecode(bytecode)
	, length(length)
	, outTree(&outTree)
{
}

void
Disassembler::EnableStatCollection(Stats& outStats)
{
	this->outStats = &outStats;
}

std::optional<uint64_t>
Disassembler::InterpretPrimitive(Opcode contextOpcode, size_t contextIp, Primitive prim)
{
	using namespace yarn::primitive;

	switch (prim.type) {
		case PUnsupported:
			spdlog::error(
				"InterpretPrimitive called on unsupported opcode {}",
				static_cast<uint32_t>(contextOpcode)
			);
			return std::nullopt;
		case PEmpty:
			return std::nullopt;

		case PU8:
			return static_cast<uint64_t>(NextU8());
		case PU16:
			return static_cast<uint64_t>(NextU16());
		case PU32:
			return static_cast<uint64_t>(NextU32());
		case PU64:
			return static_cast<uint64_t>(NextU64());

		case PInsn:
			return static_cast<uint64_t>(Disassemble());

		case PDebugInfo:
			OptionalDebugInfo();
			return std::nullopt;

		case PAnsiString: {
			size_t start = ip;
			while (CurrentByte() != '\x00') {
				Advance();
			}
			Advance();
			size_t end = ip;
			const char* data = reinterpret_cast<const char*>(&bytecode[start]);
			std::string ansiString{ std::string_view(data, end - start) };
			return outTree->AppendString(std::move(ansiString));
		}

		case PSentinel:
			NextU8();
			return std::nullopt;

		case PInsns: {
			std::vector<uint64_t> insns;
			while (CurrentByte() != prim.arg && !AtEnd()) {
				insns.push_back(static_cast<uint64_t>(Disassemble()));
			}
			Advance(); // skip the sentinel byte
			return outTree->AppendDataFromVector(insns);
		}
	}
}

NodeIndex
Disassembler::Disassemble()
{
	size_t ipAtStart = ip;
	Opcode opcode = NextOpcode();
	Node node = Node{ ipAtStart, opcode };

	if (opcode == Opcode::OutOfBounds) {
		spdlog::warn("Disassembling opcode resulted in an out-of-bounds read");
		outTree->SetFirstErrorIfNull(ipAtStart);
		if (outStats != nullptr) {
			outStats->outOfBoundsReads += 1;
		}
		return outTree->AppendNode(node);
	}

	const Rule& rule = encoding.Rule(opcode);

	if (rule.IsUnsupported()) {
		spdlog::warn(
			"Disassembling opcode '{}' ({} at IP={:04x}) is not supported. Remaining bytecode will "
			"be skipped. This function will not be patchable",
			OpcodeToString(opcode),
			static_cast<uint32_t>(opcode),
			ipAtStart
		);
		// NOTE: Check IsBytecodeBogusAt before calling SetFirstErrorIfNull.
		// This way we can detect whether this opcode was disassembled as part of a larger chain
		// of failure, or whether this is the initiator of the chain.
		if (outStats != nullptr && !outTree->IsBytecodeBogusAt(ip)) {
			outStats->occurrencesOfUnknownOpcodes[static_cast<size_t>(opcode)] += 1;
		}
		outTree->SetFirstErrorIfNull(ipAtStart);
		return outTree->AppendNode(node);
	}

	if (rule.useData) {
		DataIndex data = outTree->AppendData(rule.dataCount);
		size_t indexInData = 0;
		for (size_t i = 0; i < rule.dataCount; ++i) {
			if (auto result = InterpretPrimitive(opcode, ipAtStart, rule.data[i])) {
				outTree->Data(data, indexInData) = *result;
				indexInData += 1;
			}
		}
		return outTree->AppendNode(node.WithU64(data));
	} else {
		auto result = InterpretPrimitive(opcode, ipAtStart, rule.base);
		return outTree->AppendNode(node.WithU64(result.value_or(0)));
	}
}

Opcode
Disassembler::NextOpcode()
{
	switch (CurrentLowOpcode()) {
		case Opcode::HighNative0:
		case Opcode::HighNative1:
		case Opcode::HighNative2:
		case Opcode::HighNative3:
		case Opcode::HighNative4:
		case Opcode::HighNative5:
		case Opcode::HighNative6:
		case Opcode::HighNative7:
		case Opcode::HighNative8:
		case Opcode::HighNative9:
		case Opcode::HighNative10:
		case Opcode::HighNative11:
		case Opcode::HighNative12:
		case Opcode::HighNative13:
		case Opcode::HighNative14:
		case Opcode::HighNative15: {
			uint32_t level =
				static_cast<uint32_t>(CurrentByte() - static_cast<uint8_t>(Opcode::HighNative0));
			Advance();
			uint8_t offset = CurrentByte();
			Advance();

			return static_cast<Opcode>(level * 256 + offset);
		}

		default: {
			Opcode opcode = CurrentLowOpcode();
			Advance();

			return opcode;
		}
	}
}

Disassembler::Stats::Summary
Disassembler::Stats::ComputeSummary()
{
	Summary summary;

	for (size_t i = 0; i < opcodeCount; ++i) {
		if (occurrencesOfUnknownOpcodes[i] > 0) {
			summary.occurrencesOfUnknownOpcodes.push_back(
				std::make_pair(static_cast<Opcode>(i), occurrencesOfUnknownOpcodes[i])
			);
		}
		summary.totalOccurrencesOfUnknownOpcodes += occurrencesOfUnknownOpcodes[i];
	}
	std::sort(
		summary.occurrencesOfUnknownOpcodes.begin(),
		summary.occurrencesOfUnknownOpcodes.end(),
		[](const std::pair<Opcode, uint32_t>& a, const std::pair<Opcode, uint32_t>& b) {
			return a.second > b.second;
		}
	);

	return summary;
}
