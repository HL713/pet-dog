/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : servo.h
  * @brief          : Servo PWM control layer for SG90 servos.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __SERVO_H
#define __SERVO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"

#define SERVO_COUNT 5U

typedef enum
{
    SERVO_RIGHT_FRONT = 0,
    SERVO_RIGHT_REAR  = 1,
    SERVO_LEFT_FRONT   = 2,
    SERVO_LEFT_REAR    = 3,
    SERVO_TAIL         = 4
} ServoId_t;

typedef struct
{
    uint16_t minPulseUs;
    uint16_t centerPulseUs;
    uint16_t maxPulseUs;
    int8_t direction;
} ServoCalib_t;

void Servo_Init(void);
void Servo_SetPulse(uint8_t id, uint16_t pulseUs);
void Servo_SetAngle(uint8_t id, float angleDeg);
void Servo_SetCalibration(uint8_t id, uint16_t minPulseUs, uint16_t centerPulseUs,
                          uint16_t maxPulseUs, int8_t direction);
void Servo_GetCalibration(uint8_t id, ServoCalib_t *calib);
void Servo_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_H */
