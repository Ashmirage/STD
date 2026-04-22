- 把比赛提供的代码复制过来

- 使用方法
  ```c
  // 初始化
  void Hardware_init(void)
  {
      uart_init(115200);
      // 你原来的其他初始化...
  
      W25QXX_Init(); 
      Send_printf("W25Q ID = 0x%04X\r\n", W25QXX_TYPE);
  }
  
  // 读和写的方法
  uint8_t tx[] = "123456";
  uint8_t rx[sizeof(tx)] = {0};
  
  W25QXX_Write(tx, 0x000000, sizeof(tx));   // 写到 0 地址
  W25QXX_Read(rx, 0x000000, sizeof(rx));    // 从 0 地址读回
  
  Send_printf("rx = %s\r\n", rx);
  ```