#include <common.h>

void OnAnimate_LastSpin(struct Thread *t, struct Driver* d)
{
    short sVar1;
    u_short interp;
    int numFrames;
    int startFrame;
    struct Instance *inst;

    // get instance from thread
    inst = t->inst;

    // if animType is just driving
    if (inst->animIndex == 0)
    {
        // get number of frames in animation
        numFrames = VehAnim_Instance_GetNumAnimFrames(inst, 0);

        // if numFrames not zero
        if (numFrames)
        {
            sVar1 = d->turnAngleCurr;

            // get animation frame
            startFrame = inst->animFrame;

            if (0 < sVar1)
            {
                if (d->unk_LerpToForwards < 0)
                {
                    // last frame
                    startFrame = numFrames - 1;
                }
            }
            else if (0 < d->unk_LerpToForwards)
            {
                // start from beginning
                startFrame = 0;
            }

            // Interpolate animation frame by speed
            interp = InterpBySpeed(inst->animFrame, 3, startFrame);

            // set animation frame
            inst->animFrame = interp;
        }
        return;
    }
    OnAnimate_Spinning(t,d);
}
