#include "abit/ue/memory.hpp"

#include "abit/procs/27.hpp"
#include "abit/procs/68.hpp"
#include "abit/procs/ed.hpp"

void*
ue::appMalloc(size_t size)
{
	// NOTE: The second parameter to appMalloc and appRealloc doesn't seem to be used, so we set it
	// to 0.
	size_t unused = 0;
	return abit::procs::P_2710c5454623b68795ff644e3f85a58d_0.Call<void*>(size, unused);
}

void*
ue::appRealloc(void* mem, size_t size)
{
	size_t unused = 0;
	return abit::procs::P_ed92b5428dbe39954427779cdcdd9926_0.Call<void*>(mem, size, unused);
}

void
ue::appFree(void* mem)
{
	return abit::procs::P_682827449562310e31d6f3fc832dc25e_0.Call<void>(mem);
}
