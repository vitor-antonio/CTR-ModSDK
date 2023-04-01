struct BossGarageDoor
{
	int direction; // 1, 0, -1

	// so you can't spam open/close
	int cooldown;

	struct Instance* garageTopInst;

	// 0x14 bytes large
};

struct WarpPad
{
	// 0x0
	struct Instance* inst[5];
	
	// 0x14
	char unk54[0x3C];
	
	// 0x28
	// some other rotation vector?
	
	// 0x50
	short specLightGem[4];
	
	// 0x58
	short specLightRelic[4];
	
	// 0x60
	short specLightToken[4];
	
	// 0x68
	short digit10s;
	
	// 0x6a
	short digit1s;
	
	// 0x6c (1b*4)
	int levelID;
	
	// 0x70
	int unk70;
	
	// 0x74 (1d*4)
	short boolWaitForAkuHint;
	
	// 0x76
	short framesWarping;
	
	// 0x78 -- size
};