#include "yarnbox/config/opcode.hpp"

#include "fmt/format.h"

#include "abit/error.hpp"

#include "yarnbox/bytecode/opcode.hpp"

#include "yarnbox/patch.hpp"

using namespace yarn;

namespace yarn {

struct LuaAllOccurrences
{};

struct LuaOpcodeWrapper
{
	Opcode opcode;
};

struct LuaOpcodeQueryWrapper
{
	Opcode opcode;
	std::vector<int32_t> indices;
	bool allOccurrences = false;
};

}

template<Injection::OpcodeQuery::Pick Pick>
static Injection::OpcodeQuery
MakeOpcodeQueryFromWrapper(LuaOpcodeQueryWrapper& wrapper)
{
	Injection::OpcodeQuery query;
	query.opcode = wrapper.opcode;
	if (wrapper.allOccurrences) {
		query.which = Injection::OpcodeQuery::AllOccurrences{};
	} else {
		query.which = wrapper.indices;
	}
	query.pick = Pick;
	return query;
}

void
config::LoadOpcodeIntoLua(sol::table& lib)
{
	auto opcodeWrapperType = lib.new_usertype<LuaOpcodeWrapper>("OpcodeWrapper");
	auto opcodeQueryWrapperType = lib.new_usertype<LuaOpcodeQueryWrapper>("OpcodeQueryWrapper");

	opcodeWrapperType[sol::meta_function::call] = [](LuaOpcodeWrapper& opcodeWrapper,
													 sol::object arg) {
		if (arg.get_type() == sol::type::table) {
			return LuaOpcodeQueryWrapper{ opcodeWrapper.opcode, arg.as<std::vector<int32_t>>() };
		} else if (arg.is<LuaAllOccurrences>()) {
			return LuaOpcodeQueryWrapper{ opcodeWrapper.opcode, {}, true };
		}
		throw abit::Error{ fmt::format(
			"an opcode query must be of form Opcode.Type{1, 2, 3} or Opcode.Type(AllOccurrences)"
		) };
	};

	opcodeQueryWrapperType["Start"]
		= sol::readonly_property(&MakeOpcodeQueryFromWrapper<Injection::OpcodeQuery::Pick::Start>);
	opcodeQueryWrapperType["Span"]
		= sol::readonly_property(&MakeOpcodeQueryFromWrapper<Injection::OpcodeQuery::Pick::Span>);
	opcodeQueryWrapperType["End"]
		= sol::readonly_property(&MakeOpcodeQueryFromWrapper<Injection::OpcodeQuery::Pick::End>);

	sol::table opcode;

#define YARN__EXPAND_OPCODE_INTO_WRAPPER_INSTANCE(Name, Index) \
	opcode[#Name] = LuaOpcodeWrapper{ Opcode::Name };

	// TODO: Maybe this is a bit excessive. I think these opcode wrappers could be made on demand,
	//  but we're missing an OpcodeFromString function. Or maybe this could all be implemented in
	//  Lua and be faster, and I'm just stubborn.
	YARN_X_OPCODES(YARN__EXPAND_OPCODE_INTO_WRAPPER_INSTANCE);

#undef YARN__EXPAND_OPCODE_INTO_WRAPPER_INSTANCE

	lib["AllOccurrences"] = LuaAllOccurrences{};
	lib["Opcode"] = std::move(opcode);
}
