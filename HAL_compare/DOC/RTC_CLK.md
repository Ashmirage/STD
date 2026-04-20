# RTC_CLK

极简 RTC 实时时钟封装：

- 文件：`BSP/RTC_CLK/RTC_clk.h`、`BSP/RTC_CLK/RTC_clk.c`
- 默认外设：`RTC (LSE)`
- 接口：`My_RTC_settime()`、`My_RTC_readtime()`、`My_RTC_time[]`

---

## 1. CubeMX 配置

- 开启 `RTC`
- 时钟源选择 `LSE`

---

## 2. 接线

| RTC 相关 | 说明 |
|---|---|
| 32.768kHz 晶振 | 按板级硬件设计 |
| 备份电池 | 如需掉电保持请接入 |

---

## 3. 使用

```c
#include "RTC_clk.h"

My_RTC_settime();
My_RTC_readtime();
// My_RTC_time: 年/月/日/时/分/秒
```

第一次测试时你应该看到：

- 初次上电写入初始时间
- 后续读取时间能持续走时

---

## 4. 注意事项

- 该模块通过备份寄存器标记判断是否首次设置
- 年份字段有两位/四位混用风险，二次封装时建议统一格式
