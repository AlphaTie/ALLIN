#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

extern lv_obj_t * kb;
extern lv_obj_t * ta1;  // 声明外部变量
extern lv_obj_t * ta2;  // 声明外部变量

static void kb_ready_event_cb(lv_event_t * e)
{
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    
    if(ta) {
        const char * txt = lv_textarea_get_text(ta);
        
        // 识别是哪个文本框并处理
        if(ta == ta1) {
            printf("Freq: %s\n", txt);
            // 处理频率数据
            if(strlen(txt) > 0) {

                // 这里可以调用设置频率的函数
                // set_ad9959_frequency(freq);
            }
        }
        else if(ta == ta2) {
            printf("Amp: %s\n", txt);
            // 处理幅度数据
            if(strlen(txt) > 0) {

                // 这里可以调用设置幅度的函数
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
            // 识别不同的文本框
            if(ta == ta1) {
                printf("ta1: %s\n", txt);
                // 清除符号
                lv_textarea_set_text(ta, "");
                // 可以触发确认处理
            }
            else if(ta == ta2) {
                printf("ta2 : %s\n", txt);
                // 清除符号
                lv_textarea_set_text(ta, "");
                // 可以触发确认处理
            }
            else {
                printf(": %s\n", txt);
            }
        }
        else {
            // 实时显示输入内容
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

