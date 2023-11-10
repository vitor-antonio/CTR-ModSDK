#include <common.h>

void DECOMP_MM_CupSelect_MenuBox(struct MenuBox *mb)
{
    char i;
    u_char cupIndex;
    u_char starIndex;
    u_char trackIndex;
    short elapsedFrames;
    u_int txtColor;
    u_int *starColor;
    int startX;
    int startY;
    struct GameTracker *gGT = sdata->gGT;
    RECT cupBox;

    if (mb->unk1e == 0)
    {
        OVR_230.cupSel_postTransition_boolStart = (mb->rowSelected != -1);
        OVR_230.cupSel_transitionState = 2;
        OVR_230.menubox_cupSelect.state &= ~(EXECUTE_FUNCPTR);
		OVR_230.menubox_cupSelect.state |= DISABLE_INPUT_ALLOW_FUNCPTRS;
        return;
    }

    elapsedFrames = OVR_230.cupSel_transitionFrames;

    // if not stationary
    if (OVR_230.cupSel_transitionState != 1)
    {
        // if transitioning in
        if (OVR_230.cupSel_transitionState == 0)
        {
            // Ghidra said it has another param of "1", when it only accepts 3 params.
            MM_TransitionInOut(&OVR_230.transitionMeta_cupSel[0], elapsedFrames, 8);

            // reduce frame count
            elapsedFrames--;

            // if no more frames
            if (elapsedFrames == 0)
            {
                // menu is now in focus
                OVR_230.cupSel_transitionState = 1;
				OVR_230.menubox_cupSelect.state &= ~(DISABLE_INPUT_ALLOW_FUNCPTRS);
				OVR_230.menubox_cupSelect.state |= EXECUTE_FUNCPTR;
            }
        }
        // if transitioning out
        else if (OVR_230.cupSel_transitionState == 2)
        {
            MM_TransitionInOut(&OVR_230.transitionMeta_cupSel[0], elapsedFrames, 8);

            // increase frame count
            elapsedFrames++;

            // if more than 12 frames pass
            if (12 < elapsedFrames)
            {
                // if cup selected
                if (OVR_230.cupSel_postTransition_boolStart != 0)
                {
                    // set cupID to the cup selected
                    gGT->cup.cupID = mb->rowSelected;

                    // set track index to zero, to go to first track
                    gGT->cup.trackIndex = 0;

                    // loop through 8 drivers
                    for (i = 0; i < 8; i++)
                    {
                        // set all points for all 8 drivers to zero
                        gGT->cup.points[i] = 0;
                    }

                    // passthrough MenuBox for the function
                    sdata->ptrDesiredMenuBox = &data.menuBox_TitleBeginTrack;

                    // set current level
                    gGT->currLEV = data.ArcadeCups[gGT->cup.cupID].CupTrack[gGT->cup.trackIndex].trackID;
                    return;
                }

                // return to character selection
                sdata->ptrDesiredMenuBox = &OVR_230.menubox_characterSelect;

                MM_Characters_RestoreIDs();
                return;
            }
        }
    }

    OVR_230.cupSel_transitionFrames = elapsedFrames;

    // "SELECT CUP RACE"
    DecalFont_DrawLine(sdata->lngStrings[0xBF],
                       (OVR_230.transitionMeta_cupSel[4].currX + 0x100),
                       (OVR_230.transitionMeta_cupSel[4].currY + 0x10), 1, 0xffff8000);
    // Loop through all four cups
    for (cupIndex = 0; cupIndex < 4; cupIndex++)
    {
        // If this cup is the one you selected
        if (cupIndex == mb->rowSelected)
        {
            // Make text flash
            txtColor = (sdata->frameCounter & 2) ? 0xffff8000 : 0xffff8004;
        }
        // If this is not the cup you're highlighting
        else
        {
            // Use solid color
            txtColor = 0xffff8000;
        }

        // draw the name of the cup
        DecalFont_DrawLine(sdata->lngStrings[data.ArcadeCups[cupIndex].lngIndex_CupName],
                            
                           (OVR_230.transitionMeta_cupSel[cupIndex].currX + (cupIndex & 1) * 200 + 0xa2),
                           (OVR_230.transitionMeta_cupSel[cupIndex].currY + (cupIndex - (cupIndex >> 0xf) >> 1) * 0x54 + 0x44),
                           3, txtColor);
    }

    // Loop through all four cups
    for (cupIndex = 0; cupIndex < 4; cupIndex++)
    {
        startX = (short) OVR_230.transitionMeta_cupSel[cupIndex].currX + (cupIndex & 1) * 200 + 0x4e;
        startY = (short) OVR_230.transitionMeta_cupSel[cupIndex].currY + (cupIndex - (cupIndex >> 0xf) >> 1) * 0x54 + 0x29;

        // loop through 3 stars to draw
        for (starIndex = 0; starIndex < 3; starIndex++)
        {
			// assuming starUnlock is never more than 32,
			// otherwise you'd do [flag>>5] >> flag&0x1f
            int starUnlock = OVR_230.cupSel_StarUnlockFlag[starIndex] + cupIndex;
            if (((sdata->gameProgress.unlocks[0] >> starUnlock) & 1) != 0)
            {
                // array of colorIDs
                // 0x11: driver_C (tropy) (blue)
                // 0x0E: driver_9 (papu) (yellow)
                // 0x16: silver

                starColor = data.ptrColor[OVR_230.cupSel_StarColorIndex[starIndex]];

				struct Icon** iconPtrArray =
					ICONGROUP_GETICONS(gGT->iconGroup[5]);

                DecalHUD_DrawPolyGT4(iconPtrArray[0x37],
                                     (startX + (cupIndex & 1) * 0xCA - 0x16),
                                     (startY + ((starIndex * 0x10) + 0x10)),

                                     // pointer to PrimMem struct
                                     &gGT->backBuffer->primMem,

                                     // pointer to OT mem
                                     gGT->tileView_UI.ptrOT,

                                     // color data
                                     starColor[0], starColor[1],
                                     starColor[2], starColor[3],

                                     0, FP(1.0));
            }
        }

        // loop through all four track icons in one cup
        for (trackIndex = 0; trackIndex < 4; trackIndex++)
        {
            // Draw Icon of each track
            MENUBOX_DrawPolyGT4(gGT->ptrIcons[data.ArcadeCups[cupIndex].CupTrack[trackIndex].iconID],
                                (startX + (trackIndex & 1) * 0x54),
                                (startY + (trackIndex >> 1) * 0x23),

                                // pointer to PrimMem struct
                                &gGT->backBuffer->primMem,

                                // pointer to OT mem
                                gGT->tileView_UI.ptrOT,

                                OVR_230.cupSel_Color,
                                OVR_230.cupSel_Color,
                                OVR_230.cupSel_Color,
                                OVR_230.cupSel_Color,
                                0, FP(0.5));
        }
		
        if (cupIndex == mb->rowSelected)
        {
			// highlight box
            cupBox.x = startX - 3;
            cupBox.y = startY - 2;
            cupBox.w = 174;
            cupBox.h = 74;

            CTR_Box_DrawClearBox(&cupBox, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL,
                                 gGT->backBuffer->otMem.startPlusFour,
                                 &gGT->backBuffer->primMem);
        }
		
		// background box
        cupBox.x = startX - 6;
        cupBox.y = startY - 4;
        cupBox.w = 180;
        cupBox.h = 78;
        
		MENUBOX_DrawInnerRect(&cupBox, 0, gGT->backBuffer->otMem.startPlusFour);
    }
}