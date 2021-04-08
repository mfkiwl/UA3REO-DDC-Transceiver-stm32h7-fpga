#include "sd.h"
#include "main.h"
#include "fatfs.h"
#include "functions.h"
#include "lcd.h"
#include "ff_gen_drv.h"
#include "user_diskio.h"
#include "system_menu.h"
#include "vocoder.h"
#include "filemanager.h"

SRAM FATFS SDFatFs = {0};
sd_info_ptr sdinfo = {
	.type = 0,
	.SECTOR_COUNT = 0,
	.BLOCK_SIZE = 512,
	.CAPACITY = 0,
};
extern Disk_drvTypeDef disk;
bool SD_RecordInProcess = false;
bool SD_CommandInProcess = false;
bool SD_underrun = false;
bool SD_NeedStopRecord = false;
uint32_t SD_RecordBufferIndex = 0;
bool SD_Present = false;
bool SD_BusyByUSB = false;
bool SD_USBCardReader = false;
uint32_t SD_Present_tryTime = 0;
bool SD_Mounted = false;
static SD_COMMAND SD_currentCommand = SDCOMM_IDLE;

SRAM static FIL File = {0};
SRAM static FILINFO fileInfo = {0};
SRAM static DIR dir = {0};
SRAM BYTE SD_workbuffer_A[_MAX_SS] = {0};
SRAM BYTE SD_workbuffer_B[_MAX_SS] = {0};
SRAM static WAV_header wav_hdr = {0};
BYTE SD_workbuffer_current = false; //false - fill A save B, true - fill B save A

static void SDCOMM_CHECKSD_handler(void);
static void SDCOMM_LISTROOT_handler(void);
static void SDCOMM_MKFS_handler(void);
static void SDCOMM_EXPORT_SETT_handler(void);
static void SDCOMM_IMPORT_SETT_handler(void);
static bool SD_WRITE_SETT_LINE(char *name, uint32_t *value, SystemMenuType type);
static bool SD_WRITE_SETT_STRING(char *name, char *value);
static void SDCOMM_PARSE_SETT_LINE(char *line);
static bool SDCOMM_CREATE_RECORD_FILE_handler(void);
static bool SDCOMM_WRITE_PACKET_RECORD_FILE_handler(void);
static void SDCOMM_LIST_DIRECTORY_handler(void);
static void SDCOMM_DELETE_FILE_handler(void);

bool SD_isIdle(void)
{
	if (SD_currentCommand == SDCOMM_IDLE)
		return true;
	else
		return false;
}

bool SD_doCommand(SD_COMMAND command, bool force)
{
	if (SD_Mounted)
	{
		if (SD_CommandInProcess && !force)
		{
			SD_underrun = true;
		}
		else
		{
			SD_CommandInProcess = true;
			SD_currentCommand = command;
			return true;
		}
		return false;
	}
	else
	{
		if (LCD_systemMenuOpened)
			LCD_showInfo("SD card not found", true);
		else
			LCD_showTooltip("SD card not found");
		return false;
	}
}

void SD_Process(void)
{
	if (SD_BusyByUSB)
		return;

	//Init card
	if (!SD_Present && (HAL_GetTick() - SD_Present_tryTime) > SD_CARD_SCAN_INTERVAL)
	{
		SD_Present_tryTime = HAL_GetTick();
		SD_Mounted = false;

		disk.is_initialized[SDFatFs.drv] = false;
		if (disk_initialize(SDFatFs.drv) == RES_OK)
		{
			println("[OK] SD Card Inserted: ", (uint32_t)(sdinfo.CAPACITY / (uint64_t)1024 / (uint64_t)1024), "Mb");
			SD_Present = true;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
	}
	//Mount volume
	if (SD_Present && !SD_Mounted)
	{
		if (f_mount(&SDFatFs, (TCHAR const *)USERPath, 0) != FR_OK)
		{
			SD_Present = false;
		}
		else
		{
			SD_Mounted = true;
		}
	}
	//Do actions
	if (SD_Mounted)
	{
		if (SD_currentCommand != SDCOMM_IDLE)
			SD_Present_tryTime = HAL_GetTick();
		switch (SD_currentCommand)
		{
		case SDCOMM_IDLE:
			//check SD card inserted if idle
			if ((SD_Present_tryTime < HAL_GetTick()) && (HAL_GetTick() - SD_Present_tryTime) > SD_CARD_SCAN_INTERVAL && !SD_RecordInProcess)
			{
				SD_doCommand(SDCOMM_CHECK_SD, false);
				return;
			}
			//
			break;
		case SDCOMM_CHECK_SD:
			SDCOMM_CHECKSD_handler();
			break;
		case SDCOMM_LIST_ROOT:
			SDCOMM_LISTROOT_handler();
			break;
		case SDCOMM_FORMAT:
			SDCOMM_MKFS_handler();
			break;
		case SDCOMM_EXPORT_SETTINGS:
			SDCOMM_EXPORT_SETT_handler();
			break;
		case SDCOMM_IMPORT_SETTINGS:
			SDCOMM_IMPORT_SETT_handler();
			break;
		case SDCOMM_START_RECORD:
			SDCOMM_CREATE_RECORD_FILE_handler();
			break;
		case SDCOMM_PROCESS_RECORD:
			SDCOMM_WRITE_PACKET_RECORD_FILE_handler();
			break;
		case SDCOMM_LIST_DIRECTORY:
			SDCOMM_LIST_DIRECTORY_handler();
			break;
		case SDCOMM_DELETE_FILE:
			SDCOMM_DELETE_FILE_handler();
			break;
		}
		SD_CommandInProcess = false;
		SD_currentCommand = SDCOMM_IDLE;
	}
}

static void SDCOMM_DELETE_FILE_handler(void)
{
	if(f_unlink((TCHAR*)SD_workbuffer_A) || f_unlink((TCHAR*)SD_workbuffer_A)) //two try'es
	{
		println("File deleted: ", (char*)SD_workbuffer_A);
		SD_doCommand(SDCOMM_LIST_DIRECTORY, true);
		SD_Process();
	}
	else
	{
		println("File delete error: ", (char*)SD_workbuffer_A);
	}
	LCD_redraw(false);
}

static void SDCOMM_LIST_DIRECTORY_handler(void)
{
	FILEMANAGER_files_count = 0;
	uint16_t FILEMANAGER_files_added = 0;
	dma_memset(FILEMANAGER_LISTING, 0, sizeof(FILEMANAGER_LISTING));
	if (f_opendir(&dir, FILEMANAGER_CurrentPath) == FR_OK)
	{
		while (f_readdir(&dir, &fileInfo) == FR_OK && fileInfo.fname[0])
		{
			if (fileInfo.fattrib & AM_DIR)
			{
				println("[DIR] ", fileInfo.fname);

				if (FILEMANAGER_files_startindex <= FILEMANAGER_files_count && FILEMANAGER_files_added < FILEMANAGER_LISTING_MAX_FILES)
				{
					strcat(FILEMANAGER_LISTING[FILEMANAGER_files_added], "[DIR] ");
					strncat(FILEMANAGER_LISTING[FILEMANAGER_files_added], fileInfo.fname, (FILEMANAGER_LISTING_MAX_FILELEN - 6));
					FILEMANAGER_files_added++;
				}
				FILEMANAGER_files_count++;
			}
		}
		f_closedir(&dir);

		f_opendir(&dir, FILEMANAGER_CurrentPath);
		while (f_readdir(&dir, &fileInfo) == FR_OK && fileInfo.fname[0])
		{
			if (!(fileInfo.fattrib & AM_DIR))
			{
				println("[FILE] ", fileInfo.fname);
				if (FILEMANAGER_files_startindex <= FILEMANAGER_files_count && FILEMANAGER_files_added < FILEMANAGER_LISTING_MAX_FILES)
				{
					strncat(FILEMANAGER_LISTING[FILEMANAGER_files_added], fileInfo.fname, (FILEMANAGER_LISTING_MAX_FILELEN));
					FILEMANAGER_files_added++;
				}
				FILEMANAGER_files_count++;
			}
		}
		f_closedir(&dir);
		println("read complete");
	}
	else
	{
		LCD_showInfo("SD error", true);
		SYSMENU_eventCloseAllSystemMenu();
		SD_Present = false;
	}
	LCD_UpdateQuery.SystemMenuRedraw = true;
}

static void SDCOMM_CHECKSD_handler(void)
{
	if (f_mount(&SDFatFs, (TCHAR const *)USERPath, 1) == FR_OK)
	{
		//sendToDebug_str("sd chk ok");
	}
	else
	{
		SD_RecordInProcess = false;
		SD_Present = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
	}
}

static bool SDCOMM_CREATE_RECORD_FILE_handler(void)
{
	char filename[64] = {0};
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
	sprintf(filename, "rec-%02d.%02d.%02d-%02d.%02d.%02d-%d.wav", sDate.Date, sDate.Month, sDate.Year, sTime.Hours, sTime.Minutes, sTime.Seconds, CurrentVFO()->Freq);
	println(filename);
	if (f_open(&File, filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{
		dma_memset(SD_workbuffer_A, 0x00, sizeof(SD_workbuffer_A));
		dma_memset(&wav_hdr, 0x00, sizeof(wav_hdr));
		// RIFF header
		wav_hdr.riffsig = 0x46464952;
		wav_hdr.filesize = sizeof(wav_hdr);
		wav_hdr.wavesig = 0x45564157;

		// format chunk
		wav_hdr.fmtsig = 0x20746D66;
		//wav_hdr.fmtsize		= 16;  //PCM
		wav_hdr.fmtsize = 0x14; //IMA-ADPCM
		//wav_hdr.type			= 1; //PCM
		wav_hdr.type = 0x11; //IMA-ADPCM
		wav_hdr.nch = 1;	 //Mono
		wav_hdr.freq = 48000;
		//wav_hdr.rate			= 96000; //PCM 16bit
		wav_hdr.rate = (wav_hdr.freq * wav_hdr.nch * 256 / 505); //IMA-ADPCM byte rate, 48000 * Nch * 256 / 505
		//wav_hdr.block			= 2; //PCM
		wav_hdr.block = 256; //IMA-ADPCM block align, mono 256, stereo 512 */
		//wav_hdr.bits			= 16; //PCM
		wav_hdr.bits = 4;			  //IMA-ADPCM
		wav_hdr.bytes_extra_data = 2; //IMA-ADPCM bytes extra data
		wav_hdr.extra_data = 505;	  //IMA-ADPCM extra data

		// data chunk
		wav_hdr.datasig = 0x61746164;
		wav_hdr.datasize = 0;

		uint32_t byteswritten;
		f_write(&File, &wav_hdr, sizeof(wav_hdr), &byteswritten);

		SD_RecordInProcess = true;
		LCD_UpdateQuery.StatusInfoBar = true;
		LCD_showTooltip("Start recording");
		return true;
	}
	else
	{
		LCD_showTooltip("SD error");
		SD_RecordInProcess = false;
		SD_Present = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
		LCD_UpdateQuery.StatusInfoBar = true;
	}
	return false;
}

static bool SDCOMM_WRITE_PACKET_RECORD_FILE_handler(void)
{
	//write to SD
	uint32_t byteswritten;
	FRESULT res;
	if (SD_workbuffer_current)
		res = f_write(&File, SD_workbuffer_A, sizeof(SD_workbuffer_A), (void *)&byteswritten);
	else
		res = f_write(&File, SD_workbuffer_B, sizeof(SD_workbuffer_B), (void *)&byteswritten);
	if ((byteswritten == 0) || (res != FR_OK))
	{
		SD_Present = false;
		SD_RecordInProcess = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
		LCD_UpdateQuery.StatusInfoBar = true;
		LCD_showTooltip("SD error");
		return false;
	}
	else
	{
		//store size
		wav_hdr.datasize += byteswritten;
	}

	//stop record
	if (SD_NeedStopRecord)
	{
		SD_RecordInProcess = false;
		LCD_UpdateQuery.StatusInfoBar = true;
		LCD_showTooltip("Stop recording");

		//update wav length
		f_lseek(&File, 0);
		f_write(&File, &wav_hdr, sizeof(wav_hdr), &byteswritten);

		f_close(&File);
		SD_NeedStopRecord = false;
		return true;
	}
	return true;
}

static bool SD_WRITE_SETT_LINE(char *name, uint32_t *value, SystemMenuType type)
{
	uint32_t byteswritten;
	char valbuff[64] = {0};
	float32_t tmp_float = 0;

	dma_memset(SD_workbuffer_A, 0x00, sizeof(SD_workbuffer_A));

	strcat((char *)SD_workbuffer_A, name);
	strcat((char *)SD_workbuffer_A, " = ");
	switch (type)
	{
	case SYSMENU_BOOLEAN:
		sprintf(valbuff, "%u", (uint8_t)*value);
		break;
	case SYSMENU_UINT8:
		sprintf(valbuff, "%u", (uint8_t)*value);
		break;
	case SYSMENU_ENUM:
		sprintf(valbuff, "%u", (uint8_t)*value);
		break;
	case SYSMENU_ENUMR:
		sprintf(valbuff, "%u", (uint8_t)*value);
		break;
	case SYSMENU_UINT16:
		sprintf(valbuff, "%u", (uint16_t)*value);
		break;
	case SYSMENU_UINT32:
		sprintf(valbuff, "%u", (uint32_t)*value);
		break;
	case SYSMENU_INT8:
		sprintf(valbuff, "%d", (int8_t)*value);
		break;
	case SYSMENU_INT16:
		sprintf(valbuff, "%d", (int16_t)*value);
		break;
	case SYSMENU_INT32:
		sprintf(valbuff, "%d", (int32_t)*value);
		break;
	case SYSMENU_FLOAT32:
		dma_memcpy(&tmp_float, value, sizeof(float32_t));
		sprintf(valbuff, "%.6f", (double)tmp_float);
		break;
	case SYSMENU_FUNCBUTTON:
		sprintf(valbuff, "%u", (uint8_t)*value);
		break;
	case SYSMENU_RUN:
	case SYSMENU_UINT32R:
	case SYSMENU_MENU:
	case SYSMENU_HIDDEN_MENU:
	case SYSMENU_INFOLINE:
		break;
	}
	strcat((char *)SD_workbuffer_A, valbuff);
	strcat((char *)SD_workbuffer_A, "\r\n");

	FRESULT res = f_write(&File, SD_workbuffer_A, strlen((char *)SD_workbuffer_A), (void *)&byteswritten);
	if ((byteswritten == 0) || (res != FR_OK))
	{
		SD_Present = false;
		return false;
	}
	return true;
}

static bool SD_WRITE_SETT_STRING(char *name, char *value)
{
	uint32_t byteswritten;

	dma_memset(SD_workbuffer_A, 0x00, sizeof(SD_workbuffer_A));

	strcat((char *)SD_workbuffer_A, name);
	strcat((char *)SD_workbuffer_A, " = ");
	strcat((char *)SD_workbuffer_A, value);
	strcat((char *)SD_workbuffer_A, "\r\n");

	FRESULT res = f_write(&File, SD_workbuffer_A, strlen((char *)SD_workbuffer_A), (void *)&byteswritten);
	if ((byteswritten == 0) || (res != FR_OK))
	{
		SD_Present = false;
		return false;
	}
	return true;
}

static void SDCOMM_EXPORT_SETT_handler(void)
{
	LCD_showInfo("Exporting...", false);
	if (f_open(&File, "wolf.ini", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK)
	{
		//TRX
		bool res = SD_WRITE_SETT_LINE("TRX.VFO_A.Freq", (uint32_t *)&TRX.VFO_A.Freq, SYSMENU_UINT32);
		if (res)
		{
			SD_WRITE_SETT_LINE("TRX.VFO_A.Mode", (uint32_t *)&TRX.VFO_A.Mode, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_A.LPF_RX_Filter_Width", (uint32_t *)&TRX.VFO_A.LPF_RX_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_A.LPF_TX_Filter_Width", (uint32_t *)&TRX.VFO_A.LPF_TX_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_A.HPF_Filter_Width", (uint32_t *)&TRX.VFO_A.HPF_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_A.ManualNotchFilter", (uint32_t *)&TRX.VFO_A.ManualNotchFilter, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_A.AutoNotchFilter", (uint32_t *)&TRX.VFO_A.AutoNotchFilter, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_A.NotchFC", (uint32_t *)&TRX.VFO_A.NotchFC, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_A.DNR_Type", (uint32_t *)&TRX.VFO_A.DNR_Type, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.VFO_A.AGC", (uint32_t *)&TRX.VFO_A.AGC, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_A.FM_SQL_threshold", (uint32_t *)&TRX.VFO_A.FM_SQL_threshold, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.VFO_B.Freq", (uint32_t *)&TRX.VFO_B.Freq, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.Mode", (uint32_t *)&TRX.VFO_B.Mode, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.LPF_RX_Filter_Width", (uint32_t *)&TRX.VFO_B.LPF_RX_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.LPF_TX_Filter_Width", (uint32_t *)&TRX.VFO_B.LPF_TX_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.HPF_Filter_Width", (uint32_t *)&TRX.VFO_B.HPF_Filter_Width, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.ManualNotchFilter", (uint32_t *)&TRX.VFO_B.ManualNotchFilter, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_B.AutoNotchFilter", (uint32_t *)&TRX.VFO_B.AutoNotchFilter, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_B.NotchFC", (uint32_t *)&TRX.VFO_B.NotchFC, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.VFO_B.DNR_Type", (uint32_t *)&TRX.VFO_B.DNR_Type, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.VFO_B.AGC", (uint32_t *)&TRX.VFO_B.AGC, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VFO_B.FM_SQL_threshold", (uint32_t *)&TRX.VFO_B.FM_SQL_threshold, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.current_vfo", (uint32_t *)&TRX.current_vfo, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_Driver", (uint32_t *)&TRX.ADC_Driver, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.LNA", (uint32_t *)&TRX.LNA, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ATT", (uint32_t *)&TRX.ATT, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ATT_DB", (uint32_t *)&TRX.ATT_DB, SYSMENU_FLOAT32);
			SD_WRITE_SETT_LINE("TRX.ATT_STEP", (uint32_t *)&TRX.ATT_STEP, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.Fast", (uint32_t *)&TRX.Fast, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_PGA", (uint32_t *)&TRX.ADC_PGA, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ANT", (uint32_t *)&TRX.ANT, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.RF_Filters", (uint32_t *)&TRX.RF_Filters, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.RF_Power", (uint32_t *)&TRX.RF_Power, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.ShiftEnabled", (uint32_t *)&TRX.ShiftEnabled, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.SHIFT_INTERVAL", (uint32_t *)&TRX.SHIFT_INTERVAL, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.TWO_SIGNAL_TUNE", (uint32_t *)&TRX.TWO_SIGNAL_TUNE, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.SAMPLERATE_MAIN", (uint32_t *)&TRX.SAMPLERATE_MAIN, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.SAMPLERATE_WFM", (uint32_t *)&TRX.SAMPLERATE_WFM, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FRQ_STEP", (uint32_t *)&TRX.FRQ_STEP, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.FRQ_FAST_STEP", (uint32_t *)&TRX.FRQ_FAST_STEP, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.FRQ_ENC_STEP", (uint32_t *)&TRX.FRQ_ENC_STEP, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.FRQ_ENC_FAST_STEP", (uint32_t *)&TRX.FRQ_ENC_FAST_STEP, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.Debug_Type", (uint32_t *)&TRX.Debug_Type, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.BandMapEnabled", (uint32_t *)&TRX.BandMapEnabled, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.InputType", (uint32_t *)&TRX.InputType, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.AutoGain", (uint32_t *)&TRX.AutoGain, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.CLAR", (uint32_t *)&TRX.CLAR, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.Dual_RX", (uint32_t *)&TRX.Dual_RX, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.Encoder_Accelerate", (uint32_t *)&TRX.Encoder_Accelerate, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.Dual_RX_Type", (uint32_t *)&TRX.Dual_RX_Type, SYSMENU_UINT8);
			SD_WRITE_SETT_STRING("TRX.CALLSIGN", TRX.CALLSIGN);
			SD_WRITE_SETT_STRING("TRX.LOCATOR", TRX.LOCATOR);
			SD_WRITE_SETT_LINE("TRX.Transverter_Enabled", (uint32_t *)&TRX.Transverter_Enabled, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.Transverter_Offset_Mhz", (uint32_t *)&TRX.Transverter_Offset_Mhz, SYSMENU_UINT16);
			//AUDIO
			SD_WRITE_SETT_LINE("TRX.IF_Gain", (uint32_t *)&TRX.IF_Gain, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.AGC_GAIN_TARGET2", (uint32_t *)&TRX.AGC_GAIN_TARGET, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.MIC_GAIN", (uint32_t *)&TRX.MIC_GAIN, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.RX_EQ_LOW", (uint32_t *)&TRX.RX_EQ_LOW, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.RX_EQ_MID", (uint32_t *)&TRX.RX_EQ_MID, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.RX_EQ_HIG", (uint32_t *)&TRX.RX_EQ_HIG, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.MIC_EQ_LOW", (uint32_t *)&TRX.MIC_EQ_LOW, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.MIC_EQ_MID", (uint32_t *)&TRX.MIC_EQ_MID, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.MIC_EQ_HIG", (uint32_t *)&TRX.MIC_EQ_HIG, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.MIC_REVERBER", (uint32_t *)&TRX.MIC_REVERBER, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.DNR_SNR_THRESHOLD", (uint32_t *)&TRX.DNR_SNR_THRESHOLD, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.DNR_AVERAGE", (uint32_t *)&TRX.DNR_AVERAGE, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.DNR_MINIMAL", (uint32_t *)&TRX.DNR_MINIMAL, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.NOISE_BLANKER", (uint32_t *)&TRX.NOISE_BLANKER, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.RX_AGC_SSB_speed", (uint32_t *)&TRX.RX_AGC_SSB_speed, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.RX_AGC_CW_speed", (uint32_t *)&TRX.RX_AGC_CW_speed, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.TX_Compressor_speed", (uint32_t *)&TRX.TX_Compressor_speed, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.TX_Compressor_maxgain", (uint32_t *)&TRX.TX_Compressor_maxgain, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.CW_LPF_Filter2", (uint32_t *)&TRX.CW_LPF_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.CW_HPF_Filter2", (uint32_t *)&TRX.CW_HPF_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.SSB_LPF_RX_Filter", (uint32_t *)&TRX.SSB_LPF_RX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.SSB_LPF_TX_Filter", (uint32_t *)&TRX.SSB_LPF_TX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.SSB_HPF_Filter", (uint32_t *)&TRX.SSB_HPF_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.AM_LPF_RX_Filter", (uint32_t *)&TRX.AM_LPF_RX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.AM_LPF_TX_Filter", (uint32_t *)&TRX.AM_LPF_TX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.FM_LPF_RX_Filter", (uint32_t *)&TRX.FM_LPF_RX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.FM_LPF_TX_Filter", (uint32_t *)&TRX.FM_LPF_TX_Filter, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.Beeper", (uint32_t *)&TRX.Beeper, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.VAD_Squelch", (uint32_t *)&TRX.VAD_Squelch, SYSMENU_BOOLEAN);
			//CW
			SD_WRITE_SETT_LINE("TRX.CWDecoder", (uint32_t *)&TRX.CWDecoder, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.CW_GENERATOR_SHIFT_HZ", (uint32_t *)&TRX.CW_GENERATOR_SHIFT_HZ, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.CW_Key_timeout", (uint32_t *)&TRX.CW_Key_timeout, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.CW_SelfHear", (uint32_t *)&TRX.CW_SelfHear, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.CW_KEYER", (uint32_t *)&TRX.CW_KEYER, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.CW_KEYER_WPM", (uint32_t *)&TRX.CW_KEYER_WPM, SYSMENU_UINT16);
			SD_WRITE_SETT_LINE("TRX.CW_GaussFilter", (uint32_t *)&TRX.CW_GaussFilter, SYSMENU_BOOLEAN);
			//SCREEN
			SD_WRITE_SETT_LINE("TRX.ColorThemeId", (uint32_t *)&TRX.ColorThemeId, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.LayoutThemeId", (uint32_t *)&TRX.LayoutThemeId, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Enabled", (uint32_t *)&TRX.FFT_Enabled, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_Zoom", (uint32_t *)&TRX.FFT_Zoom, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_ZoomCW", (uint32_t *)&TRX.FFT_ZoomCW, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.LCD_Brightness", (uint32_t *)&TRX.LCD_Brightness, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Speed", (uint32_t *)&TRX.FFT_Speed, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Sensitivity", (uint32_t *)&TRX.FFT_Sensitivity, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Averaging", (uint32_t *)&TRX.FFT_Averaging, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Window", (uint32_t *)&TRX.FFT_Window, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Height", (uint32_t *)&TRX.FFT_Height, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Style", (uint32_t *)&TRX.FFT_Style, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Color", (uint32_t *)&TRX.FFT_Color, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Compressor", (uint32_t *)&TRX.FFT_Compressor, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WTF_Moving", (uint32_t *)&TRX.WTF_Moving, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_Grid", (uint32_t *)&TRX.FFT_Grid, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Background", (uint32_t *)&TRX.FFT_Background, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_Lens", (uint32_t *)&TRX.FFT_Lens, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_HoldPeaks", (uint32_t *)&TRX.FFT_HoldPeaks, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_3D", (uint32_t *)&TRX.FFT_3D, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("TRX.FFT_Automatic", (uint32_t *)&TRX.FFT_Automatic, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.FFT_ManualBottom", (uint32_t *)&TRX.FFT_ManualBottom, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("TRX.FFT_ManualTop", (uint32_t *)&TRX.FFT_ManualTop, SYSMENU_INT16);
			//ADC
			SD_WRITE_SETT_LINE("TRX.ADC_Driver", (uint32_t *)&TRX.ADC_Driver, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_PGA", (uint32_t *)&TRX.ADC_PGA, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_RAND", (uint32_t *)&TRX.ADC_RAND, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_SHDN", (uint32_t *)&TRX.ADC_SHDN, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.ADC_DITH", (uint32_t *)&TRX.ADC_DITH, SYSMENU_BOOLEAN);
			//WIFI
			SD_WRITE_SETT_LINE("TRX.WIFI_Enabled", (uint32_t *)&TRX.WIFI_Enabled, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WIFI_TIMEZONE", (uint32_t *)&TRX.WIFI_TIMEZONE, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("TRX.WIFI_CAT_SERVER", (uint32_t *)&TRX.WIFI_CAT_SERVER, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_STRING("TRX.WIFI_AP1", TRX.WIFI_AP1);
			SD_WRITE_SETT_STRING("TRX.WIFI_AP2", TRX.WIFI_AP2);
			SD_WRITE_SETT_STRING("TRX.WIFI_AP3", TRX.WIFI_AP3);
			SD_WRITE_SETT_STRING("TRX.WIFI_PASSWORD1", TRX.WIFI_PASSWORD1);
			SD_WRITE_SETT_STRING("TRX.WIFI_PASSWORD2", TRX.WIFI_PASSWORD2);
			SD_WRITE_SETT_STRING("TRX.WIFI_PASSWORD3", TRX.WIFI_PASSWORD3);
			//SERVICES
			SD_WRITE_SETT_LINE("TRX.SPEC_Begin", (uint32_t *)&TRX.SPEC_Begin, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.SPEC_End", (uint32_t *)&TRX.SPEC_End, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("TRX.SPEC_TopDBM", (uint32_t *)&TRX.SPEC_TopDBM, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("TRX.SPEC_BottomDBM", (uint32_t *)&TRX.SPEC_BottomDBM, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("TRX.WSPR_FREQ_OFFSET", (uint32_t *)&TRX.WSPR_FREQ_OFFSET, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_160", (uint32_t *)&TRX.WSPR_BANDS_160, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_80", (uint32_t *)&TRX.WSPR_BANDS_80, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_40", (uint32_t *)&TRX.WSPR_BANDS_40, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_30", (uint32_t *)&TRX.WSPR_BANDS_30, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_20", (uint32_t *)&TRX.WSPR_BANDS_20, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_17", (uint32_t *)&TRX.WSPR_BANDS_17, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_15", (uint32_t *)&TRX.WSPR_BANDS_15, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_12", (uint32_t *)&TRX.WSPR_BANDS_12, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_10", (uint32_t *)&TRX.WSPR_BANDS_10, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_6", (uint32_t *)&TRX.WSPR_BANDS_6, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("TRX.WSPR_BANDS_2", (uint32_t *)&TRX.WSPR_BANDS_2, SYSMENU_BOOLEAN);
			//CALIBRATION
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER_INVERT", (uint32_t *)&CALIBRATE.ENCODER_INVERT, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER2_INVERT", (uint32_t *)&CALIBRATE.ENCODER2_INVERT, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER_DEBOUNCE", (uint32_t *)&CALIBRATE.ENCODER_DEBOUNCE, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER2_DEBOUNCE", (uint32_t *)&CALIBRATE.ENCODER2_DEBOUNCE, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER_SLOW_RATE", (uint32_t *)&CALIBRATE.ENCODER_SLOW_RATE, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER_ON_FALLING", (uint32_t *)&CALIBRATE.ENCODER_ON_FALLING, SYSMENU_BOOLEAN);
			SD_WRITE_SETT_LINE("CALIBRATE.ENCODER_ACCELERATION", (uint32_t *)&CALIBRATE.ENCODER_ACCELERATION, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.CICFIR_GAINER_48K", (uint32_t *)&CALIBRATE.CICFIR_GAINER_48K_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.CICFIR_GAINER_96K", (uint32_t *)&CALIBRATE.CICFIR_GAINER_96K_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.CICFIR_GAINER_192K", (uint32_t *)&CALIBRATE.CICFIR_GAINER_192K_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.CICFIR_GAINER_384K", (uint32_t *)&CALIBRATE.CICFIR_GAINER_384K_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.TXCICFIR_GAINER2", (uint32_t *)&CALIBRATE.TXCICFIR_GAINER_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.DAC_GAINER2", (uint32_t *)&CALIBRATE.DAC_GAINER_val, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_2200m", (uint32_t *)&CALIBRATE.rf_out_power_2200m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_160m", (uint32_t *)&CALIBRATE.rf_out_power_160m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_80m", (uint32_t *)&CALIBRATE.rf_out_power_80m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_40m", (uint32_t *)&CALIBRATE.rf_out_power_40m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_30m", (uint32_t *)&CALIBRATE.rf_out_power_30m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_20m", (uint32_t *)&CALIBRATE.rf_out_power_20m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_17m", (uint32_t *)&CALIBRATE.rf_out_power_17m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_15m", (uint32_t *)&CALIBRATE.rf_out_power_15m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_12m", (uint32_t *)&CALIBRATE.rf_out_power_12m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_10m", (uint32_t *)&CALIBRATE.rf_out_power_10m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_6m", (uint32_t *)&CALIBRATE.rf_out_power_6m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.rf_out_power_2m", (uint32_t *)&CALIBRATE.rf_out_power_2m, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.smeter_calibration2", (uint32_t *)&CALIBRATE.smeter_calibration, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("CALIBRATE.adc_offset", (uint32_t *)&CALIBRATE.adc_offset, SYSMENU_INT16);
			SD_WRITE_SETT_LINE("CALIBRATE.LPF_END", (uint32_t *)&CALIBRATE.LPF_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_0_START", (uint32_t *)&CALIBRATE.BPF_0_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_0_END", (uint32_t *)&CALIBRATE.BPF_0_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_1_START", (uint32_t *)&CALIBRATE.BPF_1_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_1_END", (uint32_t *)&CALIBRATE.BPF_1_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_2_START", (uint32_t *)&CALIBRATE.BPF_2_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_2_END", (uint32_t *)&CALIBRATE.BPF_2_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_3_START", (uint32_t *)&CALIBRATE.BPF_3_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_3_END", (uint32_t *)&CALIBRATE.BPF_3_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_4_START", (uint32_t *)&CALIBRATE.BPF_4_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_4_END", (uint32_t *)&CALIBRATE.BPF_4_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_5_START", (uint32_t *)&CALIBRATE.BPF_5_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_5_END", (uint32_t *)&CALIBRATE.BPF_5_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_6_START", (uint32_t *)&CALIBRATE.BPF_6_START, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_6_END", (uint32_t *)&CALIBRATE.BPF_6_END, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.BPF_HPF", (uint32_t *)&CALIBRATE.BPF_HPF, SYSMENU_UINT32);
			SD_WRITE_SETT_LINE("CALIBRATE.SWR_FWD_Calibration", (uint32_t *)&CALIBRATE.SWR_FWD_Calibration, SYSMENU_FLOAT32);
			SD_WRITE_SETT_LINE("CALIBRATE.SWR_REF_Calibration", (uint32_t *)&CALIBRATE.SWR_REF_Calibration, SYSMENU_FLOAT32);
			SD_WRITE_SETT_LINE("CALIBRATE.VCXO_correction", (uint32_t *)&CALIBRATE.VCXO_correction, SYSMENU_INT8);
			SD_WRITE_SETT_LINE("CALIBRATE.FAN_MEDIUM_START", (uint32_t *)&CALIBRATE.FAN_MEDIUM_START, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.FAN_MEDIUM_STOP", (uint32_t *)&CALIBRATE.FAN_MEDIUM_STOP, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.FAN_FULL_START", (uint32_t *)&CALIBRATE.FAN_FULL_START, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.TRX_MAX_RF_TEMP", (uint32_t *)&CALIBRATE.TRX_MAX_RF_TEMP, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.TRX_MAX_SWR", (uint32_t *)&CALIBRATE.TRX_MAX_SWR, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.FM_DEVIATION_SCALE", (uint32_t *)&CALIBRATE.FM_DEVIATION_SCALE, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.TUNE_MAX_POWER", (uint32_t *)&CALIBRATE.TUNE_MAX_POWER, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.RTC_Coarse_Calibration", (uint32_t *)&CALIBRATE.RTC_Coarse_Calibration, SYSMENU_UINT8);
			SD_WRITE_SETT_LINE("CALIBRATE.RTC_Calibration", (uint32_t *)&CALIBRATE.RTC_Calibration, SYSMENU_INT16);
		}

		if (!res)
			LCD_showInfo("SD error", true);
		else
			LCD_showInfo("Settings export complete", true);
	}
	else
	{
		LCD_showInfo("SD error", true);
		SD_Present = false;
	}
	f_close(&File);
}

static void SDCOMM_PARSE_SETT_LINE(char *line)
{
	static IRAM2 char name[64] = {0};
	static IRAM2 char value[64] = {0};
	char *istr = strstr((char *)line, " = ");
	uint16_t len = (uint16_t)((uint32_t)istr - (uint32_t)line);
	dma_memset(name, 0x00, sizeof(name));
	dma_memset(value, 0x00, sizeof(value));
	if (len > 63)
		return;
	strncpy(name, (char *)line, len);
	strncpy(value, (char *)line + len + 3, len);

	uint32_t uintval = atol(value);
	int32_t intval = atol(value);
	float32_t floatval = atof(value);
	bool bval = false;
	if (uintval > 0)
		bval = true;

	/*sendToDebug_strln(name);
	sendToDebug_strln(value);
	sendToDebug_uint8(bval, false);
	sendToDebug_uint32(uintval, false);
	sendToDebug_int32(intval, false);
	sendToDebug_float32(floatval, false);
	sendToDebug_newline();
	sendToDebug_flush();*/

	//TRX
	if (strcmp(name, "TRX.VFO_A.Freq") == 0)
		TRX.VFO_A.Freq = uintval;
	if (strcmp(name, "TRX.VFO_A.Mode") == 0)
		TRX.VFO_A.Mode = uintval;
	if (strcmp(name, "TRX.VFO_A.LPF_RX_Filter_Width") == 0)
		TRX.VFO_A.LPF_RX_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_A.LPF_TX_Filter_Width") == 0)
		TRX.VFO_A.LPF_TX_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_A.HPF_Filter_Width") == 0)
		TRX.VFO_A.HPF_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_A.ManualNotchFilter") == 0)
		TRX.VFO_A.ManualNotchFilter = bval;
	if (strcmp(name, "TRX.VFO_A.AutoNotchFilter") == 0)
		TRX.VFO_A.AutoNotchFilter = bval;
	if (strcmp(name, "TRX.VFO_A.NotchFC") == 0)
		TRX.VFO_A.NotchFC = uintval;
	if (strcmp(name, "TRX.VFO_A.DNR_Type") == 0)
		TRX.VFO_A.DNR_Type = (uint8_t)uintval;
	if (strcmp(name, "TRX.VFO_A.AGC") == 0)
		TRX.VFO_A.AGC = bval;
	if (strcmp(name, "TRX.VFO_A.FM_SQL_threshold") == 0)
		TRX.VFO_A.FM_SQL_threshold = (uint8_t)uintval;
	if (strcmp(name, "TRX.VFO_B.Freq") == 0)
		TRX.VFO_B.Freq = uintval;
	if (strcmp(name, "TRX.VFO_B.Mode") == 0)
		TRX.VFO_B.Mode = uintval;
	if (strcmp(name, "TRX.VFO_B.LPF_RX_Filter_Width") == 0)
		TRX.VFO_B.LPF_RX_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_B.LPF_TX_Filter_Width") == 0)
		TRX.VFO_B.LPF_TX_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_B.HPF_Filter_Width") == 0)
		TRX.VFO_B.HPF_Filter_Width = uintval;
	if (strcmp(name, "TRX.VFO_B.ManualNotchFilter") == 0)
		TRX.VFO_B.ManualNotchFilter = bval;
	if (strcmp(name, "TRX.VFO_B.AutoNotchFilter") == 0)
		TRX.VFO_B.AutoNotchFilter = bval;
	if (strcmp(name, "TRX.VFO_B.NotchFC") == 0)
		TRX.VFO_B.NotchFC = uintval;
	if (strcmp(name, "TRX.VFO_B.DNR_Type") == 0)
		TRX.VFO_B.DNR_Type = (uint8_t)uintval;
	if (strcmp(name, "TRX.VFO_B.AGC") == 0)
		TRX.VFO_B.AGC = bval;
	if (strcmp(name, "TRX.VFO_B.FM_SQL_threshold") == 0)
		TRX.VFO_B.FM_SQL_threshold = (uint8_t)uintval;
	if (strcmp(name, "TRX.current_vfo") == 0)
		TRX.current_vfo = bval;
	if (strcmp(name, "TRX.LNA") == 0)
		TRX.LNA = bval;
	if (strcmp(name, "TRX.ATT") == 0)
		TRX.ATT = bval;
	if (strcmp(name, "TRX.ATT_DB") == 0)
		TRX.ATT_DB = floatval;
	if (strcmp(name, "TRX.ATT_STEP") == 0)
		TRX.ATT_STEP = (uint8_t)uintval;
	if (strcmp(name, "TRX.Fast") == 0)
		TRX.Fast = bval;
	if (strcmp(name, "TRX.ANT") == 0)
		TRX.ANT = bval;
	if (strcmp(name, "TRX.RF_Filters") == 0)
		TRX.RF_Filters = bval;
	if (strcmp(name, "TRX.RF_Power") == 0)
		TRX.RF_Power = (uint8_t)uintval;
	if (strcmp(name, "TRX.ShiftEnabled") == 0)
		TRX.ShiftEnabled = bval;
	if (strcmp(name, "TRX.SHIFT_INTERVAL") == 0)
		TRX.SHIFT_INTERVAL = (uint16_t)uintval;
	if (strcmp(name, "TRX.TWO_SIGNAL_TUNE") == 0)
		TRX.TWO_SIGNAL_TUNE = bval;
	if (strcmp(name, "TRX.SAMPLERATE_MAIN") == 0)
		TRX.SAMPLERATE_MAIN = (uint8_t)uintval;
	if (strcmp(name, "TRX.SAMPLERATE_WFM") == 0)
		TRX.SAMPLERATE_WFM = (uint8_t)uintval;
	if (strcmp(name, "TRX.FRQ_STEP") == 0)
		TRX.FRQ_STEP = (uint16_t)uintval;
	if (strcmp(name, "TRX.FRQ_FAST_STEP") == 0)
		TRX.FRQ_FAST_STEP = (uint16_t)uintval;
	if (strcmp(name, "TRX.FRQ_ENC_STEP") == 0)
		TRX.FRQ_ENC_STEP = (uint16_t)uintval;
	if (strcmp(name, "TRX.FRQ_ENC_FAST_STEP") == 0)
		TRX.FRQ_ENC_FAST_STEP = uintval;
	if (strcmp(name, "TRX.Debug_Type") == 0)
		TRX.Debug_Type = (uint8_t)uintval;
	if (strcmp(name, "TRX.BandMapEnabled") == 0)
		TRX.BandMapEnabled = bval;
	if (strcmp(name, "TRX.InputType") == 0)
		TRX.InputType = (uint8_t)uintval;
	if (strcmp(name, "TRX.AutoGain") == 0)
		TRX.AutoGain = bval;
	if (strcmp(name, "TRX.CLAR") == 0)
		TRX.CLAR = bval;
	if (strcmp(name, "TRX.Dual_RX") == 0)
		TRX.Dual_RX = bval;
	if (strcmp(name, "TRX.Encoder_Accelerate") == 0)
		TRX.Encoder_Accelerate = bval;
	if (strcmp(name, "TRX.Dual_RX_Type") == 0)
		TRX.Dual_RX_Type = (DUAL_RX_TYPE)uintval;
	if (strcmp(name, "TRX.CALLSIGN") == 0)
	{
		dma_memset(TRX.CALLSIGN, 0x00, sizeof(TRX.CALLSIGN));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.CALLSIGN))
			lens = sizeof(TRX.CALLSIGN);
		strncpy(TRX.CALLSIGN, value, lens);
	}
	if (strcmp(name, "TRX.LOCATOR") == 0)
	{
		dma_memset(TRX.LOCATOR, 0x00, sizeof(TRX.LOCATOR));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.LOCATOR))
			lens = sizeof(TRX.LOCATOR);
		strncpy(TRX.LOCATOR, value, lens);
	}
	if (strcmp(name, "TRX.Transverter_Enabled") == 0)
		TRX.Transverter_Enabled = bval;
	if (strcmp(name, "TRX.Transverter_Offset_Mhz") == 0)
		TRX.Transverter_Offset_Mhz = (uint16_t)uintval;
	//AUDIO
	if (strcmp(name, "TRX.IF_Gain") == 0)
		TRX.IF_Gain = (uint8_t)uintval;
	if (strcmp(name, "TRX.AGC_GAIN_TARGET2") == 0)
		TRX.AGC_GAIN_TARGET = (int8_t)intval;
	if (strcmp(name, "TRX.MIC_GAIN") == 0)
		TRX.MIC_GAIN = (uint8_t)uintval;
	if (strcmp(name, "TRX.RX_EQ_LOW") == 0)
		TRX.RX_EQ_LOW = (int8_t)intval;
	if (strcmp(name, "TRX.RX_EQ_MID") == 0)
		TRX.RX_EQ_MID = (int8_t)intval;
	if (strcmp(name, "TRX.RX_EQ_HIG") == 0)
		TRX.RX_EQ_HIG = (int8_t)intval;
	if (strcmp(name, "TRX.MIC_EQ_LOW") == 0)
		TRX.MIC_EQ_LOW = (int8_t)intval;
	if (strcmp(name, "TRX.MIC_EQ_MID") == 0)
		TRX.MIC_EQ_MID = (int8_t)intval;
	if (strcmp(name, "TRX.MIC_EQ_HIG") == 0)
		TRX.MIC_EQ_HIG = (int8_t)intval;
	if (strcmp(name, "TRX.MIC_REVERBER") == 0)
		TRX.MIC_REVERBER = (uint8_t)uintval;
	if (strcmp(name, "TRX.DNR_SNR_THRESHOLD") == 0)
		TRX.DNR_SNR_THRESHOLD = (uint8_t)uintval;
	if (strcmp(name, "TRX.DNR_AVERAGE") == 0)
		TRX.DNR_AVERAGE = (uint8_t)uintval;
	if (strcmp(name, "TRX.DNR_MINIMAL") == 0)
		TRX.DNR_MINIMAL = (uint8_t)uintval;
	if (strcmp(name, "TRX.NOISE_BLANKER") == 0)
		TRX.NOISE_BLANKER = uintval;
	if (strcmp(name, "TRX.RX_AGC_SSB_speed") == 0)
		TRX.RX_AGC_SSB_speed = (uint8_t)uintval;
	if (strcmp(name, "TRX.RX_AGC_CW_speed") == 0)
		TRX.RX_AGC_CW_speed = (uint8_t)uintval;
	if (strcmp(name, "TRX.TX_Compressor_speed") == 0)
		TRX.TX_Compressor_speed = (uint8_t)uintval;
	if (strcmp(name, "TRX.TX_Compressor_maxgain") == 0)
		TRX.TX_Compressor_maxgain = (uint8_t)uintval;
	if (strcmp(name, "TRX.CW_LPF_Filter2") == 0)
		TRX.CW_LPF_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.CW_HPF_Filter2") == 0)
		TRX.CW_HPF_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.SSB_LPF_RX_Filter") == 0)
		TRX.SSB_LPF_RX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.SSB_LPF_TX_Filter") == 0)
		TRX.SSB_LPF_TX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.SSB_HPF_Filter") == 0)
		TRX.SSB_HPF_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.AM_LPF_RX_Filter") == 0)
		TRX.AM_LPF_RX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.AM_LPF_TX_Filter") == 0)
		TRX.AM_LPF_TX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.FM_LPF_RX_Filter") == 0)
		TRX.FM_LPF_RX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.FM_LPF_TX_Filter") == 0)
		TRX.FM_LPF_TX_Filter = (uint16_t)uintval;
	if (strcmp(name, "TRX.Beeper") == 0)
		TRX.Beeper = uintval;
	if (strcmp(name, "TRX.VAD_Squelch") == 0)
		TRX.VAD_Squelch = uintval;
	//CW
	if (strcmp(name, "TRX.CWDecoder") == 0)
		TRX.CWDecoder = uintval;
	if (strcmp(name, "TRX.CW_GENERATOR_SHIFT_HZ") == 0)
		TRX.CW_GENERATOR_SHIFT_HZ = (uint16_t)uintval;
	if (strcmp(name, "TRX.CW_Key_timeout") == 0)
		TRX.CW_Key_timeout = (uint16_t)uintval;
	if (strcmp(name, "TRX.CW_SelfHear") == 0)
		TRX.CW_SelfHear = (uint16_t)uintval;
	if (strcmp(name, "TRX.CW_KEYER") == 0)
		TRX.CW_KEYER = uintval;
	if (strcmp(name, "TRX.CW_KEYER_WPM") == 0)
		TRX.CW_KEYER_WPM = (uint16_t)uintval;
	if (strcmp(name, "TRX.CW_GaussFilter") == 0)
		TRX.CW_GaussFilter = uintval;
	//SCREEN
	if (strcmp(name, "TRX.ColorThemeId") == 0)
		TRX.ColorThemeId = (uint8_t)uintval;
	if (strcmp(name, "TRX.LayoutThemeId") == 0)
		TRX.LayoutThemeId = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Enabled") == 0)
		TRX.FFT_Enabled = uintval;
	if (strcmp(name, "TRX.FFT_Zoom") == 0)
		TRX.FFT_Zoom = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_ZoomCW") == 0)
		TRX.FFT_ZoomCW = (uint8_t)uintval;
	if (strcmp(name, "TRX.LCD_Brightness") == 0)
		TRX.LCD_Brightness = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Speed") == 0)
		TRX.FFT_Speed = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Sensitivity") == 0)
		TRX.FFT_Sensitivity = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Averaging") == 0)
		TRX.FFT_Averaging = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Window") == 0)
		TRX.FFT_Window = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Height") == 0)
		TRX.FFT_Height = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Style") == 0)
		TRX.FFT_Style = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Color") == 0)
		TRX.FFT_Color = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Compressor") == 0)
		TRX.FFT_Compressor = uintval;
	if (strcmp(name, "TRX.WTF_Moving") == 0)
		TRX.WTF_Moving = uintval;
	if (strcmp(name, "TRX.FFT_Grid") == 0)
		TRX.FFT_Grid = (int8_t)intval;
	if (strcmp(name, "TRX.FFT_Background") == 0)
		TRX.FFT_Background = uintval;
	if (strcmp(name, "TRX.FFT_Lens") == 0)
		TRX.FFT_Lens = uintval;
	if (strcmp(name, "TRX.FFT_HoldPeaks") == 0)
		TRX.FFT_HoldPeaks = uintval;
	if (strcmp(name, "TRX.FFT_3D") == 0)
		TRX.FFT_3D = (uint8_t)uintval;
	if (strcmp(name, "TRX.FFT_Automatic") == 0)
		TRX.FFT_Automatic = uintval;
	if (strcmp(name, "TRX.FFT_ManualBottom") == 0)
		TRX.FFT_ManualBottom = (int16_t)intval;
	if (strcmp(name, "TRX.FFT_ManualTop") == 0)
		TRX.FFT_ManualTop = (int16_t)intval;
	//ADC
	if (strcmp(name, "TRX.ADC_Driver") == 0)
		TRX.ADC_Driver = uintval;
	if (strcmp(name, "TRX.ADC_PGA") == 0)
		TRX.ADC_PGA = uintval;
	if (strcmp(name, "TRX.ADC_RAND") == 0)
		TRX.ADC_RAND = uintval;
	if (strcmp(name, "TRX.ADC_SHDN") == 0)
		TRX.ADC_SHDN = uintval;
	if (strcmp(name, "TRX.ADC_DITH") == 0)
		TRX.ADC_DITH = uintval;
	//WIFI
	if (strcmp(name, "TRX.WIFI_Enabled") == 0)
		TRX.WIFI_Enabled = uintval;
	if (strcmp(name, "TRX.WIFI_TIMEZONE") == 0)
		TRX.WIFI_TIMEZONE = (int8_t)intval;
	if (strcmp(name, "TRX.WIFI_CAT_SERVER") == 0)
		TRX.WIFI_CAT_SERVER = uintval;
	if (strcmp(name, "TRX.WIFI_AP1") == 0)
	{
		dma_memset(TRX.WIFI_AP1, 0x00, sizeof(TRX.WIFI_AP1));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_AP1))
			lens = sizeof(TRX.WIFI_AP1);
		strncpy(TRX.WIFI_AP1, value, lens);
	}
	if (strcmp(name, "TRX.WIFI_AP2") == 0)
	{
		dma_memset(TRX.WIFI_AP2, 0x00, sizeof(TRX.WIFI_AP2));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_AP2))
			lens = sizeof(TRX.WIFI_AP2);
		strncpy(TRX.WIFI_AP2, value, lens);
	}
	if (strcmp(name, "TRX.WIFI_AP3") == 0)
	{
		dma_memset(TRX.WIFI_AP3, 0x00, sizeof(TRX.WIFI_AP3));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_AP3))
			lens = sizeof(TRX.WIFI_AP3);
		strncpy(TRX.WIFI_AP3, value, lens);
	}
	if (strcmp(name, "TRX.WIFI_PASSWORD1") == 0)
	{
		dma_memset(TRX.WIFI_PASSWORD1, 0x00, sizeof(TRX.WIFI_PASSWORD1));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_PASSWORD1))
			lens = sizeof(TRX.WIFI_PASSWORD1);
		strncpy(TRX.WIFI_PASSWORD1, value, lens);
	}
	if (strcmp(name, "TRX.WIFI_PASSWORD2") == 0)
	{
		dma_memset(TRX.WIFI_PASSWORD2, 0x00, sizeof(TRX.WIFI_PASSWORD2));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_PASSWORD2))
			lens = sizeof(TRX.WIFI_PASSWORD2);
		strncpy(TRX.WIFI_PASSWORD2, value, lens);
	}
	if (strcmp(name, "TRX.WIFI_PASSWORD3") == 0)
	{
		dma_memset(TRX.WIFI_PASSWORD3, 0x00, sizeof(TRX.WIFI_PASSWORD3));
		uint32_t lens = strlen(value);
		if (lens > sizeof(TRX.WIFI_PASSWORD3))
			lens = sizeof(TRX.WIFI_PASSWORD3);
		strncpy(TRX.WIFI_PASSWORD3, value, lens);
	}
	//SERVICES
	if (strcmp(name, "TRX.SPEC_Begin") == 0)
		TRX.SPEC_Begin = uintval;
	if (strcmp(name, "TRX.SPEC_End") == 0)
		TRX.SPEC_End = uintval;
	if (strcmp(name, "TRX.SPEC_TopDBM") == 0)
		TRX.SPEC_TopDBM = (int16_t)intval;
	if (strcmp(name, "TRX.SPEC_BottomDBM") == 0)
		TRX.SPEC_BottomDBM = (int16_t)intval;
	if (strcmp(name, "TRX.WSPR_FREQ_OFFSET") == 0)
		TRX.WSPR_FREQ_OFFSET = (int16_t)intval;
	if (strcmp(name, "TRX.WSPR_BANDS_160") == 0)
		TRX.WSPR_BANDS_160 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_80") == 0)
		TRX.WSPR_BANDS_80 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_40") == 0)
		TRX.WSPR_BANDS_40 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_30") == 0)
		TRX.WSPR_BANDS_30 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_20") == 0)
		TRX.WSPR_BANDS_20 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_17") == 0)
		TRX.WSPR_BANDS_17 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_15") == 0)
		TRX.WSPR_BANDS_15 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_12") == 0)
		TRX.WSPR_BANDS_12 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_10") == 0)
		TRX.WSPR_BANDS_10 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_6") == 0)
		TRX.WSPR_BANDS_6 = uintval;
	if (strcmp(name, "TRX.WSPR_BANDS_2") == 0)
		TRX.WSPR_BANDS_2 = uintval;
	//CALIBRATION
	if (strcmp(name, "CALIBRATE.ENCODER_INVERT") == 0)
		CALIBRATE.ENCODER_INVERT = bval;
	if (strcmp(name, "CALIBRATE.ENCODER2_INVERT") == 0)
		CALIBRATE.ENCODER2_INVERT = bval;
	if (strcmp(name, "CALIBRATE.ENCODER_DEBOUNCE") == 0)
		CALIBRATE.ENCODER_DEBOUNCE = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.ENCODER2_DEBOUNCE") == 0)
		CALIBRATE.ENCODER2_DEBOUNCE = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.ENCODER_SLOW_RATE") == 0)
		CALIBRATE.ENCODER_SLOW_RATE = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.ENCODER_ON_FALLING") == 0)
		CALIBRATE.ENCODER_ON_FALLING = bval;
	if (strcmp(name, "CALIBRATE.ENCODER_ACCELERATION") == 0)
		CALIBRATE.ENCODER_ACCELERATION = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.CICFIR_GAINER_48K") == 0)
		CALIBRATE.CICFIR_GAINER_48K_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.CICFIR_GAINER_96K") == 0)
		CALIBRATE.CICFIR_GAINER_96K_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.CICFIR_GAINER_192K") == 0)
		CALIBRATE.CICFIR_GAINER_192K_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.CICFIR_GAINER_384K") == 0)
		CALIBRATE.CICFIR_GAINER_384K_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.TXCICFIR_GAINER2") == 0)
		CALIBRATE.TXCICFIR_GAINER_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.DAC_GAINER2") == 0)
		CALIBRATE.DAC_GAINER_val = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_2200m") == 0)
		CALIBRATE.rf_out_power_2200m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_160m") == 0)
		CALIBRATE.rf_out_power_160m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_80m") == 0)
		CALIBRATE.rf_out_power_80m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_40m") == 0)
		CALIBRATE.rf_out_power_40m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_30m") == 0)
		CALIBRATE.rf_out_power_30m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_20m") == 0)
		CALIBRATE.rf_out_power_20m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_17m") == 0)
		CALIBRATE.rf_out_power_17m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_15m") == 0)
		CALIBRATE.rf_out_power_15m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_12m") == 0)
		CALIBRATE.rf_out_power_12m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_10m") == 0)
		CALIBRATE.rf_out_power_10m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_6m") == 0)
		CALIBRATE.rf_out_power_6m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.rf_out_power_2m") == 0)
		CALIBRATE.rf_out_power_2m = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.smeter_calibration2") == 0)
		CALIBRATE.smeter_calibration = (int16_t)intval;
	if (strcmp(name, "CALIBRATE.adc_offset") == 0)
		CALIBRATE.adc_offset = (int16_t)intval;
	if (strcmp(name, "CALIBRATE.LPF_END") == 0)
		CALIBRATE.LPF_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_0_START") == 0)
		CALIBRATE.BPF_0_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_0_END") == 0)
		CALIBRATE.BPF_0_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_1_START") == 0)
		CALIBRATE.BPF_1_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_1_END") == 0)
		CALIBRATE.BPF_1_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_2_START") == 0)
		CALIBRATE.BPF_2_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_2_END") == 0)
		CALIBRATE.BPF_2_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_3_START") == 0)
		CALIBRATE.BPF_3_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_3_END") == 0)
		CALIBRATE.BPF_3_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_4_START") == 0)
		CALIBRATE.BPF_4_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_4_END") == 0)
		CALIBRATE.BPF_4_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_5_START") == 0)
		CALIBRATE.BPF_5_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_5_END") == 0)
		CALIBRATE.BPF_5_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_6_START") == 0)
		CALIBRATE.BPF_6_START = uintval;
	if (strcmp(name, "CALIBRATE.BPF_6_END") == 0)
		CALIBRATE.BPF_6_END = uintval;
	if (strcmp(name, "CALIBRATE.BPF_HPF") == 0)
		CALIBRATE.BPF_HPF = uintval;
	if (strcmp(name, "CALIBRATE.SWR_FWD_Calibration") == 0)
		CALIBRATE.SWR_FWD_Calibration = floatval;
	if (strcmp(name, "CALIBRATE.SWR_REF_Calibration") == 0)
		CALIBRATE.SWR_REF_Calibration = floatval;
	if (strcmp(name, "CALIBRATE.VCXO_correction") == 0)
		CALIBRATE.VCXO_correction = (int8_t)intval;
	if (strcmp(name, "CALIBRATE.FAN_MEDIUM_START") == 0)
		CALIBRATE.FAN_MEDIUM_START = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.FAN_MEDIUM_STOP") == 0)
		CALIBRATE.FAN_MEDIUM_STOP = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.FAN_FULL_START") == 0)
		CALIBRATE.FAN_FULL_START = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.TRX_MAX_RF_TEMP") == 0)
		CALIBRATE.TRX_MAX_RF_TEMP = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.TRX_MAX_SWR") == 0)
		CALIBRATE.TRX_MAX_SWR = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.FM_DEVIATION_SCALE") == 0)
		CALIBRATE.FM_DEVIATION_SCALE = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.TUNE_MAX_POWER") == 0)
		CALIBRATE.TUNE_MAX_POWER = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.RTC_Coarse_Calibration") == 0)
		CALIBRATE.RTC_Coarse_Calibration = (uint8_t)uintval;
	if (strcmp(name, "CALIBRATE.RTC_Calibration") == 0)
		CALIBRATE.RTC_Calibration = intval;
}

static void SDCOMM_IMPORT_SETT_handler(void)
{
	char readedLine[64] = {0};
	LCD_showInfo("Importing...", false);
	if (f_open(&File, "wolf.ini", FA_READ) == FR_OK)
	{
		uint32_t bytesread = 1;
		while (bytesread != 0)
		{
			FRESULT res = f_read(&File, SD_workbuffer_A, sizeof(SD_workbuffer_A), (void *)&bytesread);
			uint16_t start_index = 0;
			if (res == FR_OK && bytesread != 0)
			{
				//sendToDebug_str((char*)workbuffer);
				char *istr = strstr((char *)SD_workbuffer_A + start_index, "\r\n"); // look for the end of the line
				while (istr != NULL && start_index < sizeof(SD_workbuffer_A))
				{
					uint16_t len = (uint16_t)((uint32_t)istr - ((uint32_t)SD_workbuffer_A + start_index));
					if (len <= 64)
					{
						dma_memset(readedLine, 0x00, sizeof(readedLine));
						strncpy(readedLine, (char *)SD_workbuffer_A + start_index, len);
						start_index += len + 2;
						istr = strstr((char *)SD_workbuffer_A + start_index, "\r\n"); // look for the end of the line
						SDCOMM_PARSE_SETT_LINE(readedLine);
					}
					else
						break;
				}
			}
		}
	}
	else
	{
		f_close(&File);
		LCD_showInfo("SD error", true);
		SD_Present = false;
		return;
	}
	f_close(&File);
	NeedSaveSettings = true;
	LCD_showInfo("Settings import complete", true);
}

static void SDCOMM_MKFS_handler(void)
{
	LCD_showInfo("Start formatting...", false);
	FRESULT res = f_mkfs((TCHAR const *)USERPath, FM_FAT32, 0, SD_workbuffer_A, sizeof SD_workbuffer_A);
	if (res == FR_OK)
	{
		LCD_showInfo("SD Format complete", true);
	}
	else
	{
		LCD_showInfo("SD Format error", true);
		SD_Present = false;
	}
}

static void SDCOMM_LISTROOT_handler(void)
{
	if (f_opendir(&dir, "/") == FR_OK)
	{
		while (1)
		{
			if (f_readdir(&dir, &fileInfo) == FR_OK && fileInfo.fname[0])
			{
				char *fn = fileInfo.fname;
				if (fileInfo.fattrib & AM_DIR)
				{
					print("[DIR]  ");
				}
				if (strlen(fn))
				{
					print(fn);
					//sendToDebug_uint32(strlen(fn),false);
				}
				else
				{
					//sendToDebug_str(fileInfo.fname);
					//sendToDebug_uint32(strlen((char*)fileInfo.fname),false);
				}
			}
			else
				break;
			println("");
		}
		f_closedir(&dir);
		println("read complete");
	}
	else
	{
		SD_Present = false;
	}
}

//-----------------------------------------------
static uint8_t SPIx_WriteRead(uint8_t Byte)
{
	uint8_t SPIx_receivedByte = 0;

	if (!SPI_Transmit(&Byte, &SPIx_receivedByte, 1, SD_CS_GPIO_Port, SD_CS_Pin, false, SPI_SD_PRESCALER, false))
		println("SD SPI R Err");

	return SPIx_receivedByte;
}

static void SPI_SendByte(uint8_t bt)
{
	SPIx_WriteRead(bt);
}

static uint8_t SPI_ReceiveByte(void)
{
	return SPIx_WriteRead(0xFF);
}

void SPI_Release(void)
{
	SPIx_WriteRead(0xFF);
}

uint8_t SPI_wait_ready(void)
{
	uint8_t res;
	uint16_t cnt;
	cnt = 0;
	do
	{ //BUSY
		res = SPI_ReceiveByte();
		cnt++;
	} while ((res != 0xFF) && (cnt < 0xFFFF));
	if (cnt >= 0xFFFF)
		return 1;
	return res;
}

uint8_t SD_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t n, res;
	// ACMD<n> is the command sequense of CMD55-CMD<n>
	if (cmd & 0x80)
	{
		cmd &= 0x7F;
		res = SD_cmd(CMD55, 0);
		if (res > 1)
			return res;
	}
	// Select the card
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
	SPI_ReceiveByte();
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
	SPI_ReceiveByte();
	// Send a command packet
	SPI_SendByte(cmd);					// Start + Command index
	SPI_SendByte((uint8_t)(arg >> 24)); // Argument[31..24]
	SPI_SendByte((uint8_t)(arg >> 16)); // Argument[23..16]
	SPI_SendByte((uint8_t)(arg >> 8));	// Argument[15..8]
	SPI_SendByte((uint8_t)arg);			// Argument[7..0]
	n = 0x01;							// Dummy CRC + Stop
	if (cmd == CMD0)
	{
		n = 0x95;
	} // Valid CRC for CMD0(0)
	if (cmd == CMD8)
	{
		n = 0x87;
	} // Valid CRC for CMD8(0x1AA)
	SPI_SendByte(n);
	// Receive a command response
	n = 10; // Wait for a valid response in timeout of 10 attempts
	do
	{
		res = SPI_ReceiveByte();
	} while ((res & 0x80) && --n);
	return res;
}

void SD_PowerOn(void)
{
	HAL_Delay(20);
}

SRAM uint8_t SD_Read_Block_tmp[SD_MAXBLOCK_SIZE] = {0};
uint8_t SD_Read_Block(uint8_t *buff, uint32_t btr)
{
	uint8_t result;
	uint16_t cnt;
	SPI_Release(); //FF token
	cnt = 0;
	do
	{
		result = SPI_ReceiveByte();
		cnt++;
	} while ((result != 0xFE) && (cnt < 0xFFFF));
	if (cnt >= 0xFFFF)
		return 0;

	dma_memset(buff, 0xFF, btr);
	//for (cnt = 0; cnt < btr; cnt++)
	//buff[cnt] = SPI_ReceiveByte();
	if (!SPI_Transmit(NULL, SD_Read_Block_tmp, btr, SD_CS_GPIO_Port, SD_CS_Pin, false, SPI_SD_PRESCALER, true))
		println("SD SPI R Err");
	dma_memcpy(buff, SD_Read_Block_tmp, btr);

	SPI_Release();
	SPI_Release();
	return 1;
}

SRAM uint8_t SD_Write_Block_tmp[SD_MAXBLOCK_SIZE] = {0};
uint8_t SD_Write_Block(uint8_t *buff, uint8_t token, bool dma)
{
	uint8_t result;
	uint16_t cnt;
	SPI_wait_ready(); /* Wait for card ready */
	SPI_SendByte(token);
	if (token != 0xFD)
	{ /* Send data if token is other than StopTran */
		//for (cnt = 0; cnt < sdinfo.BLOCK_SIZE; cnt++)
		//SPI_SendByte(buff[cnt]);

		if (dma)
		{
			dma_memcpy(SD_Write_Block_tmp, buff, sizeof(SD_Write_Block_tmp));
			if (!SPI_Transmit(SD_Write_Block_tmp, NULL, sdinfo.BLOCK_SIZE, SD_CS_GPIO_Port, SD_CS_Pin, false, SPI_SD_PRESCALER, dma))
			{
				println("SD SPI W Err");
				return 0;
			}
		}
		else
		{
			if (!SPI_Transmit(buff, NULL, sdinfo.BLOCK_SIZE, SD_CS_GPIO_Port, SD_CS_Pin, false, SPI_SD_PRESCALER, dma))
			{
				println("SD SPI W Err");
				return 0;
			}
		}

		SPI_Release();
		SPI_Release();
		result = SPI_ReceiveByte();
		if ((result & 0x05) != 0x05)
			return 0;
		cnt = 0;
		do
		{ //BUSY
			result = SPI_ReceiveByte();
			cnt++;
		} while ((result != 0xFF) && (cnt < 0xFFFF));

		if (cnt >= 0xFFFF)
			return 0;
	}
	return 1;
}

uint8_t sd_ini(void)
{
	uint8_t i, cmd;
	int16_t tmr;
	uint32_t temp;
	//char sd_str_buff[60]={0};

	sdinfo.type = 0;
	uint8_t ocr[4];
	uint8_t csd[16];
	temp = hspi2.Init.BaudRatePrescaler;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; //156.25 kbbs (96 kbps)
	HAL_SPI_Init(&hspi2);
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);
	for (i = 0; i < 10; i++)
		SPI_Release();
	hspi2.Init.BaudRatePrescaler = temp;
	HAL_SPI_Init(&hspi2);
	HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_SET);
	if (SD_cmd(CMD0, 0) == 1) // Enter Idle state
	{
		SPI_Release();
		//OCR
		if (SD_cmd(CMD8, 0x1AA) == 1) // SDv2
		{
			for (i = 0; i < 4; i++)
				ocr[i] = SPI_ReceiveByte();
			//sendToDebug_strln("SDv2");
			//sprintf(sd_str_buff,"OCR: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",ocr[0],ocr[1],ocr[2],ocr[3]);
			//sendToDebug_str(sd_str_buff);
			// Get trailing return value of R7 resp
			if (ocr[2] == 0x01 && ocr[3] == 0xAA) // The card can work at vdd range of 2.7-3.6V
			{
				for (tmr = 12000; tmr && SD_cmd(ACMD41, 1UL << 30); tmr--)
					; // Wait for leaving idle state (ACMD41 with HCS bit)
				if (tmr && SD_cmd(CMD58, 0) == 0)
				{ // Check CCS bit in the OCR
					for (i = 0; i < 4; i++)
						ocr[i] = SPI_ReceiveByte();
					//sprintf(sd_str_buff,"OCR: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",ocr[0],ocr[1],ocr[2],ocr[3]);
					//sendToDebug_str(sd_str_buff);
					sdinfo.type = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; // SDv2 (HC or SC)
				}
			}
		}
		else //SDv1 or MMCv3
		{
			if (SD_cmd(ACMD41, 0) <= 1)
			{
				sdinfo.type = CT_SD1;
				cmd = ACMD41; // SDv1
							  //sendToDebug_strln("SDv1");
			}
			else
			{
				sdinfo.type = CT_MMC;
				cmd = CMD1; // MMCv3
							//sendToDebug_strln("MMCv3");
			}
			for (tmr = 25000; tmr && SD_cmd(cmd, 0); tmr--)
				; // Wait for leaving idle state
			sdinfo.BLOCK_SIZE = 512;
			if (!tmr || SD_cmd(CMD16, sdinfo.BLOCK_SIZE) != 0) // Set R/W block length to 512
				sdinfo.type = 0;
		}

		//GET_SECTOR_COUNT // Get drive capacity in unit of sector (DWORD)
		if ((SD_cmd(CMD9, 0) == 0))
		{
			sdinfo.BLOCK_SIZE = 512;

			SPI_ReceiveByte();
			SPI_ReceiveByte(); //clean buff ???
			for (i = 0; i < 16; i++)
				csd[i] = SPI_ReceiveByte();

			/*sprintf(sd_str_buff,"CSD: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",csd[0],csd[1],csd[2],csd[3]);
			print(sd_str_buff);
			sprintf(sd_str_buff,"CSD: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",csd[4],csd[5],csd[6],csd[7]);
			print(sd_str_buff);
			sprintf(sd_str_buff,"CSD: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",csd[8],csd[9],csd[10],csd[11]);
			print(sd_str_buff);
			sprintf(sd_str_buff,"CSD: 0x%02X 0x%02X 0x%02X 0x%02X\r\n",csd[12],csd[13],csd[14],csd[15]);
			print(sd_str_buff);*/

			if ((csd[0] >> 6) == 1) // SDC ver 2.00 // High Capacity - CSD Version 2.0
			{
				uint32_t C_SIZE = (DWORD)csd[9] + ((DWORD)csd[8] << 8) + ((DWORD)(csd[7] & 0x3F) << 16) + 1;
				//println(C_SIZE); //7674 in 4gb, 15248 in 8gb
				sdinfo.SECTOR_COUNT = C_SIZE << 10;
				//println("SDHC sector count: ", sdinfo.SECTOR_COUNT);
			}
			if (sdinfo.SECTOR_COUNT == 0) // Standard Capacity - CSD Version 1.0
			{
				uint32_t csize = ((csd[5] & 0x03) << 10) + ((WORD)csd[6] << 2) + ((WORD)(csd[7] & 0xC0) >> 6);
				//println(csize);
				BYTE READ_BL_LEN = (csd[5] & 0xF0) >> 4;
				uint32_t BLOCK_LEN = pow(2, READ_BL_LEN);
				//println(BLOCK_LEN);
				BYTE C_SIZE_MULT = (csd[9] & 0x03) | ((csd[10] & 0x80) >> 5);
				uint32_t MULT = pow(2, (C_SIZE_MULT + 2));
				//println(MULT);
				uint32_t BLOCKNR = (csize + 1) * MULT;
				//println(BLOCKNR);
				uint32_t SECTOR_COUNT = BLOCKNR * BLOCK_LEN / sdinfo.BLOCK_SIZE;
				//println(SECTOR_COUNT);
				sdinfo.SECTOR_COUNT = SECTOR_COUNT;
				//println("SDSC sector count: ", sdinfo.SECTOR_COUNT);
			}
			if ((csd[0] >> 6) != 1 || sdinfo.SECTOR_COUNT == 0)
			{ // SDC ver 1.XX or MMC ver 3
				BYTE n = (BYTE)((csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2);
				DWORD csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
				sdinfo.SECTOR_COUNT = csize << (n - 9);
				//println("SDC1 sector count: ", sdinfo.SECTOR_COUNT);
			}
			sdinfo.CAPACITY = (uint64_t)sdinfo.SECTOR_COUNT * (uint64_t)sdinfo.BLOCK_SIZE;
		}
		//
	}
	else
	{
		return 1;
	}
	//sprintf(sd_str_buff, "Type SD: 0x%02X\r\n",sdinfo.type);
	//sendToDebug_str(sd_str_buff);
	return 0;
}
