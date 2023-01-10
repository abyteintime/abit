#include "abit/ue/memory.hpp"

#include "abit/procs/global.hpp"

void*
ue::appMalloc(size_t size)
{
	// NOTE: The second parameter to appMalloc and appRealloc doesn't seem to be used, so we set it
	// to 0.
	size_t unused = 0;
	return abit::procs::global::appMalloc.Call<void*>(size, unused);
}

void*
ue::appRealloc(void* mem, size_t size)
{
	size_t unused = 0;
	return abit::procs::global::appRealloc.Call<void*>(mem, size, unused);
}

void
ue::appFree(void* mem)
{
	return abit::procs::global::appFree.Call<void>(mem);
}
