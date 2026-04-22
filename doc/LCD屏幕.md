- 可以直接使用

- 触摸功能需要包含他给的那些文件,也可以直接使用,使用方法如下
  ```c
  // 初始化需要这两行代码
  LCD_Init(); //LCD显示屏初始化
  TP_Init();
  
  // 使用的时候需要放到20ms执行一次的调度器里面
  // 20ms执行一次
  static void Loop_50hz(void)
  {
  	if (tp_dev.scan(0))  // 正常扫描，电容屏就这么用
      {
          uint16_t x = tp_dev.x[0];
          uint16_t y = tp_dev.y[0];
  		Send_printf("x=%d,y=%d\r\n",x,y);
          // x,y 就是当前第 1 个触点的屏幕坐标
      }
  }
  
  ```

- 触摸可以显示轨迹
  ```c
  if (tp_dev.scan(0))  // 正常扫描，电容屏就这么用
      {
          uint16_t x = tp_dev.x[0];
          uint16_t y = tp_dev.y[0];
  		TP_Draw_Big_Point(x,y,RED);
  //		Send_printf("x=%d,y=%d\r\n",x,y);
          // x,y 就是当前第 1 个触点的屏幕坐标
      }
  ```

  