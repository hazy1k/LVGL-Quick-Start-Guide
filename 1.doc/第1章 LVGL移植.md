# 第一章 LVGL移植

## 1. LVGL初始化流程

在学习 LVGL移植相关知识之前，我们需要先简单地了解一下 LVGL的初始化流程，这样即可知道整个初始化过程中所涉及的一些关键配置，而这些关键配置是在移植过程中需要重点关注的。 LVGL 初始化流程分为以下几个步骤：

1. 调用 lv_init 函数， 初始化 LVGL 图形库。 在这一步的初始化中，涉及的内容非常的多，包括：内存管理、文件系统、定时器，等等。

2. 调用 lv_port_disp_init函数和 lv_port_indev_init函数， 注册显示设备和输入设备。注意： 在注册显示设备和输入设备之前，必须先初始化 LVGL 图形库（调用 lv_init 函数） 。

3. 为 LVGL 提供时基。 用户可以使用定时器，在其中断里面定时调用 lv_tick_inc 函数，为 LVGL 提供时基。 如果工程中带有 OS 操作系统，则可以使用相应的时钟函数来为LVGL 提供时基。

4. 定时处理 LVGL 任务。 用户需要每隔几毫秒调用一次 lv_timer_handler 函数， 以处理 LVGL 相关的任务，该函数可以放在 while 循环中，但延时不宜过大， 需要确保 5 毫秒以内

## 2.lv_conf.h文件解析

首先，我们需要明确一个点： lv_conf.h 是一个用户级别的文件， 它不属于内核的部分，因此， 在不同的工程中， 该文件有可能存在差异。 lv_conf.h 文件具有两大功能：

1. 配置功能：内存、 屏幕刷新周期、输入设备的读取周期， 等等； 

2. 裁剪功能： 使能/失能某些功能，有效地优化 Flash 的分配。

| 模块介绍             | 描述                          |
| ---------------- | --------------------------- |
| **颜色设置**         | 颜色深度、屏幕透明                   |
| **内存设置**         | 分配内存大小                      |
| **HAL(硬件抽象层)设置** | 刷新周期、输入设备读取周期以及 `tick` 时钟来源 |
| **特征设置**         | 绘画、加速刷新、日志、断言以及帧率计算         |
| **编译器设置**        | 设置编译器的配置                    |
| **字体设置**         | 选择系统字体以及声明自定义字体             |
| **文本设置**         | 设置字符编码、文本特性                 |
| **部件设置**         | 使能/失能核心部件                   |
| **特别功能**         | 额外的部件、主题、布局、第三方库            |
| **实例设置**         | 开启/关闭 LVGL 演示实例             |

### 2.1 颜色设置 (Color Settings)

这个模块配置了 LVGL 如何处理颜色。

- **颜色深度 (Color Depth):** `LV_COLOR_DEPTH`。这是最重要的颜色设置，定义了每个像素使用多少位来表示颜色（例如，16位、24位、32位）。
  - **影响：** 颜色深度越高，能显示的颜色数量越多，图像质量越好，但内存占用和渲染时间也越大。需要根据实际显示屏的颜色深度来设置。
- **屏幕透明 (Screen Transparency):** `LV_COLOR_SCREEN_TRANSP`。通常用于支持屏幕半透明或透明显示。
  - **影响：** 启用透明度会增加内存和渲染开销。

```c
/*
*颜色设置
*/
/* 颜色深度: 1(每像素 1 字节), 8(RGB332), 16(RGB565), 32(ARGB8888) */
#define LV_COLOR_DEPTH 16
/* 交换 2 字节的 RGB565 颜色。如果显示有 8 位接口(例如 SPI) */
#define LV_COLOR_16_SWAP 0
/* 1: 启用屏幕透明.
* 对 OSD 或其他有重叠的 gui 很有用.
* 要求' LV_COLOR_DEPTH = 32 '颜色和屏幕的样式应该被修改: `style.body.opa = ...`*/
#define LV_COLOR_SCREEN_TRANSP 0
/* 调整颜色混合功能四舍五入。 gpu 可能会以不同的方式计算颜色混合。
* 0:取整， 64:从 x.75 取整， 128:从 half 取整， 192:从 x.25 取整， 254:从 half 取整 */
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0: 128)
/* 如果使用色度键，将不会绘制这种颜色的图像像素) */
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00) /* 纯绿 */
```

### 2.2 内存设置 (Memory Settings)

此模块配置了 LVGL 内部动态内存分配的方式和大小。

- **分配内存大小 (`LV_MEM_SIZE`):** 定义 LVGL 内部堆的总大小，LVGL 的所有动态内存分配（如创建对象、图片缓存等）都将从这个堆中进行。
  - **影响：** 设置过小可能导致内存不足，过大则浪费内存。需要根据应用程序中将要创建的对象的数量和大小进行估算。
- **内存分配器 (`LV_MEM_CUSTOM`):** 可以选择使用 LVGL 自带的内存分配器，或者使用自定义的 `malloc`/`free` 函数（例如，FreeRTOS 的 `pvPortMalloc`/`vPortFree`）。
  - **影响：** 如果使用 FreeRTOS，通常会配置为使用 FreeRTOS 的堆管理函数，以确保内存管理的统一性和线程安全。

### 2.3 HAL (硬件抽象层) 设置

此模块配置了与底层硬件交互相关的周期和时钟来源。

- **刷新周期 (`LV_DISP_DEF_REFR_PERIOD`):** LVGL 建议的显示器刷新周期，以毫秒为单位。这是一个指导值，实际刷新由你的显示驱动决定。
  - **影响：** 影响屏幕的流畅度。
- **输入设备读取周期 (`LV_INDEV_DEF_READ_PERIOD`):** LVGL 建议的输入设备（如触摸屏、按键）读取周期，以毫秒为单位。
  - **影响：** 影响用户输入的响应速度。
- **`tick` 时钟来源 (`LV_TICK_CUSTOM` / `LV_TICK_TIMER_INCLUDE` / `LV_TICK_TIMER_ISR`):** LVGL 需要一个周期性的“tick”事件来驱动其内部定时器和动画。这通常通过一个硬件定时器中断来实现。
  - **影响：** LVGL 的所有时间相关功能都依赖于这个 `tick`。需要确保 `tick` 源稳定且与 `LV_TICK_CUSTOM` 或 `LV_TICK_RATE_HZ` 等配置匹配。

### 2.4 特征设置 (Feature Settings)

此模块配置了 LVGL 的各种高级功能和调试选项。

- **绘画 (`LV_USE_GPU_XXX`):** 是否启用 GPU 加速功能（如果硬件支持）。
  - **影响：** 启用 GPU 可以显著提升渲染性能，但需要相应的硬件支持和驱动实现。
- **加速刷新 (`LV_USE_FLEX` / `LV_USE_GRID` 等布局管理器):** 启用 LVGL 提供的布局管理器。
  - **影响：** 布局管理器可以简化UI设计，但会增加一些代码量。
- **日志 (`LV_USE_LOG`):** 启用 LVGL 的内部日志系统，用于输出调试信息。
  - **影响：** 调试时非常有用，但在发布版本中通常禁用以节省资源。
- **断言 (`LV_USE_ASSERT_MEM` / `LV_USE_ASSERT_OBJ` 等):** 启用 LVGL 内部的断言检查，用于在开发阶段捕获错误。
  - **影响：** 开发时很有用，但在发布版本中通常禁用以避免运行时开销。
- **帧率计算 (`LV_USE_PERF_MONITOR`):** 启用性能监视器，可以实时显示帧率、CPU使用率等信息。
  - **影响：** 调试和优化UI性能时很有用，但在发布版本中通常禁用。

### 2.5 编译器设置 (Compiler Settings)

此模块通常用于配置编译器相关的优化和特性。

- 例如，`LV_ATTRIBUTE_FAST_MEM`, `LV_ATTRIBUTE_TASK_STACK_ALIGN` 等，用于优化内存访问和任务堆栈对齐。
  - **影响：** 影响编译器的代码生成和性能。通常在特定平台或编译器下进行调整。

### 2.6 字体设置 (Font Settings)

此模块配置了 LVGL 将使用的字体。

- **选择系统字体 (`LV_FONT_DEFAULT`):** 设置默认的字体。LVGL 提供了多种内置字体，或可以导入自定义字体。
  - **影响：** 影响UI文本的显示效果和内存占用。字体越大、字形越复杂，占用的Flash或RAM空间越多。
- **声明自定义字体:** 允许你通过工具将自定义字体转换为 LVGL 格式，并在 `lv_conf.h` 中声明，以便在UI中使用。

### 2.7 文本设置 (Text Settings)

此模块配置了文本渲染和编码相关的特性。

- **字符编码 (`LV_TXT_ENC_UTF8`):** 是否使用 UTF-8 编码来处理文本。
  - **影响：** 支持多语言显示的关键。
- **文本特性 (`LV_USE_BIDI` / `LV_USE_TEXT_SCROLL_ANIM` 等):** 是否启用双向文本（例如阿拉伯语、希伯来语）支持，文本滚动动画等。
  - **影响：** 提供更丰富的文本显示效果，但会增加代码量和资源消耗。

### 2.8 部件设置 (Widget Settings)

此模块用于使能或失能 LVGL 的各个核心部件（Widgets）。

- **使能/失能核心部件 (`LV_USE_BTN` / `LV_USE_LABEL` / `LV_USE_SLIDER` 等):** LVGL 提供了大量的内置部件，如按钮、标签、滑块、列表、图表等。你可以根据你的UI设计只启用你需要的部件，以减小编译后的库大小。
  - **影响：** 未使用的部件应该被禁用，以减少代码占用和内存。这是优化 LVGL 库大小的重要手段。

### 2.9 特别功能 (Special Features)

此模块配置了 LVGL 的额外功能、主题、布局和集成第三方库。

- **额外的部件:** 例如 `LV_USE_CANVAS`, `LV_USE_CHART` 等，这些通常是更复杂的部件。
- **主题 (`LV_USE_THEME_DEFAULT` / `LV_USE_THEME_MONO` 等):** LVGL 提供了多种内置主题，可以快速改变UI的整体风格。
- **布局 (`LV_USE_FLEX` / `LV_USE_GRID`):** 启用弹性布局或网格布局管理器，方便进行复杂UI的排布。
- **第三方库 (`LV_USE_QRCODE` / `LV_USE_PNG` / `LV_USE_JPG` 等):** 是否集成二维码生成、PNG/JPG 图片解码等第三方功能。
  - **影响：** 启用这些功能会增加库的大小，并且可能需要额外配置和链接相应的第三方库。

### 2.10 实例设置 (Example Settings)

此模块用于开启或关闭 LVGL 提供的演示实例。

- **开启/关闭 LVGL 演示实例 (`LV_USE_DEMO_WIDGETS` / `LV_USE_DEMO_BENCHMARK` 等):** LVGL 提供了一系列演示代码和基准测试，用于展示其功能和性能。
  - **影响：** 这些实例代码仅用于开发和测试，在发布版本中通常应该禁用，以避免不必要的代码和资源占用。

## 3. 显示接口

在绘制 UI 之前， LVGL 必须要注册一个绘制缓冲区以及显示驱动， 该显示驱动可以把像素阵列（在缓冲区中） 复制到显示器的指定区域。

| 函数                               | 描述            |
| -------------------------------- | ------------- |
| `lv_disp_drv_init()`             | 初始化显示驱动       |
| `lv_disp_draw_buf_init()`        | 初始化显示缓冲区      |
| `lv_disp_drv_register()`         | 注册一个显示驱动      |
| `lv_disp_drv_update()`           | 在运行时更新驱动程序    |
| `lv_disp_remove()`               | 移除显示器         |
| `lv_disp_set_default()`          | 设置默认显示设备      |
| `lv_disp_get_default()`          | 获取默认显示设备      |
| `lv_disp_get_hor_res()`          | 获取显示器的水平分辨率   |
| `lv_disp_get_ver_res()`          | 获取显示器的垂直分辨率   |
| `lv_disp_get_physical_hor_res()` | 获取显示器的物理水平分辨率 |
| `lv_disp_get_physical_ver_res()` | 获取显示器的物理垂直分辨率 |
| `lv_disp_get_offset_x()`         | 获取物理显示的水平偏移   |
| `lv_disp_get_offset_y()`         | 获取物理显示的垂直偏移   |
| `lv_disp_get_antialiasing()`     | 获取是否启用了抗锯齿功能  |
| `lv_disp_get_dpi()`              | 获取显示器的 DPI    |
| `lv_disp_set_rotation()`         | 设置旋转          |
| `lv_disp_get_rotation()`         | 获取当前旋转参数      |
| `lv_disp_get_next()`             | 获取下一个显示设备     |
| `lv_disp_get_draw_buf`           | 获取显示器的内部缓冲区   |

### 3.1 显示驱动注册与管理

- **`lv_disp_drv_init()`**
  
  - **描述：** 初始化一个 `lv_disp_drv_t` 结构体变量。这个结构体包含了显示驱动的所有配置信息和回调函数。在使用显示驱动前必须先初始化。
  - **函数原型：** `void lv_disp_drv_init( lv_disp_drv_t *drv );`
  - **参数：** `drv` - 指向要初始化的显示驱动结构体的指针。
  - **注意事项：** 这只是初始化结构体，并不注册驱动。

- **`lv_disp_draw_buf_init()`**
  
  - **描述：** 初始化一个 `lv_disp_draw_buf_t` 结构体变量，用于管理 LVGL 的绘图缓冲区。
  - **函数原型：** `void lv_disp_draw_buf_init( lv_disp_draw_buf_t *draw_buf, void *buf1, void *buf2, uint32_t buf_size );`
  - **参数：**
    - `draw_buf`: 指向要初始化的绘图缓冲区结构体的指针。
    - `buf1`: 第一个绘图缓冲区（必填）。
    - `buf2`: 第二个绘图缓冲区（可选，用于双缓冲）。传入 `NULL` 表示单缓冲。
    - `buf_size`: 缓冲区大小，以像素点数量为单位。
  - **注意事项：** 绘图缓冲区是 LVGL 渲染画面的地方，然后这些数据通过显示驱动复制到实际的屏幕。双缓冲可以减少画面撕裂。

- **`lv_disp_drv_register()`**
  
  - **描述：** 注册一个显示驱动到 LVGL 核心。一个系统可以注册多个显示驱动，但通常只有一个是默认的。
  - **函数原型：** `lv_disp_t *lv_disp_drv_register( lv_disp_drv_t *drv );`
  - **参数：** `drv` - 指向已初始化并配置好的显示驱动结构体的指针。
  - **返回值：** 注册成功的显示器句柄。

- **`lv_disp_drv_update()`**
  
  - **描述：** 在运行时更新一个已注册显示驱动的配置。例如，可以更改分辨率、偏移等。
  - **函数原型：** `void lv_disp_drv_update( lv_disp_t *disp, lv_disp_drv_t *new_drv );`
  - **参数：**
    - `disp`: 要更新的显示器句柄。
    - `new_drv`: 指向包含新配置的显示驱动结构体的指针。

- **`lv_disp_remove()`**
  
  - **描述：** 从 LVGL 中移除一个已注册的显示器。
  - **函数原型：** `void lv_disp_remove( lv_disp_t *disp );`
  - **参数：** `disp` - 要移除的显示器句柄。

### 3.2 默认显示设备操作

- **`lv_disp_set_default()`**
  
  - **描述：** 设置当前默认的显示设备。所有没有明确指定显示器的对象，都将在默认显示器上创建。
  - **函数原型：** `void lv_disp_set_default( lv_disp_t *disp );`
  - **参数：** `disp` - 要设置为默认的显示器句柄。

- **`lv_disp_get_default()`**
  
  - **描述：** 获取当前默认显示设备的句柄。
  - **函数原型：** `lv_disp_t *lv_disp_get_default( void );`
  - **返回值：** 默认显示设备的句柄。

### 3.3 显示器属性获取

- **`lv_disp_get_hor_res()`**
  
  - **描述：** 获取显示器的水平分辨率（逻辑分辨率）。
  - **函数原型：** `lv_coord_t lv_disp_get_hor_res( lv_disp_t *disp );`
  - **参数：** `disp` - 显示器句柄。如果为 `NULL`，则获取默认显示器的分辨率。

- **`lv_disp_get_ver_res()`**
  
  - **描述：** 获取显示器的垂直分辨率（逻辑分辨率）。
  - **函数原型：** `lv_coord_t lv_disp_get_ver_res( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_physical_hor_res()`**
  
  - **描述：** 获取显示器的物理水平分辨率。在屏幕有偏移或部分显示时可能与逻辑分辨率不同。
  - **函数原型：** `lv_coord_t lv_disp_get_physical_hor_res( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_physical_ver_res()`**
  
  - **描述：** 获取显示器的物理垂直分辨率。
  - **函数原型：** `lv_coord_t lv_disp_get_physical_ver_res( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_offset_x()`**
  
  - **描述：** 获取物理显示器的水平偏移。
  - **函数原型：** `lv_coord_t lv_disp_get_offset_x( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_offset_y()`**
  
  - **描述：** 获取物理显示器的垂直偏移。
  - **函数原型：** `lv_coord_t lv_disp_get_offset_y( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_antialiasing()`**
  
  - **描述：** 获取是否启用了抗锯齿功能。
  - **函数原型：** `bool lv_disp_get_antialiasing( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

- **`lv_disp_get_dpi()`**
  
  - **描述：** 获取显示器的每英寸点数 (DPI)。
  - **函数原型：** `uint32_t lv_disp_get_dpi( lv_disp_t *disp );`
  - **参数：** 同 `lv_disp_get_hor_res()`。

### 3.4 屏幕旋转 API 函数

- **`lv_disp_set_rotation()`**
  
  - **描述：** 设置显示器的旋转角度。LVGL 会自动处理旋转后的坐标转换。
  - **函数原型：** `void lv_disp_set_rotation( lv_disp_t *disp, lv_display_rotation_t rotation );`
  - **参数：**
    - `disp`: 目标显示器句柄。
    - `rotation`: 旋转角度，枚举类型（例如 `LV_DISPLAY_ROTATION_0`, `LV_DISPLAY_ROTATION_90`, `LV_DISPLAY_ROTATION_180`, `LV_DISPLAY_ROTATION_270`）。

- **`lv_disp_get_rotation()`**
  
  - **描述：** 获取当前显示器的旋转参数。
  - **函数原型：** `lv_display_rotation_t lv_disp_get_rotation( lv_disp_t *disp );`
  - **参数：** `disp` - 显示器句柄。
  - **返回值：** 当前的旋转角度。

### 3.5 其他显示接口 API 函数

- **`lv_disp_get_next()`**
  
  - **描述：** 获取下一个已注册的显示设备句柄。当系统中注册了多个显示器时，此函数可以用于遍历它们。
  - **函数原型：** `lv_disp_t *lv_disp_get_next( lv_disp_t *disp );`
  - **参数：** `disp` - 当前显示器句柄。传入 `NULL` 将返回第一个显示器句柄。
  - **返回值：** 下一个显示器句柄；如果没有更多显示器，则返回 `NULL`。

- **`lv_disp_get_draw_buf`**
  
  - **描述：** 获取指定显示器的内部绘图缓冲区（`lv_disp_draw_buf_t` 结构体）的指针。
  - **函数原型：** `lv_disp_draw_buf_t *lv_disp_get_draw_buf( lv_disp_t *disp );`
  - **参数：** `disp` - 显示器句柄。
  - **返回值：** 绘图缓冲区结构体的指针。

## 4. 输入设备

输入设备可以让用户和计算机系统之间进行信息交换，在 LVGL中，支持的输入设备包括触摸屏、 键盘、 鼠标、编码器以及按钮，它们都是在 lv_port_indev_template.c 文件中进行配置的。 我们需要成功地驱动自己的输入设备，就必须在 LVGL 中将其注册。

| 函数                        | 描述        |
| ------------------------- | --------- |
| `lv_indev_drv_init()`     | 初始化输入设备   |
| `lv_indev_drv_register()` | 注册输入设备    |
| `lv_indev_drv_update()`   | 在运行时更新驱动  |
| `lv_indev_delete()`       | 移除输入设备    |
| `lv_indev_get_next()`     | 获取下一个输入设备 |
| `lv_indev_read()`         | 从输入设备读取数据 |

### 4.1 输入设备驱动注册与管理

- **`lv_indev_drv_init()`**
  
  - **描述：** 初始化一个 `lv_indev_drv_t` 结构体变量。这个结构体包含了输入设备驱动的所有配置信息和回调函数，特别是读取输入数据的回调函数。在使用输入设备驱动前必须先初始化。
  - **函数原型：** `void lv_indev_drv_init( lv_indev_drv_t *drv );`
  - **参数：** `drv` - 指向要初始化的输入设备驱动结构体的指针。
  - **注意事项：** 这只是初始化结构体，并不注册驱动。

- **`lv_indev_drv_register()`**
  
  - **描述：** 注册一个输入设备驱动到 LVGL 核心。
  - **函数原型：** `lv_indev_t *lv_indev_drv_register( lv_indev_drv_t *drv );`
  - **参数：** `drv` - 指向已初始化并配置好的输入设备驱动结构体的指针。
  - **返回值：** 注册成功的输入设备句柄。
  - **注意事项：** 在驱动结构体中，最重要的字段是 `type`（输入设备类型，如触摸屏、键盘等）和 `read_cb`（读取输入数据的回调函数）。

- **`lv_indev_drv_update()`**
  
  - **描述：** 在运行时更新一个已注册输入设备驱动的配置。例如，可以更改触摸屏的校准参数或键盘的映射。
  - **函数原型：** `void lv_indev_drv_update( lv_indev_t *indev, lv_indev_drv_t *new_drv );`
  - **参数：**
    - `indev`: 要更新的输入设备句柄。
    - `new_drv`: 指向包含新配置的输入设备驱动结构体的指针。

- **`lv_indev_delete()`**
  
  - **描述：** 从 LVGL 中移除一个已注册的输入设备。
  - **函数原型：** `void lv_indev_delete( lv_indev_t *indev );`
  - **参数：** `indev` - 要移除的输入设备句柄。

### 4.2 输入设备数据读取

- **`lv_indev_read()`**
  - **描述：** 这是一个内部函数，**通常由 LVGL 内部的 `lv_timer_handler()` 函数周期性调用**，以从所有注册的输入设备读取数据。开发者通常不需要直接调用此函数。
  - **函数原型：** `void lv_indev_read( lv_indev_t *indev, lv_indev_data_t *data );`
  - **参数：**
    - `indev`: 目标输入设备句柄。
    - `data`: 指向 `lv_indev_data_t` 结构体的指针，用于存储读取到的输入数据（例如，触摸点的坐标、按键的状态等）。
  - **注意事项：** 真正的底层硬件读取逻辑是在 `lv_indev_drv_t` 结构体中注册的 `read_cb` 回调函数中实现。

### 4.3 输入设备遍历

- **`lv_indev_get_next()`**
  - **描述：** 获取下一个已注册的输入设备句柄。当系统中注册了多个输入设备时，此函数可以用于遍历它们。
  - **函数原型：** `lv_indev_t *lv_indev_get_next( lv_indev_t *indev );`
  - **参数：** `indev` - 当前输入设备句柄。传入 `NULL` 将返回第一个输入设备句柄。
  - **返回值：** 下一个输入设备句柄；如果没有更多输入设备，则返回 `NULL`。

### 4.4 **如何使用输入设备 API 函数**

1. **定义并初始化 `lv_indev_drv_t` 结构体：**
   - 设置 `type` (例如 `LV_INDEV_TYPE_POINTER` 用于触摸屏/鼠标，`LV_INDEV_TYPE_KEYPAD` 用于键盘等)。
   - **最重要的是，实现 `read_cb` 回调函数**，这个函数将负责从底层硬件（如触摸控制器）读取数据，并将数据填充到 `lv_indev_data_t` 结构体中。
   - 调用 `lv_indev_drv_init(&my_indev_drv);`。
2. **注册输入设备驱动：**
   - 调用 `lv_indev_drv_register(&my_indev_drv);`。
3. **在主循环中调用 `lv_timer_handler()`：**
   - `lv_timer_handler()` 会周期性地调用所有注册输入设备的 `read_cb` 函数来获取最新输入数据，并将其传递给 LVGL 内部进行处理。

## 5. LVGL时基

在 RTOS 中，任务的切换需要依赖系统定时器，而 LVGL 同样也需要一个时基， 这样它才可以知道动画以及任务所经过的时间。

### 5.1 `lv_tick_get()` 函数

- **描述：** 该函数可以获取自 LVGL 启动以来经过的毫秒数。这个值是一个单调递增的计数器。
- **函数原型：** `uint32_t lv_tick_get(void);`
- **返回值：** 经过的毫秒数 (`uint32_t` 类型)。
- **重要性：**
  - 它是 LVGL 内部所有时间相关的基准，包括动画、定时器、输入设备去抖等。
  - 在嵌入式系统中，你需要将底层系统（如硬件定时器或RTOS的系统节拍）的周期性中断与 LVGL 的 `lv_tick_inc()` 函数关联起来，以确保 `lv_tick_get()` 返回正确的值。例如，如果每毫秒发生一次中断，那么在中断服务函数中调用 `lv_tick_inc(1)`。
  - 该函数返回的值会随着时间推移而不断增加，最终可能会溢出（大约 49.7 天后对于 `uint32_t`）。通常在计算时间差时需要考虑溢出情况。

### 5.2 `lv_tick_elaps()` 函数

- **描述：** 该函数可以获取自上一个时间戳 `prev_tick` 以来经过的毫秒数。这个函数会正确处理 `uint32_t` 计数器溢出的情况，因此在计算时间间隔时非常有用。

- **函数原型：** `uint32_t lv_tick_elaps(uint32_t prev_tick);`

- **参数：**
  
  | `prev_tick` | 上一个时间戳（一个之前通过 `lv_tick_get()` 获取的值）。 |
  | ----------- | ------------------------------------ |

- **返回值：** 自 `prev_tick` 以来经过的毫秒数。

- **重要性：**
  
  - 它是用于可靠地测量时间间隔的关键函数。由于 `uint32_t` 计数器会溢出，简单地做减法 `lv_tick_get() - prev_tick` 在溢出时会得到错误的结果。`lv_tick_elaps()` 内部会正确处理这种循环计数器的溢出。
  - **示例用法：**

```c
uint32_t start_time = lv_tick_get();
// ... 执行一些操作 ...
uint32_t elapsed_time = lv_tick_elaps(start_time);
// elapsed_time 现在包含了从 start_time 到当前时刻经过的毫秒数
```

## 6. LVGL 任务处理

LVGL 的任务处理函数（ lv_timer_handler） 类似于 RTOS 切换任务的任务调度函数。lv_timer_handler 函数对于定时的要求并不严格， 一般来说，用户只需要确保在 5 毫秒以内调用一次该函数即可，以保持系统响应的速度。 值得注意的是， LVGL 的任务处理并不是抢占式的，而是采用轮询的方式。

要处理 LVGL 的任务或者回调函数， 用户只需要将 lv_timer_handler 函数定时调用即可，该函数可放在以下地方：

1. main 函数的循环。

2. 定时器的中断（优先级需要低于 lv_tick_inc）。

3. 定时执行的 OS 任务。

---


