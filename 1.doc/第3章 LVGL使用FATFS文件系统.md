# 第三章 LVGL使用FATFS文件系统

## 1. FATFS文件系统移植

LVGL 支持 POSIX、 WIN32、 STDIO 和 FATFS 文件系统接口， 而在小型的嵌入式系统中， FATFS 文件系统是较为常用的。 接下来，我们以 FATFS 为例， 介绍 LVGL 文件系统的移植：

1. 复制 FATFS 相关文件

2. 添加 FATFS 文件

在工程中，创建 Middlewares/FATFS 分组，添加 Middlewares/FATFS 文件夹下的文件，如下图所示：

![屏幕截图 2025-08-22 150152.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-02-26-屏幕截图%202025-08-22%20150152.png)

3. 添加硬件驱动文件

请根据实际的开发板，将 SD、 SPI、外部 FLASH 等硬件相关的文件添加到 Drivers/BSP分组中。如果工程中缺少 HAL 库相关的外设驱动，请一并添加到相应的分组！

4. 添加 LVGL 文件系统接口文件

新建 Middlewares/lvgl/src/fadrv 分组，添加 LVGL工程的 lvgl\src\extra\libs\fsdrv 文件夹下的lv_fs_fasfs.c 文件，如下图所示：

![屏幕截图 2025-08-22 150254.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-03-36-屏幕截图%202025-08-22%20150254.png)

5. 添加头文件路径，如下图所示：

![屏幕截图 2025-08-22 150352.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-03-59-屏幕截图%202025-08-22%20150352.png)

6. 打开 lv_conf.h 文件， 使能 FATFS 文件系统，如下图所示：

![屏幕截图 2025-08-22 150414.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-04-18-屏幕截图%202025-08-22%20150414.png)

7. 打开 lv_fs_fatfs.c 文件，修改 fs_init 函数， 如下源码所示：

```c
/**
 * @brief       初始化存储设备和文件系统
 * @param       无
 * @retval      无
 */
static void fs_init(void)
{
    /*Initialize the SD card and FatFS itself.
     *Better to do it in your code to keep this library untouched for easy updating*/
    uint8_t res;

    /* 初始化 SD 卡和 FatFS 本身
     * 最好在自己的库中完成，一遍以后更新 */
    while (sd_init())               /* 初始化 SD 卡 */
    {
        lcd_show_string(10, 10, 200, 24, 24, "SD Card Error!", RED);
        printf("SD Card Error, Please Check!\r\n");
        LED0_TOGGLE();
        HAL_Delay(200);
    }

    LED0(0);

    exfuns_init();                  /* 为 fatfs 相关变量申请内存 */
    res = f_mount(fs[0], "0:", 1);  /* 挂载 SD 卡 */

    if (0 != res)
    {
        lcd_show_string(10, 40, 200, 24, 24, "SD Card Mount Fail!", RED);
        printf("SD Card Mount Fail, Please Check!\r\n");
        LED0_TOGGLE();
        HAL_Delay(200);
    }
}
```

## 2. 文件系统原理解析

LVGL 文件系统和 FATFS 是类似的， 它只不过把 FATFS 的相关接口进行了封装， 其最终调用的还是 FATFS 文件系统的底层函数， 示意图如下：

![屏幕截图 2025-08-22 150542.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-05-46-屏幕截图%202025-08-22%20150542.png)

由上图可知， 当用户在应用层调用 lv_fs_open 函数时，该函数会调用 lv_fs_fatfs.c 文件中的 fs_open 函数， 而 fs_open 函数则会调用 FATFS 文件系统的接口函数（f_open） 。 值得注意的是， 其他的应用层接口函数调用流程也是类似的。

### 2.1 文件系统相关的 API 函数

LVGL 提供了一个抽象的文件系统接口，它本身不实现任何文件系统，而是定义了一套标准函数。这使得 LVGL 可以与任何底层文件系统（如 FatFs、LittleFS、POSIX 文件系统等）进行对接。你只需要提供一个驱动程序，将 LVGL 的 API 调用映射到你实际使用的文件系统函数上，就可以让 LVGL 具备文件读写能力。这对于需要从 SD 卡、SPI Flash 等存储设备中加载图片、字体等资源的场景非常有用。

| **函数**              | **描述**       |
| ------------------- | ------------ |
| **操作文件 API**        |              |
| `lv_fs_open()`      | 打开一个文件       |
| `lv_fs_close()`     | 关闭一个文件       |
| `lv_fs_read()`      | 读取文件内容       |
| `lv_fs_write()`     | 写数据到文件中      |
| `lv_fs_seek()`      | 设置文件读/写指针的位置 |
| `lv_fs_tell()`      | 获取文件读/写指针的位置 |
| **操作文件夹 API**       |              |
| `lv_fs_dir_open()`  | 打开一个目录       |
| `lv_fs_dir_read()`  | 读取目录中的下一个条目  |
| `lv_fs_dir_close()` | 关闭一个目录       |

#### 2.1.1 配置要求

1. 在 `lv_conf.h` 中将 `LV_USE_FS_...` 相关的宏定义为 1，以启用相应的文件系统接口（例如 `LV_USE_FS_STDIO`、`LV_USE_FS_POSIX` 或 `LV_USE_FS_FATFS`）。
2. 必须注册一个文件系统驱动。你需要创建一个 `lv_fs_drv_t` 类型的变量，为其分配盘符（如 'S'），并填充其内部的函数指针，最后通过 `lv_fs_drv_register()` 将其注册到 LVGL。

---

#### 2.1.2 操作文件 API

- `lv_fs_open()`
  
  - **描述：** 打开一个指定路径的文件，并指定操作模式（读、写）。
  - **函数原型：** `lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode);`
  - **参数：**
    - `file_p`: 指向一个 `lv_fs_file_t` 结构体变量的指针，用于保存文件的状态信息。
    - `path`: 文件的完整路径（包含盘符，例如 "S:/folder/file.txt"）。
    - `mode`: 文件的打开模式，可以是 `LV_FS_MODE_WR`（写）或 `LV_FS_MODE_RD`（读）的组合。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回其他错误码。

- `lv_fs_close()`
  
  - **描述：** 关闭一个已经打开的文件。
  - **函数原型：** `lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p);`
  - **参数：** `file_p` - 指向要关闭的文件的 `lv_fs_file_t` 结构体指针。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

- `lv_fs_read()`
  
  - **描述：** 从文件中读取指定字节数的数据到缓冲区。
  - **函数原型：** `lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);`
  - **参数：**
    - `file_p`: 指向要读取的文件的 `lv_fs_file_t` 结构体指针。
    - `buf`: 指向用于存储读取数据的缓冲区的指针。
    - `btr`: （Bytes To Read）想要读取的字节数。
    - `br`: （Bytes Read）指向一个 `uint32_t` 变量的指针，函数会通过它返回实际读取到的字节数。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

- `lv_fs_write()`
  
  - **描述：** 将缓冲区中的数据写入到文件中。
  - **函数原型：** `lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);`
  - **参数：**
    - `file_p`: 指向要写入的文件的 `lv_fs_file_t` 结构体指针。
    - `buf`: 指向包含要写入数据的缓冲区的指针。
    - `btw`: （Bytes To Write）想要写入的字节数。
    - `bw`: （Bytes Written）指向一个 `uint32_t` 变量的指针，函数会通过它返回实际写入的字节数。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

- `lv_fs_seek()`
  
  - **描述：** 在文件中移动读/写指针到指定位置。
  - **函数原型：** `lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence);`
  - **参数：**
    - `file_p`: 指向目标文件的 `lv_fs_file_t` 结构体指针。
    - `pos`: 偏移量。
    - `whence`: 偏移的起始位置。`LV_FS_SEEK_SET`（文件开头），`LV_FS_SEEK_CUR`（当前位置），`LV_FS_SEEK_END`（文件末尾）。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

- `lv_fs_tell()`
  
  - **描述：** 获取当前读/写指针在文件中的位置。
  - **函数原型：** `lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t * pos_p);`
  - **参数：**
    - `file_p`: 指向目标文件的 `lv_fs_file_t` 结构体指针。
    - `pos_p`: 指向一个 `uint32_t` 变量的指针，函数会通过它返回当前指针的位置。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

---

#### 2.1.2 操作文件夹 API

- `lv_fs_dir_open()`
  
  - **描述：** 打开一个目录以准备读取其内容。
  - **函数原型：** `lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path);`
  - **参数：**
    - `rddir_p`: 指向一个 `lv_fs_dir_t` 结构体变量的指针，用于保存目录的状态信息。
    - `path`: 目录的完整路径（包含盘符）。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。

- `lv_fs_dir_read()`
  
  - **描述：** 读取目录中的下一个条目（文件或子目录）的名称。
  - **函数原型：** `lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn);`
  - **参数：**
    - `rddir_p`: 指向已打开目录的 `lv_fs_dir_t` 结构体指针。
    - `fn`: 指向一个字符数组（缓冲区）的指针，用于存储读取到的条目名称。
  - **返回值：** 如果成功读取一个条目，返回 `LV_FS_RES_OK`。如果已无更多条目可读，返回 `LV_FS_RES_OK` 但 `fn[0]` 会被置为 `\0`。如果发生错误，则返回其他错误码。
  - **注意事项：** 为 `fn` 提供的缓冲区需要足够大，以容纳可能的最长文件名。

- `lv_fs_dir_close()`
  
  - **描述：** 关闭一个已经打开的目录。
  - **函数原型：** `lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p);`
  - **参数：** `rddir_p` - 指向要关闭的目录的 `lv_fs_dir_t` 结构体指针。
  - **返回值：** 如果操作成功，返回 `LV_FS_RES_OK`；否则返回错误码。









## 3. 文件系统测试

主要测试 LVGL 文件系统的文件读取功能。 实验现象： SD 卡中的测试文件将会被打开并读取其内容， 然后把得到的内容打印到串口。 与此同时， LED0 闪烁， 提示系统正在运行.

### 3.1 程序流程图

![屏幕截图 2025-08-22 152353.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-15-23-57-屏幕截图%202025-08-22%20152353.png)

### 3.2 FreeRTOS任务配置

```c
/******************************************************************************************************/
/* 任务配置 */

#define START_TASK_PRIO     1           // 任务优先级
#define START_STK_SIZE      128         // 任务堆栈大小 
TaskHandle_t StartTask_Handler;         // 任务句柄 
void start_task(void *pvParameters);    // 任务函数 

#define LV_DEMO_TASK_PRIO   3          
#define LV_DEMO_STK_SIZE    1024        
TaskHandle_t LV_DEMOTask_Handler;      
void lv_demo_task(void *pvParameters); 

#define LED_TASK_PRIO       4           
#define LED_STK_SIZE        128        
TaskHandle_t LEDTask_Handler;          
void led_task(void *pvParameters);     
/******************************************************************************************************/
```

### 3.3 FreeRTOS任务实现

```c
// FreeRTOS启动
void lvgl_demo(void)
{
    lv_init();            // lvgl系统初始化 
    lv_port_disp_init();  // lvgl显示接口初始化,放在lv_init()的后面 
    lv_port_indev_init(); // lvgl输入接口初始化,放在lv_init()的后面
    xTaskCreate((TaskFunction_t )start_task,         // 任务函数 
                (const char*    )"start_task",       // 任务名称 
                (uint16_t       )START_STK_SIZE,     // 任务堆栈大小 
                (void*          )NULL,               // 传递给任务函数的参数 
                (UBaseType_t    )START_TASK_PRIO,    // 任务优先级 
                (TaskHandle_t*  )&StartTask_Handler);// 任务句柄 
    vTaskStartScheduler(); // 开启任务调度 
}

// 开始任务
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); 
    /* 创建LVGL任务 */
    xTaskCreate((TaskFunction_t )lv_demo_task,
                (const char*    )"lv_demo_task",
                (uint16_t       )LV_DEMO_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LV_DEMO_TASK_PRIO,
                (TaskHandle_t*  )&LV_DEMOTask_Handler);
    /* LED测试任务 */
    xTaskCreate((TaskFunction_t )led_task,
                (const char*    )"led_task",
                (uint16_t       )LED_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )LED_TASK_PRIO,
                (TaskHandle_t*  )&LEDTask_Handler);
    taskEXIT_CRITICAL();            
    vTaskDelete(StartTask_Handler);
}

// LVGL运行例程任务
void lv_demo_task(void *pvParameters)
{
    lv_mainstart(); // 要测试的demo
    while(1)
    {
        lv_timer_handler(); // LVGL计时器
        vTaskDelay(5);
    }
}

// LED测试任务
void led_task(void *pvParameters)
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(1000);
    }
}
```

### 3.4 文件系统测试

```c
#include "lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

/**
 * @brief  获取指针位置
 * @param  fd：文件指针
 * @return 返回名称
 */
long lv_tell(lv_fs_file_t *fd)
{
    uint32_t pos = 0;
    lv_fs_tell(fd, &pos); // 获取文件指针位置
    return pos;
}

/**
 * @brief  文件系统测试
 * @param  无
 * @return 无
 */
static void lv_fs_test(void)
{
    char rbuf[30] = {0};
    uint32_t rsize = 0;
    lv_fs_file_t fd;
    lv_fs_res_t res;
    res = lv_fs_open(&fd, "0:/SYSTEM/LV_FATFS/Fatfs_test.txt", LV_FS_MODE_RD); // 尝试打开测试文件
    if (res != LV_FS_RES_OK) // 打开失败
    {
        printf("open 0:/Fatfs_test.txt ERROR\n");
        return ;
    }
    lv_tell(&fd); // 获取文件指针位置
    lv_fs_seek(&fd, 0,LV_FS_SEEK_SET); // 回到文件起始位置
    lv_tell(&fd);
    res = lv_fs_read(&fd, rbuf, 100, &rsize); // 读取文件内容
    if (res != LV_FS_RES_OK)
    {
        printf("read ERROR\n");
        return ;
    }
    lv_tell(&fd);
    printf("READ(%d): %s",rsize , rbuf); // 打印文件内容
    lv_fs_close(&fd); // 关闭文件
}

/**
 * @brief  文件系统演示
 * @param  无
 * @return 无
 */
void lv_mainstart(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act()); // 创建标签
    lv_label_set_text(label, "FATFS TEST"); // 设置标签内容
    lv_obj_center(label); // 居中显示
    lv_fs_test(); // 文件系统测试
}
```

### 3.5 主函数

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "mpu.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "lvgl_demo.h"
#include "lv_conf.h"

int main(void)
{
    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    lvgl_demo();                        /* 运行lvgl示例程 */
}
```

---


