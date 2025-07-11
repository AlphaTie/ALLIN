#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

extern lv_obj_t * kb;
extern lv_obj_t * ta1;  // �����ⲿ����
extern lv_obj_t * ta2;  // �����ⲿ����

static void kb_ready_event_cb(lv_event_t * e)
{
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    
    if(ta) {
        const char * txt = lv_textarea_get_text(ta);
        
        // ʶ�����ĸ��ı��򲢴���
        if(ta == ta1) {
            printf("Freq: %s\n", txt);
            // ����Ƶ������
            if(strlen(txt) > 0) {

                // ������Ե�������Ƶ�ʵĺ���
                // set_ad9959_frequency(freq);
            }
        }
        else if(ta == ta2) {
            printf("Amp: %s\n", txt);
            // �����������
            if(strlen(txt) > 0) {

                // ������Ե������÷��ȵĺ���
                // set_ad9959_amplitude(amp);
            }
        }
        

    }
}

static void ta_event_cb_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    const char * txt = lv_textarea_get_text(ta);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        if(strcmp(txt, LV_SYMBOL_OK) == 0) {
            // ʶ��ͬ���ı���
            if(ta == ta1) {
                printf("ta1: %s\n", txt);
                // �������
                lv_textarea_set_text(ta, "");
                // ���Դ���ȷ�ϴ���
            }
            else if(ta == ta2) {
                printf("ta2 : %s\n", txt);
                // �������
                lv_textarea_set_text(ta, "");
                // ���Դ���ȷ�ϴ���
            }
            else {
                printf(": %s\n", txt);
            }
        }
        else {
            // ʵʱ��ʾ��������
            if(ta == ta1) {
                printf(": %s\n", txt);
            }
            else if(ta == ta2) {
                printf(": %s\n", txt);
            }
        }
    }
}

void events_init(lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen, ta_event_cb_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(kb, kb_ready_event_cb, LV_EVENT_READY, NULL);
}

