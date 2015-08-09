
液晶屏接口说明
// LCD Pin 7  : SCLK  --  P3.5
// LCD Pin 8  : DI    --  P3.6
// LCD Pin 9  : CS    --  P3.7
// LCD Pin 10 : VCC   --  3.3V / 5.0V
// LCD Pin 11 : GND   --  GND
// LCD Pin 12 : LEDA  --  CONNECT TO VCC ON LED /CONNECT TO GND OFF LED
// LCD Pin 13 : RES   --  CONNECT VCC
// LCD Pin 14 : A0    --  P8.2

本次工程中 没有使用软件复位，而是直接将该引脚（res）连接VCC上。