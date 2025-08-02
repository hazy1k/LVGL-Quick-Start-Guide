#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "mpu.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "btim.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "lv_demo_music.h"

int main(void)
{
    sys_cache_enable();                 /* ��L1-Cache */
    HAL_Init();                         /* ��ʼ��HAL�� */
    sys_stm32_clock_init(240, 2, 2, 4); /* ����ʱ��, 480Mhz */
    delay_init(480);                    /* ��ʱ��ʼ�� */
    usart_init(115200);                 /* ���ڳ�ʼ��Ϊ115200 */
    led_init();                         /* ��ʼ��LED */
    mpu_memory_protection();            /* ������ش洢���� */
    lcd_init();                         /* ��ʼ��LCD */
    key_init();                         /* ��ʼ������ */
    my_mem_init(SRAMIN);                /* ��ʼ���ڲ��ڴ��(AXI) */
    btim_timx_int_init(10-1, 24000-1);  /* ��ʼ����ʱ�� */
    
    lv_init();                          /* lvglϵͳ��ʼ�� */
    lv_port_disp_init();                /* lvgl��ʾ�ӿڳ�ʼ��,����lv_init()�ĺ��� */
    lv_port_indev_init();               /* lvgl����ӿڳ�ʼ��,����lv_init()�ĺ��� */
    lv_demo_music();                    /* �ٷ����� */

    while (1)
    {
        lv_timer_handler();             /* lvgl��ʱ�� */
    }
}
