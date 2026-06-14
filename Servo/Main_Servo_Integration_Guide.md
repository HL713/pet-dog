# main.c 调用 Servo 的说明

这份文档说明在当前 STM32 机器狗工程中，`main.c` 应该如何调用舵机底层接口。

对应文件：

- `Core/Src/main.c`
- `Core/Inc/servo.h`
- `Core/Src/servo.c`

---

## 1. 目标

`main.c` 的职责不是直接控制舵机细节，而是：

1. 初始化硬件
2. 初始化舵机驱动
3. 进入主循环
4. 后续把语音、动作状态机接进来

也就是说，`main.c` 只做“调度”，不做“具体动作实现”。

---

## 2. 必须包含的头文件

在 `main.c` 的 `/* USER CODE BEGIN Includes */` 区域里加入：

```c
#include "OLED.h"
#include "servo.h"
```

如果后面还要加语音模块，还会再加：

```c
#include "usart.h"
```

### 为什么要包含 `servo.h`

因为 `Servo_Init()`、`Servo_SetPulse()` 这些函数都在 `servo.h` 里声明。  
如果 `main.c` 没有包含它，就会出现“隐式声明”警告。

---

## 3. 初始化顺序

建议初始化顺序如下：

```c
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_TIM1_Init();
MX_TIM2_Init();
Servo_Init();
OLED_Init();
```

### 这个顺序为什么合理

- `HAL_Init()`
  - 初始化 HAL 库和 SysTick

- `SystemClock_Config()`
  - 把系统时钟配置到 72MHz

- `MX_GPIO_Init()`
  - 初始化通用 GPIO

- `MX_TIM1_Init()` / `MX_TIM2_Init()`
  - 先把舵机用的 PWM 定时器初始化好

- `Servo_Init()`
  - 启动 PWM 输出通道
  - 将所有舵机回中位

- `OLED_Init()`
  - 初始化显示屏

---

## 4. 为什么 `Servo_Init()` 要放在定时器初始化之后

`Servo_Init()` 内部会调用：

```c
HAL_TIM_PWM_Start(...)
```

所以它必须在 `MX_TIM1_Init()` 和 `MX_TIM2_Init()` 之后执行。  
如果定时器还没初始化，PWM 无法正常启动。

---

## 5. 最小可运行调用方式

如果你现在只是想验证舵机是否工作，`main.c` 里最小结构可以这样写：

```c
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  Servo_Init();

  while (1)
  {
    Servo_SetPulse(SERVO_TAIL, 1500);
    HAL_Delay(1000);
    Servo_SetPulse(SERVO_TAIL, 1300);
    HAL_Delay(1000);
    Servo_SetPulse(SERVO_TAIL, 1700);
    HAL_Delay(1000);
  }
}
```

### 这段代码的作用

- 上电后先让所有舵机回中
- 然后只测试尾巴舵机
- 通过尾巴摆动验证 PWM、供电、引脚、方向是否正确

---

## 6. 更推荐的写法

实际项目里，不建议把测试代码直接写死在 `while(1)`。

推荐写成一个测试函数：

```c
static void Servo_TestTail(void)
{
    Servo_SetPulse(SERVO_TAIL, 1500);
    HAL_Delay(1000);
    Servo_SetPulse(SERVO_TAIL, 1300);
    HAL_Delay(1000);
    Servo_SetPulse(SERVO_TAIL, 1700);
    HAL_Delay(1000);
}
```

然后在主函数里调用：

```c
Servo_TestTail();
```

这样以后你把测试代码删掉也更方便。

---

## 7. 语音模块接入后 `main.c` 的职责

后面你把语音模块接进来后，`main.c` 还应该负责：

1. 初始化 `USART3`
2. 初始化语音解析模块
3. 轮询或处理中断收到的语音命令
4. 根据命令切换动作状态

此时 `main.c` 的结构会变成：

```c
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_TIM1_Init();
MX_TIM2_Init();
MX_USART3_UART_Init();
Servo_Init();
ASR_Init();
OLED_Init();
```

---

## 8. `main.c` 里不要做的事

不要在 `main.c` 里直接写太多底层逻辑，比如：

- 不要直接操作 `TIMx->CCR`
- 不要自己算 PWM 波形
- 不要把站立/走路/摆尾全部写在 `while(1)` 里
- 不要让语音接收和动作执行混在一起

这些都应该分别放在：

- `servo.c`
- `dog_motion.c`
- `asr.c`

---

## 9. 你当前的代码警告怎么来的

你之前那个警告：

```c
function "Servo_Init" declared implicitly
```

说明：

- `main.c` 里调用了 `Servo_Init()`
- 但没有包含 `servo.h`

正确修复方式就是在 `main.c` 中加入：

```c
#include "servo.h"
```

---

## 10. 推荐的工程组织方式

### `servo.h / servo.c`
负责：

- PWM 启动
- 单舵机控制
- 标定管理

### `dog_motion.h / dog_motion.c`
负责：

- 站立
- 摆尾
- 前进
- 停止

### `asr.h / asr.c`
负责：

- 语音模块串口接收
- 语音命令解析

### `main.c`
负责：

- 初始化
- 调度
- 主循环

---

## 11. 建议你现在先做的事情

如果你要先验证舵机，建议按这个顺序：

1. 在 `main.c` 加 `#include "servo.h"`
2. 在初始化部分调用 `Servo_Init()`
3. 先只测试 `SERVO_TAIL`
4. 确认尾巴能左右摆
5. 再去测试四个脚

---

## 12. 一句话总结

`main.c` 对舵机的正确用法是：

- 先包含 `servo.h`
- 再初始化定时器
- 再调用 `Servo_Init()`
- 主循环里只做动作调度，不直接碰底层 PWM

