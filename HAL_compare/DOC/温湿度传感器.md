# DHT11

极简温湿度驱动：

- 文件：`BSP/DHT11/DHT11.h`、`BSP/DHT11/DHT11.c`
- 默认引脚：`PC6`
- 接口：`DHT11_read_data()`、`DHT11_update_data()`、`DHT11_getdata()`

---

## 1. CubeMX 配置

- `PC6`：可在输入上拉与开漏输出间切换（驱动内部动态切换）
- 需要可用的微秒延时基准（`DWT_Init()` + `delay_us()`）

---

## 2. 接线

| DHT11 | STM32 |
|---|---|
| DATA | PC6 |
| VCC | 3.3V |
| GND | GND |

---

## 3. 使用

```c
#include "delay_us.h"
#include "DHT11.h"

DWT_Init();
struct DHT11_data d = DHT11_read_data();
if (d.status == DHT11_DATA_OK) {
    // d.temperature, d.humidity
}
```

第一次测试时你应该看到：

- `status` 为 `DHT11_DATA_OK` 时可读到温湿度整数值
- 读取失败时返回 `DHT11_DATA_FAILED` 或 `DHT11_ACK_FAILED`

---

## 4. 注意事项

- 读取函数是阻塞式，不建议高频调用
- 采样周期建议 >= 0.5s
- 接口名里存在 `DTH11_check`（历史命名）
