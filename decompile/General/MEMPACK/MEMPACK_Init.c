#include <common.h>

#ifdef REBUILD_PC
// with 8mb expansion, we have bonus PrimMem
char memory[8*1024*1024];
#endif

void DECOMP_MEMPACK_Init(int ramSize)
{
	struct Mempack* ptrMempack;

	// Get the pointer to the memory allocation system
	ptrMempack = sdata->PtrMempack;

#ifdef REBUILD_PC

	// must be a 24-bit address
	// Visual Studio -> Properties -> Linker -> Advanced -> 
	// Base Address, Randomized Base Address, Fixed Base Address
	ptrMempack->start = &memory[0];
	memset(memory, 0, 8*1024*1024);
	
	ptrMempack->endOfAllocator = &memory[8*1024*1024 - 4];
	ptrMempack->lastFreeByte = &memory[8*1024*1024 - 4];

#else
	
	// start of memory allocation system
	// To Do: make this dynamic depending on OVR Region 3,
	// 			like it is in the original assembly that
	//			ghidra wont read
	ptrMempack->start = (void *)0x800ba9f0;

	// Set pointer to end of CTR Memory allocation
	// It adds so much so that it becomes 801FF800
	// with the '80' prefix
	ptrMempack->endOfAllocator = (void *)(ramSize + 0x7ffff800);
	ptrMempack->lastFreeByte = (void *)(ramSize + 0x7ffff800);
#endif

	ptrMempack->numBookmarks = 0;

	// dont set endOfMemory, waste of time
	// dont set packSize, waste of time

	// set end of allocation to the start of allocation
	ptrMempack->firstFreeByte = ptrMempack->start;
}