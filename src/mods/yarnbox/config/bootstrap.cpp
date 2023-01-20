#include "config.hpp"

namespace yarn::config {
const char* bootstrapLua = R"lua(

-- Yarnbox bootstrap file
-- Implements the high-level configuration DSL.

local Yarnbox = _yarnbox
_yarnbox = nil

-- Enums

Opcode = Yarnbox.Opcode

-- Patch declarations

Yarnbox.outPatches = {}

local function MakeSingularInjection(data)
	local i = Yarnbox.Injection.new()
	i.into = data.into
	i.select = data.select
	i.generate = data.generate
	return i
end

function injections(comment)
	return function (data)
		for i = 1, #data do
			data[i] = MakeSingularInjectionPatch(data[i])
		end

		local p = Yarnbox.Patch.new()
		p.comment = comment
		p:MakeIntoInjection(injections)
		table.insert(Yarnbox.outPatches, p)
	end
end

function injection(comment)
	return function (data)
		return injections(comment) { data }
	end
end

AllOccurrences = Yarnbox.AllOccurrences

-- Injection code generators

function StaticFinalFunctionCall(arg)
	if type(arg) == "table" then
		return Yarnbox.StaticFinalFunctionCall(arg)
	else
		return Yarnbox.StaticFinalFunctionCall { arg }
	end
end

-- Object references

Function = Yarnbox.GetFunction

)lua";
}
