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
#include "semphr.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
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
/*************************************************************************************************/

#define TASK1_PRIO      2                   /* priority */
#define TASK1_STK_SIZE  1024                 /* stack size */
TaskHandle_t  Task1_Handler;								/* handler */
void task1(void *pvParameters){
		uint32_t Freq=0;
    while(1)
    {	
				lv_task_handler();
        vTaskDelay(10);        
    }


};         

/*************************************************************************************************/
extern uint8_t adc1cplt;
extern uint16_t ADC_DMA_BUFFER[ADC_DMA_LENGTH*2];
extern ADC_HandleTypeDef hadc1;
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t  Task2_Handler;								
void task2(void *pvParameters){
	
		uint16_t i;
    while(1) {  
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_DMA_BUFFER, ADC_DMA_LENGTH * 2);
        if (xSemaphoreTake(adc1cpltsemaphoreHandle, pdMS_TO_TICKS(5000)) == pdTRUE) {
            HAL_ADC_Stop_DMA(&hadc1);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); /* 1s */
    }
}    


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
					if (hadc->Instance == ADC1) {
							printf("Semaphore give");
							if (adc1cpltsemaphoreHandle != NULL) {
									xSemaphoreGiveFromISR(adc1cpltsemaphoreHandle, &xHigherPriorityTaskWoken);
							}
							
							portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
					}

}
/*************************************************************************************************/
#define START_TASK_PRIO 1                   
#define START_STK_SIZE  512            
TaskHandle_t  StartTask_Handler;  			

void start_task(void *pvParameters){
   taskENTER_CRITICAL();       
 
		printf("start task in");
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1_Handler);
								
		xTaskCreate((TaskFunction_t )task2,
                (const char*    )"task2",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2_Handler);														
		vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();            


}
/*************************************************************************************************/
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

