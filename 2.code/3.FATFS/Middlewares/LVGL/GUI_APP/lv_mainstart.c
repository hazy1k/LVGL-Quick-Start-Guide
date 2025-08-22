#include "lv_mainstart.h"
#include "lvgl.h"
#include <stdio.h>

/**
 * @brief  ��ȡָ��λ��
 * @param  fd���ļ�ָ��
 * @return ��������
 */
long lv_tell(lv_fs_file_t *fd)
{
    uint32_t pos = 0;
    lv_fs_tell(fd, &pos); // ��ȡ�ļ�ָ��λ��
    return pos;
}

/**
 * @brief  �ļ�ϵͳ����
 * @param  ��
 * @return ��
 */
static void lv_fs_test(void)
{
    char rbuf[30] = {0};
    uint32_t rsize = 0;
    lv_fs_file_t fd;
    lv_fs_res_t res;
    res = lv_fs_open(&fd, "0:/SYSTEM/LV_FATFS/Fatfs_test.txt", LV_FS_MODE_RD); // ���Դ򿪲����ļ�
    if (res != LV_FS_RES_OK) // ��ʧ��
    {
        printf("open 0:/Fatfs_test.txt ERROR\n");
        return ;
    }
    lv_tell(&fd); // ��ȡ�ļ�ָ��λ��
    lv_fs_seek(&fd, 0,LV_FS_SEEK_SET); // �ص��ļ���ʼλ��
    lv_tell(&fd);
    res = lv_fs_read(&fd, rbuf, 100, &rsize); // ��ȡ�ļ�����
    if (res != LV_FS_RES_OK)
    {
        printf("read ERROR\n");
        return ;
    }
    lv_tell(&fd);
    printf("READ(%d): %s",rsize , rbuf); // ��ӡ�ļ�����
    lv_fs_close(&fd); // �ر��ļ�
}

/**
 * @brief  �ļ�ϵͳ��ʾ
 * @param  ��
 * @return ��
 */
void lv_mainstart(void)
{
    lv_obj_t *label = lv_label_create(lv_scr_act()); // ������ǩ
    lv_label_set_text(label, "FATFS TEST"); // ���ñ�ǩ����
    lv_obj_center(label); // ������ʾ
    lv_fs_test(); // �ļ�ϵͳ����
}
