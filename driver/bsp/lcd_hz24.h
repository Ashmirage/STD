#ifndef __LCD_HZ24_H
#define __LCD_HZ24_H

#include "lcd.h"

/*
 * HAL 版本移植过来的精简接口:
 * 显示一个 24x24 汉字点阵。
 *
 * x, y : 汉字左上角坐标
 * dot  : 72 字节点阵数组, 逐行式, 每行 3 字节
 * mode : 0 非叠加(背景填 BACK_COLOR), 1 叠加(背景透明)
 */
static void LCD_ShowHZ24_Dot(u16 x,u16 y,const u8 *dot,u8 mode)
{
	u8 row;
	u8 col_byte;
	u8 bit;

	if(dot == NULL)return;
	if(x >= lcddev.width || y >= lcddev.height)return;

	for(row = 0; row < 24; row++)
	{
		u16 py = y + row;
		if(py >= lcddev.height)break;

		for(col_byte = 0; col_byte < 3; col_byte++)
		{
			u8 data = dot[row * 3 + col_byte];

			for(bit = 0; bit < 8; bit++)
			{
				u16 px = x + col_byte * 8 + bit;
				if(px >= lcddev.width)break;

				if(data & (0x80 >> bit))LCD_Fast_DrawPoint(px,py,POINT_COLOR);
				else if(mode == 0)LCD_Fast_DrawPoint(px,py,BACK_COLOR);
			}
		}
	}
}

#endif
