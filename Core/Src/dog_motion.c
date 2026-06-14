#include "dog_motion.h"

#include <stdint.h>
#include "OLED.h"
#include "servo.h"
#include "face_data.h"
#include "face_anim.h"

#define DOG_FACE_X            32
#define DOG_FACE_Y             0
#define DOG_FRAME_INTERVAL_MS 180U
#define DOG_FORWARD_SHIFT_MS   120U
#define DOG_FORWARD_LIFT_MS    120U
#define DOG_FORWARD_CENTER_MS   90U
#define DOG_FORWARD_SETTLE_MS  120U
#define DOG_FORWARD_END_MS     120U
#define DOG_ACTION_STEP_MS     120U
#define DOG_ACTION_CENTER_MS    90U
#define DOG_BACKWARD_BIAS_US    30U
#define DOG_SERVO_60          1200U
#define DOG_SERVO_90          1500U
#define DOG_SERVO_120         1800U

typedef struct
{
    uint16_t pulseUs[SERVO_COUNT];
    const uint8_t *face;
    uint32_t durationMs;
} DogMotionFrame_t;

typedef struct
{
    const DogMotionFrame_t *frames;
    uint8_t frameCount;
    uint8_t loopEnabled;
} DogMotionSequence_t;

#define DOG_POSE(rf, rr, lf, lr, tail, facePtr, ms) \
    { { (rf), (rr), (lf), (lr), (tail) }, (facePtr), (ms) }

static DogState_t s_currentState = DOG_STATE_IDLE;
static DogState_t s_activeState = DOG_STATE_IDLE;
static uint8_t s_activeFrame = 0U;
static uint32_t s_nextFrameTick = 0U;

static const DogMotionFrame_t s_idleFrames[] =
{
    /*
     * Idle / standby:
     * keep the front legs unchanged and push both rear legs backward by the
     * same amount.
     * Keep it as a single stable hold frame for easy debugging.
     */
    DOG_POSE(1500U, 2400U, 1500U, 2400U, 1500U, g_idleFrames[0], 0U),
};

static const DogMotionFrame_t s_sleepFrames[] =
{
    DOG_POSE(1500U, 1500U, 1500U, 1500U, 1500U, g_faceSleep, 0U),
};

static const DogMotionFrame_t s_sitFrames[] =
{
    /* Sit hold: front legs keep the stand pose, rear legs fold forward and stay there. */
    DOG_POSE(1480U, 800U, 1480U, 2200U, 1500U, g_faceCute, 0U),
};

static const DogMotionFrame_t s_standFrames[] =
{
    /* Stand at attention: keep the stable upright pose you just validated. */
    DOG_POSE(1480U, 1560U, 1480U, 1560U, 1500U, g_faceCute, 0U),
};

static const DogMotionFrame_t s_stretchFrames[] =
{
    /* Stretch: keep the base pose, then make a light forward/backward swing. */
    DOG_POSE(800U, 800U, 2200U, 2200U, 1500U, g_faceCute, 260U),
    DOG_POSE(760U, 760U, 2240U, 2240U, 1500U, g_faceCute, 260U),
    DOG_POSE(800U, 800U, 2200U, 2200U, 1500U, g_faceCute, 260U),
    DOG_POSE(840U, 840U, 2160U, 2160U, 1500U, g_faceCute, 260U),
};

static const DogMotionFrame_t s_happyFrames[] =
{
    DOG_POSE(1450U, 1600U, 1450U, 1600U, 1500U, g_faceHappy, 0U),
};

static const DogMotionFrame_t s_angryFrames[] =
{
    DOG_POSE(1450U, 1600U, 1450U, 1600U, 1500U, g_faceAngry, 0U),
};

static const DogMotionFrame_t s_forwardFrames[] =
{
    /*
     * Forward gait:
     * keep the diagonal stepping pattern that was already stable.
     */
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_60,  DOG_SERVO_120, DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_SHIFT_MS),
    DOG_POSE(DOG_SERVO_120, DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_60,  1500U, g_faceExcited, DOG_FORWARD_LIFT_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_120, DOG_SERVO_60,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_CENTER_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_SETTLE_MS),
    DOG_POSE(DOG_SERVO_60,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_120, 1500U, g_faceExcited, DOG_FORWARD_SHIFT_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_120, DOG_SERVO_60,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_LIFT_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_CENTER_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1450U, g_faceExcited, DOG_FORWARD_END_MS),
};

static const DogMotionFrame_t s_backwardFrames[] =
{
    /*
     * Backward keeps the same stability timing as forward, but mirrors the
     * leg direction so the support pair steps in the opposite direction.
     * A small left bias is applied here to compensate the observed right-rear drift.
     */
    DOG_POSE(DOG_SERVO_90,  (DOG_SERVO_120 + DOG_BACKWARD_BIAS_US), (DOG_SERVO_60 - DOG_BACKWARD_BIAS_US),  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_SHIFT_MS),
    DOG_POSE((DOG_SERVO_60 - DOG_BACKWARD_BIAS_US),  DOG_SERVO_90,  DOG_SERVO_90,  (DOG_SERVO_120 + DOG_BACKWARD_BIAS_US), 1500U, g_faceExcited, DOG_FORWARD_LIFT_MS),
    DOG_POSE(DOG_SERVO_90,  (DOG_SERVO_60 - DOG_BACKWARD_BIAS_US), (DOG_SERVO_120 + DOG_BACKWARD_BIAS_US), DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_CENTER_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_SETTLE_MS),
    DOG_POSE((DOG_SERVO_120 - DOG_BACKWARD_BIAS_US), DOG_SERVO_90,  DOG_SERVO_90,  (DOG_SERVO_60 + DOG_BACKWARD_BIAS_US), 1500U, g_faceExcited, DOG_FORWARD_SHIFT_MS),
    DOG_POSE(DOG_SERVO_90,  (DOG_SERVO_60 + DOG_BACKWARD_BIAS_US), (DOG_SERVO_120 - DOG_BACKWARD_BIAS_US), DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_LIFT_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1500U, g_faceExcited, DOG_FORWARD_CENTER_MS),
    DOG_POSE(DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  DOG_SERVO_90,  1450U, g_faceExcited, DOG_FORWARD_END_MS),
};

static const DogMotionFrame_t s_tailWagFrames[] =
{
    /* Neutral -> left wink -> neutral -> right wink */
    DOG_POSE(1500U, 1500U, 1500U, 1500U, 1500U, g_blinkFrames[0], DOG_ACTION_CENTER_MS),
    DOG_POSE(1500U, 1500U, 1500U, 1500U, 1300U, g_blinkFrames[1], DOG_ACTION_STEP_MS),
    DOG_POSE(1500U, 1500U, 1500U, 1500U, 1700U, g_blinkFrames[0], DOG_ACTION_STEP_MS),
    DOG_POSE(1500U, 1500U, 1500U, 1500U, 1500U, g_blinkFrames[2], DOG_ACTION_CENTER_MS),
};

static const DogMotionSequence_t s_idleSequence =
{
    s_idleFrames, 1U, 0U
};

static const DogMotionSequence_t s_sleepSequence =
{
    s_sleepFrames, 1U, 0U
};

static const DogMotionSequence_t s_sitSequence =
{
    s_sitFrames, 1U, 0U
};

static const DogMotionSequence_t s_standSequence =
{
    s_standFrames, 1U, 0U
};

static const DogMotionSequence_t s_stretchSequence =
{
    s_stretchFrames, 4U, 1U
};

static const DogMotionSequence_t s_happySequence =
{
    s_happyFrames, 1U, 0U
};

static const DogMotionSequence_t s_angrySequence =
{
    s_angryFrames, 1U, 0U
};

static const DogMotionSequence_t s_forwardSequence =
{
    s_forwardFrames, 8U, 1U
};

static const DogMotionSequence_t s_backwardSequence =
{
    s_backwardFrames, 8U, 1U
};

static const DogMotionSequence_t s_tailWagSequence =
{
    s_tailWagFrames, 4U, 1U
};

static void DogMotion_ShowFace(const uint8_t *face)
{
    OLED_Clear();
    OLED_ShowImage(DOG_FACE_X, DOG_FACE_Y, FACE_WIDTH, FACE_HEIGHT, face);
    OLED_Update();
}

static void DogMotion_ApplyFrame(const DogMotionFrame_t *frame)
{
    uint8_t i;

    if (frame == NULL)
    {
        return;
    }

    for (i = 0U; i < SERVO_COUNT; i++)
    {
        Servo_SetPulse(i, frame->pulseUs[i]);
    }

    DogMotion_ShowFace(frame->face);
}

static const DogMotionSequence_t *DogMotion_GetSequence(DogState_t state)
{
    switch (state)
    {
        case DOG_STATE_SLEEP:
            return &s_sleepSequence;
        case DOG_STATE_FORWARD:
            return &s_forwardSequence;
        case DOG_STATE_BACKWARD:
            return &s_backwardSequence;
        case DOG_STATE_STAND:
            return &s_standSequence;
        case DOG_STATE_SIT:
            return &s_sitSequence;
        case DOG_STATE_STRETCH:
            return &s_stretchSequence;
        case DOG_STATE_HAPPY:
            return &s_happySequence;
        case DOG_STATE_ANGRY:
            return &s_angrySequence;
        case DOG_STATE_TAIL_WAG:
            return &s_tailWagSequence;
        case DOG_STATE_IDLE:
        default:
            return &s_idleSequence;
    }
}

static void DogMotion_SetCurrentFrame(DogState_t state, uint8_t frameIndex)
{
    const DogMotionSequence_t *sequence = DogMotion_GetSequence(state);

    if (frameIndex >= sequence->frameCount)
    {
        frameIndex = 0U;
    }

    DogMotion_ApplyFrame(&sequence->frames[frameIndex]);
}

void DogMotion_Init(void)
{
    s_currentState = DOG_STATE_IDLE;
    s_activeState = DOG_STATE_IDLE;
    s_activeFrame = 0U;
    s_nextFrameTick = 0U;

    Servo_StopAll();
    DogMotion_SetCurrentFrame(DOG_STATE_IDLE, 0U);
}

void DogMotion_SetState(DogState_t state)
{
    if (state == s_currentState)
    {
        return;
    }

    s_currentState = state;
    s_activeState = state;
    s_activeFrame = 0U;
    s_nextFrameTick = 0U;

    DogMotion_SetCurrentFrame(s_activeState, 0U);
}

void DogMotion_Task(uint32_t now)
{
    const DogMotionSequence_t *sequence;

    sequence = DogMotion_GetSequence(s_activeState);

    if ((sequence->loopEnabled == 0U) || (sequence->frameCount <= 1U))
    {
        return;
    }

    if (s_nextFrameTick == 0U)
    {
        s_nextFrameTick = now + sequence->frames[s_activeFrame].durationMs;
        return;
    }

    if ((int32_t)(now - s_nextFrameTick) < 0)
    {
        return;
    }

    s_activeFrame++;
    if (s_activeFrame >= sequence->frameCount)
    {
        s_activeFrame = 0U;
    }

    DogMotion_ApplyFrame(&sequence->frames[s_activeFrame]);
    s_nextFrameTick = now + sequence->frames[s_activeFrame].durationMs;
}

DogState_t DogMotion_GetCurrentState(void)
{
    return s_currentState;
}
