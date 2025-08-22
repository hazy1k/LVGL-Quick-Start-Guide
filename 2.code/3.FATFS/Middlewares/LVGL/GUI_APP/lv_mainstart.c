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
