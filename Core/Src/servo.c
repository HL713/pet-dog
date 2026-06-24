/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : servo.c
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

#include "servo.h"

typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t channel;
} ServoChannelMap_t;

static const ServoChannelMap_t ServoChannelMap[SERVO_COUNT] =
{
    { &htim1, TIM_CHANNEL_1 }, /* Right front -> PA8  */
    { &htim1, TIM_CHANNEL_2 }, /* Right rear  -> PA9  */
    { &htim1, TIM_CHANNEL_3 }, /* Left front  -> PA10 */
    { &htim1, TIM_CHANNEL_4 }, /* Left rear   -> PA11 */
    { &htim2, TIM_CHANNEL_1 }, /* Tail        -> PA15 */
};

static ServoCalib_t s_servoCalib[SERVO_COUNT] =
{
    { 500U, 1500U, 2500U,  1 },
    { 500U, 1500U, 2500U,  1 },
    { 500U, 1500U, 2500U,  1 },
    { 500U, 1500U, 2500U,  1 },
    { 500U, 1500U, 2500U,  1 },
};

static uint16_t Servo_ClampPulse(uint16_t pulseUs)
{
    if (pulseUs < 500U)
    {
        return 500U;
    }

    if (pulseUs > 2500U)
    {
        return 2500U;
    }

    return pulseUs;
}

static uint16_t Servo_ClampWithinCalibration(uint8_t id, uint16_t pulseUs)
{
    const ServoCalib_t *calib = &s_servoCalib[id];
    uint16_t minPulse = calib->minPulseUs;
    uint16_t maxPulse = calib->maxPulseUs;

    if (minPulse > maxPulse)
    {
        uint16_t tmp = minPulse;
        minPulse = maxPulse;
        maxPulse = tmp;
    }

    if (pulseUs < minPulse)
    {
        return minPulse;
    }

    if (pulseUs > maxPulse)
    {
        return maxPulse;
    }

    return pulseUs;
}

void Servo_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    Servo_StopAll();
}

void Servo_SetCalibration(uint8_t id, uint16_t minPulseUs, uint16_t centerPulseUs,
                          uint16_t maxPulseUs, int8_t direction)
{
    if (id >= SERVO_COUNT)
    {
        return;
    }

    s_servoCalib[id].minPulseUs = minPulseUs;
    s_servoCalib[id].centerPulseUs = centerPulseUs;
    s_servoCalib[id].maxPulseUs = maxPulseUs;
    s_servoCalib[id].direction = (direction >= 0) ? 1 : -1;
}

void Servo_GetCalibration(uint8_t id, ServoCalib_t *calib)
{
    if ((id >= SERVO_COUNT) || (calib == NULL))
    {
        return;
    }

    *calib = s_servoCalib[id];
}

void Servo_SetPulse(uint8_t id, uint16_t pulseUs)
{
    if (id >= SERVO_COUNT)
    {
        return;
    }

    pulseUs = Servo_ClampPulse(pulseUs);
    pulseUs = Servo_ClampWithinCalibration(id, pulseUs);

    __HAL_TIM_SET_COMPARE(ServoChannelMap[id].htim, ServoChannelMap[id].channel, pulseUs);
}

void Servo_SetAngle(uint8_t id, float angleDeg)
{
    const ServoCalib_t *calib;
    float clampedAngle;
    float ratio;
    int32_t pulse;
    uint16_t pulseUs;

    if (id >= SERVO_COUNT)
    {
        return;
    }

    calib = &s_servoCalib[id];

    if (angleDeg < 0.0f)
    {
        clampedAngle = 0.0f;
    }
    else if (angleDeg > 180.0f)
    {
        clampedAngle = 180.0f;
    }
    else
    {
        clampedAngle = angleDeg;
    }

    ratio = (clampedAngle - 90.0f) / 90.0f;

    if (calib->direction < 0)
    {
        ratio = -ratio;
    }

    if (ratio >= 0.0f)
    {
        pulse = (int32_t)calib->centerPulseUs +
                (int32_t)((calib->maxPulseUs - calib->centerPulseUs) * ratio);
    }
    else
    {
        pulse = (int32_t)calib->centerPulseUs +
                (int32_t)((calib->centerPulseUs - calib->minPulseUs) * ratio);
    }

    if (pulse < 0)
    {
        pulse = 0;
    }

    pulseUs = (uint16_t)pulse;

    Servo_SetPulse(id, pulseUs);
}

void Servo_StopAll(void)
{
    uint8_t i;

    for (i = 0; i < SERVO_COUNT; i++)
    {
        Servo_SetPulse(i, s_servoCalib[i].centerPulseUs);
    }
}
