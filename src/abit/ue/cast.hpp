#pragma once

#include <type_traits>

#include "abit/ue/UObject.hpp"

namespace ue {

template<typename To>
const To*
Cast(const UObject* object)
{
	if (object != nullptr && object->InstanceOf(To::StaticClass())) {
		return reinterpret_cast<To*>(object);
	} else {
		return nullptr;
	}
}

template<typename To>
To*
Cast(UObject* object)
{
	if (object != nullptr && object->InstanceOf(To::StaticClass())) {
		return reinterpret_cast<To*>(object);
	} else {
		return nullptr;
	}
}

}
