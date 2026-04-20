# AD

极简 ADC 采样驱动：

- 文件：`BSP/AD/AD.h`、`BSP/AD/AD.c`
- 默认外设：`ADC3 + DMA`
- 接口：`AD_init()`、`AD_Value[]`

---

## 1. CubeMX 配置

- `ADC3`：常规转换开启
- `DMA`：建议 `Circular` 模式（`BSP/AD/notice.txt`）
- 采样通道与引脚：待确认

---

## 2. 接线


## 3. 使用

```c
#include "AD.h"

AD_init();
uint16_t raw = AD_Value[0];
```

第一次测试时你应该看到：

- `AD_Value[0]` 会随输入电压变化
- 12bit 下通常范围在 `0~4095`

---

## 4. 注意事项

- 当前 `HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AD_Value, 1)` 长度是 `1`
- 若你要用 `AD_Value[1..3]`，需要同步检查 ADC 多通道与 DMA 长度
- 使用多个通道加DMA转运时,增加采样时间,否则程序一直进中断卡住
