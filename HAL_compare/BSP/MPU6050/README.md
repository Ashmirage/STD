# MPU6050

极简六轴驱动：

- 文件：`BSP/MPU6050/MPU6050.h`
- 方式：软件模拟 I2C
- 默认引脚：`SCL=PB6`，`SDA=PB7`
- 接口：`MPU6050_Init()`、`MPU6050_ReadAll()`

---

## 1. 需要接的引脚

这个驱动只用到 4 根线：

- `VCC`
- `GND`
- `SCL`
- `SDA`

其余 `AD0 / INT / XDA / XCL`，这个基础驱动**都不用接**。

> 说明：MPU6050 **可以读取温度**，但这是芯片内部温度，主要用于参考和温漂补偿，
> 不是用来精确测环境温度的。所以第一次测试时，更推荐先看 `AX / AY / AZ` 是否会随板子转动而变化。

---

## 2. CubeMX 配置

因为这个驱动用的是**软件模拟 I2C**，所以不需要开 I2C 外设，只要配置两个 GPIO。

### GPIO

- `PB6`：`Output Open Drain`，建议 `Pull-up`
- `PB7`：`Output Open Drain`，建议 `Pull-up`

如果你的模块板子上已经自带 I2C 上拉，`No pull` 也可以。
真正稳定运行时，**外部上拉**通常比 MCU 内部上拉更可靠。

---

## 3. 接线

| MPU6050 | STM32 |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SCL | PB6 |
| SDA | PB7 |

其余 `AD0 / INT / XDA / XCL` 不接即可。

---

## 4. 使用

```c
#include "OLED.h"
#include "MPU6050.h"

MPU6050_DataTypeDef mpu_data;   // MPU6050读回来的原始数据
HAL_StatusTypeDef mpu_status;   // MPU6050初始化结果

OLED_Init();
OLED_Clear();

mpu_status = MPU6050_Init();
if (mpu_status == HAL_OK) {
    OLED_ShowString(1, 1, "MPU6050 OK");
} else {
    OLED_ShowString(1, 1, "MPU6050 FAIL");
}

while (1) {
    if (mpu_status == HAL_OK && MPU6050_ReadAll(&mpu_data) == HAL_OK) {
        OLED_ShowString(2, 1, "AX:");
        OLED_ShowSignedNum(2, 4, (int32_t)mpu_data.ax, 5);

        OLED_ShowString(3, 1, "AY:");
        OLED_ShowSignedNum(3, 4, (int32_t)mpu_data.ay, 5);

        OLED_ShowString(4, 1, "AZ:");
        OLED_ShowSignedNum(4, 4, (int32_t)mpu_data.az, 5);
    } else {
        OLED_ShowString(2, 1, "READ FAIL");
    }

    HAL_Delay(200);
}
```

第一次测试时你应该看到：

- 第1行显示 `MPU6050 OK` 或 `MPU6050 FAIL`
- 第2~4行显示 `AX / AY / AZ`
- 轻轻转动板子时，数值会变化

---

## 5. 改引脚 / 改地址

如果你换了引脚，只改 `MPU6050.h` 顶部宏：

```c
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SCL_Pin GPIO_PIN_6
#define MPU6050_SDA_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_7
```

如果你后面确实要用 `AD0` 改地址，再把地址改成：

```c
#define MPU6050_ADDR 0x69U
```
