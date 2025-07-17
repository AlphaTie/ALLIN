 #include "dev.h"
 #include "24cxx.h"
 #include <stdio.h>

 /* DEV_ADDR_BASE����У׼����������EEPROM�����λ�ã���ʼ��ַ��
  * DEV_DATA_LENGTH����У׼�����ĳ���(�ֽ���)
  */
 #define DEV_ADDR_BASE   40
 #define DEV_DATA_LENGTH 12

uint8_t  testwrt[4]={1,2,3,4};
uint8_t  testrd[4]={0};

 /**
  * @brief   ����У׼����
  * @note    ����������EEPROMоƬ���棨24C02������ʼ��ַΪDEV_ADDR_BASE
  * @retval  ��
  */
 void cal_write_adjust_data(void)
 {
     uint8_t *p;
    
     /* pָ��tp_dev.xfac�ĵ�ַ
      * p+4����tp_dev.yfac�ĵ�ַ
      * p+8����tp_dev.xoff�ĵ�ַ
      * p+10����tp_dev.yoff�ĵ�ַ
      * �ܹ�ռ��12���ֽڣ�4��������
      * p+12���ڴ�ű�ǵ��败�����Ƿ�У׼�����ݣ�0x0A��
      * ��p[12]д��0x0A����Ѿ�У׼��
      */
     p = testwrt;                            /* ָ���׵�ַ */
     at24cxx_write(DEV_ADDR_BASE, p,4);                /* ����12���ֽ����ݣ�xfac��yfac��xc��yc�� */
     at24cxx_write_one_byte(DEV_ADDR_BASE + 4, 0x0A);   /* ����У׼ֵ */
 }

 /**
  * @brief   ��ȡ������EEPROM�����У׼ֵ
  * @param   ��
  * @retval  ��ȡ���
  * @arg     0: ʧ��
  * @arg     1: �ɹ�
  */
uint8_t cal_read_adjust_data(void)
 {
     uint8_t *p;
     uint8_t temp;
    
     p = testrd;
     /* ����������ֱ��ָ��tp_dev.xfac��ַ���б���ģ���ȡ��ʱ�򣬽���ȡ����������
      * д��ָ��tp_dev.xfac���׵�ַ���Ϳ��Ի�ԭд���ȥ��ֵ��������Ҫ���������
      * ������
      */
     at24cxx_read(DEV_ADDR_BASE, p, 4);                 /* ��ȡ12�ֽ����� */
     temp = at24cxx_read_one_byte(DEV_ADDR_BASE + 4);   /* ��ȡУ׼״̬��� */
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




