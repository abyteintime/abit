#include "yarnbox/registry.hpp"

#include "abit/loader/logging.hpp"

#include "yarnbox/ue/UObject/fmt.hpp"

using namespace yarn;
using namespace ue;

void
Registry::RegisterClass(UClass* uclass)
{
	FString name = uclass->GetName();
	Class data{ uclass };
	auto ptr = std::make_shared<Class>(std::move(data));
	classesByName[name.ToString()] = ptr;
	classesByPointer[uclass] = std::move(ptr);
}

bool
Registry::IsClassRegistered(ue::UClass* uclass)
{
	return classesByPointer.count(uclass) > 0;
}

void
Registry::RegisterFunction(Function&& function)
{
	std::string name = function.unreal->GetName().ToString();
	if (function.unreal->outer->objectClass != UClass::StaticClass()) {
		spdlog::warn(
			"Cannot register function '{}' that is not declared inside a class. It is declared "
			"inside {}",
			name,
			UObjectFmt{ function.unreal->outer }
		);
		return;
	}

	UClass* declaringClass = reinterpret_cast<UClass*>(function.unreal->outer);
	if (!IsClassRegistered(declaringClass)) {
		RegisterClass(declaringClass);
	}

	Class* classData = GetClassByPointer(declaringClass);
	if (classData->functions.count(name) > 0) {
		spdlog::warn(
			"Function '{}' from {} was registered twice", name, UObjectFmt{ declaringClass }
		);
	}
	classData->functions[name] = std::move(function);
}

const Class*
Registry::GetClassByName(const std::string& name) const
{
	if (classesByName.count(name) > 0) {
		return &*classesByName.at(name);
	} else {
		return nullptr;
	}
}

const Class*
Registry::GetClassByPointer(ue::UClass* pointer) const
{
	if (classesByPointer.count(pointer) > 0) {
		return &*classesByPointer.at(pointer);
	} else {
		return nullptr;
	}
}

Class*
Registry::GetClassByPointer(ue::UClass* pointer)
{
	if (classesByPointer.count(pointer) > 0) {
		return &*classesByPointer.at(pointer);
	} else {
		return nullptr;
	}
}
