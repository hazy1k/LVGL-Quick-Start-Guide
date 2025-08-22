#ifndef __NORFLASH_EX_H
#define __NORFLASH_EX_H

#include "sys.h"

void norflash_ex_erase_chip(void);              /* NOR FLASH ȫƬ���� */
uint16_t norflash_ex_read_id(void);             /* NOR FLASH��ȡID */
void norflash_ex_erase_sector(uint32_t addr);   /* NOR FLASH �������� */
uint8_t norflash_ex_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);  /* NOR FLASHд������ */
void norflash_ex_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);      /* NOR FLASH��ȡ���� */

#endif
