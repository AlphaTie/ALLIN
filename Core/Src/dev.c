//#include "dev.h"
//#include "24cxx.h"


///* DEV_ADDR_BASE����У׼����������EEPROM�����λ�ã���ʼ��ַ��
// * DEV_DATA_LENGTH
// */
//#define DEV_ADDR_BASE   40
//#define DEV_DATA_LENGTH



///**
// * @brief   ����У׼����
// * @note    ����������EEPROMоƬ���棨24C02������ʼ��ַΪDEV_SAVE_ADDR_BASE
// * @param   ��
// * @retval  ��
// */
//void tp_save_adjust_data(void)
//{
//    uint8_t *p;
//    
//    /* pָ��tp_dev.xfac�ĵ�ַ
//     * p+4����tp_dev.yfac�ĵ�ַ
//     * p+8����tp_dev.xoff�ĵ�ַ
//     * p+10����tp_dev.yoff�ĵ�ַ
//     * �ܹ�ռ��12���ֽڣ�4��������
//     * p+12���ڴ�ű�ǵ��败�����Ƿ�У׼�����ݣ�0x0A��
//     * ��p[12]д��0x0A����Ѿ�У׼��
//     */
//    p = (uint8_t *)&tp_dev.xfac;                            /* ָ���׵�ַ */
//    at24cxx_write(DEV_SAVE_ADDR_BASE, p, 12);                /* ����12���ֽ����ݣ�xfac��yfac��xc��yc�� */
//    at24cxx_write_one_byte(DEV_SAVE_ADDR_BASE + 12, 0x0A);   /* ����У׼ֵ */
//}

///**
// * @brief   ��ȡ������EEPROM�����У׼ֵ
// * @param   ��
// * @retval  ��ȡ���
// * @arg     0: ʧ��
// * @arg     1: �ɹ�
// */
//static uint8_t tp_get_adjust_data(void)
//{
//    uint8_t *p;
//    uint8_t temp;
//    
//    p = (uint8_t *)&tp_dev.xfac;
//    /* ����������ֱ��ָ��tp_dev.xfac��ַ���б���ģ���ȡ��ʱ�򣬽���ȡ����������
//     * д��ָ��tp_dev.xfac���׵�ַ���Ϳ��Ի�ԭд���ȥ��ֵ��������Ҫ���������
//     * ������
//     */
//    at24cxx_read(TP_SAVE_ADDR_BASE, p, 12);                 /* ��ȡ12�ֽ����� */
//    temp = at24cxx_read_one_byte(TP_SAVE_ADDR_BASE + 12);   /* ��ȡУ׼״̬��� */
//    if (temp == 0x0A)
//    {
//        return 1;
//    }
//    
//    return 0;
//}









