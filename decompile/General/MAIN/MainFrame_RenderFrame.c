#include <common.h>

// all in this file
void DrawControllerError(struct GameTracker* gGT, struct GamepadSystem* gGamepads);
void DrawFinalLap(struct GameTracker* gGT);
void RainLogic(struct GameTracker* gGT);
void MenuHighlight();
void RenderAllWeather(struct GameTracker* gGT);
void RenderAllConfetti(struct GameTracker* gGT);
void RenderAllStars(struct GameTracker* gGT);
void RenderAllHUD(struct GameTracker* gGT);
void RenderAllBeakerRain(struct GameTracker* gGT);
void RenderAllBoxSceneSplitLines(struct GameTracker* gGT);
void RenderBucket_QueueAllInstances(struct GameTracker* gGT);
void RenderAllNormalParticles(struct GameTracker* gGT);
void RenderDispEnv_World(struct GameTracker* gGT);
void RenderAllFlag0x40(struct GameTracker* gGT);
void RenderAllTitleDPP(struct GameTracker* gGT);
void RenderBucket_ExecuteAllInstances(struct GameTracker* gGT);
void RenderAllTires(struct GameTracker* gGT);
void RenderAllShadows(struct GameTracker* gGT);
void RenderAllHeatParticles(struct GameTracker* gGT);
void RenderAllLevelGeometry(struct GameTracker* gGT);
void MultiplayerWumpaHUD(struct GameTracker* gGT);
void WindowBoxLines(struct GameTracker* gGT);
void WindowDivsionLines(struct GameTracker* gGT);
void RenderDispEnv_UI(struct GameTracker* gGT);
void RenderVSYNC(struct GameTracker* gGT);
void RenderFMV();
void RenderSubmit(struct GameTracker* gGT);

// original CTR funcs
void UI_CupStandings_InputAndDraw();
void VB_EndEvent_DrawMenu();
void RR_EndEvent_DrawMenu();
void AA_EndEvent_DrawMenu();
void TT_EndEvent_DrawMenu();
void CC_EndEvent_DrawMenu();
void AH_Map_Main();
void AH_MaskHint_Start(short requestedHint, short bool_interruptWarppad);
void CS_BoxScene_InstanceSplitLines();
void RB_Player_ToggleInvisible();
void RB_Player_ToggleFlicker();
void RB_Burst_ProcessBucket(struct Thread* thread);
void RB_Blowup_ProcessBucket(struct Thread* thread);
void RB_Spider_DrawWebs(struct Thread* thread, struct TileView* tileView);
void RB_Follower_ProcessBucket(struct Thread* thread);
void RB_StartText_ProcessBucket(struct Thread* thread);
void DECOMP_AH_WarpPad_AllWarppadNum();
u_int MM_Video_CheckIfFinished(int param_1);

#ifdef USE_60FPS
void PatchModel_60fps(struct Model* m)
{
	struct ModelHeader* h;
	struct ModelAnim** a;
	int i;
	int j;
	int loopNum;
	struct Thread* search;

	// error check (yes, needed)
	if(m == 0) return;

#ifndef REBUILD_PC
	// ignore ND box, intro models, oxide intro, podiums, etc
	if(LOAD_IsOpen_Podiums()) return;
#endif

	// model header
	h = m->headers;

	// skip if the model is patched
	if(h[0].name[0xf] == 1) return;

	// record the model is patched
	h[0].name[0xf] = 1;

	#if 0
	// max graphics
	h[0].maxDistanceLOD = 0x7fff;
	#endif

	#if 0
	// min graphics
	for(i = 0; i < m->numHeaders-1; i++)
	{
		h[i].maxDistanceLOD = 0;
	}
	#endif

	// loop through headers
	for(i = 0; i < m->numHeaders; i++)
	{		
		// pointer to array of pointers
		a = h[i].ptrAnimations;

		// number of animations
		loopNum = h[i].numAnimations;

		// loop through all animations
		for(j = 0; j < loopNum; j++)
		{
			// skip doubling, if interpolation already happens,
			// known to happen in spiders, and drivers for
			// low LOD anims, and high LOD crashing + reversing
			if(a[j]->numFrames & 0x8000) continue;
			
			// multiply by 2
			a[j]->numFrames =
			a[j]->numFrames << 1;

			// should only need to subtract one,
			// but then many animations break on last frame,
			// need to patch code that manipulates last frame
			a[j]->numFrames--;

			// negative, or flag?
			a[j]->numFrames =
			a[j]->numFrames | 0x8000;
		}
	}
}
void ScanInstances_60FPS(struct GameTracker* gGT)
{
	struct JitPool* instPool = &sdata->gGT->JitPools.instance;
	
	// check if pool is empty
	if(instPool->free.count == 0) return;
	if(instPool->taken.first == 0) return;

	for(
			struct Instance* inst = instPool->taken.first; 
			inst != 0; 
			inst = inst->next
		)
	{
		PatchModel_60fps(inst->model);
	}
	
	if(gGT->level1 != 0)
	for(int i = 0; i < gGT->level1->numInstances; i++)
	{
		struct Instance* inst = gGT->level1->ptrInstDefs[i].ptrInstance;
		
		if(inst != 0)
			PatchModel_60fps(inst->model);
	}
}
#endif

void DECOMP_MainFrame_RenderFrame(struct GameTracker* gGT, struct GamepadSystem* gGamepads)
{
	struct Level* lev = gGT->level1;
	
	#ifdef USE_60FPS
	if ((gGT->renderFlags & 0x20) != 0)
		ScanInstances_60FPS(gGT);
	#endif
	
#ifndef REBUILD_PS1
	DrawControllerError(gGT, gGamepads);
	DrawFinalLap(gGT);
	ElimBG_HandleState(gGT);
	
	if((gGT->renderFlags & 0x21) != 0)
		MainFrame_VisMemFullFrame(gGT, gGT->level1);
#endif
	
	if((gGT->renderFlags & 1) != 0)
		if(gGT->visMem1 != 0)
			if(lev != 0)
				DECOMP_CTR_CycleTex_LEV(
					lev->ptr_anim_tex,
					gGT->timer);
				
	if(
		(sdata->ptrActiveMenuBox != 0) ||
		((gGT->gameMode1 & END_OF_RACE) != 0)
	)
	{
		DECOMP_MENUBOX_CollectInput();
	}
	
	if(sdata->ptrActiveMenuBox != 0)
		if(sdata->Loading.stage == -1)
			DECOMP_MENUBOX_ProcessState();

#ifndef REBUILD_PS1	
	RainLogic(gGT);
	EffectSfxRain_MakeSound(gGT);
#endif

	MenuHighlight();
	
#ifdef REBUILD_PS1

	// This is temporary until RainLogic is done
	DECOMP_TileView_SetMatrixVP(&gGT->tileView[0]);
	DECOMP_TileView_SetMatrixVP(&gGT->tileView[1]);
	DECOMP_TileView_SetMatrixVP(&gGT->tileView[2]);
	DECOMP_TileView_SetMatrixVP(&gGT->tileView[3]);

	// This is temporary until RenderBucket is done
	if ((gGT->renderFlags & 0x20) != 0)
	{
		void TEST_DrawInstances(struct GameTracker* gGT);
		RenderDispEnv_World(gGT); // == RenderDispEnv_World ==
		TEST_DrawInstances(gGT);
		RenderDispEnv_World(gGT); // == RenderDispEnv_World ==
	}
	
	// This is temporary, until RenderAllHUD is done
	if(
		(gGT->numPlyrCurrGame != 1) ||
		((gGT->hudFlags & 8) == 0) ||
		((gGT->gameMode1 & START_OF_RACE) == 0)
	)
	{
		// why is this needed? adv hub crashes otherwise
		if((gGT->gameMode1 & 0x40000000) == 0)
		if(gGT->level1 != 0)
		
		if((gGT->hudFlags & 1) != 0)
		{
			if((gGT->gameMode1 & ADVENTURE_ARENA) != 0)
			{
				DECOMP_UI_RenderFrame_AdvHub();
			}
			
			else
			{
				DECOMP_UI_RenderFrame_Racing();
			}
		}
	}
	
#endif

#ifndef REBUILD_PS1	
	RenderAllWeather(gGT);
	RenderAllConfetti(gGT);
	RenderAllStars(gGT);
	
	#if 0
	// Multiplayer PixelLOD Part 1
	#endif
	
	RenderAllHUD(gGT);
	RenderAllBeakerRain(gGT);
	RenderAllBoxSceneSplitLines(gGT);
	
	RenderBucket_QueueAllInstances(gGT);
	RenderAllNormalParticles(gGT);
	RenderDispEnv_World(gGT); // == RenderDispEnv_World ==
	
	#if 0
	// Multiplayer PixelLOD Part 2
	#endif
	
	RenderAllFlag0x40(gGT); // I need a better name
	RenderAllTitleDPP(gGT);
	
	RenderBucket_ExecuteAllInstances(gGT);
	
	RenderAllTires(gGT);
	RenderAllShadows(gGT);
	RenderAllHeatParticles(gGT);
#endif
	
	DECOMP_TileView_FadeAllWindows();
	
	if((gGT->renderFlags & 1) != 0)
	{
#ifndef REBUILD_PS1
		RenderAllLevelGeometry(gGT);
#else
		for (int i = 0; i < gGT->numPlyrCurrGame; i++)
		{
			// 226-229
			// placeholder for DrawLevelOvr1P
			TEST_226(
				0,
				&gGT->tileView[i],
				gGT->level1->ptr_mesh_info,
				&gGT->backBuffer->primMem,
				0,
				0); // waterEnvMap?
				
			// placeholder for DrawSky_Full
			TEST_DrawSkybox(
				gGT->level1->ptr_skybox,
				&gGT->tileView[i],
				&gGT->backBuffer->primMem);
		}
#endif
		
		
		RenderDispEnv_World(gGT); // == RenderDispEnv_World ==
		MultiplayerWumpaHUD(gGT);
		
		#if 0
		// Multiplayer Pixel LOD Part 3
		#endif
		
		if(
			// if not cutscene
			// if not in adventure arena
			// if not in main menu
			((gGT->gameMode1 & (GAME_CUTSCENE | ADVENTURE_ARENA | MAIN_MENU)) == 0) &&
			
			// if loading is 100% finished
			(sdata->Loading.stage != -4)
		)
		{
			DECOMP_DotLights(gGT);
		
			if((gGT->renderFlags & 0x8000) != 0)
			{
				WindowBoxLines(gGT);
				WindowDivsionLines(gGT);
			}
		
		}
		
#ifndef REBUILD_PS1
		// if game is not loading
		if (sdata->Loading.stage == -1) 
		{
			// If game is not paused
			if ((gGT->gameMode1 & PAUSE_ALL) == 0)
			{
				RobotcarWeapons_Update();
			}
	
			StartLine_Update();
		}
#endif
	}

#ifndef REBUILD_PS1
	// If in main menu, or in adventure arena,
	// or in End-Of-Race menu
	if ((gGT->gameMode1 & (ADVENTURE_ARENA | END_OF_RACE | MAIN_MENU)) != 0) {
		unk80047d64();
	}
#endif
	
	// clear swapchain
	if (
			((gGT->renderFlags & 0x2000) != 0) &&
			(
				(lev->clearColor[0].enable != 0) ||
				(lev->clearColor[1].enable != 0)
			)
		)
	{
		DECOMP_CAM_ClearScreen(gGT);
	}
	
	if ((gGT->renderFlags & 0x1000) != 0)
	{
		DECOMP_TitleFlag_DrawSelf();
	}

	RenderDispEnv_UI(gGT);
	
#ifndef REBUILD_PS1
	gGT->countTotalTime = 
		RCNT_GetTime_Total();
#endif
	
	RenderVSYNC(gGT);
	
#ifndef REBUILD_PS1
	RenderFMV();
	
	gGT->countTotalTime =
		RCNT_GetTime_Elapsed(gGT->countTotalTime,0);
#endif
	
	RenderSubmit(gGT);
}

#ifndef REBUILD_PS1
void DrawControllerError(struct GameTracker* gGT, struct GamepadSystem* gGamepads)
{
	int posY;
	int lngArrStart;
	RECT window;
	int i;
	
	// dont draw error if demo mode, or cutscene,
	// or if no controllers are missing currently
	if(gGT->boolDemoMode == 1) return;
	if((gGT->gameMode1 & GAME_CUTSCENE) != 0) return;
	if(MainFrame_HaveAllPads(gGT->numPlyrNextGame) == 1) return;
	
	// if main menu is open, assume 230 loaded,
	// quit if menu is at highest level (no ptrNext to draw)
	if(sdata->ptrActiveMenuBox == 0x800B4540)
		if((*(int*)0x800b4548 & 0x10) == 0) return;
	
	// position of error
	posY = data.errorPosY[sdata->errorMessagePosIndex];

	// "Controller 1" or "Controller 2"
	lngArrStart = 0;
	
	window.x = 0xffec;
	window.y = posY - 3;
	window.w = 0x228;
	window.h = 0;
	
	// if more than 2 players, or if multitap used
	if(
		(gGT->numPlyrNextGame > 2) || 
		(gGamepads->slotBuffer[0].controllerData == (PAD_ID_MULTITAP << 4))
	)
	{
		// change to "1A", "1B", "1C", "1D",
		lngArrStart = 2;
	}
	
	for(i = 0; i < gGT->numPlyrNextGame; i++)
	{
		struct ControllerPacket* ptrControllerPacket = gGamepads->gamepad[i].ptrControllerPacket;
		
		if(ptrControllerPacket != 0)
			if(ptrControllerPacket->isControllerConnected == 0) continue;
		
		// if controller is unplugged
		
		DecalFont_DrawLine(
			sdata->lngStrings
			[
				data.lngIndex_gamepadUnplugged[lngArrStart + i]
			],
			0x100, posY + window.h, FONT_SMALL, (JUSTIFY_CENTER | ORANGE));

		// add for each line
		window.h += 8;
	}
	
	// PLEASE CONNECT A CONTROLLER
	DecalFont_DrawLine(
		sdata->lngStrings[0xac/4],
		0x100, posY + window.h, FONT_SMALL, (JUSTIFY_CENTER | ORANGE));
		
	// add for each line
	window.h += 8;
	
	// add 3 pixels above, 3 pixels bellow
	window.h += 6;
		
	DECOMP_MENUBOX_DrawInnerRect(&window, 1, gGT->backBuffer->otMem.startPlusFour);
}

void DrawFinalLap(struct GameTracker* gGT)
{
	int i;
	int textTimer;
	struct TileView* tileView;
	
	int startX;
	int endX;
	int posY;
	
	short resultPos[2];
	
	// number of players
	for(i = 0; i < 4; i++)
	{		
		// time remaining in animation
		textTimer = sdata->finalLapTextTimer[i];
		
		// skip if not drawing "FINAL LAP"
		if(textTimer == 0)
			continue;
		
		#ifdef USE_60FPS
		// if crossed finish line less than 1 second ago
		if((gGT->elapsedEventTime - gGT->drivers[i]->lapTime) < 960)
			
			// if StartLine.c initialized this to 90 (which we can't change yet),
			// for more info, CTRL + F and search: (&DAT_8008d2a0)[iVar10] = 0x5a;
			if(sdata->finalLapTextTimer[i] == 90)
				
				// change to 180, for 3 seconds at 60fps
				sdata->finalLapTextTimer[i] = 180;
				
		// update register with changed value
		textTimer = sdata->finalLapTextTimer[i];
		#endif
		
		// turn "time remaining" into "time elapsed",
		// 90 frames total in animation, 1.5 seconds
		textTimer = FPS_DOUBLE(90) - textTimer;
		
		// camera
		tileView = &gGT->tileView[i];
		
		// << 0x10, >> 0x12
		posY = tileView->rect.h / 4;
		
		// fly from right to center
		if(textTimer <= FPS_DOUBLE(10))
		{
			startX = tileView->rect.w + 100;
			endX = tileView->rect.w / 2;

			goto DrawFinalLapString;
		}
		
		// sit in center
		if(textTimer <= FPS_DOUBLE(0x50))
		{
			startX = tileView->rect.w / 2;
			endX = startX;
			
			// for duration
			textTimer -= FPS_DOUBLE(10);

			goto DrawFinalLapString;
		}

		// fly from center to left
		startX = tileView->rect.w / 2;
		endX = -100;
		textTimer -= FPS_DOUBLE(0x50);
		
DrawFinalLapString:

		UI_Lerp2D_Linear(&resultPos, startX, posY, endX, posY, textTimer, FPS_DOUBLE(10));

		// need to specify OT, or else "FINAL LAP" will draw on top of character icons,
		// and by doing this, "FINAL LAP" draws under the character icons instead
		DecalFont_DrawLineOT(
			sdata->lngStrings[0x8cc/4],
			resultPos[0], resultPos[1],
			FONT_BIG, (JUSTIFY_CENTER | ORANGE),
			tileView->ptrOT);
			
		sdata->finalLapTextTimer[i]--;
	}
}

void RainLogic(struct GameTracker* gGT)
{
	int i;
	struct QuadBlock* camQB;
	int numPlyrCurrGame;
	
	numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	for(i = 0; i < numPlyrCurrGame; i++)
	{
		TileView_UpdateFrustum(&gGT->tileView[i]);
		
		camQB = gGT->cameraDC[i].ptrQuadBlock;
		
		// skip if camera isn't over quadblock
		if(camQB == 0) continue;
		
		// assume numPlayers is never zero,
		// assume weather_intensity is always valid
		
		gGT->rainBuffer[i].numParticles_max =
			(camQB->weather_intensity << 2) / numPlyrCurrGame;
			
		gGT->rainBuffer[i].vanishRate =
			(camQB->weather_vanishRate << 2) / numPlyrCurrGame;
	}
}
#endif

void MenuHighlight()
{
	int fc;
	int trig;
	
	fc = FPS_HALF(sdata->frameCounter) << 7;
	trig = DECOMP_MATH_Sin(fc);
	
	trig = (trig << 6) >> 0xc;
	
	// sine curve of green, plus base color
	sdata->menuRowHighlight_Normal = ((trig + 0x40) * 0x100) | 0x80;
	sdata->menuRowHighlight_Green = ((trig + 0xA0) * 0x100) | 0x400040;
}

#ifndef REBUILD_PS1
void RenderAllWeather(struct GameTracker* gGT)
{
	int numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	// only for single player, 
	// probably Naughty Dog's last-minute hack
	if(numPlyrCurrGame != 1) return;
	
	// only if rain is enabled
	if((gGT->renderFlags & 2) == 0) return;
	
	RenderWeather(
		&gGT->tileView[0],
		&gGT->backBuffer->primMem,
		&gGT->rainBuffer[0],
		numPlyrCurrGame,
		gGT->gameMode1 & PAUSE_ALL);
}

void RenderAllConfetti(struct GameTracker* gGT)
{
	int i;
	int numWinners = gGT->numWinners;
	
	// only if someone needs confetti
	if(numWinners == 0) return;
	
	// only if confetti is enabled
	if((gGT->renderFlags & 4) == 0) return;
	
	for(i = 0; i < numWinners; i++)
	{
		DrawConfetti(
			&gGT->tileView[gGT->winnerIndex[i]],
			&gGT->backBuffer->primMem,
			&gGT->confetti,
			gGT->frameTimer_Confetti,
			gGT->gameMode1 & PAUSE_ALL);
	}
}

void RenderAllStars(struct GameTracker* gGT)
{
	// only if stars are enabled
	if((gGT->renderFlags & 8) == 0) return;

	// quit if no stars exist
	if(gGT->stars.numStars == 0) return;

	RenderStars(
		&gGT->tileView[0],
		&gGT->backBuffer->primMem,
		&gGT->stars,
		gGT->numPlyrCurrGame);
}

void RenderAllHUD(struct GameTracker* gGT)
{
	int hudFlags;
	int gameMode1;
	
	hudFlags = gGT->hudFlags;
	gameMode1 = gGT->gameMode1;
	
	// if not drawing intro-race title bars
	if(
		(gGT->numPlyrCurrGame != 1) ||
		((hudFlags & 8) == 0) ||
		((gameMode1 & START_OF_RACE) == 0)
	)
	{
		// if no hud
		if((hudFlags & 1) == 0)
		{
			// if standings
			if((hudFlags & 4) != 0)
			{
				CupStandings_InputAndDraw();
			}
		}
		
		// if hud
		else
		{
			// if not adv hub
			if((gameMode1 & ADVENTURE_ARENA) == 0)
			{
				// if not drawing end of race
				if(
					// end of race is not reached
					((gameMode1 & END_OF_RACE) == 0) ||
					
					// cooldown after end of race not expired
					(gGT->timerEndOfRaceVS != 0)
				)
				{
					// not crystal challenge
					if((gameMode1 & CRYSTAL_CHALLENGE) == 0)
					{
						DECOMP_UI_RenderFrame_Racing();
					}
					
					// if crystal challenge
					else
					{
						DECOMP_UI_RenderFrame_CrystChall();
					}
				}
				
				// drawing end of race
				else
				{
					if(
						// VS mode, and Cup
						((gGT->gameMode1 & ARCADE_MODE) == 0) && 
						((gGT->gameMode2 & CUP_ANY_KIND) != 0)
					  )
					{
						// disable drawing hud,
						// enable drawing "standings"
						gGT->hudFlags = (hudFlags & 0xfe) | 4;
						return;
					}
					
					// temporary, until we rewrite MainGameEnd_Initialize
					if((gGT->gameMode1 & RELIC_RACE) == 0)
					{
						// all 221-225 overlays share the same
						// function address, so call as one func
						void OVR_Region1();
						OVR_Region1();
					}
					
					// except relic, until we rewrite MainGameEnd_Initialize
					else
					{
						void DECOMP_RR_EndEvent_DrawMenu();
						DECOMP_RR_EndEvent_DrawMenu();
					}
					
					return;
				}
			}
			
			// if adv hub
			else
			{
				// load on last frame of waiting to load 232,
				// leave transition at 1 (see later in func),
				// and load the 232 overlay
				if(gGT->overlayTransition > 1)
				{
					gGT->overlayTransition--;
					if(gGT->overlayTransition == 1)
						LOAD_OvrThreads(2);
				}
				
				// if 233 is still loaded
				if(LOAD_IsOpen_AdvHub() == 0)
				{
					// if any transition is over
					if(gGT->tileView_UI.fadeFromBlack_currentValue > 0xfff)
					{
						DECOMP_UI_RenderFrame_AdvHub();
					}
				}
				
				// if 232 overlay is loaded
				else
				{
					// if any transition is over
					if(gGT->tileView_UI.fadeFromBlack_currentValue > 0xfff)
					{
						AH_Map_Main();
						
						if(sdata->AkuHint_RequestedHint != -1)
						{
							AH_MaskHint_Start(
								sdata->AkuHint_RequestedHint,
								sdata->AkuHint_boolInterruptWarppad
							);
							
							//erase submitted request
							sdata->AkuHint_RequestedHint = -1;
							sdata->AkuHint_boolInterruptWarppad = 0;
						}
					}
					
					// if first frame of transition to 232
					if(gGT->overlayTransition != 0)
					{
						gGT->overlayTransition = 0;
						
						INSTANCE_LevDelayedLInBs(
							gGT->level1->ptrInstDefs,
							gGT->level1->numInstances);
							
						// allow instances again
						gGT->gameMode2 &= ~(DISABLE_LEV_INSTANCE);

						// fade transition
						gGT->tileView_UI.fadeFromBlack_desiredResult = 0x1000;
						gGT->tileView_UI.fade_step = 0x2aa;
					}
				}
			}
		}
	}
	
	// if drawing intro-race title bars
	else
	{
		UI_RaceStart_IntroText1P();
	}
}

void RenderAllBeakerRain(struct GameTracker* gGT)
{
	int numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	// only for 1P/2P
	if(numPlyrCurrGame > 2) return;
	
	// only if beaker rain is enabled
	if((gGT->renderFlags & 0x10) == 0) return;
	
	RedBeaker_RenderRain(
		&gGT->tileView[0],
		&gGT->backBuffer->primMem,
		&gGT->JitPools.rain,
		numPlyrCurrGame,
		gGT->gameMode1 & PAUSE_ALL);
}

void RenderAllBoxSceneSplitLines(struct GameTracker* gGT)
{
	// check 233 overlay, cause levelID is set
	// and MainFrame_RenderFrame runs, before 233 loads
	if(LOAD_IsOpen_Podiums() != 0)
	{
		// ND Box Scene
		if(gGT->levelID == NAUGHTY_DOG_CRATE)
		{
			CS_BoxScene_InstanceSplitLines();
		}
	}
}

void RenderBucket_QueueAllInstances(struct GameTracker* gGT)
{
	int lod;
	int* RBI;
	int numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	if((gGT->renderFlags & 0x20) == 0) return;

	lod = numPlyrCurrGame - 1;
	if((gGT->gameMode1 & RELIC_RACE) != 0)
		lod |= 4;
	
	RBI = RenderBucket_QueueLevInstances(
		&gGT->cameraDC[0],
		&gGT->backBuffer->otMem,
		gGT->ptrRenderBucketInstance,
		sdata->LOD[lod],
		numPlyrCurrGame,
		gGT->gameMode1 & PAUSE_ALL);
		
	RBI = RenderBucket_QueueNonLevInstances(
		gGT->JitPools.instance.taken.first,
		&gGT->backBuffer->otMem,
		RBI,
		sdata->LOD[lod],
		numPlyrCurrGame,
		gGT->gameMode1 & PAUSE_ALL);
		
	// Aug prototype
#if 0
		// ptrEnd of otmem is less than ptrCurr otmem
    if (*(uint *)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x98) <
        *(uint *)(*(int *)(PTR_DAT_8008d2ac + 0x10) + 0x9c)) {
      printf("OTMEM OVERFLOW!\n");
    }
#endif

	// null terminator at end of list
	*RBI = 0;
}

void RenderAllNormalParticles(struct GameTracker* gGT)
{
	int i;
	
	if((gGT->renderFlags & 0x200) == 0) return;
	
	for(i = 0; i < gGT->numPlyrCurrGame; i++)
	{
		Particle_RenderList(
			&gGT->tileView[i],
			gGT->particleList_ordinary);
	}
}
#endif

void RenderDispEnv_World(struct GameTracker* gGT)
{
	int i;
	struct TileView* tileView;
	for(i = 0; i < gGT->numPlyrCurrGame; i++)
	{
		tileView = &gGT->tileView[i];
		DECOMP_TileView_SetDrawEnv_Normal(
			&tileView->ptrOT[0x3ff],
			tileView, gGT->backBuffer, 0, 0);
	}
}

#ifndef REBUILD_PS1
// I need a better name
void RenderAllFlag0x40(struct GameTracker* gGT)
{
	int i;
	struct TileView* tileView;
	
	if((gGT->renderFlags & 0x40) == 0) return;
	
	if(LOAD_IsOpen_RacingOrBattle() != 0)
	{
		RB_Player_ToggleInvisible();
		RB_Player_ToggleFlicker();
		RB_Burst_ProcessBucket(gGT->threadBuckets[BURST].thread);
		RB_Blowup_ProcessBucket(gGT->threadBuckets[BLOWUP].thread);
		RB_Spider_DrawWebs(gGT->threadBuckets[SPIDER].thread, &gGT->tileView[0]);
		RB_Follower_ProcessBucket(gGT->threadBuckets[FOLLOWER].thread);
		
		// skipping RB_StartText_ProcessBucket, it's empty in 231
	}
	
	if(LOAD_IsOpen_AdvHub() != 0)
	{
		if((gGT->gameMode1 & ADVENTURE_ARENA) != 0)
		{
			DECOMP_AH_WarpPad_AllWarppadNum();
		}
	}
	
	Turbo_ProcessBucket(gGT->threadBuckets[TURBO].thread);

	for(i = 0; i < gGT->numPlyrCurrGame; i++)
	{
		tileView = &gGT->tileView[i];
		DrawSkidMarks_Main(gGT->threadBuckets[PLAYER].thread, tileView);
		DrawSkidMarks_Main(gGT->threadBuckets[ROBOT].thread, tileView);
	}
}

void RenderAllTitleDPP(struct GameTracker* gGT)
{
	if((gGT->gameMode1 & MAIN_MENU) == 0) return;
	if(LOAD_IsOpen_MainMenu() == 0) return;
	DECOMP_MM_Title_SetTrophyDPP();
}

void RenderBucket_ExecuteAllInstances(struct GameTracker* gGT)
{
	if((gGT->renderFlags & 0x20) == 0) return;
	
	RenderBucket_Execute(
		gGT->ptrRenderBucketInstance,
		&gGT->backBuffer->primMem);
}

void RenderAllTires(struct GameTracker* gGT)
{
	int i;
	struct Thread* th;
	int numPlyrCurrGame;
	struct PrimMem* gGT_primMem;
	if((gGT->renderFlags & 0x80) == 0) return;

	// replace checking number of AIs, with 
	// checking if the threadBucket exists,
	// then roll this up into a loop
	
	gGT_primMem = &gGT->backBuffer->primMem;
	numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	// player, robot, ghost
	for(i = 0; i < 3; i++)
	{
		th = gGT->threadBuckets[i].thread;
		if(th == 0) continue;
		
		DrawTires_Solid(th, gGT_primMem, numPlyrCurrGame);
		DrawTires_Reflection(th, gGT_primMem, numPlyrCurrGame);
	}
}

void RenderAllShadows(struct GameTracker* gGT)
{
	if((gGT->renderFlags & 0x200) == 0) return;
	DrawShadows_Main();
}

void RenderAllHeatParticles(struct GameTracker* gGT)
{
	if((gGT->renderFlags & 0x800) == 0) return;
	
	Torch_Main(
		gGT->particleList_heatWarp,
		&gGT->tileView[0],
		&gGT->backBuffer->primMem,
		gGT->numPlyrCurrGame,
		gGT->swapchainIndex * 0x128);
}

void RenderAllLevelGeometry(struct GameTracker* gGT)
{
	int i;
	int distToScreen;
	int numPlyrCurrGame;
	struct Level* level1;
	struct TileView* tileView;
	struct mesh_info* ptr_mesh_info;
	
	level1 = gGT->level1;
	if(level1 == 0) return;
	
	ptr_mesh_info = level1->ptr_mesh_info;
	if(ptr_mesh_info == 0) return;
	
	numPlyrCurrGame = gGT->numPlyrCurrGame;
	
	if(numPlyrCurrGame == 1)
	{	
		CTR_ClearRenderLists_1P2P(gGT, 1);
		
		// === Temporary 60FPS macros ===
		// Emulate 30fps on 60fps for SCVert and OVert
		
		// if no SCVert
		if((level1->configFlags & 4) == 0)
		{
			// assume OVert (no primitives generated here)
			AnimateWater1P(FPS_HALF(gGT->timer), level1->numWaterVertices,
				level1->ptr_water, level1->ptr_tex_waterEnvMap,
				gGT->visMem1->visOVertList[0]);
		}
		
		// if SCVert
		else
		{
			// draw SCVert (no primitives generated here
			AnimateQuad(gGT->timer << FPS_LEFTSHIFT(7), 
				level1->numSCVert, level1->ptrSCVert,
				gGT->visMem1->visSCVertList[0]);
		}
		
		// camera of player 1
		tileView = &gGT->tileView[0];
		
		if(
			// adv character selection screen
			(gGT->levelID == ADVENTURE_CHARACTER_SELECT) ||
			
			// cutscene that's not Crash Bandicoot intro
			// where he's sleeping and snoring on a hill
			(
				((gGT->gameMode1 & GAME_CUTSCENE) != 0) &&
				(gGT->levelID != INTRO_CRASH)
			)
		)
		{	
			// relationship between near-clip and far-clip,
			// for each RenderList LOD set in the level
			*(int*)0x1f800014 = 0x1e00;
			*(int*)0x1f800018 = 0x640;
			*(int*)0x1f80001c = 0x640;
			*(int*)0x1f800020 = 0x500;
			*(int*)0x1f800024 = 0x280;
			*(int*)0x1f800028 = 0x140;
			*(int*)0x1f80002c = 0x640+0x140;
		}
		
		// every non-cutscene,
		// except for Crash Bandicoot intro
		else
		{
			// 0x1c2 in 1P mode
			distToScreen = tileView->distanceToScreen_PREV;
			
			// int and unsigned int have specific purposes
			*(unsigned int*)0x1f800014 = distToScreen * 0x2080;
			if(*(int*)0x1f800014 < 0) *(int*)0x1f800014 = *(int*)0x1f800014 + 0xff;
			*(int*)0x1f800014 = *(int*)0x1f800014 >> 8; // 0x3921
			
			*(int*)0x1f800018 = distToScreen * 0x1a;	// 0x2DB4
			*(int*)0x1f80001c = distToScreen * 0x18;	// 0x2A30
			*(int*)0x1f800020 = distToScreen * 0xc;		// 0x1518
			*(int*)0x1f800024 = distToScreen * 7;		// 0xC4E
			*(int*)0x1f80002c = *(int*)0x1f800018 + 0x140; // 0x2EF4
			
			// int and unsigned int have specific purposes
			*(unsigned int*)0x1f800028 = distToScreen * 0x380;
			if(*(int*)0x1f800028 < 0) *(int*)0x1f800028 = *(int*)0x1f800028 + 0xff;
			*(int*)0x1f800028 = *(int*)0x1f800028 >> 8; // 0x627
		}
		
		RenderLists_PreInit();
		gGT->bspLeafsDrawn = 0;
		
		gGT->bspLeafsDrawn += 
		  RenderLists_Init1P2P(
			ptr_mesh_info->bspRoot,
			level1->visMem->visLeafList[0],
			tileView,
			&gGT->LevRenderLists[0],
			level1->visMem->bspList[0],
			gGT->numPlyrCurrGame);
			
		// 226-229
		DrawLevelOvr1P(
			&gGT->LevRenderLists[0],
			tileView,
			ptr_mesh_info,
			&gGT->backBuffer->primMem,
			gGT->visMem1->visFaceList[0],
			level1->ptr_tex_waterEnvMap); // waterEnvMap?
			
		DrawSky_Full(
			level1->ptr_skybox,
			tileView,
			&gGT->backBuffer->primMem);
			
		// skybox gradient
		if((level1->configFlags & 1) != 0)
		{
			goto SkyboxGlow;
		}
		
		return;
	}
	
	if(numPlyrCurrGame == 2)
	{
		CTR_ClearRenderLists_1P2P(gGT, 2);
		
		// if no SCVert
		if((level1->configFlags & 4) == 0)
		{
			// assume OVert (no primitives generated here)
			AnimateWater2P(gGT->timer, level1->numWaterVertices,
				level1->ptr_water, level1->ptr_tex_waterEnvMap,
				gGT->visMem1->visOVertList[0],
				gGT->visMem1->visOVertList[1]);
		}
		
		RenderLists_PreInit();
		gGT->bspLeafsDrawn = 0;
		
		for(i = 0; i < numPlyrCurrGame; i++)
		{
			gGT->bspLeafsDrawn += 
			  RenderLists_Init1P2P(
				ptr_mesh_info->bspRoot,
				level1->visMem->visLeafList[i],
				&gGT->tileView[i],
				&gGT->LevRenderLists[i],
				level1->visMem->bspList[i],
				gGT->numPlyrCurrGame);
		}
		
		// 226-229
		DrawLevelOvr2P(
			&gGT->LevRenderLists[0],
			&gGT->tileView[0],
			ptr_mesh_info,
			&gGT->backBuffer->primMem,
			gGT->visMem1->visFaceList[0],
			gGT->visMem1->visFaceList[1],
			level1->ptr_tex_waterEnvMap); // waterEnvMap?
			
		goto SkyboxGlow;
	}
	
	// 3P or 4P
	CTR_ClearRenderLists_3P4P(gGT, numPlyrCurrGame);
	
	// if no SCVert
	if((level1->configFlags & 4) == 0)
	{
		if(numPlyrCurrGame == 3)
		{
			// assume OVert (no primitives generated here)
			AnimateWater3P(gGT->timer, level1->numWaterVertices,
				level1->ptr_water, level1->ptr_tex_waterEnvMap,
				gGT->visMem1->visOVertList[0],
				gGT->visMem1->visOVertList[1],
				gGT->visMem1->visOVertList[2]);
		}
		
		else // 4P mode
		{
			// assume OVert (no primitives generated here)
			AnimateWater4P(gGT->timer, level1->numWaterVertices,
				level1->ptr_water, level1->ptr_tex_waterEnvMap,
				gGT->visMem1->visOVertList[0],
				gGT->visMem1->visOVertList[1],
				gGT->visMem1->visOVertList[2],
				gGT->visMem1->visOVertList[3]);
		}
	}
	
	RenderLists_PreInit();
	gGT->bspLeafsDrawn = 0;
	
	for(i = 0; i < numPlyrCurrGame; i++)
	{
		gGT->bspLeafsDrawn += 
		  RenderLists_Init3P4P(
			ptr_mesh_info->bspRoot,
			level1->visMem->visLeafList[i],
			&gGT->tileView[i],
			&gGT->LevRenderLists[i],
			level1->visMem->bspList[i]);
	}
	
	if(numPlyrCurrGame == 3)
	{
		// 226-229
		DrawLevelOvr3P(
			&gGT->LevRenderLists[0],
			&gGT->tileView[0],
			ptr_mesh_info,
			&gGT->backBuffer->primMem,
			gGT->visMem1->visFaceList[0],
			gGT->visMem1->visFaceList[1],
			gGT->visMem1->visFaceList[2],
			level1->ptr_tex_waterEnvMap); // waterEnvMap?
	}
	
	else // 4P mode
	{
		// 226-229
		DrawLevelOvr4P(
			&gGT->LevRenderLists[0],
			&gGT->tileView[0],
			ptr_mesh_info,
			&gGT->backBuffer->primMem,
			gGT->visMem1->visFaceList[0],
			gGT->visMem1->visFaceList[1],
			gGT->visMem1->visFaceList[2],
			gGT->visMem1->visFaceList[3],
			level1->ptr_tex_waterEnvMap); // waterEnvMap?
	}
		
SkyboxGlow:
		
	// skybox gradient
	for(i = 0; i < numPlyrCurrGame; i++)
	{
		tileView = &gGT->tileView[i];
		CAM_SkyboxGlow(
			&level1->glowGradient[0],
			tileView,
			&gGT->backBuffer->primMem,
			&tileView->ptrOT[0x3ff]);
	}
	
	return;
}
#endif

void MultiplayerWumpaHUD(struct GameTracker* gGT)
{
	if((gGT->hudFlags & 1) == 0) return;
	if(gGT->numPlyrCurrGame < 2) return;
	
#ifndef REBUILD_PS1
	UI_RenderFrame_Wumpa3D_2P3P4P(gGT);
#endif
}

void WindowBoxLines(struct GameTracker* gGT)
{
	int i;
	
	// only battle and 3P4P mode allowed
	if((gGT->gameMode1 & BATTLE_MODE) == 0) return;
	if(gGT->numPlyrCurrGame < 3) return;
	
	for(i = 0; i < gGT->numPlyrCurrGame; i++)
	{
		DECOMP_MENUBOX_DrawOuterRect_LowLevel(

			// dimensions, thickness
			&gGT->tileView[i].rect,4,2,

			// color data
			data.ptrColor[gGT->drivers[i]->BattleHUD.teamID + PLAYER_BLUE],

			0,

			// tileView_UI = 0x1388
			&gGT->tileView_UI.ptrOT[3]);
	}
}

void WindowDivsionLines(struct GameTracker* gGT)
{
	POLY_F4* p;
	int numPlyrCurrGame;

	numPlyrCurrGame = gGT->numPlyrCurrGame;

	// horizontal bar
#ifdef USE_NEW2P
	if (numPlyrCurrGame > 2)
#else
    if (numPlyrCurrGame > 1)
#endif
	{		
		p = gGT->backBuffer->primMem.curr;

		// set R, G, B, CODE, all to zero,
		// this makes black color, and invalid CODE
		*(int*)&p->r0 = 0;

		// this sets CODE to the proper value
		setPolyF4(p);

		// Make four (x,y) coordinates
		p->y0 = 0x6a;
		p->y1 = 0x6a;
		p->x0 = 0;
		p->x1 = 0x200;

		p->x2 = 0;
		p->y2 = 0x6e;
		p->x3 = 0x200;
		p->y3 = 0x6e;

		// Draw a bar from left to right,
		// dividing the screen in half on top and bottom
		AddPrim(&gGT->tileView_UI.ptrOT[3],p);

		gGT->backBuffer->primMem.curr = (void*)(p + 1);
    }

	// vertical bar
#ifdef USE_NEW2P
	if (numPlyrCurrGame > 1)
#else
    if (numPlyrCurrGame > 2)
#endif
	{
		// test for USE_NEW2P
		#if 0
		gGT->drivers[0]->numWumpas = 10;
		gGT->drivers[0]->heldItemID = 3;
		gGT->drivers[1]->numWumpas = 10;
		gGT->drivers[1]->heldItemID = 3;
		#endif
		
		p = gGT->backBuffer->primMem.curr;

		// set R, G, B, CODE, all to zero,
		// this makes black color, and invalid CODE
		*(int*)&p->r0 = 0;

		// this sets CODE to the proper value
		setPolyF4(p);

		// Make four (x,y) coordinates
		p->x0 = 0xfd;
		p->x2 = 0xfd;
		p->y0 = 0;
		p->x1 = 0x103;

		p->y1 = 0;
		p->y2 = 0xd8;
		p->x3 = 0x103;
		p->y3 = 0xd8;

		// Draw a bar from left to right,
		// dividing the screen in half on top and bottom
		AddPrim(&gGT->tileView_UI.ptrOT[3],p);

		// backBuffer->primMem.curr
		gGT->backBuffer->primMem.curr = (void*)(p + 1);
    }

	// if numPlyrCurrGame is 3
    if (numPlyrCurrGame == '\x03')
	{
		// This is useless, cause it gets cleared
		// to black anyway, even without this block,
		// at least it does it Crash Cove, does it always?

		p = gGT->backBuffer->primMem.curr;

		// set R, G, B, CODE, all to zero,
		// this makes black color, and invalid CODE
		*(int*)&p->r0 = 0;

		// this sets CODE to the proper value
		setPolyF4(p);

		// xy0
		p->x0 = 0x100;
		p->x2 = 0x100;
		p->y0 = 0x6c;
		p->y1 = 0x6c;
		p->x1 = 0x200;
		p->y2 = 0xd8;
		p->x3 = 0x200;
		p->y3 = 0xd8;

		// Draw a bar from left to right,
		// dividing the screen in half on top and bottom
		AddPrim(&gGT->tileView_UI.ptrOT[3],p);

		// backBuffer->primMem.curr
		gGT->backBuffer->primMem.curr = (void*)(p + 1);
    }
}

void RenderDispEnv_UI(struct GameTracker* gGT)
{
	struct TileView* tileView = &gGT->tileView_UI;
	
	DECOMP_TileView_SetDrawEnv_Normal(
		&tileView->ptrOT[4],
		tileView, gGT->backBuffer, 0, 0);
}

__attribute__((optimize("O0")))
int ReadyToFlip(struct GameTracker* gGT)
{
	return
	
			// if DrawOTag finished
			(gGT->bool_DrawOTag_InProgress == 0) &&
			
			// two VSYNCs passed, 30fps lock
			(sdata->vsyncTillFlip < 1);
}

__attribute__((optimize("O0")))
int ReadyToBreak(struct GameTracker* gGT)
{
	return
	
		// if more than 6 VSYNCs passed since
		// the last successful draw, FPS < 10fps
		gGT->vSync_between_drawSync > 6;
}

void RenderVSYNC(struct GameTracker* gGT)
{	
	// render checkered flag
	if((gGT->renderFlags & 0x1000) != 0)
	{
		VSync(0);
	}

	while(1)
	{

#ifdef REBUILD_PC
		// must be called in the loop,
		// or else it wont properly sync
		DrawSync(0);
#endif
		
		if(ReadyToFlip(gGT))
		{
			// quit, end of stall
			return;
		}
		
#ifndef REBUILD_PC
		if(ReadyToBreak(gGT))
		{
			// just quit and try the next frame
			BreakDraw();
			return;
		}
#endif
	}
}

#ifndef REBUILD_PS1
void RenderFMV()
{
	if(sdata->boolPlayVideoSTR == 1)
	{
		MM_Video_CheckIfFinished(1);
		
		MoveImage(
			&sdata->videoSTR_src_vramRect,
			sdata->videoSTR_dst_vramX, 
			sdata->videoSTR_dst_vramY);
			
		DrawSync(0);
	}
}
#endif

void RenderSubmit(struct GameTracker* gGT)
{
	// 1 VSYNC = 60fps
	// 2 VSYNCs = 30fps
	
#ifdef REBUILD_PC
	
	sdata->vsyncTillFlip = FPS_HALF(2);
#else
	
	// do I need the "if"? will it ever be nullptr?
	if(gGT->frontBuffer != 0)
	{
		sdata->vsyncTillFlip = FPS_HALF(2);
		
		// skip debug stuff
		
		PutDispEnv(&gGT->frontBuffer->dispEnv);
		PutDrawEnv(&gGT->frontBuffer->drawEnv);
	}
	
	// swap=0, get db[1]
	// swap=1, get db[0]
	gGT->frontBuffer = &gGT->db[1 - gGT->swapchainIndex];
#endif
		
	gGT->bool_DrawOTag_InProgress = 1;
	
	DrawOTag(&gGT->tileView[0].ptrOT[0x3ff]);
	
	gGT->frameTimer_notPaused = gGT->frameTimer_VsyncCallback;
}