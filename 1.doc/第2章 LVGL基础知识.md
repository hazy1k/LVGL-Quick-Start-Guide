# 第二章 LVGL基础知识

## 1. LVGL对象介绍

在 LVGL 中，用户界面的基本构建成分是对象，也称为小部件， 例如： 按钮、标签、图片、列表、图表、 文本区域， 等等。 值得注意的是， LVGL 图形库虽然是由 C 语言开发的，但其所采用的是一种面对对象编程思维，这就涉及到了“类”的概念。

在 C语言中，并没有“类”的概念，而 LVGL通过结构体的形式实现了“类”的功能（并非真的“类” ），具体的实现逻辑如下图所示：

![屏幕截图 2025-08-21 172344.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/21-17-23-58-屏幕截图%202025-08-21%20172344.png)

由上图可知，在 LVGL 中，首先定义了 lv_obj_t 这个结构体，然后通过这个结构体去实例化一个基础对象（lv_obj），这个基础对象将作为父对象，去衍生更多的子对象（其他部件）。

值得注意的是，通过这种“类”的方式去衍生其他部件，所衍生出来的部件将会继承父对象的一些基本属性，例如大小、位置、样式，等等，因此，我们可以通过一套统一的函数去管理不同部件的基本属性。

### 1.1 对象的基本属性

在 LVGL 中，每个对象都有一些相同的基本属性，例如：

1. 大小

2. 父类

3. 样式

4. 事件

5. 位置

为了方便地设置基本属性， LVGL 设计了一套通用的属性设置函数， 它们可以用于设置各个部件的基本属性，这里我们以部件的大小属性为例， 来看一下函数的具体实现：

```c
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
```

该函数的第一个入口参数（*obj） 为某个需要设置属性的部件（例如开关、按钮等），第二、三个入口参数分别为部件的宽度和高度。 除了该函数之外，还有很多设置基本属性的函数，它们都是 lv_obj_set_xxx 的格式，我们后面用到某一个属性设置的时候，再详细地介绍这些函数。

### 1.2 对象的私有属性

在对象衍生的过程中，不同的对象（部件）会拥有一些特殊的属性，也称为私有属性。例如，滑块具有以下私有属性：

1. 当前值；

2. 范围值。

对于这些私有的属性，每种部件都有相应的 API 函数来进行设置，例如滑块的当前值和范围值设置， 源码如下所示：

```c
/* 设置滑块的私有属性 */
lv_slider_set_range(slider1, 0, 100); /* 设置滑块的范围值 */
lv_slider_set_value(slider1, 40, LV_ANIM_ON); /* 设置滑块当前值 */
```

### 1.3 创建对象与删除对象

在 LVGL 中， 用户可以在程序运行时动态创建或者删除一个对象。 当对象被创建时，将消耗一定的内存，而当其被删除时，这部分内存将得到释放。

1. 创建对象

LVGL 每个对象的创建函数都很类似， 它们具有高度统一的风格， 源码如下所示：

```c
lv_obj_t * lv_<widget>_create(lv_obj_t * parent);
```

在上述源码中， widget 代表的是不同的部件，例如开关（ switch）、按钮（ btn）、图片（img），等等。 一般情况下， 创建对象的函数只有一个形参，那就是*parent， 它指向父类，该父类可以是当前的活动屏幕（lv_scr_act）或者是其他的部件。

2. 删除对象

用户需要删除一个对象， 可使用以下几个函数：

1. lv_obj_del(lv_obj_t * obj)， 立即删除一个对象， 并该对象的子类一起删除。

2. lv_obj_del_async(lv_obj_t * obj)， 下一次执行 lv_timer_handler 后删除对象。

3. lv_obj_clean(lv_obj_t * obj)： 立刻删除一个对象的全部子类。

4. lv_obj_del_delayed(lv_obj_t * obj, uint32_t delay_ms)：延时 delay_ms毫秒再删除对象

## 2. LVGL布局

### 2.1 对象的坐标位置

在 LVGL 中，所有对象的坐标都以**像素**为单位，并且是相对于其**父对象**的。

- **原点：** `(0, 0)` 坐标点位于父对象的左上角。
- **X 轴：** 水平方向，向右递增。
- **Y 轴：** 垂直方向，向下递增。

**相关 API 函数：**

- **`lv_obj_set_x(lv_obj_t *obj, lv_coord_t x)`:** 设置对象的 X 坐标。
- **`lv_obj_set_y(lv_obj_t *obj, lv_coord_t y)`:** 设置对象的 Y 坐标。
- **`lv_obj_set_pos(lv_obj_t *obj, lv_coord_t x, lv_coord_t y)`:** 同时设置对象的 X 和 Y 坐标。
- **`lv_obj_get_x(lv_obj_t *obj)`:** 获取对象的 X 坐标。
- **`lv_obj_get_y(lv_obj_t *obj)`:** 获取对象的 Y 坐标。
- **`lv_obj_get_coords(lv_obj_t *obj, lv_area_t *coords)`:** 获取对象的绝对坐标（相对于屏幕左上角）。

**示例：**

```c
lv_obj_t *parent = lv_obj_create(lv_scr_act()); // 在屏幕上创建一个父对象
lv_obj_set_size(parent, 200, 100);
lv_obj_set_pos(parent, 50, 50); // 父对象位于屏幕 (50, 50) 处

lv_obj_t *child = lv_obj_create(parent); // 在父对象上创建一个子对象
lv_obj_set_size(child, 50, 30);
lv_obj_set_pos(child, 10, 20); // 子对象位于父对象内部的 (10, 20) 处

// 此时子对象相对于屏幕的绝对坐标为 (50+10, 50+20) = (60, 70)
```

### 2.2 对象的大小

对象的大小同样以**像素**为单位，通过宽度和高度来定义。

**相关 API 函数：**

- **`lv_obj_set_width(lv_obj_t *obj, lv_coord_t w)`:** 设置对象的宽度。
- **`lv_obj_set_height(lv_obj_t *obj, lv_coord_t h)`:** 设置对象的高度。
- **`lv_obj_set_size(lv_obj_t *obj, lv_coord_t w, lv_coord_t h)`:** 同时设置对象的宽度和高度。
- **`lv_obj_get_width(lv_obj_t *obj)`:** 获取对象的宽度。
- **`lv_obj_get_height(lv_obj_t *obj)`:** 获取对象的高度。

**百分比大小：**

LVGL 也支持使用百分比来设置大小，这使得UI能够更好地适应不同分辨率的屏幕。百分比是相对于**父对象**的大小。

- **`LV_SIZE_CONTENT`:** 特殊值，表示对象的大小应该根据其内容自动调整（例如，标签的大小根据文本内容自动调整）。

**示例：**

```c
lv_obj_t *parent = lv_obj_create(lv_scr_act());
lv_obj_set_size(parent, 200, 100);

lv_obj_t *child = lv_obj_create(parent);
lv_obj_set_width(child, lv_pct(50)); // 宽度是父对象宽度的一半 (200 * 0.5 = 100像素)
lv_obj_set_height(child, lv_pct(80)); // 高度是父对象高度的80% (100 * 0.8 = 80像素)
```

### 2.3 对象的对齐

对象的对齐功能允许你相对于父对象或其他参考对象来方便地定位一个对象，而无需手动计算精确的坐标。

**对齐类型：**

LVGL 提供了多种对齐方式，例如：

* **`LV_ALIGN_TOP_LEFT`:** 顶部居左对齐
* **`LV_ALIGN_TOP_MID`:** 顶部居中对齐
* **`LV_ALIGN_TOP_RIGHT`:** 顶部居右对齐
* **`LV_ALIGN_LEFT_MID`:** 左侧居中对齐
* **`LV_ALIGN_CENTER`:** 居中对齐
* **`LV_ALIGN_RIGHT_MID`:** 右侧居中对齐
* **`LV_ALIGN_BOTTOM_LEFT`:** 底部居左对齐
* **`LV_ALIGN_BOTTOM_MID`:** 底部居中对齐
* **`LV_ALIGN_BOTTOM_RIGHT`:** 底部居右对齐
* 还有用于内容内部对齐的 `LV_ALIGN_OUT_XXX` 等。

**相关 API 函数：**

* **`lv_obj_set_align(lv_obj_t *obj, lv_align_t align)`:** 设置对象相对于其 **父对象** 的对齐方式。
* **`lv_obj_align(lv_obj_t *obj, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)`:** 设置对象相对于其 **父对象** 的对齐方式，并带有额外的水平 (`x_ofs`) 和垂直 (`y_ofs`) 偏移。
* **`lv_obj_align_to(lv_obj_t *obj, const lv_obj_t *base, lv_align_t align, lv_coord_t x_ofs, lv_coord_t y_ofs)`:** 设置对象相对于指定的 **基准对象** 的对齐方式，并带有额外的偏移。

**示例：**

```c
lv_obj_t *parent_screen = lv_scr_act(); // 假设在屏幕上操作

lv_obj_t *btn1 = lv_btn_create(parent_screen);
lv_obj_set_size(btn1, 100, 50);
lv_obj_set_align(btn1, LV_ALIGN_CENTER); // 按钮居中对齐到屏幕

lv_obj_t *label1 = lv_label_create(btn1);
lv_label_set_text(label1, "Click Me");
lv_obj_set_align(label1, LV_ALIGN_CENTER); // 标签居中对齐到按钮内部

lv_obj_t *btn2 = lv_btn_create(parent_screen);
lv_obj_set_size(btn2, 80, 40);
lv_obj_align_to(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0); // 按钮2对齐到按钮1的右侧中间，并偏移20像素
```

## 3. LVGL样式属性

### 3.1 LVGL 样式设置方法

LVGL 样式的核心是 `lv_style_t` 结构体。您可以通过以下几种主要方法来设置样式：

1. **直接设置到对象上 (Local Styles):**
   - 使用 `lv_obj_set_style_XXX(obj, value, selector)` 函数直接为特定对象的特定部分和状态设置样式属性。
   - 这是最直接的方式，但如果多个对象有相同属性，会造成代码重复。
2. **创建共享样式对象 (Shared Styles):**
   - 创建一个 `lv_style_t` 变量，并使用 `lv_style_init(&my_style)` 初始化。
   - 使用 `lv_style_set_XXX(&my_style, value)` 函数为这个样式对象设置属性。
   - 使用 `lv_obj_add_style(obj, &my_style, selector)` 将这个共享样式添加到对象上。
   - 优点：样式可以被多个对象重用，减少内存占用和代码量，方便统一修改。
3. **继承与级联：**
   - 样式属性是继承的：子对象会继承父对象的样式属性，除非子对象有自己的特定属性覆盖。
   - 样式是级联的：一个对象可以附加多个样式，属性会按添加顺序和优先级进行级联。

**Selector (选择器):**  
`selector` 参数在很多样式设置函数中都会出现，它是一个按位或的组合，用于指定样式应用到对象的哪个**部分**和**状态**。

- **部件组成部分 (Part):** 例如 `LV_PART_MAIN`, `LV_PART_SCROLLBAR`, `LV_PART_INDICATOR` 等。
- **部件的状态 (State):** 例如 `LV_STATE_DEFAULT` (默认), `LV_STATE_PRESSED` (按下), `LV_STATE_CHECKED` (选中), `LV_STATE_DISABLED` (禁用) 等。

### 3.2 部件组成部分 (Parts of an Object)

LVGL 的每个部件（对象）都可能由一个或多个“部分”（Parts）组成。每个部分都可以独立地拥有自己的样式。例如：

- **`LV_PART_MAIN`:** 对象的主体部分。
- **`LV_PART_SCROLLBAR`:** 滚动条。
- **`LV_PART_INDICATOR`:** 进度条的指示器部分，或者滑块的滑块部分。
- **`LV_PART_ITEMS`:** 列表中的项目。
- **`LV_PART_SELECTED`:** 列表中被选中的项目。
- ...等等。

通过指定 `selector` 中的 `LV_PART_XXX`，你可以精确地为对象的不同部分设置样式。

### 3.3 部件的状态 (States of an Object)

LVGL 对象可以处于不同的状态，每个状态下可以有不同的样式。当对象状态改变时，LVGL 会自动切换到相应的样式。常见状态包括：

- **`LV_STATE_DEFAULT`:** 默认状态，无特殊操作。
- **`LV_STATE_PRESSED`:** 按下或点击时。
- **`LV_STATE_CHECKED`:** 被选中时（如复选框、开关）。
- **`LV_STATE_FOCUSED`:** 获得焦点时（通常通过键盘或编码器导航）。
- **`LV_STATE_DISABLED`:** 被禁用时，不可交互。
- **`LV_STATE_EDITED`:** 编辑状态（如文本输入框）。
- **`LV_STATE_HOVERED`:** 鼠标悬停时（仅在支持鼠标的PC模拟器上可见）。

通过指定 `selector` 中的 `LV_STATE_XXX`，你可以为对象的不同交互状态定义不同的视觉反馈。

### 3.4 LVGL 样式属性 (LVGL Style Properties)

这是样式系统中最核心的部分，定义了可以设置的各种视觉属性。

#### 3.4.1 大小与位置属性 (Size and Position Properties)

这些属性通常与 `lv_obj_set_size()` 和 `lv_obj_set_pos()` 等函数独立使用，但也可以通过样式来设定默认值。

- **`LV_STYLE_WIDTH` / `LV_STYLE_HEIGHT`:** 对象的宽度/高度。
- **`LV_STYLE_X` / `LV_STYLE_Y`:** 对象的X/Y坐标。

#### 3.4.2 填充属性 (Padding Properties)

定义对象内容与其边框之间的空间。

- **`LV_STYLE_PAD_TOP` / `LV_STYLE_PAD_BOTTOM` / `LV_STYLE_PAD_LEFT` / `LV_STYLE_PAD_RIGHT`:** 上下左右的内边距。
- **`LV_STYLE_PAD_ROW` / `LV_STYLE_PAD_COLUMN`:** 在布局管理器中行/列之间的填充。
- **`LV_STYLE_PAD_ALL` / `LV_STYLE_PAD_HOR` / `LV_STYLE_PAD_VER`:** 快速设置所有方向或水平/垂直方向的内边距。

#### 3.4.3 背景属性 (Background Properties)

定义对象的背景外观。

- **`LV_STYLE_BG_COLOR`:** 背景颜色。
- **`LV_STYLE_BG_OPA`:** 背景不透明度 (0-255)。
- **`LV_STYLE_BG_GRAD_COLOR`:** 背景渐变颜色。
- **`LV_STYLE_BG_GRAD_DIR`:** 背景渐变方向。
- **`LV_STYLE_BG_MAIN_STOP` / `LV_STYLE_BG_GRAD_STOP`:** 渐变停止位置。
- **`LV_STYLE_BG_IMG_SRC`:** 背景图片源。
- **`LV_STYLE_BG_IMG_OPA`:** 背景图片不透明度。
- **`LV_STYLE_BG_IMG_TILING`:** 背景图片是否平铺。

#### 3.4.4 边框属性 (Border Properties)

定义对象的边框外观。

- **`LV_STYLE_BORDER_COLOR`:** 边框颜色。
- **`LV_STYLE_BORDER_OPA`:** 边框不透明度。
- **`LV_STYLE_BORDER_WIDTH`:** 边框宽度。
- **`LV_STYLE_BORDER_SIDE`:** 边框显示在哪一侧 (如 `LV_BORDER_SIDE_FULL`, `LV_BORDER_SIDE_TOP` 等)。
- **`LV_STYLE_BORDER_POST`:** 是否在绘制子对象后绘制边框。
- **`LV_STYLE_RADIUS`:** 对象的圆角半径。

#### 3.4.5 轮廓属性 (Outline Properties)

在边框之外绘制的轮廓线。

- **`LV_STYLE_OUTLINE_COLOR`:** 轮廓颜色。
- **`LV_STYLE_OUTLINE_OPA`:** 轮廓不透明度。
- **`LV_STYLE_OUTLINE_WIDTH`:** 轮廓宽度。
- **`LV_STYLE_OUTLINE_PAD`:** 轮廓与边框之间的距离。

#### 3.4.6 阴影属性 (Shadow Properties)

定义对象的阴影效果。

- **`LV_STYLE_SHADOW_COLOR`:** 阴影颜色。
- **`LV_STYLE_SHADOW_OPA`:** 阴影不透明度。
- **`LV_STYLE_SHADOW_WIDTH`:** 阴影宽度（扩散程度）。
- **`LV_STYLE_SHADOW_OFS_X` / `LV_STYLE_SHADOW_OFS_Y`:** 阴影的水平/垂直偏移。
- **`LV_STYLE_SHADOW_SPREAD`:** 阴影的扩展程度。

#### 3.4.7 图片属性 (Image Properties)

控制对象中显示图片的属性（不同于背景图片）。

- **`LV_STYLE_IMG_OPA`:** 图片不透明度。
- **`LV_STYLE_IMG_RECOLOR`:** 图片重着色。
- **`LV_STYLE_IMG_RECOLOR_OPA`:** 图片重着色不透明度。

#### 3.4.8 线条属性 (Line Properties)

控制绘制线条的属性。

- **`LV_STYLE_LINE_COLOR`:** 线条颜色。
- **`LV_STYLE_LINE_OPA`:** 线条不透明度。
- **`LV_STYLE_LINE_WIDTH`:** 线条宽度。
- **`LV_STYLE_LINE_ROUNDED`:** 线条末端是否圆角。
- **`LV_STYLE_LINE_DASH_WIDTH` / `LV_STYLE_LINE_DASH_GAP`:** 虚线样式。

#### 3.4.9 圆弧属性 (Arc Properties)

控制绘制圆弧的属性。

- **`LV_STYLE_ARC_COLOR`:** 圆弧颜色。
- **`LV_STYLE_ARC_OPA`:** 圆弧不透明度。
- **`LV_STYLE_ARC_WIDTH`:** 圆弧宽度。
- **`LV_STYLE_ARC_ROUNDED`:** 圆弧末端是否圆角。

#### 3.4.10 文本属性 (Text Properties)

控制对象中显示文本的属性。

- **`LV_STYLE_TEXT_COLOR`:** 文本颜色。
- **`LV_STYLE_TEXT_OPA`:** 文本不透明度。
- **`LV_STYLE_TEXT_FONT`:** 文本字体。
- **`LV_STYLE_TEXT_LETTER_SPACING`:** 字母间距。
- **`LV_STYLE_TEXT_LINE_SPACING`:** 行间距。
- **`LV_STYLE_TEXT_DECOR`:** 文本装饰（下划线、删除线）。
- **`LV_STYLE_TEXT_ALIGN`:** 文本对齐方式。
- **`LV_STYLE_TEXT_OVERFLOW`:** 文本溢出处理。

#### 3.4.11 其他属性 (Other Properties)

可能包括一些不属于上述类别的通用属性。

- **`LV_STYLE_TRANSFORM_WIDTH` / `LV_STYLE_TRANSFORM_HEIGHT`:** 变换后的宽度/高度。
- **`LV_STYLE_TRANSP_FIX`:** 是否强制透明度。
- **`LV_STYLE_TRANSITION`:** 定义状态切换时的动画效果。

## 4. LVGL 滚动属性

在 LVGL 中， 任何对象都是支持滚动的。 如果一个对象在其父对象的区域之外，则该父对象将变为可滚动的， 并且在其内部出现滚动条， 如下图所示：

![屏幕截图 2025-08-21 174103.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/21-17-41-07-屏幕截图%202025-08-21%20174103.png)

### 4.1 滚动的类型 (Scroll Types)

LVGL 中的滚动行为可以通过多种方式来控制，这主要体现在其内部的实现和用户的体验上。

- **滚动手势 (Scroll Gestures):** 这是最常见的滚动方式，用户通过触摸拖动或鼠标滚轮来实现滚动。LVGL 会自动处理滚动的物理效果，如惯性滚动和回弹。
- **程序化滚动 (Programmatic Scrolling):** 可以通过 API 函数来控制对象的滚动位置，例如将内容滚动到特定位置或使特定子对象可见。

**相关 API 函数（通常用于程序化滚动）：**

- `lv_obj_scroll_by(lv_obj_t *obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)`: 按指定像素量滚动对象。
- `lv_obj_scroll_to(lv_obj_t *obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)`: 滚动到绝对坐标位置。
- `lv_obj_scroll_to_x(lv_obj_t *obj, lv_coord_t x, lv_anim_enable_t anim_en)`: 滚动到绝对X坐标。
- `lv_obj_scroll_to_y(lv_obj_t *obj, lv_coord_t y, lv_anim_enable_t anim_en)`: 滚动到绝对Y坐标。
- `lv_obj_scroll_to_view(lv_obj_t *obj, lv_anim_enable_t anim_en)`: 将一个子对象滚动到父对象的可见视图中。

### 4.2 滚动条模式 (Scrollbar Modes)

滚动条是可视化滚动位置的指示器。LVGL 提供了多种滚动条模式来控制其显示行为：

- **`LV_SCROLLBAR_MODE_OFF`:** 禁用滚动条。
- **`LV_SCROLLBAR_MODE_ON`:** 始终显示滚动条。
- **`LV_SCROLLBAR_MODE_ACTIVE`:** 当内容可以滚动时显示滚动条。
- **`LV_SCROLLBAR_MODE_AUTO`:** 仅当用户正在滚动时显示滚动条，滚动结束后自动隐藏。这是默认模式，通常提供最佳用户体验。

**相关 API 函数：**

- `lv_obj_set_scrollbar_mode(lv_obj_t *obj, lv_scrollbar_mode_t mode)`: 设置对象的滚动条模式。
- `lv_obj_get_scrollbar_mode(lv_obj_t *obj)`: 获取对象的滚动条模式。

**样式属性：**  
滚动条本身也是一个 `LV_PART_SCROLLBAR` 部件，可以通过样式属性来定制其颜色、大小、圆角等。

- `LV_STYLE_SCROLLBAR_COLOR`
- `LV_STYLE_SCROLLBAR_OPA`
- `LV_STYLE_SCROLLBAR_WIDTH`
- `LV_STYLE_SCROLLBAR_RADIUS`

### 4.3 滚动的事件类型 (Scroll Event Types)

当对象的滚动状态发生变化时，LVGL 会触发相应的事件，允许应用程序对滚动行为做出响应。

- **`LV_EVENT_SCROLL_BEGIN`:** 滚动开始时触发。
- **`LV_EVENT_SCROLL_END`:** 滚动结束（包括惯性滚动停止和回弹结束）时触发。
- **`LV_EVENT_SCROLL`:** 滚动过程中持续触发。
- **`LV_EVENT_SCROLL_BEGINNING`:** 滚动内容到达起始位置（最左/最上）时触发。
- **`LV_EVENT_SCROLL_END`:** 滚动内容到达结束位置（最右/最下）时触发。

可以通过 `lv_obj_add_event_cb()` 函数为对象添加事件回调来监听这些滚动事件。

**示例：**

```c
static void scroll_event_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_SCROLL_BEGIN) {
        LV_LOG_USER("Scroll begin on object");
    } else if (code == LV_EVENT_SCROLL_END) {
        LV_LOG_USER("Scroll end on object");
    } else if (code == LV_EVENT_SCROLL) {
        // LV_LOG_USER("Scrolling..."); // 可能会非常频繁，注意日志输出量
    }
}

lv_obj_t *panel = lv_obj_create(lv_scr_act());
lv_obj_set_size(panel, 200, 150);
lv_obj_set_scroll_dir(panel, LV_DIR_VER); // 允许垂直滚动
lv_obj_add_event_cb(panel, scroll_event_cb, LV_EVENT_ALL, NULL); // 监听所有滚动相关事件
```

### 4.4 设置滚动方向 (Set Scroll Direction)

您可以限制一个对象只能在特定方向上滚动，或在所有方向上滚动。

- **`LV_DIR_NONE`:** 禁用滚动。
- **`LV_DIR_HOR`:** 只允许水平滚动。
- **`LV_DIR_VER`:** 只允许垂直滚动。
- **`LV_DIR_ALL`:** 允许水平和垂直双向滚动。
- **`LV_DIR_LEFT` / `LV_DIR_RIGHT` / `LV_DIR_TOP` / `LV_DIR_BOTTOM`:** 更细粒度的控制，例如只允许向左滚动。

**相关 API 函数：**

- `lv_obj_set_scroll_dir(lv_obj_t *obj, lv_dir_t dir)`: 设置对象的滚动方向。
- `lv_obj_get_scroll_dir(lv_obj_t *obj)`: 获取对象的滚动方向。

### 4.5 滚动的其他特性 (Other Scroll Features)

- **滚动边界检查和回弹 (Scroll Bounce):** 当内容滚动到边缘时，LVGL 通常会提供一个视觉回弹效果，表示已到达边界。
  - 可以通过 `LV_SCROLL_FLAG_NO_BOUNCE` 等标志禁用回弹。
- **捕捉（Snapping）：** 滚动结束后，内容可以自动对齐到特定的点或子对象。这在实现类似轮播图或列表选择器时非常有用。
  - 通过 `lv_obj_set_scroll_snap_x()` / `lv_obj_set_scroll_snap_y()` 进行设置。
- **内容适应 (Fit Content):** 父对象可以根据其子对象的总大小自动调整其可滚动区域。
  - 通过 `lv_obj_set_scroll_fit(obj, LV_SCROLL_FIT_TIGHT)` 等样式或属性设置。
- **滚动动画 (Scroll Animation):** 所有的程序化滚动操作都可以选择是否带动画效果 (`lv_anim_enable_t anim_en`)，提供更流畅的用户体验。

**相关 API 函数（样式属性）：**

- `lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLLABLE)`: 使对象可滚动。这是开启滚动功能的基本标志。
- `lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE)`: 禁用对象的滚动。
- `lv_obj_set_scroll_snap_x(obj, LV_SCROLL_SNAP_START/CENTER/END)`: 设置水平捕捉点。
- `lv_obj_set_scroll_snap_y(obj, LV_SCROLL_SNAP_START/CENTER/END)`: 设置垂直捕捉点。

## 5. LVGL动画属性

优秀的过渡动画可以让用户的 GUI 变得更具高级感。 在 LVGL 中，用户可指定动画的开始值和结束值，该动画将通过回调函数来处理。

### 5.1 创建动画

在 LVGL 中，动画通过配置一个 `lv_anim_t` 结构体来创建。这个结构体定义了动画的所有参数，例如动画作用的对象、动画属性、起始值、结束值、持续时间、延迟、回调函数等。

**主要步骤：**

1. **初始化 `lv_anim_t` 结构体：** 使用 `lv_anim_init(&a)`。
2. **设置动画的基本参数：**
   - `lv_anim_set_var(&a, obj)`: 设置动画将作用的对象。
   - `lv_anim_set_exec_cb(&a, (lv_anim_exec_cb_t)lv_obj_set_x)`: 设置执行回调函数，这是一个函数指针，用于根据动画的当前值更新对象的某个属性。例如，`lv_obj_set_x` 用于更新 X 坐标。
   - `lv_anim_set_values(&a, start_value, end_value)`: 设置动画的起始值和结束值。
   - `lv_anim_set_time(&a, duration_ms)`: 设置动画的持续时间，以毫秒为单位。
3. **启动动画：** 使用 `lv_anim_start(&a)`。

### 5.2 设置动画路径 (Animation Path / Easing Function)

动画路径（也称为缓动函数/Easing Function）定义了动画值随时间变化的速率曲线。它决定了动画是匀速的、先快后慢、先慢后快、带有弹跳效果等。LVGL 提供了多种内置的缓动函数。

**相关 API 函数：**

- `lv_anim_set_path_cb(&a, lv_anim_path_linear)`: 设置动画路径回调函数。
  - **常用路径函数：**
    - `lv_anim_path_linear`: 线性路径（匀速）。
    - `lv_anim_path_ease_in`: 缓入（开始慢，逐渐加速）。
    - `lv_anim_path_ease_out`: 缓出（开始快，逐渐减速）。
    - `lv_anim_path_ease_in_out`: 缓入缓出（开始和结束慢，中间快）。
    - `lv_anim_path_overshoot`: 超调（动画结束时稍微超出目标，然后回弹）。
    - `lv_anim_path_bounce`: 弹跳（动画结束时有多次弹跳效果）。

### 5.3 设置动画速度 (Animation Speed)

动画速度直接由动画的持续时间 (`lv_anim_set_time`) 和可能的延迟 (`lv_anim_set_delay`) 来控制。

**相关 API 函数：**

- **`lv_anim_set_time(&a, duration_ms)`:** 设置动画的持续时间，以毫秒为单位。时间越短，速度越快。
- **`lv_anim_set_delay(&a, delay_ms)`:** 设置动画开始前的延迟时间，以毫秒为单位。
- **`lv_anim_set_repeat_count(&a, count)`:** 设置动画重复的次数。`LV_ANIM_REPEAT_INFINITE` 表示无限重复。
- **`lv_anim_set_playback_time(&a, playback_duration_ms)`:** 设置动画反向播放的持续时间。
- **`lv_anim_set_playback_delay(&a, playback_delay_ms)`:** 设置动画反向播放前的延迟时间。

### 5.4 删除动画

动画在完成单次或所有重复播放后会自动删除。您也可以手动删除正在运行的动画。

**相关 API 函数：**

- **`lv_anim_del(void *var, lv_anim_exec_cb_t exec_cb)`:** 删除与特定对象和执行回调函数关联的动画。
  - `var`: 动画作用的对象。
  - `exec_cb`: 动画的执行回调函数。如果为 `NULL`，则删除与 `var` 关联的所有动画。
- **`lv_anim_del_all()`:** 删除所有正在运行的动画。

### 5.5 动画时间线 (Animation Timeline)

动画时间线是 LVGL 8.0 引入的一个强大功能，它允许您将多个独立的动画组合成一个同步播放的序列。您可以在时间线上定义每个动画的开始时间、持续时间，从而创建复杂的组合动画效果。

#### 5.5.1 动画时间线相关 API 函数

- **`lv_anim_timeline_create()`:** 创建一个动画时间线对象。
- **`lv_anim_timeline_del()`:** 删除一个动画时间线。
- **`lv_anim_timeline_add()`:** 向时间线中添加一个动画，并指定其开始时间。
- **`lv_anim_timeline_set_progress()`:** 设置时间线的播放进度（0-65535），用于手动控制时间线动画。
- **`lv_anim_timeline_start()`:** 启动时间线动画。
- **`lv_anim_timeline_stop()`:** 停止时间线动画。
- **`lv_anim_timeline_set_reverse()`:** 设置时间线是否反向播放。
- **`lv_anim_timeline_set_loop_count()`:** 设置时间线的循环次数。

**示例（概念性）：**

```c
lv_anim_timeline_t *timeline = lv_anim_timeline_create();

lv_anim_t a1;
lv_anim_init(&a1);
// ... 配置a1动画 (例如，移动对象A) ...
lv_anim_timeline_add(timeline, 0, &a1); // 动画a1从时间线开始时播放

lv_anim_t a2;
lv_anim_init(&a2);
// ... 配置a2动画 (例如，改变对象B的透明度) ...
lv_anim_timeline_add(timeline, 500, &a2); // 动画a2在时间线开始500ms后播放

lv_anim_timeline_start(timeline); // 启动整个时间线动画
```

## 6. LVGL软件定时器

LVGL 有一个内置的软件定时器，它构建在硬件定时器基础之上，使系统能够提供不受硬件定时器资源限制的定时服务， 其实现的功能与硬件定时器也是类似的。 值得注意的是，因为lv_timer_handler 函数并不是准时调用的，所以导致了软件定时器有一定的误差。

LVGL 提供了一个内置的、非抢占式的软件定时器系统。你可以注册一个函数，让它被周期性地调用。这些定时器在 `lv_timer_handler()` 函数中被统一处理和调用，因此你可以在定时器的回调函数中安全地调用任何 LVGL API。这对于创建动画、响应传感器数据或执行任何需要周期性运行的任务都非常有用。

| **函数**                        | **描述**               |
| ----------------------------- | -------------------- |
| `lv_timer_create()`           | 创建一个新的定时器            |
| `lv_timer_create_basic()`     | 创建一个基本的定时器（不带参数）     |
| `lv_timer_del()`              | 删除一个定时器              |
| `lv_timer_pause()`            | 暂停一个定时器              |
| `lv_timer_resume()`           | 恢复一个定时器              |
| `lv_timer_enable()`           | 启用或禁用定时器             |
| `lv_timer_set_cb()`           | 设置定时器的回调函数           |
| `lv_timer_set_period()`       | 设置定时器的周期             |
| `lv_timer_set_repeat_count()` | 设置定时器的重复次数           |
| `lv_timer_ready()`            | 标记一个定时器，使其在下次处理时立即执行 |
| `lv_timer_reset()`            | 重置定时器的周期             |
| `lv_timer_get_idle()`         | 获取定时器处理器的空闲时间百分比     |
| `lv_timer_get_next()`         | 获取下一个将要执行的定时器        |

下面我将详细介绍这些函数：

核心要求:

所有定时器的正常运行，都依赖于在一个循环中（例如 `main` 函数的 `while(1)` 循环）中周期性地调用 `lv_timer_handler()` 函数。这个函数负责检查哪个定时器到期并执行其回调函数。

### 6.1 创建定时器

- `lv_timer_create()`
  - **描述：** 创建一个功能完整的定时器，可以指定回调函数、周期和用户数据。
  - **函数原型：** `lv_timer_t * lv_timer_create(lv_timer_cb_t timer_cb, uint32_t period, void * user_data);`
  - **参数：**
    - `timer_cb`: 一个函数指针，指向定时器到期时要执行的回调函数。该函数的原型应为 `void (*lv_timer_cb_t)(lv_timer_t *);`。
    - `period`: 定时器的执行周期，单位为毫秒（ms）。
    - `user_data`: 一个指向用户自定义数据的指针，该指针会在调用回调函数时作为参数（`timer->user_data`）传递回去。
  - **返回值：** 返回一个指向 `lv_timer_t` 结构体的指针，即定时器的句柄。如果创建失败，则返回 `NULL`。
- `lv_timer_create_basic()`
  - **描述：** 创建一个最基本的定时器，之后可以通过 `lv_timer_set_*` 系列函数来设置其具体参数。
  - **函数原型：** `lv_timer_t * lv_timer_create_basic(void);`
  - **返回值：** 返回一个指向 `lv_timer_t` 结构体的指针，即定时器的句柄。

### 6.2 控制定时器

- `lv_timer_del()`
  - **描述：** 删除一个已经创建的定时器，并释放其占用的内存。
  - **函数原型：** `void lv_timer_del(lv_timer_t * timer);`
  - **参数：** `timer` - 要删除的定时器的句柄。
- `lv_timer_pause()`
  - **描述：** 暂停一个正在运行的定时器。定时器的时间不会再被计算，回调函数也不会被执行，直到调用 `lv_timer_resume()`。
  - **函数原型：** `void lv_timer_pause(lv_timer_t * timer);`
  - **参数：** `timer` - 要暂停的定时器的句柄。
- `lv_timer_resume()`
  - **描述：** 恢复一个被暂停的定时器。
  - **函数原型：** `void lv_timer_resume(lv_timer_t * timer);`
  - **参数：** `timer` - 要恢复的定时器的句柄。
- `lv_timer_enable()`
  - **描述：** 启用或禁用一个定时器。这实际上是 `lv_timer_pause` 和 `lv_timer_resume` 的一个封装。
  - **函数原型：** `void lv_timer_enable(bool en);`
  - **参数：** `en` - 如果为 `true`，则恢复/启用定时器；如果为 `false`，则暂停/禁用定时器。

### 6.3 配置定时器

- `lv_timer_set_cb()`
  - **描述：** 修改一个已存在定时器的回调函数。
  - **函数原型：** `void lv_timer_set_cb(lv_timer_t * timer, lv_timer_cb_t timer_cb);`
  - **参数：**
    - `timer`: 目标定时器的句柄。
    - `timer_cb`: 新的回调函数。
- `lv_timer_set_period()`
  - **描述：** 修改一个已存在定时器的执行周期。
  - **函数原型：** `void lv_timer_set_period(lv_timer_t * timer, uint32_t period);`
  - **参数：**
    - `timer`: 目标定时器的句柄。
    - `period`: 新的执行周期，单位为毫秒（ms）。
- `lv_timer_set_repeat_count()`
  - **描述：** 设置定时器重复执行的次数。默认情况下，定时器会无限次重复。当定时器执行完指定次数后，它将被自动删除。
  - **函数原型：** `void lv_timer_set_repeat_count(lv_timer_t * timer, int32_t repeat_count);`
  - **参数：**
    - `timer`: 目标定时器的句柄。
    - `repeat_count`: 要重复的次数。设置为 `-1` 表示无限次重复。

### 6.4 状态与执行

- `lv_timer_ready()`
  - **描述：** 手动将一个定时器标记为“就绪”状态。被标记后，该定时器将在下一次调用 `lv_timer_handler()` 时立即执行一次，而无需等待其周期结束。
  - **函数原型：** `void lv_timer_ready(lv_timer_t * timer);`
  - **参数：** `timer` - 目标定时器的句柄。
- `lv_timer_reset()`
  - **描述：** 重置一个定时器。调用此函数后，定时器会重新开始计时，并在经过其设定的完整周期 (`period`) 后再次执行。
  - **函数原型：** `void lv_timer_reset(lv_timer_t * timer);`
  - **参数：** `timer` - 目标定时器的句柄。

### 6.5 获取信息

- `lv_timer_get_idle()`
  - **描述：** 获取 `lv_timer_handler()` 的空闲时间百分比。这个值可以用来衡量系统在处理 LVGL 定时器任务上的负载情况。注意，它只衡量 `lv_timer_handler` 内部的空闲时间，而不是整个系统的CPU空闲率。
  - **函数原型：** `uint32_t lv_timer_get_idle(void);`
  - **返回值：** 0 到 100 之间的空闲百分比。
- `lv_timer_get_next()`
  - **描述：** 获取链表中下一个将要到期并执行的定时器。
  - **函数原型：** `lv_timer_t * lv_timer_get_next(lv_timer_t * timer);`
  - **参数：** `timer` - 一个定时器句柄。如果为 `NULL`，则返回第一个定时器。
  - **返回值：** 返回下一个定时器的句柄，如果没有更多定时器则返回 `NULL`。

## 7. LVGL事件

事件(Event)在 LVGL 中非常重要，它是连接用户 GUI 界面和硬件设备（例如 LED）之间的桥梁，我们可以将其理解为某种同类型操作动作的集合， 例如， 短按、长按、按下并释放、聚焦，等等。 下面我们以一个 LED 控制的示例，帮助大家理解事件的作用：

![屏幕截图 2025-08-22 102134.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-10-21-40-屏幕截图%202025-08-22%20102134.png)

在上图的示例中， 当用户短按按钮部件（发生事件），将会触发相应的事件回调函数，在该回调函数中，我们可以调用底层的 LED 驱动，从而实现 LED 的控制。

接下来，我们介绍 LVGL的事件处理机制， 该机制非常完善， 它能够监听事件，识别事件源，并完成事件处理， 处理机制的示意图如下：

![屏幕截图 2025-08-22 103028.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/22-10-30-32-屏幕截图%202025-08-22%20103028.png)

由上图可知， LVGL 的事件处理机制分为三部分：

1. 事件源， 能够产生事件的部件，在 LVGL 中，每一个部件都可以触发事件。

2. 事件， 用户对部件的操作动作，例如： 短按、 长按等。

3. 事件监听器， 接收事件、解释事件并处理用户代码。

---
