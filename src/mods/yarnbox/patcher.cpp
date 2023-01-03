#include "yarnbox/patcher.hpp"

#include "abit/loader/ensure.hpp"
#include "abit/loader/logging.hpp"

#include "yarnbox/ue/UObject/fmt.hpp"

using namespace yarn;
using namespace ue;

static void
ApplyReplacement(const Patch::Replacement& replacement, const Registry& registry)
{
	const Class* replacementClass = registry.GetClassByName(replacement.className);
	ABIT_ENSURE(replacementClass != nullptr, "class '{}' does not exist", replacement.className);

	UStruct* parentType = replacementClass->unreal->parentType;
	ABIT_ENSURE(
		parentType->objectClass == UClass::StaticClass(),
		"{}, the parent type of {}, is not a class",
		UObjectFmt{ parentType },
		UObjectFmt{ replacementClass->unreal }
	);

	const Class* parentClass = registry.GetClassByPointer(reinterpret_cast<UClass*>(parentType));

	ABIT_ENSURE(
		replacementClass->functions.count(replacement.function) > 0,
		"function '{}' does not exist within replacement class '{}'",
		replacement.function,
		replacement.className
	);
	ABIT_ENSURE(
		parentClass->functions.count(replacement.function) > 0,
		"function '{}' does not exist within the parent class '{}' of replacement class '{}'.\n"
		"Available functions are:{}",
		replacement.function,
		parentClass->unreal->GetName().ToString(),
		replacement.className,
		[parentClass] {
			std::string text;
			for (const auto& [key, value] : parentClass->functions) {
				text += fmt::format("\n  - {}", key);
			}
			return text;
		}()
	);

	UFunction* functionToReplace = parentClass->functions.at(replacement.function).unreal;
	UFunction* replacementFunction = replacementClass->functions.at(replacement.function).unreal;
	functionToReplace->bytecode.Clear();
	functionToReplace->bytecode.ExtendByCopying(replacementFunction->bytecode);

	spdlog::debug(
		"Replacement patch for {}.{} was applied successfully",
		UObjectFmt{ parentType },
		replacement.function
	);
}

void
yarn::ApplyPatch(const Patch& patch, const Registry& registry)
{
	spdlog::debug("Applying patch '{}'", patch.comment);

	try {
		if (const auto* replacement = std::get_if<Patch::Replacement>(&patch.data)) {
			ApplyReplacement(*replacement, registry);
		} else if (const auto* injection = std::get_if<Patch::Injection>(&patch.data)) {
			spdlog::error("Cannot apply injection patch: injections are not yet implemented");
		}
	} catch (abit::Error e) {
		spdlog::error("Cannot apply patch '{}': {}", patch.comment, e.what);
	}
}
