# REMOTE

极简红外遥控（NEC）驱动：

- 文件：`BSP/REMOTE/remote.h`、`BSP/REMOTE/remote.c`
- 默认外设：`TIM1_CH1` 输入捕获
- 默认引脚：`PA8`
- 接口：`remote_init()`、`remote_scan()`

---

## 1. CubeMX 配置

- `TIM1 CH1`：输入捕获
- `PA8`：复用为 `TIM1_CH1`
- 开启 `TIM1` 中断

---

## 2. 接线

| 红外接收头 | STM32 |
|---|---|
| OUT | PA8 |
| VCC | 3.3V 或模块要求电压 |
| GND | GND |

---

## 3. 使用

```c
#include "remote.h"

remote_init();
uint8_t key = remote_scan();
if (key != 0) {
    // key 为按键值
}
```

第一次测试时你应该看到：

- 按下遥控器后，`remote_scan()` 返回非 0 键值

---

## 4. 注意事项

- 默认地址过滤 `REMOTE_ID` 为 `0`
- 需要确保 `TIM_All_callback.c` 已参与编译，否则回调不生效
- 建议在循环或调度中持续调用 `remote_scan()`
