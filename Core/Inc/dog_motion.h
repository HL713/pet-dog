#ifndef __DOG_MOTION_H
#define __DOG_MOTION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "dog_state.h"

void DogMotion_Init(void);
void DogMotion_SetState(DogState_t state);
void DogMotion_Task(uint32_t now);
DogState_t DogMotion_GetCurrentState(void);

#ifdef __cplusplus
}
#endif

#endif /* __DOG_MOTION_H */
