/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for IdleTask */
osThreadId_t IdleTaskHandle;
uint32_t IdleTaskBuffer[ 128 ];
osStaticThreadDef_t IdleTaskControlBlock;
const osThreadAttr_t IdleTask_attributes = {
  .name = "IdleTask",
  .stack_mem = &IdleTaskBuffer[0],
  .stack_size = sizeof(IdleTaskBuffer),
  .cb_mem = &IdleTaskControlBlock,
  .cb_size = sizeof(IdleTaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for chassisTask */
osThreadId_t chassisTaskHandle;
uint32_t chassisTaskBuffer[ 1024 ];
osStaticThreadDef_t chassisTaskControlBlock;
const osThreadAttr_t chassisTask_attributes = {
  .name = "chassisTask",
  .stack_mem = &chassisTaskBuffer[0],
  .stack_size = sizeof(chassisTaskBuffer),
  .cb_mem = &chassisTaskControlBlock,
  .cb_size = sizeof(chassisTaskControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for gimbalTask */
osThreadId_t gimbalTaskHandle;
uint32_t gimbalTaskBuffer[ 1024 ];
osStaticThreadDef_t gimbalTaskControlBlock;
const osThreadAttr_t gimbalTask_attributes = {
  .name = "gimbalTask",
  .stack_mem = &gimbalTaskBuffer[0],
  .stack_size = sizeof(gimbalTaskBuffer),
  .cb_mem = &gimbalTaskControlBlock,
  .cb_size = sizeof(gimbalTaskControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for shootTask */
osThreadId_t shootTaskHandle;
uint32_t shootTaskBuffer[ 512 ];
osStaticThreadDef_t shootTaskControlBlock;
const osThreadAttr_t shootTask_attributes = {
  .name = "shootTask",
  .stack_mem = &shootTaskBuffer[0],
  .stack_size = sizeof(shootTaskBuffer),
  .cb_mem = &shootTaskControlBlock,
  .cb_size = sizeof(shootTaskControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for commonTask */
osThreadId_t commonTaskHandle;
uint32_t commonTaskBuffer[ 512 ];
osStaticThreadDef_t commonTaskControlBlock;
const osThreadAttr_t commonTask_attributes = {
  .name = "commonTask",
  .stack_mem = &commonTaskBuffer[0],
  .stack_size = sizeof(commonTaskBuffer),
  .cb_mem = &commonTaskControlBlock,
  .cb_size = sizeof(commonTaskControlBlock),
  .priority = (osPriority_t) osPriorityRealtime,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartIdleTask(void *argument);
extern void chassisTaskEntry(void *argument);
extern void gimbalTaskEntry(void *argument);
extern void shootTaskEntry(void *argument);
extern void commonTaskEntry(void *argument);

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
  /* creation of IdleTask */
  IdleTaskHandle = osThreadNew(StartIdleTask, NULL, &IdleTask_attributes);

  /* creation of chassisTask */
  chassisTaskHandle = osThreadNew(chassisTaskEntry, NULL, &chassisTask_attributes);

  /* creation of gimbalTask */
  gimbalTaskHandle = osThreadNew(gimbalTaskEntry, NULL, &gimbalTask_attributes);

  /* creation of shootTask */
  shootTaskHandle = osThreadNew(shootTaskEntry, NULL, &shootTask_attributes);

  /* creation of commonTask */
  commonTaskHandle = osThreadNew(commonTaskEntry, NULL, &commonTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartIdleTask */
/**
  * @brief  Function implementing the IdleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartIdleTask */
void StartIdleTask(void *argument)
{
  /* USER CODE BEGIN StartIdleTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIdleTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
