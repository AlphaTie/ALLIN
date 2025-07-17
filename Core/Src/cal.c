#include "cal.h"
#include "main.h"
#include <stdio.h>
#include "lcd.h"
//This file contains the calculate functions

float vpp2dbm(float vpp)
{
    // Convert Vpp to dBm
    // Formula: dBm = 10 * log10((Vpp^2) / (50^2))
    // Assuming a 50-ohm system
    return 10 * log10((vpp * vpp) / (50 * 50));

}

float dbm2vpp(float dbm)
{
    // Convert dBm to Vpp
    // Formula: Vpp = sqrt((10^(dBm/10)) * (50^2))
    // Assuming a 50-ohm system
    return sqrt(pow(10, dbm / 10) * (50 * 50));
}

uint16_t Vpp2AmpCtrl(float vpp)
{
    // Convert Vpp to AD9959 amplitude control value
    // Assuming a 10-bit DAC, Vpp range is 0-250mVpp
    // Formula: AmpCtrl = Vpp * 1000 / 260
    uint16_t Ctrl = (uint16_t)(vpp * 1000 / 235) + 1;
    if(Ctrl > 1023) {
        Ctrl = 1023; // Max value for 10-bit DAC
    }   
    return Ctrl;
}

uint16_t Vrms2AmpCtrl(float vrms)
{
    // Convert Vrms to AD9959 amplitude control value
    // Assuming a 10-bit DAC, Vrms range is 0-250mVrms
    // Formula: AmpCtrl = Vrms * 1000 / 260
		printf("Vrms INPUT:%f\n",vrms);
    uint16_t Ctrl = (uint16_t)(vrms * 2.828427 * 1000 / 235 ) + 1 ;
    if(Ctrl > 1023 ) {
        Ctrl = 1023 ; // Max value for 10-bit DAC
    }   
		printf("Vrms OUTPUT:%f\n",Ctrl*235/2828.427);
    return Ctrl;
}

uint16_t dbm2AmpCtrl(float dbm)
{
    return Vpp2AmpCtrl(dbm2vpp(dbm));
}

uint16_t Voffset2DacCtrl(float vpp)
{
    // Convert Vpp to AD9959 DAC output value
    // Assuming a 12-bit DAC, Vpp range is 0-3300mVpp
    // Formula: DacCtrl = Vpp * 4096 / 3300
    uint16_t Ctrl = (uint16_t)(vpp * 4096 / 3300);
    if(Ctrl > 4095) {
        Ctrl = 4095; // Max value for 12-bit DAC
    }   
    return Ctrl;
}

#define ADC_DMA_LENGTH 1024
extern float FFT_INPUT[ADC_DMA_LENGTH*2];
extern float FFT_OUTPUT[ADC_DMA_LENGTH];

void fft(uint16_t* adc_buff){
		
		arm_cfft_instance_f32 scfft;
		arm_cfft_init_f32(&scfft,ADC_DMA_LENGTH);
		uint16_t i;
		
		
		for(i=0;i < ADC_DMA_LENGTH;i++)
		{
			FFT_INPUT[2*i] = (float)adc_buff[i]*3.3/4096;
			FFT_INPUT[2*i+1] = 0;
		}
		
		arm_cfft_f32(&scfft,FFT_INPUT,0,1);
		arm_cmplx_mag_f32(FFT_INPUT, FFT_OUTPUT, ADC_DMA_LENGTH);
}

#define NUM_TAPS 64  // 滤波器阶数
#define BLOCK_SIZE 1024  // 每次处理 1024 点信号

// FIR 滤波器系数
float32_t firCoeffs[NUM_TAPS] = {
    -0.135621343174, 0.009098013178292,  0.01075744529296,  0.01341877298801,
    0.01675672745391,  0.02039396955489,  0.02387129382639,  0.02680164682186,
    0.02876701073511,  0.02950050516448,  0.02875817466084,  0.02650637457705,
    0.02275659777412,  0.01775282319676,  0.01175745076483,  0.00524671109947,
   -0.00136391344442,-0.007489393649218, -0.01273317678325, -0.01655948448774,
   -0.01886902199015, -0.01893371740906, -0.01735387695371, -0.01410495895891,
  -0.009200129937612,-0.003229608556294, 0.003385507662789,  0.01004018381041,
    0.01621447970897,   0.0213607892105,  0.02506202964367,  0.02699191517791,
    0.02699191517791,  0.02506202964367,   0.0213607892105,  0.01621447970897,
    0.01004018381041, 0.003385507662789,-0.003229608556294,-0.009200129937612,
   -0.01410495895891, -0.01735387695371, -0.01893371740906, -0.01886902199015,
   -0.01655948448774, -0.01273317678325,-0.007489393649218, -0.00136391344442,
    0.00524671109947,  0.01175745076483,  0.01775282319676,  0.02275659777412,
    0.02650637457705,  0.02875817466084,  0.02950050516448,  0.02876701073511,
    0.02680164682186,  0.02387129382639,  0.02039396955489,  0.01675672745391,
    0.01341877298801,  0.01075744529296, 0.009098013178292,   -0.135621343174
};

float32_t firState[BLOCK_SIZE + NUM_TAPS - 1] = {0};
float32_t inputSignal[BLOCK_SIZE];  // DMA 提供的 1024 点采样数据
float32_t outputSignal[BLOCK_SIZE];

void draw_waveform_split(float32_t* inputSignal, float32_t* outputSignal, int block_size) {
    // 分别计算输入和输出信号的最大最小值
    float32_t min_input, max_input, min_output, max_output;
    uint32_t max_input_index, max_output_index;
    
    // 使用ARM DSP库函数计算最值和索引
    arm_min_f32(inputSignal, block_size, &min_input, NULL);
    arm_max_f32(inputSignal, block_size, &max_input, &max_input_index);
    arm_min_f32(outputSignal, block_size, &min_output, NULL);
    arm_max_f32(outputSignal, block_size, &max_output, &max_output_index);
    
    // 定义显示区域
    #define INPUT_Y_TOP 0
    #define INPUT_Y_BOTTOM 235
    #define OUTPUT_Y_TOP 245
    #define OUTPUT_Y_BOTTOM 480
    #define DISPLAY_WIDTH 400
    
    int x_prev_in = 0, y_prev_in = 0;
    int x_prev_out = 0, y_prev_out = 0;
    
    // 绘制波形
    for (int n = 0; n < block_size; n++) {
        // 计算X坐标（两个信号共用）
        int x_pixel = (n * DISPLAY_WIDTH) / block_size;
        
        // 输入信号Y坐标映射到上半部分 (0-235)
        int y_pixel_in = INPUT_Y_BOTTOM - (int)((inputSignal[n] - min_input) * 
                        (INPUT_Y_BOTTOM - INPUT_Y_TOP) / (max_input - min_input));
        
        // 输出信号Y坐标映射到下半部分 (245-480)
        int y_pixel_out = OUTPUT_Y_BOTTOM - (int)((outputSignal[n] - min_output) * 
                         (OUTPUT_Y_BOTTOM - OUTPUT_Y_TOP) / (max_output - min_output));
        
        // 绘制输入信号（红色）
        if (n > 0) {
            lcd_draw_line(x_prev_in, y_prev_in, x_pixel, y_pixel_in, 0xF800); // 红色
        }
        x_prev_in = x_pixel;
        y_prev_in = y_pixel_in;
        
        // 绘制输出信号（绿色）
        if (n > 0) {
            lcd_draw_line(x_prev_out, y_prev_out, x_pixel, y_pixel_out, 0x07E0); // 绿色
        }
        x_prev_out = x_pixel;
        y_prev_out = y_pixel_out;
    }
    
    // 标记最大值点
    int max_input_x = (max_input_index * DISPLAY_WIDTH) / block_size;
    int max_input_y = INPUT_Y_BOTTOM - (int)((max_input - min_input) * 
                     (INPUT_Y_BOTTOM - INPUT_Y_TOP) / (max_input - min_input));
    
    int max_output_x = (max_output_index * DISPLAY_WIDTH) / block_size;
    int max_output_y = OUTPUT_Y_BOTTOM - (int)((max_output - min_output) * 
                      (OUTPUT_Y_BOTTOM - OUTPUT_Y_TOP) / (max_output - min_output));
    
    // 绘制最大值标记点（小圆圈）
    for(int i = -2; i <= 2; i++) {
        for(int j = -2; j <= 2; j++) {
            if(i*i + j*j <= 4) { // 圆形标记
                // 输入信号最大值标记（蓝色）
                if(max_input_x + i >= 0 && max_input_x + i < DISPLAY_WIDTH && 
                   max_input_y + j >= INPUT_Y_TOP && max_input_y + j <= INPUT_Y_BOTTOM) {
                    lcd_draw_point(max_input_x + i, max_input_y + j, 0x001F); // 蓝色
                }
                // 输出信号最大值标记（蓝色）
                if(max_output_x + i >= 0 && max_output_x + i < DISPLAY_WIDTH && 
                   max_output_y + j >= OUTPUT_Y_TOP && max_output_y + j <= OUTPUT_Y_BOTTOM) {
                    lcd_draw_point(max_output_x + i, max_output_y + j, 0x001F); // 蓝色
                }
            }
        }
    }
    
    // 绘制分隔线
    lcd_draw_line(0, 240, DISPLAY_WIDTH, 240, 0x0000); // 黑色分隔线
    
    // 显示数值信息（如果有字符显示函数）
    char text_buffer[50];
    
    // 输入信号信息
    sprintf(text_buffer, "Input Max: %.4fV", max_input);
    // lcd_show_string(10, 10, text_buffer, 0x0000); // 黑色文字
    
    sprintf(text_buffer, "Input Min: %.4fV", min_input);
    // lcd_show_string(10, 30, text_buffer, 0x0000);
    
    // 输出信号信息  
    sprintf(text_buffer, "Output Max: %.4fV", max_output);
    // lcd_show_string(10, 250, text_buffer, 0x0000);
    
    sprintf(text_buffer, "Output Min: %.4fV", min_output);
    // lcd_show_string(10, 270, text_buffer, 0x0000);
    
    // 增益信息
    float32_t gain_db = 20.0f * log10f(max_output / max_input);
    sprintf(text_buffer, "Gain: %.2f dB", gain_db);
    // lcd_show_string(10, 290, text_buffer, 0x0000);
    
    // 调试输出到串口
    printf("Input: Max=%.4fV(idx=%d), Min=%.4fV\n", max_input, (int)max_input_index, min_input);
    printf("Output: Max=%.4fV(idx=%d), Min=%.4fV\n", max_output, (int)max_output_index, min_output);
    printf("Peak Gain: %.2f dB\n", gain_db);
}

void fir_test(uint16_t* adc_buff) {
    arm_fir_instance_f32 fir;

    // 初始化 FIR 滤波器
    arm_fir_init_f32(&fir, NUM_TAPS, firCoeffs, firState, BLOCK_SIZE);

    // 将 ADC 数据转换为浮点数
    for (int i = 0; i < BLOCK_SIZE; i++) {
        inputSignal[i] = (float32_t)adc_buff[i]*3.3/4095;
    }

    // 滤波计算
    arm_fir_f32(&fir, inputSignal, outputSignal, BLOCK_SIZE);

    // 清屏
    lcd_fill(0, 0, 400, 480, 0xFFFF);  // 假设屏幕高度为480
    
    // 绘制分离的波形
    draw_waveform_split(inputSignal, outputSignal, BLOCK_SIZE);
}