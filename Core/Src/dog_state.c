#include "dog_state.h"

static DogState_t s_currentState = DOG_STATE_IDLE;
static DogState_t s_targetState = DOG_STATE_IDLE;

void DogState_Init(void)
{
    s_currentState = DOG_STATE_IDLE;
    s_targetState = DOG_STATE_IDLE;
}

void DogState_SetCommand(DogCommand_t command)
{
    switch (command)
    {
        case DOG_CMD_SLEEP:
            s_targetState = DOG_STATE_SLEEP;
            break;
        case DOG_CMD_FORWARD:
            s_targetState = DOG_STATE_FORWARD;
            break;
        case DOG_CMD_BACKWARD:
            s_targetState = DOG_STATE_BACKWARD;
            break;
        case DOG_CMD_STAND:
            s_targetState = DOG_STATE_STAND;
            break;
        case DOG_CMD_SIT:
            s_targetState = DOG_STATE_SIT;
            break;
        case DOG_CMD_STRETCH:
            s_targetState = DOG_STATE_STRETCH;
            break;
        case DOG_CMD_HAPPY:
            s_targetState = DOG_STATE_HAPPY;
            break;
        case DOG_CMD_ANGRY:
            s_targetState = DOG_STATE_ANGRY;
            break;
        case DOG_CMD_TAIL_WAG:
            s_targetState = DOG_STATE_TAIL_WAG;
            break;
        case DOG_CMD_IDLE:
        default:
            s_targetState = DOG_STATE_IDLE;
            break;
    }
}

void DogState_Task(uint32_t now)
{
    (void)now;

    if (s_currentState != s_targetState)
    {
        s_currentState = s_targetState;
    }
}

DogState_t DogState_GetCurrent(void)
{
    return s_currentState;
}
