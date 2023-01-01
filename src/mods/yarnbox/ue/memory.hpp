#pragma once

#include <cstdint>

namespace ue {

void*
appMalloc(size_t size);

void*
appRealloc(void* mem, size_t size);

void
appFree(void* mem);

}
