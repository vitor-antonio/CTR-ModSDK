
// used all over 231,
// Plant, FlameJet, etc
struct HitboxDesc
{
	// check collision
	struct Instance* inst;
	struct Thread* thread;
	struct Thread* bucket;
	struct BoundingBox bbox;
	
	// post collision
	struct Thread* threadHit; // from bucket
	void* funcThCollide;
};

struct MaskHeadWeapon
{
	// 0x0
	short rot[3];

	// 0x6
	short duration;

	// 0x8
	struct Instance* maskBeamInst;

	// 0xC
	short pos[3];

	// 0x12
	short scale;

	// 0x14 bytes large
};

// bomb, missile, warpball
// yes, the bomb chases you and steers towards targets
struct TrackerWeapon
{
	// 0x0
	struct Driver* driverTarget; // being chased

	// 0x4
	struct Driver* driverParent; // who shot me

	// 0x8
	struct Instance* instParent; // of driver who shot me

	// 0xC
	void* ptrParticle;

	// 0x10
	short vel[3];

	// 0x16
	// & 1 - used 10 wumpa fruit
	u_short flags;

	// 0x18
	short dir[3];

	// 0x1e

	// 0x20
	// frameCount_DontHurtParent
	// set to 10?
	char data4[0x4];

	// 0x24
	int audioPtr;

	// 0x28
	unsigned int distanceToTarget;

	// === This point and beyond is Warpball ===
	
	// 0x2c
	// lev->0x14c + "this" * 0xC
	int respawnPointIndex;
	
	// 0x30
	int fadeAway_frameCount5;
	
	// 0x34
	// bitshift with driver->driverID
	// one bit for every driver hit
	u_int driversHit;
	
	// 0x38
	// distY from track floor
	int distFromGround;

	// 0x3C
	struct CheckpointNode* ptrNodeCurr;

	// 0x40
	struct CheckpointNode* ptrNodeNext;

	// 0x44
	u_char nodeCurrIndex;
	
	// 0x45
	u_char nodeNextIndex;
	
	// 0x46
	short padding;
	
	// 0x48
	int timeAlive;

	// 0x52 - turnAround count

	// 0x58 bytes large
};

// don't know where to put this but it probably exists
struct RainCloud
{
	char unknown[5];
	// 0x6
	// I guess this is used for randomizing the items?
	short itemScrollRandom;
};

// when the shield is worn
struct Shield
{
	// 0x0
	int animFrame;
	
	// 0x4
	short duration;
	
	// 0x6
	// & 2 - shooting
	// & 4 - blue shield
	// & ??? - shooting
	short flags;
	
	// 0x8
	struct Instance* instColor;
	
	// 0xC
	struct Instance* instHighlight;

	// 0x10
	short highlightRot[3];
	short highlightTimer;
	
	// 0x18
};

// when the shield is thrown
struct ShieldBomb
{
	// 0x58 bytes
};

struct MineWeapon;

struct WeaponSlot231
{
	// 0x0
	struct WeaponSlot231* next;
	struct WeaponSlot231* prev;
	
	// 0x8
	struct MineWeapon* mineWeapon;
};

// Tnt, Nitro, Beaker
struct MineWeapon
{
	// 0x0
	struct Driver* driverTarget; // who hit me

	// 0x4
	struct Instance* instParent; // of driver who placed me

	// 0x8
	struct Instance* crateInst; // if colliding with one

	// 0xc
	short velocity[3];

	// 0x12
	// used by tnt, and potion_inAir
	short stopFallAtY;

	// 0x14
	// causes explosion if != 0,
	// this is how mine pool destroys oldest mine
	short boolDestroyed;

	// 0x16
	// animation frame (on head)
	short numFramesOnHead;

	// 0x18
	struct WeaponSlot231* weaponSlot231;

	// 0x1C
	// relative to driver
	short deltaPos[3];

	// 0x22
	// how many more jumps until
	// the tnt flies off your head
	short jumpsRemaining;

	// 0x24
	// number of frames that mine can't hurt parent
	short frameCount_DontHurtParent;
	
	short unk26;

	// 0x28
	// 1 - red beaker
	// 2 - thrown (papu or komodo joe) (tnt/potion)
	u_short extraFlags;
	
	// 0x2a
	short cooldown; 
};

struct Baron
{
	// 0x0
	// for the baron plane
	char unused[0x24];
	
	// 0x24
	u_int soundID_flags;
	
	// 0x28
	// unused, for baron
	struct Instance* otherInst;
	
	// 0x2c
	short pointIndex;
	short footerPaddingUnused;
	
	// 0x30 bytes large
};

struct Blade
{
	int angle;

	// 0x4 bytes large
};

struct Crate
{
	int cooldown;
	int boolPauseCooldown;
	
	// 0x8 bytes large
};

struct Crystal
{
	short rot[3];
	short padding;

	// 0x8 bytes large
};

struct CtrLetter
{
	short rot[3];
	short padding;
	// 0x8 bytes large
};

struct StartBanner
{
	short unk0;
	short unk1;
	// 0x4 bytes large
};

struct Armadillo
{
	// 0x0
	short rotCurr[3];

	// 0x6
	short velX;

	// 0x8
	short rotDesired[3];

	// 0xe
	short velZ;

	// 0x10
	// at 0x500, it stops, and turns around
	short timeRolling;

	// 0x12
	// unused
	short numFramesSpinning;

	// 0x14
	// starts at zero, rolls, gets to high number,
	// then rolls back, and number goes back to zero
	short distFromSpawn;

	// 0x16
	short spawnPosX;

	// 0x18
	short spawnPosZ;

	// 0 for one direction, 1 for the other
	// 0x1a
	short direction;

	// frames at edge, waiting to roll again
	// 0x1c
	short timeAtEdge;

	// 0x20 bytes large
};

struct Fireball
{
	// 0x0
	short cycleTimer;

	// 0x2
	short fireballID;

	// 0x4
	// puts fireballs on different cycles,
	// just like spiders
	short cooldown;

	// 0x6 (maybe a rot[3]?)
	short unused[3];

	// 0xC
	short velY;

	// 0xE
	// unused
	short direction;

	// 0x10 bytes large
};

struct FlameJet
{
	// 0x0
	int cycleTimer;

	// 0x4
	int dirX;

	// 0x8
	int dirZ;

	// 0xC
	// adds 0x100 per frame
	int unk;

	// 0xe
	int cooldown;

	// 0x10
	void* audioPtr;

	// 0x14 bytes large
};

#if 0
struct Fruit
{
	// 0x4 bytes large
};
#endif

struct Minecart
{
	// 0x0
	short posStart[3];

	// 0x6
	short betweenPoints_currFrame;

	// 0x8
	short posEnd[3];

	// 0xe
	short dir[3];

	// 0x14
	int posIndex;

	// 0x18
	short rotCurr[3];

	// 0x1e
	short betweenPoints_numFrames;

	// 0x20
	short rotDesired[3];

	// 0x26
	short rotSpeed;

	// 0x28
	int audioPtr;

	// 0x2c bytes large
};

struct Orca
{
	// 0x0
	short startPos[3];

	// 0x6
	short orcaID;

	// 0x8
	short endPos[3];

	// 0xE
	short cooldown;

	// 0x10
	short instDefRot[3];

	// 0x16
	short animIndex;

	// 0x26
	short numFrames;

	// 0x28
	short midpoint[3]; // ?

	// 0x2e
	short direction;

	// 0x30 bytes large
};

struct Plant
{
	// 0x0
	short cycleCount;

	// 0x2
	short cooldown;
	
	// 0x4
	// 0: left side of track
	// 1: right side of track
	short LeftOrRight;

	// 0x6
	short boolEatingPlayer;

	// 0x8 bytes large
};

struct Seal
{
	// 0x0
	short spawnPos[3];

	// 0x6
	short sealID;

	// 0x8
	// unused, should erase
	short endPos[3];

	// 0xe
	short distFromSpawn;

	// 0x10
	short rotCurr[3];
	short padding16;

	// 0x18
	short rotDesired[3];

	// 0x1e
	short direction;

	// 0x20
	// unused, should erase
	short rotDesiredAlt[3];

	// 0x26
	short numFramesSpinning;

	// 0x28
	short vel[3];
	short padding2e;

	// 0x30 bytes large
};

struct Snowball
{
	// 0x0
	short rot_unused[3];

	// 0x6
	short pointIndex;

	// 0x8
	short numPoints;

	// 0xA
	short snowID;

	// 0xC
	int audioPtr;

	// 0x10 bytes large
};

struct Spider
{
	// 0x0
	// counts five times
	short animLoopCount;

	// 0x2
	short spiderID;

	// 0x4
	short delay;

	// 0x6
	short boolNearRoof; // 0 on ground, 1 near roof

	// 0x8

	// 0xC
	struct Instance* inst;

	// end of struct, 8 bytes large
};

struct Teeth
{
	// 0x0
	// -1: closing
	// 0: not moving
	// 1: opening
	int direction;

	// 0x4
	// countdown starts when door
	// is fully open, door starts
	// closing when it is done
	int timeOpen;

	// 0x8 bytes large
};

struct Turtle
{
	// 0x0
	short timer;
	
	// 0x2
	// 0 from moment it hits top to moment it hits bottom
	// 1 from moment it hits bottom to moment it hits top
	short direction;
	
	// 0x4
	short unk4;

	// 0x6
	short turtleID;

	// 0x8
	// 0 - fully up (big jump)
	// 1 - not fully up (small jump)
	short state;

	// 0xC bytes large
};
