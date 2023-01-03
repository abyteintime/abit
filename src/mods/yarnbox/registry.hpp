#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "yarnbox/ue/UClass.hpp"
#include "yarnbox/ue/UFunction.hpp"

namespace yarn {

struct Function
{
	ue::UFunction* unreal;
};

struct Class
{
	ue::UClass* unreal;
	std::unordered_map<std::string, Function> functions;
};

class Registry
{
	std::unordered_map<std::string, std::shared_ptr<Class>> classesByName;
	std::unordered_map<ue::UClass*, std::shared_ptr<Class>> classesByPointer;

public:
	void RegisterClass(ue::UClass* uclass);
	bool IsClassRegistered(ue::UClass* uclass);
	void RegisterFunction(Function&& function);

	const Class* GetClassByName(const std::string& name) const;
	const Class* GetClassByPointer(ue::UClass* pointer) const;
	Class* GetClassByPointer(ue::UClass* pointer);
};

}
