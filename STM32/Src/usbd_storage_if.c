#include "usbd_storage_if.h"
#include "sd.h"
#include "lcd.h"

const int8_t STORAGE_Inquirydata_FS[] = {
	/* 36 */
	/* LUN 0 */
	0x00,
	0x80,
	0x02,
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,
	0x00,
	'S', 'T', 'M', ' ', ' ', ' ', ' ', ' ', /* Manufacturer : 8 bytes */
	'P', 'r', 'o', 'd', 'u', 'c', 't', ' ', /* Product      : 16 Bytes */
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	'0', '.', '0', '1' /* Version      : 4 Bytes */
};

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t STORAGE_Init_FS(uint8_t lun);
static int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size);
static int8_t STORAGE_IsReady_FS(uint8_t lun);
static int8_t STORAGE_IsWriteProtected_FS(uint8_t lun);
static int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len);
static int8_t STORAGE_GetMaxLun_FS(void);

USBD_StorageTypeDef USBD_Storage_Interface_fops_FS =
	{
		STORAGE_Init_FS,
		STORAGE_GetCapacity_FS,
		STORAGE_IsReady_FS,
		STORAGE_IsWriteProtected_FS,
		STORAGE_Read_FS,
		STORAGE_Write_FS,
		STORAGE_GetMaxLun_FS,
		(int8_t *)STORAGE_Inquirydata_FS};

int8_t STORAGE_Init_FS(uint8_t lun)
{
	return (USBD_OK);
}

int8_t STORAGE_GetCapacity_FS(uint8_t lun, uint32_t *block_num, uint16_t *block_size)
{
	if (!SD_Present || !SD_USBCardReader)
	{
		*block_num = 0;
		*block_size = 0;
		return (USBD_OK);
	}

	*block_num = sdinfo.SECTOR_COUNT;
	*block_size = sdinfo.BLOCK_SIZE;
	return (USBD_OK);
}

int8_t STORAGE_IsReady_FS(uint8_t lun)
{
	if (!SD_USBCardReader)
		return (USBD_FAIL);

	if (!SD_Present || SD_RecordInProcess || SD_CommandInProcess)
		return (USBD_FAIL);
	return (USBD_OK);
}

int8_t STORAGE_IsWriteProtected_FS(uint8_t lun)
{
	return (USBD_OK);
}

int8_t STORAGE_Read_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (!SD_USBCardReader)
		return (USBD_FAIL);

	if (SPI_busy || SPI_process || SD_BusyByUSB || !SD_Present || SD_RecordInProcess || SD_CommandInProcess)
		return (USBD_FAIL);

	//HAL_SD_ReadBlocks(&hsd, buf, blk_addr, (uint32_t) blk_len, 10);
	SD_BusyByUSB = true;
	if (!(sdinfo.type & CT_BLOCK))
		blk_addr *= 512; // Convert to byte address if needed

	if (blk_len == 1) // Single block read
	{
		if ((SD_cmd(CMD17, blk_addr) == 0) && SD_Read_Block(buf, 512)) // READ_SINGLE_BLOCK
			blk_len = 0;
	}
	else // Multiple block read
	{
		if (SD_cmd(CMD18, blk_addr) == 0) // READ_MULTIPLE_BLOCK
		{
			do
			{
				if (!SD_Read_Block(buf, 512))
					break;
				buf += 512;
			} while (--blk_len);
			SD_cmd(CMD12, 0); // STOP_TRANSMISSION
		}
	}
	SPI_Release();

	SD_BusyByUSB = false;
	if (blk_len == 0)
		SD_Present_tryTime = HAL_GetTick() + 10000;
	else
	{
		SD_Mounted = false;
		SD_Present = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
	}

	return blk_len ? USBD_FAIL : USBD_OK;
}

int8_t STORAGE_Write_FS(uint8_t lun, uint8_t *buf, uint32_t blk_addr, uint16_t blk_len)
{
	if (!SD_USBCardReader)
		return (USBD_FAIL);

	if (SPI_busy || SPI_process || SD_BusyByUSB || !SD_Present || SD_RecordInProcess || SD_CommandInProcess)
		return (USBD_FAIL);

	//HAL_SD_WriteBlocks(&hsd, buf, blk_addr, (uint32_t) blk_len, 10);
	SD_BusyByUSB = true;
	if (!(sdinfo.type & CT_BLOCK))
		blk_addr *= 512; /* Convert to byte address if needed */
	if (blk_len == 1)	 /* Single block write */
	{
		if ((SD_cmd(CMD24, blk_addr) == 0) /* WRITE_BLOCK */
			&& SD_Write_Block((BYTE *)buf, 0xFE, true))
			blk_len = 0;
	}
	else /* Multiple block write */
	{
		if (sdinfo.type & CT_SDC)
		{
			SD_cmd(ACMD23, blk_len); /* Predefine number of sectors */
		}

		if (SD_cmd(CMD25, blk_addr) == 0)
		{ /* WRITE_MULTIPLE_BLOCK */
			do
			{
				if (!SD_Write_Block((BYTE *)buf, 0xFC, true))
					break;
				buf += 512;
			} while (--blk_len);
			if (!SD_Write_Block(0, 0xFD, true)) /* STOP_TRAN token */
			{
				blk_len = 1;
			}
			SPI_wait_ready();
		}
	}
	SPI_Release();

	SD_BusyByUSB = false;
	if (blk_len == 0)
		SD_Present_tryTime = HAL_GetTick() + 10000;
	else
	{
		SD_Mounted = false;
		SD_Present = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
	}

	return blk_len ? USBD_FAIL : USBD_OK;
}

int8_t STORAGE_GetMaxLun_FS(void)
{
	return 0;
}
