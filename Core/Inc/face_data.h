#ifndef __FACE_DATA_H
#define __FACE_DATA_H

#include <stdint.h>

#define FACE_WIDTH 64U
#define FACE_HEIGHT 64U
#define FACE_BYTES (FACE_WIDTH * FACE_HEIGHT / 8U)

extern const uint8_t g_faceHappy[FACE_BYTES];
extern const uint8_t g_faceAngry[FACE_BYTES];
extern const uint8_t g_faceSleep[FACE_BYTES];
extern const uint8_t g_faceExcited[FACE_BYTES];
extern const uint8_t g_faceCute[FACE_BYTES];

#endif
