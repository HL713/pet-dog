#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "servo.h"
#include "asr.h"
#include "dog_state.h"
#include "dog_motion.h"
/* USER CODE END Includes */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN 0 */
static void App_ShowBootScreen(void)
{
    OLED_Clear();
    OLED_ShowString(0, 0, "PETDOG", OLED_8X16);
    OLED_ShowString(0, 24, "READY", OLED_8X16);
    OLED_Update();
}

static void App_HandleAsrCommand(void)
{
    DogCommand_t command = ASR_GetLastCommand();

    if (command == DOG_CMD_NONE)
    {
        return;
    }

    DogState_SetCommand(command);
    ASR_ClearLastCommand();
}

static void App_SyncMotionState(void)
{
    static DogState_t s_lastMotionState = (DogState_t)0xFF;
    DogState_t currentState = DogState_GetCurrent();

    if (currentState == s_lastMotionState)
    {
        return;
    }

    DogMotion_SetState(currentState);
    s_lastMotionState = currentState;
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_USART3_UART_Init();
    MX_TIM3_Init();

    /* USER CODE BEGIN 2 */
    OLED_Init();
    Servo_Init();
    Servo_StopAll();
    ASR_Init();
    DogState_Init();
    DogMotion_Init();
    DogState_SetCommand(DOG_CMD_IDLE);
    App_ShowBootScreen();
    HAL_Delay(500);
    /* USER CODE END 2 */

    /* USER CODE BEGIN WHILE */
    while (1)
    {
        ASR_Task();
        App_HandleAsrCommand();
        DogState_Task(HAL_GetTick());
        App_SyncMotionState();
        DogMotion_Task(HAL_GetTick());
    }
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
