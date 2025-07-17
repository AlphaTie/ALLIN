/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "semphr.h"
#include "cal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_DMA_LENGTH 1024
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */



/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for adc1cpltsemaphore */
osSemaphoreId_t adc1cpltsemaphoreHandle;
const osSemaphoreAttr_t adc1cpltsemaphore_attributes = {
  .name = "adc1cpltsemaphore"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

extern IWDG_HandleTypeDef hiwdg;

/*************************************************************************************************/

#define TASK1_PRIO      1                   /* priority */
#define TASK1_STK_SIZE  1024                 /* stack size */
TaskHandle_t  Task1_Handler;								/* handler */
void task1(void *pvParameters){
		uint32_t Freq=0;
    while(1)
    {	
				lv_task_handler();
				HAL_IWDG_Refresh(&hiwdg);
        vTaskDelay(10);        
    }


};         

/*************************************************************************************************/
/**
 @note three ADC Handler share one public task function 
*/
extern uint16_t ADC_DMA_BUFFER[ADC_DMA_LENGTH];
extern uint16_t ADC2_DMA_BUFFER[ADC_DMA_LENGTH];
extern uint16_t ADC3_DMA_BUFFER[ADC_DMA_LENGTH];
extern float FFT_OUTPUT[ADC_DMA_LENGTH];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
#define ADCTASK_PRIO      2                 /* Priority   */                   
#define ADCTASK_STK_SIZE  512                /* Stacksize */                 
TaskHandle_t  ADCTask_Handler[3];							/* Handler    */
void ADCprocess(void *pvParameters){
	
		
		uint8_t adc_id= (uint32_t) pvParameters;  
    while(1) {  
        // HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DMA_BUFFER, ADC_DMA_LENGTH * 2);
        // if (xSemaphoreTake(adc1cpltsemaphoreHandle, pdMS_TO_TICKS(5000)) == pdTRUE) {
        //     HAL_ADC_Stop_DMA(&hadc1);
				uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
				
        // }
				if(ulNotificationValue > 0) {
					if(adc_id==1){
						HAL_ADC_Stop_DMA(&hadc1);
						fir_test(ADC_DMA_BUFFER);
            printf("ADC1\n");
						vTaskDelay(pdMS_TO_TICKS(1000));
						HAL_ADC_Start_DMA(&hadc2, (uint32_t *)ADC2_DMA_BUFFER, ADC_DMA_LENGTH);
					}
					else if(adc_id==2){
						HAL_ADC_Stop_DMA(&hadc2);
            printf("ADC2\n");
						vTaskDelay(pdMS_TO_TICKS(1));
						HAL_ADC_Start_DMA(&hadc3, (uint32_t *)ADC3_DMA_BUFFER, ADC_DMA_LENGTH);
					}
					else if(adc_id==3){
						HAL_ADC_Stop_DMA(&hadc3);
            printf("ADC3\n");
						vTaskDelay(pdMS_TO_TICKS(1));
						HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DMA_BUFFER, ADC_DMA_LENGTH);
					}
					HAL_IWDG_Refresh(&hiwdg);
				}	
					vTaskDelay(pdMS_TO_TICKS(10)); /* 1s */
					
    }
}    


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
			if (hadc->Instance == ADC1) {
					
					vTaskNotifyGiveFromISR(ADCTask_Handler[0], &xHigherPriorityTaskWoken);
					}
			else if(hadc->Instance == ADC2){
					
					vTaskNotifyGiveFromISR(ADCTask_Handler[1], &xHigherPriorityTaskWoken);
					}
      else if(hadc->Instance == ADC3){
           
          vTaskNotifyGiveFromISR(ADCTask_Handler[2], &xHigherPriorityTaskWoken);
          }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
/*************************************************************************************************/
#define START_TASK_PRIO 3                  
#define START_STK_SIZE  512            
TaskHandle_t  StartTask_Handler;  			

void start_task(void *pvParameters){
        
 
		printf("start task in");
    if(xTaskCreate((TaskFunction_t )task1,
									(const char*    )"task1",
									(uint16_t       )TASK1_STK_SIZE,
									(void*          )NULL,
									(UBaseType_t    )TASK1_PRIO,
									(TaskHandle_t*  )&Task1_Handler)!= pdPASS) {
        printf("Failed to create LVGL task\n");
        
    }
								
    if (xTaskCreate((TaskFunction_t )ADCprocess, "adc task", ADCTASK_STK_SIZE, (void* )1, ADCTASK_PRIO, &ADCTask_Handler[0]) != pdPASS) {
        printf("Failed to create ADC task 1\n");
        
    }
    if (xTaskCreate((TaskFunction_t )ADCprocess, "adc task2", ADCTASK_STK_SIZE, (void* )2, ADCTASK_PRIO, &ADCTask_Handler[1]) != pdPASS) {
        printf("Failed to create ADC task 2\n");
        while(1);
    }
    if (xTaskCreate((TaskFunction_t )ADCprocess, "adc task3", ADCTASK_STK_SIZE, (void* )3, ADCTASK_PRIO, &ADCTask_Handler[2]) != pdPASS) {
        printf("Failed to create ADC task 3\n");
        while(1);
    }

    printf("All tasks created successfully. Starting ADC1...\n");
			

				
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DMA_BUFFER, ADC_DMA_LENGTH);
		vTaskDelay(pdMS_TO_TICKS(100));
		
		vTaskDelete(StartTask_Handler); 
          


}
/*************************************************************************************************/
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 3 */
void vApplicationTickHook(void)
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
	lv_tick_inc(1);
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	printf("OS Stack OverFlow");
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of adc1cpltsemaphore */
  adc1cpltsemaphoreHandle = osSemaphoreNew(1, 1, &adc1cpltsemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	xTaskCreate((TaskFunction_t )start_task,
              (const char*    )"start_task",
              (uint16_t       )START_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )START_TASK_PRIO,
              (TaskHandle_t*  )&StartTask_Handler);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

