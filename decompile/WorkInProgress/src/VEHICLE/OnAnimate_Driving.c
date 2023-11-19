#include <common.h>

void OnAnimate_Driving(struct Thread *t, struct Driver *d)
{
    int iVar7;
    int iVar9;
    u_short interp;
    int numFrames;
    int startFrame;
    char currAnimIndex;
    char currFrame;

    u_char animType = 0;
    struct GameTracker *gGT = sdata->gGT;
    struct Instance *inst = t->inst;

    if (d->instTntRecv == 0 && d->kartState != 10)
    {
        if (d->fireSpeed < 0)
        {
            // check if you are driving backwards
            // (0) forwards
            // (1) backwards
            animType = (d->baseSpeed < 1);
        }
        // if player height is far from quadblock height
        if ((0x600 < d->unknownDimensionCurr || inst->animIndex == 3) && (0x8000 < d->posCurr[1] - d->quadBlockHeight))
        {
            // jumping animation
            animType = 3;
        }
    }

    currAnimIndex = inst->animIndex;

    numFrames = VehAnim_Instance_GetNumAnimFrames(inst, currAnimIndex);

    if (numFrames <= 0) return;    

    // if animation changes
    if (animType != currAnimIndex)
    {
        // crashing
        if (currAnimIndex == 2)
        {
            /// start on the last frame
            startFrame = VehAnim_Instance_GetNumAnimFrames(inst,0) - 1;
        }
        else
        {
            // starting frame
            startFrame = VehAnim_Instance_GetStartFrame(currAnimIndex, numFrames);
        }

        // frame not aligned
        if (inst->animFrame != startFrame)
        {
            u_int animSpeed = 2;
            // if steering
            if (currAnimIndex == 0)
            {
                animSpeed = 6;
            }
            // crashing
            else if (currAnimIndex == 2)
            {
                animSpeed = 1;
                // kart emote frame
                d->matrixIndex = *(char *)&inst->animFrame;
            }

            // curr frame, animation moving speed per frame, starting frame
            interp = InterpBySpeed(inst->animFrame, animSpeed, startFrame);
            inst->animFrame = (short)interp;

            // if not quite at the start
            if ((currAnimIndex - 2) > 1) return;

            currFrame = *(char *)&inst->animFrame;
            // kart animation frame
            d->matrixIndex = currAnimIndex;
            
            // kart animation Index
            if (currAnimIndex == 0)
                d->matrixArray = 0;

            return;
        }

        numFrames = VehAnim_Instance_GetNumAnimFrames(inst, animType);

        if (numFrames <= 0) return;
        
        // set animation
        inst->animIndex = (char)animType;
        // starting frame
        inst->animFrame = VehAnim_Instance_GetStartFrame(animType, numFrames);
        // init kart anim
        d->matrixArray = 0;
        d->matrixIndex = 0;
    }
    else
    {   // just steering
        if (animType == 0)
        {
            // half number of frames
            iVar9 = numFrames >> 1;
            // if you don't have a TNT over you
            if (d->instTntRecv == 0)
            {
                short burnTimer = d->burnTimer;
                // not currently burned or just start burning
                if ((burnTimer == 0) || (479 < burnTimer))
                {   
                    // negative turning stat while braking
                    iVar9 = -0x40;
                    // if you're not in accel prevention
                    if ((d->actionsFlagSet & 8) == 0)
                    {
                        iVar7 = d->simpTurnState;
                        // negative character's turn stat
                        iVar9 = -(d->const_TurnRate);
                    }
                    else
                    {
                        // positive turning stat while braking
                        animType = 0x40;
                        iVar7 = d->ampTurnState;
                    }
                    // seems like iVar9 gets set to 0 if you're turning, or to last frame index if you're not
                    iVar9 = MapToRange(-(iVar7), iVar9, animType, 0, (numFrames - 1));
                }
                else
                {
                    iVar9 = (((burnTimer >> 5) % 5) >> 2) - 8 + iVar9;
                    
                    inst->animFrame = (short)iVar9;
                    struct Particle *p = Particle_CreateInstance(0, gGT->iconGroup[1], &data.emSet_BurnSmoke[0]);
                    if (p != NULL)
                    {
                        p->unk18 = d->instSelf->unk50;
                        p->driverInst = d->instSelf;
                        p->unk19 = d->driverID;
                    }
                }
            }
        }
        else
        {   
            // jump animation
            if (animType == 3)
            {
                interp = InterpBySpeed(inst->animFrame, 1, numFrames - 1);
                inst->animFrame = interp;

                // if you're getting mask grabbed
                if (d->kartState == 5) return;                

                char charID = data.characterIDs[d->driverID];

                switch (charID)
                {
                // if this is Penta
                case 13:
                    charID = 10; // Use Coco's
                    break;
                // if this is Fake Crash or Oxide
                case 14:
                case 15:
                    charID = 7; // Use Crash's
                    break;
                default:
                    charID += 7;
                }
                // set AnimType based on charID
                d->matrixArray = charID;
                // set animation frame
                d->matrixIndex = *(char *)&inst->animFrame;
                return;
            }
            // last frame
            iVar9 = numFrames - 1;
        }
        interp = InterpBySpeed(inst->animFrame, 1, iVar9);
        inst->animFrame = interp;
    }
}
