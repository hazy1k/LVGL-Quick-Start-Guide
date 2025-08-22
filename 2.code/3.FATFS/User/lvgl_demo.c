#include "lvgl_demo.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_mainstart.h"

/******************************************************************************************************/
/* �������� */

#define START_TASK_PRIO     1           // �������ȼ�
#define START_STK_SIZE      128         // �����ջ��С 
TaskHandle_t StartTask_Handler;         // ������ 
void start_task(void *pvParameters);    // ������ 

#define LV_DEMO_TASK_PRIO   3          
#define LV_DEMO_STK_SIZE    1024        
TaskHandle_t LV_DEMOTask_Handler;      
void lv_demo_task(void *pvParameters); 

#define LED_TASK_PRIO       4           
#define LED_STK_SIZE        128        
TaskHandle_t LEDTask_Handler;          
void led_task(void *pvParameters);     
/******************************************************************************************************/

// FreeRTOS����
void lvgl_demo(void)
{
    lv_init();            // lvglϵͳ��ʼ�� 
    lv_port_disp_init();  // lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ��� 
    lv_port_indev_init(); // lvgl����ӿڳ�ʼ��,����lv_init()�ĺ���
    xTaskCreate((TaskFunction_t )start_task,         // ������ 
                (const char*    )"start_task",       // �������� 
                (uint16_t       )START_STK_SIZE,     // �����ջ��С 
                (void*          )NULL,               // ���ݸ��������Ĳ��� 
                (UBaseType_t    )START_TASK_PRIO,    // �������ȼ� 
                (TaskHandle_t*  )&StartTask_Handler);// ������ 
    vTaskStartScheduler(); // ����������� 
}

// ��ʼ����
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); 
    /* ����LVGL���� */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);
    /* LED�������� */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);
    taskEXIT_CRITICAL();            
    vTaskDelete(StartTask_Handler);
}

// LVGL������������
void lv_demo_task(void *pvParameters)
{
    lv_mainstart(); // Ҫ���Ե�demo
    while(1)
    {
        lv_timer_handler(); // LVGL��ʱ��
        vTaskDelay(5);
    }
}

// LED��������
void led_task(void *pvParameters)
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(1000);
    }
}
