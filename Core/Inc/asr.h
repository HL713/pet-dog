#ifndef __ASR_H
#define __ASR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

typedef enum
{
    DOG_CMD_NONE = 0,
    DOG_CMD_IDLE,
    DOG_CMD_SLEEP,
    DOG_CMD_FORWARD,
    DOG_CMD_BACKWARD,
    DOG_CMD_STAND,
    DOG_CMD_SIT,
    DOG_CMD_STRETCH,
    DOG_CMD_HAPPY,
    DOG_CMD_ANGRY,
    DOG_CMD_TAIL_WAG
} DogCommand_t;

typedef enum
{
    ASR_CODE_IDLE      = 0x01U,
    ASR_CODE_SLEEP     = 0x02U,
    ASR_CODE_FORWARD   = 0x03U,
    ASR_CODE_BACKWARD  = 0x04U,
    ASR_CODE_STAND     = 0x05U,
    ASR_CODE_SIT       = 0x06U,
    ASR_CODE_STRETCH   = 0x07U,
    ASR_CODE_HAPPY     = 0x08U,
    ASR_CODE_ANGRY     = 0x09U,
    ASR_CODE_TAIL_WAG  = 0x0AU
} AsrCommandCode_t;

void ASR_Init(void);
void ASR_Task(void);
void ASR_RxByte(uint8_t byte);
void ASR_UartRxCpltCallback(UART_HandleTypeDef *huart);
DogCommand_t ASR_GetLastCommand(void);
void ASR_ClearLastCommand(void);

#ifdef __cplusplus
}
#endif

#endif /* __ASR_H */
