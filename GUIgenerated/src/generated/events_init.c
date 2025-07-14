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

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif


/************************************************************************************************************************************************/
extern lv_obj_t * kb;
extern lv_obj_t * ta1;  // 声明外部变量
extern lv_obj_t * ta2;  // 声明外部变量
uint8_t Channel = 0; // AD9959 通道 


// 单独的数据处理函数
static void process_current_input(lv_obj_t * kb) {
    lv_obj_t * ta = lv_keyboard_get_textarea(kb);
    if(!ta) return;
    
    const char * txt = lv_textarea_get_text(ta);
    
    if(ta == ta1) {
        // 频率处理
        if(strlen(txt) > 0) {
            float txtvalue = atof(txt);
            uint32_t freq2wrt;
            
            if(strstr(txt, "MHZ")) {
                freq2wrt = (uint32_t)(txtvalue * 1000000);
            }
            else if(strstr(txt, "KHZ")) {
                freq2wrt = (uint32_t)(txtvalue * 1000);
            }
            else if(strstr(txt, "HZ")) {
                freq2wrt = (uint32_t)txtvalue;
            }
            else {
                freq2wrt = (uint32_t)txtvalue;
            }
            
            if(freq2wrt < 1 || freq2wrt > 100000000) {
                printf("Freq overflow\n");
                return;
            }
            
            printf("Set Freq: %u Hz\n", freq2wrt);
            AD9959_Write_Frequence(Channel, freq2wrt);
            
            // 清空输入框
            lv_textarea_set_text(ta, "");
            // 隐藏键盘
            
        }
    }
    else if(ta == ta2) {
        // 幅度处理
        if(strlen(txt) > 0) {
            float txtvalue = atof(txt);
            uint16_t amp2wrt = 500;
            
            if(strstr(txt, "Vpp")) {
                amp2wrt = Vpp2AmpCtrl(txtvalue);
            }
            else if(strstr(txt, "Vrms")) {
                amp2wrt = Vrms2AmpCtrl(txtvalue);
            }
            else if(strstr(txt, "dBm")) {
                amp2wrt = dbm2AmpCtrl(txtvalue);
            }
            else {
                amp2wrt = (uint16_t)txtvalue;
            }
            
            if(amp2wrt < 1 || amp2wrt > 1023) {
                printf("Amp overflow\n");
                return;
            }
            
            printf("Set Amp: %d \n", amp2wrt);
            AD9959_Write_Amplitude(Channel, amp2wrt);
				}
		}
}


static void kb_all_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * kb = lv_event_get_target(e);
        uint16_t btn_id = lv_btnmatrix_get_selected_btn(kb);
        const char * btn_text = lv_btnmatrix_get_btn_text(kb, btn_id);
        
        printf("Key:%s\n", btn_text);  // 简化调试输出
        
        // 只处理特殊按键
        if(strcmp(btn_text, LV_SYMBOL_CLOSE) == 0) {
            lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
            printf("Keyboard hidden\n");
            return;
        }
        
        if(strcmp(btn_text, LV_SYMBOL_OK) == 0) {
            // 确认键 - 执行数据处理
            process_current_input(kb);
            return;
        }
        
        // 其他按键不做复杂处理，让LVGL默认处理
    }
}


static void ta_event_cb_handler(lv_event_t * e)
{ 
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    const char * txt = lv_textarea_get_text(ta);
    if(code==LV_EVENT_CLICKED){
        //clear the hidden flag of the keyboard,make it visible
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);

    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
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


static void screen_sw_2_event_handler (lv_event_t *e)
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
            lv_obj_clear_flag(guider_ui.screen_cont_cali, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        case (false):
        {
            lv_obj_add_flag(guider_ui.screen_cont_cali, LV_OBJ_FLAG_HIDDEN);
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
            lv_obj_clear_flag(guider_ui.screen_cont_input, LV_OBJ_FLAG_HIDDEN);
            break;
        }
        case (false):
        {
            lv_obj_add_flag(guider_ui.screen_cont_input, LV_OBJ_FLAG_HIDDEN);
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
    lv_obj_add_event_cb(ui->screen_sw_2, screen_sw_2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->screen_sw_1, screen_sw_1_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ta1, ta_event_cb_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ta2, ta_event_cb_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(kb, kb_all_event_cb, LV_EVENT_ALL, NULL);
}


void events_init(lv_ui *ui)
{

}
