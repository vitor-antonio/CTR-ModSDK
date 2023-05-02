// Version = MDD of release, all 1999
// For example, May 7 would be 507

#define AugReview 805
#define SepReview 903
#define UsaRetail 926
#define JpnTrial 1006
#define EurRetail 1020
#define JpnRetail 1111

#if BUILD == EurRetail
#define SCREEN_HEIGHT 236
#define FPS 25
#define ELAPSED_MS 40
#else
#define SCREEN_HEIGHT 216
#define FPS 30
#define ELAPSED_MS 32
#endif
#define SCREEN_WIDTH 512
#define SECOND (FPS * ELAPSED_MS)
#define MINUTE (SECOND * 60)