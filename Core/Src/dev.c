//#include "dev.h"
//#include "24cxx.h"


///* DEV_ADDR_BASE定义校准参数保存在EEPROM里面的位置（起始地址）
// * DEV_DATA_LENGTH
// */
//#define DEV_ADDR_BASE   40
//#define DEV_DATA_LENGTH



///**
// * @brief   保存校准参数
// * @note    参数保存在EEPROM芯片里面（24C02），起始地址为DEV_SAVE_ADDR_BASE
// * @param   无
// * @retval  无
// */
//void tp_save_adjust_data(void)
//{
//    uint8_t *p;
//    
//    /* p指向tp_dev.xfac的地址
//     * p+4则是tp_dev.yfac的地址
//     * p+8则是tp_dev.xoff的地址
//     * p+10则是tp_dev.yoff的地址
//     * 总共占用12个字节（4个参数）
//     * p+12用于存放标记电阻触摸屏是否校准的数据（0x0A）
//     * 往p[12]写入0x0A标记已经校准过
//     */
//    p = (uint8_t *)&tp_dev.xfac;                            /* 指向首地址 */
//    at24cxx_write(DEV_SAVE_ADDR_BASE, p, 12);                /* 保存12个字节数据（xfac、yfac、xc、yc） */
//    at24cxx_write_one_byte(DEV_SAVE_ADDR_BASE + 12, 0x0A);   /* 保存校准值 */
//}

///**
// * @brief   获取保存在EEPROM里面的校准值
// * @param   无
// * @retval  获取结果
// * @arg     0: 失败
// * @arg     1: 成功
// */
//static uint8_t tp_get_adjust_data(void)
//{
//    uint8_t *p;
//    uint8_t temp;
//    
//    p = (uint8_t *)&tp_dev.xfac;
//    /* 由于我们是直接指向tp_dev.xfac地址进行保存的，读取的时候，将读取出来的数据
//     * 写入指向tp_dev.xfac的首地址，就可以还原写入进去的值，而不需要理会具体的数
//     * 据类型
//     */
//    at24cxx_read(TP_SAVE_ADDR_BASE, p, 12);                 /* 读取12字节数据 */
//    temp = at24cxx_read_one_byte(TP_SAVE_ADDR_BASE + 12);   /* 读取校准状态标记 */
//    if (temp == 0x0A)
//    {
//        return 1;
//    }
//    
//    return 0;
//}









