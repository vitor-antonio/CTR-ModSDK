struct BigEntry
{
	int offset;
	int size;
};

struct BigHeader
{
	int cdpos;
	int numEntry;

	// "numEntry" number of entries
	//struct BigEntry entry[0];
};
#define BIG_GETENTRY(x) \
	((unsigned int)x + sizeof(struct BigHeader))

struct DramPointerMap
{
	int numBytes;
	
	//int offsets[0];
};

#define DRAM_GETOFFSETS(x) \
	((unsigned int)x + sizeof(struct DramPointerMap))

struct VramHeader
{
	char data[0xC];
	
	// 0xC
	RECT rect;
	
	// 0x14
	//u_char pixels[0];
};

#define VRAMHEADER_GETPIXLES(x) \
	((unsigned int)x + sizeof(struct VramHeader))

enum LevVramIndex
{
	LVI_VRAM = 0,
	LVI_LEV = 1,
	LVI_PTR = 2,
};

enum LoadType
{
	// ordinary read to ram
	LT_RAW = 1,
	
	// read with pointer map at the end
	LT_DRAM = 2,
	
	// read with vram transfer
	LT_VRAM = 3
};

struct LoadQueueSlot
{
	// 0x0
	struct BigHeader* ptrBigfileCdPos;

	// 0x4
	unsigned short flags;

	// 0x6
	unsigned short type;

	// 0x8
	unsigned int subfileIndex;

	// 0xC
	void* ptrDestination;

	// 0x10
	unsigned int size;

	// 0x14
	union
	{
		// last 4 bytes will either be a function pointer,
		// or a flag that signifies there is no function pointer

		void (*funcPtr)(struct LoadQueueSlot*);

		unsigned int flags;

	} callback;
};

_Static_assert(sizeof(struct LoadQueueSlot) == 0x18);