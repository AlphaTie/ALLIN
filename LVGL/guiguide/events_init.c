/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "lvgl.h"
#include "AD9959.h"
#include <stdlib.h>
#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


extern lv_obj_t * kb;
extern lv_obj_t * ta1;  // 声明外部变量
extern lv_obj_t * ta2;  // 声明外部变量
uint8_t Channel = 0; // AD9959 通道 

static void screen_roller_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        lv_obj_t * roller = lv_event_get_target(e);
        //使用lv_roller_get_selected(roller);向Channel赋值
        Channel = lv_roller_get_selected(roller);
        printf("Selected Channel: %d\n", Channel);
        
        break;
    }
    default:
        break;
    }
}


static void kb_ready_event_cb(lv_event_t * e)
{
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    
    if(ta) {
        const char * txt = lv_textarea_get_text(ta);
        
        // 识别是哪个文本框并处理
        if(ta == ta1) {
            // 处理频率数据
            if(strlen(txt) > 0) {
                float txtvalue = atof(txt); // 将字符串转换为浮点数
                uint32_t freq2wrt;
                
                // 检测单位并转换
                if(strstr(txt, "MHZ") ) {
                    // 输入包含MHz单位，转换为Hz
                    freq2wrt = (uint32_t)(txtvalue * 1000000);
                }
                else if(strstr(txt, "KHZ")) {
                    // 输入包含kHz单位，转换为Hz
                    freq2wrt = (uint32_t)(txtvalue * 1000);
                }
                else if( strstr(txt, "HZ")) {
                    // 输入包含Hz单位，直接使用
                    freq2wrt = (uint32_t)txtvalue;
                }
                else {
                    // 没有单位，默认为Hz
                    freq2wrt = (uint32_t)txtvalue;
                }
                
                // 范围检查
                if(freq2wrt < 1 || freq2wrt > 100000000) {  // 1Hz - 100MHz
                    printf("Overflow (1Hz - 100MHz)\n");
                    return;
                }
                
                printf("Set Freq: %d Hz\n", freq2wrt);
                AD9959_Write_Frequence(Channel, freq2wrt);
            }
        }
        else if(ta == ta2) {
            printf("Amp: %s\n", txt);
            // 处理幅度数据
            if(strlen(txt) > 0) {
                uint16_t amp = atoi(txt);
                
                // 范围检查
                if(amp > 1023) {  // 假设10位DAC
                    printf("Overflow (0-1023)\n");
                    return;
                }
                
                printf("Set Amp: %d\n", amp);
                AD9959_Write_Amplitude(Channel, amp);
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

static void screen_sw_1_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        lv_obj_t * status_obj = lv_event_get_target(e);
        int status = lv_obj_has_state(status_obj, LV_STATE_CHECKED) ? true : false;

        switch (status) {
        case (true):
        {
            lv_obj_add_flag(guider_ui.screen_cont_2, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        case (false):
        {
            lv_obj_clear_flag(guider_ui.screen_cont_2, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
}

void events_init_screen (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->screen_roller_1, screen_roller_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_sw_1, screen_sw_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ta1, ta_event_cb_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ta2, ta_event_cb_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(kb, kb_ready_event_cb, LV_EVENT_READY, NULL);
}


void events_init(lv_ui *ui)
{

}
