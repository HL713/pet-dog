#ifndef __FACE_ANIM_H
#define __FACE_ANIM_H

#include <stdint.h>
#include "face_data.h"

#define BLINK_FRAME_COUNT 3U
#define IDLE_FRAME_COUNT 3U
#define TAIL_FRAME_COUNT 3U

extern const uint8_t g_blinkFrames[BLINK_FRAME_COUNT][FACE_BYTES];
extern const uint8_t g_idleFrames[IDLE_FRAME_COUNT][FACE_BYTES];
extern const uint8_t g_tailFrames[TAIL_FRAME_COUNT][FACE_BYTES];

#endif
