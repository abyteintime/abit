-- Yarnbox bootstrap file
-- Implements the high-level configuration DSL.

local Yarnbox = _yarnbox
_yarnbox = nil

-- Support

function print(...)
	local message_t = {}
	for i = 1, select('#', ...) do
		message_t[i] = tostring(select(i, ...))
	end
	Yarnbox.LogInfo(table.concat(message_t, '\t'))
end

-- Enums

Opcode = Yarnbox.Opcode

-- Patch declarations

Yarnbox.outPatches = {}

local function MakeSingularInjection(data)
	local i = Yarnbox.Injection.new()
	i.into = data.into
	if type(data.select) ~= "table" then
		data.select = { data.select }
	end
	i.select = data.select
	i.generate = data.generate
	return i
end

function injections(comment)
	return function (data)
		for i = 1, #data do
			data[i] = MakeSingularInjection(data[i])
		end

		local p = Yarnbox.Patch.new()
		p.comment = comment
		p:MakeIntoInjection(data)
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

function StaticFinalFunctionCall(func, options)
	local generator = Yarnbox.StaticFinalFunctionCall(func)
	if options ~= nil then
		assert(type(options) == "table", "the second argument to StaticFinalFunctionCall must be a table")
		for k, v in pairs(options) do
			generator[k] = v
		end
	end
	return generator:Downcast()
end

-- Object references

Function = Yarnbox.GetFunction
