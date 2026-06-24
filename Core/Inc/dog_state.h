#ifndef __DOG_STATE_H
#define __DOG_STATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "asr.h"

typedef enum
{
    DOG_STATE_IDLE = 0,
    DOG_STATE_SLEEP,
    DOG_STATE_FORWARD,
    DOG_STATE_BACKWARD,
    DOG_STATE_STAND,
    DOG_STATE_SIT,
    DOG_STATE_STRETCH,
    DOG_STATE_HAPPY,
    DOG_STATE_ANGRY,
    DOG_STATE_TAIL_WAG,
    DOG_STATE_RELIEF_PRESSURE
} DogState_t;

void DogState_Init(void);
void DogState_Task(uint32_t now);
void DogState_SetCommand(DogCommand_t command);
DogState_t DogState_GetCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* __DOG_STATE_H */
