#include <common.h>

void MainDrawCb_DrawSync()
{
	struct GameTracker* gGT;
	gGT = sdata->gGT;
	
	if (gGT->bool_DrawOTag_InProgress == 1)
		gGT->bool_DrawOTag_InProgress = 0;
	
	return;
}