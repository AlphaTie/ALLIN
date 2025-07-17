 #include "dev.h"
 #include "24cxx.h"
 #include <stdio.h>

 /* DEV_ADDR_BASE定义校准参数保存在EEPROM里面的位置（起始地址）
  * DEV_DATA_LENGTH定义校准参数的长度(字节数)
  */
 #define DEV_ADDR_BASE   40
 #define DEV_DATA_LENGTH 12

uint8_t  testwrt[4]={1,2,3,4};
uint8_t  testrd[4]={0};

 /**
  * @brief   保存校准参数
  * @note    参数保存在EEPROM芯片里面（24C02），起始地址为DEV_ADDR_BASE
  * @retval  无
  */
 void cal_write_adjust_data(void)
 {
     uint8_t *p;
    
     /* p指向tp_dev.xfac的地址
      * p+4则是tp_dev.yfac的地址
      * p+8则是tp_dev.xoff的地址
      * p+10则是tp_dev.yoff的地址
      * 总共占用12个字节（4个参数）
      * p+12用于存放标记电阻触摸屏是否校准的数据（0x0A）
      * 往p[12]写入0x0A标记已经校准过
      */
     p = testwrt;                            /* 指向首地址 */
     at24cxx_write(DEV_ADDR_BASE, p,4);                /* 保存12个字节数据（xfac、yfac、xc、yc） */
     at24cxx_write_one_byte(DEV_ADDR_BASE + 4, 0x0A);   /* 保存校准值 */
 }

 /**
  * @brief   获取保存在EEPROM里面的校准值
  * @param   无
  * @retval  获取结果
  * @arg     0: 失败
  * @arg     1: 成功
  */
uint8_t cal_read_adjust_data(void)
 {
     uint8_t *p;
     uint8_t temp;
    
     p = testrd;
     /* 由于我们是直接指向tp_dev.xfac地址进行保存的，读取的时候，将读取出来的数据
      * 写入指向tp_dev.xfac的首地址，就可以还原写入进去的值，而不需要理会具体的数
      * 据类型
      */
     at24cxx_read(DEV_ADDR_BASE, p, 4);                 /* 读取12字节数据 */
     temp = at24cxx_read_one_byte(DEV_ADDR_BASE + 4);   /* 读取校准状态标记 */
     if (temp == 0x0A)
     {
					printf("READ OK");
					for(uint8_t i=0;i<4;i++)
					{
						printf("%d",p[i]);
					}
         return 1;
     }
    
     return 0;
 }



void EEPROM_init(void)
{
	at24cxx_init();

}




