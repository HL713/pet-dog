#include "asr.h"

#include <string.h>
#include "usart.h"

static volatile uint8_t s_lastCommandCode = ASR_CODE_IDLE;
static volatile uint8_t s_hasNewCommand = 0U;
static uint8_t s_rxByte = 0U;

static DogCommand_t ASR_CodeToCommand(uint8_t code)
{
    switch (code)
    {
        case ASR_CODE_IDLE:
            return DOG_CMD_IDLE;
        case ASR_CODE_SLEEP:
            return DOG_CMD_SLEEP;
        case ASR_CODE_FORWARD:
            return DOG_CMD_FORWARD;
        case ASR_CODE_BACKWARD:
            return DOG_CMD_BACKWARD;
        case ASR_CODE_SIT:
            return DOG_CMD_SIT;
        case ASR_CODE_STRETCH:
            return DOG_CMD_STRETCH;
        case ASR_CODE_HAPPY:
            return DOG_CMD_HAPPY;
        case ASR_CODE_ANGRY:
            return DOG_CMD_ANGRY;
        case ASR_CODE_TAIL_WAG:
            return DOG_CMD_TAIL_WAG;
        default:
            return DOG_CMD_NONE;
    }
}

void ASR_Init(void)
{
    s_lastCommandCode = ASR_CODE_IDLE;
    s_hasNewCommand = 0U;
    memset(&s_rxByte, 0, sizeof(s_rxByte));

    HAL_UART_Receive_IT(&huart3, &s_rxByte, 1U);
}

void ASR_RxByte(uint8_t byte)
{
    s_lastCommandCode = byte;
    s_hasNewCommand = 1U;
}

void ASR_Task(void)
{
    if (s_hasNewCommand == 0U)
    {
        return;
    }

    s_hasNewCommand = 0U;

    if (ASR_CodeToCommand(s_lastCommandCode) == DOG_CMD_NONE)
    {
        s_lastCommandCode = ASR_CODE_IDLE;
    }
}

DogCommand_t ASR_GetLastCommand(void)
{
    return ASR_CodeToCommand(s_lastCommandCode);
}

void ASR_ClearLastCommand(void)
{
    s_lastCommandCode = ASR_CODE_IDLE;
    s_hasNewCommand = 0U;
}

void ASR_UartRxCpltCallback(UART_HandleTypeDef *huart)
{
    if ((huart == NULL) || (huart->Instance != USART3))
    {
        return;
    }

    ASR_RxByte(s_rxByte);
    HAL_UART_Receive_IT(&huart3, &s_rxByte, 1U);
}
