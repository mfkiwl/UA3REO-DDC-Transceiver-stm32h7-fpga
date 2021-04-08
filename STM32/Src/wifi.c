#include "wifi.h"
#include "main.h"
#include "stm32h7xx_hal.h"
#include "functions.h"
#include "settings.h"
#include "trx_manager.h"
#include "lcd.h"
#include <stdlib.h>
#include "usbd_cat_if.h"

static WiFiProcessingCommand WIFI_ProcessingCommand = WIFI_COMM_NONE;
static void (*WIFI_ProcessingCommandCallback)(void);

static SRAM char WIFI_AnswerBuffer[WIFI_ANSWER_BUFFER_SIZE] = {0};
static SRAM char WIFI_readedLine[WIFI_LINE_BUFFER_SIZE] = {0};
static SRAM char tmp[WIFI_LINE_BUFFER_SIZE] = {0};
static SRAM int16_t WIFI_RLEStreamBuffer[WIFI_RLE_BUFFER_SIZE] = {0};
static SRAM uint16_t WIFI_RLEStreamBuffer_index = 0;
static SRAM uint16_t WIFI_RLEStreamBuffer_part = 0;
static uint32_t WIFI_Answer_ReadIndex = 0;
static uint32_t commandStartTime = 0;
static uint8_t WIFI_FoundedAP_Index = 0;
static bool WIFI_stop_auto_ap_list = false;

static void WIFI_SendCommand(char *command);
static bool WIFI_WaitForOk(void);
static bool WIFI_ListAP_Sync(void);
static bool WIFI_TryGetLine(void);
static void WIFI_sendHTTPRequest(void);
static void WIFI_getHTTPResponse(void);
static void WIFI_printImage_stream_callback(void);
static void WIFI_printImage_stream_partial_callback(void);

bool WIFI_connected = false;
bool WIFI_CAT_server_started = false;
volatile WiFiState WIFI_State = WIFI_UNDEFINED;
static char WIFI_FoundedAP_InWork[WIFI_FOUNDED_AP_MAXCOUNT][MAX_WIFIPASS_LENGTH] = {0};
volatile char WIFI_FoundedAP[WIFI_FOUNDED_AP_MAXCOUNT][MAX_WIFIPASS_LENGTH] = {0};
bool WIFI_IP_Gotted = false;
char WIFI_IP[15] = {0};
char WIFI_AP[MAX_WIFIPASS_LENGTH] = {0};
static uint16_t WIFI_HTTP_Response_Status = 0;
static uint32_t WIFI_HTTP_Response_ContentLength = 0;
SRAM static char WIFI_HOSTuri[128] = {0};
SRAM static char WIFI_GETuri[128] = {0};
SRAM static char WIFI_HTTRequest[128] = {0};
SRAM static char WIFI_HTTResponseHTML[WIFI_HTML_RESP_BUFFER_SIZE] = {0};

void WIFI_Init(void)
{
	//wifi uart speed = 115200 * 8 = 921600  / * 16 = 1843200
	WIFI_SendCommand("AT+UART_CUR=921600,8,1,0,1\r\n"); //uart config
	HAL_Delay(100);
	//WIFI_WaitForOk();
	huart6.Init.BaudRate = 921600;
	HAL_UART_Init(&huart6);

	WIFI_SendCommand("ATE0\r\n"); //echo off
	WIFI_WaitForOk();
	WIFI_SendCommand("AT+GMR\r\n"); //system info ESP8266
	WIFI_WaitForOk();
	/*
	AT version:1.7.4.0(May 11 2020 19:13:04)
	SDK version:3.0.4(9532ceb)
	compile time:May 27 2020 10:12:20
	Bin version(Wroom 02):1.7.4
	*/
	WIFI_SendCommand("AT\r\n");
	WIFI_WaitForOk();
	if (strstr(WIFI_readedLine, "OK") != NULL)
	{
		println("[WIFI] WIFI Module Inited");
		WIFI_State = WIFI_INITED;

		// check if there are active connections, if yes - don't create a new one
		WIFI_SendCommand("AT+CIPSTATUS\r\n");
		while (WIFI_TryGetLine())
		{
			if (strstr(WIFI_readedLine, "STATUS:2") != NULL)
			{
				WIFI_SendCommand("AT+CWAUTOCONN=1\r\n"); //AUTOCONNECT
				WIFI_WaitForOk();
				WIFI_State = WIFI_READY;
				WIFI_connected = true;
				LCD_UpdateQuery.StatusInfoGUI = true;
				println("[WIFI] Connected");
			}
		}
	}
	if (WIFI_State == WIFI_UNDEFINED)
	{
		WIFI_State = WIFI_NOTFOUND;
		println("[WIFI] WIFI Module Not Found");
	}
}

void WIFI_Process(void)
{
	static IRAM2 char com_t[128] = {0};
	static IRAM2 char tz[2] = {0};
	static IRAM2 char com[128] = {0};
	dma_memset(com_t, 0x00, sizeof(com_t));
	dma_memset(tz, 0x00, sizeof(tz));
	dma_memset(com, 0x00, sizeof(com));

	if (WIFI_State == WIFI_NOTFOUND)
		return;
	if (WIFI_State == WIFI_UNDEFINED)
	{
		WIFI_Init();
		return;
	}

	/////////

	switch (WIFI_State)
	{
	case WIFI_INITED:
		//sendToDebug_str3("[WIFI] Start connecting to AP: ", TRX.WIFI_AP, "\r\n");
		WIFI_SendCommand("AT+CWAUTOCONN=0\r\n"); //AUTOCONNECT OFF
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+RFPOWER=82\r\n"); //rf power
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CWMODE=1\r\n"); //station mode
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CWDHCP=1,1\r\n"); //DHCP
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPDNS_CUR=1,\"8.8.8.8\"\r\n"); //DNS
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CWHOSTNAME=\"UA3REO\"\r\n"); //Hostname
		WIFI_WaitForOk();
		//WIFI_SendCommand("AT+CWCOUNTRY=1,\"RU\",1,13\r\n"); //Country
		//WIFI_WaitForOk();
		//WIFI_SendCommand("AT+CIPSERVER=0\r\n"); //Stop CAT Server
		//WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPMUX=1\r\n"); //Multiple server connections
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPSERVERMAXCONN=3\r\n"); //Max server connections
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPSSLSIZE=4096\r\n"); //SSL size
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPSSLCCONF=2\r\n"); //SSL config
		WIFI_WaitForOk();
		WIFI_SendCommand("AT+CIPRECVMODE=0\r\n"); //TCP receive passive mode
		WIFI_WaitForOk();

		strcat(com_t, "AT+CIPSNTPCFG=1,");
		sprintf(tz, "%d", TRX.WIFI_TIMEZONE);
		strcat(com_t, tz);
		strcat(com_t, ",\"0.pool.ntp.org\",\"1.pool.ntp.org\"\r\n");
		WIFI_SendCommand(com_t); //configure SNMP
		WIFI_WaitForOk();

		WIFI_stop_auto_ap_list = false;
		WIFI_IP_Gotted = false;
		WIFI_State = WIFI_CONFIGURED;
		break;
	case WIFI_CONFIGURED:
		if (strcmp(TRX.WIFI_AP1, "WIFI-AP") == 0 && strcmp(TRX.WIFI_AP2, "WIFI-AP") == 0 && strcmp(TRX.WIFI_AP3, "WIFI-AP") == 0 &&
			strcmp(TRX.WIFI_PASSWORD1, "WIFI-PASSWORD") == 0 && strcmp(TRX.WIFI_PASSWORD2, "WIFI-PASSWORD") == 0 && strcmp(TRX.WIFI_PASSWORD3, "WIFI-PASSWORD") == 0)
			break;
		if (WIFI_stop_auto_ap_list)
			break;
		WIFI_ListAP_Sync();
		bool AP1_exist = false;
		bool AP2_exist = false;
		bool AP3_exist = false;
		for (uint8_t i = 0; i < WIFI_FOUNDED_AP_MAXCOUNT; i++)
		{
			if (strcmp((char *)WIFI_FoundedAP[i], TRX.WIFI_AP1) == 0)
				AP1_exist = true;
			else if (strcmp((char *)WIFI_FoundedAP[i], TRX.WIFI_AP2) == 0)
				AP2_exist = true;
			else if (strcmp((char *)WIFI_FoundedAP[i], TRX.WIFI_AP3) == 0)
				AP3_exist = true;
		}
		if (AP1_exist && strlen(TRX.WIFI_PASSWORD1) > 5)
		{
			println("[WIFI] Start connecting to AP1: ", TRX.WIFI_AP1);
			strcat(com, "AT+CWJAP_CUR=\"");
			strcat(com, TRX.WIFI_AP1);
			strcat(com, "\",\"");
			strcat(com, TRX.WIFI_PASSWORD1);
			strcat(com, "\"\r\n");
			WIFI_SendCommand(com); //connect to AP
			//WIFI_WaitForOk();
			WIFI_State = WIFI_CONNECTING;
			strcpy(WIFI_AP, TRX.WIFI_AP1);
		}
		if (AP2_exist && strlen(TRX.WIFI_PASSWORD2) > 5)
		{
			println("[WIFI] Start connecting to AP2: ", TRX.WIFI_AP2);
			strcat(com, "AT+CWJAP_CUR=\"");
			strcat(com, TRX.WIFI_AP2);
			strcat(com, "\",\"");
			strcat(com, TRX.WIFI_PASSWORD2);
			strcat(com, "\"\r\n");
			WIFI_SendCommand(com); //connect to AP
			//WIFI_WaitForOk();
			WIFI_State = WIFI_CONNECTING;
			strcpy(WIFI_AP, TRX.WIFI_AP2);
		}
		if (AP3_exist && strlen(TRX.WIFI_PASSWORD3) > 5)
		{
			println("[WIFI] Start connecting to AP: ", TRX.WIFI_AP3);
			strcat(com, "AT+CWJAP_CUR=\"");
			strcat(com, TRX.WIFI_AP3);
			strcat(com, "\",\"");
			strcat(com, TRX.WIFI_PASSWORD3);
			strcat(com, "\"\r\n");
			WIFI_SendCommand(com); //connect to AP
			//WIFI_WaitForOk();
			WIFI_State = WIFI_CONNECTING;
			strcpy(WIFI_AP, TRX.WIFI_AP3);
		}
		break;

	case WIFI_CONNECTING:
		WIFI_TryGetLine();
		if (strstr(WIFI_readedLine, "GOT IP") != NULL)
		{
			println("[WIFI] Connected");
			WIFI_SendCommand("AT+CWAUTOCONN=1\r\n"); //AUTOCONNECT
			WIFI_WaitForOk();
			WIFI_State = WIFI_READY;
			WIFI_connected = true;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
		if (strstr(WIFI_readedLine, "WIFI DISCONNECT") != NULL)
		{
			println("[WIFI] Disconnected");
			//WIFI_State = WIFI_CONFIGURED;
			WIFI_connected = false;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
		if (strstr(WIFI_readedLine, "FAIL") != NULL)
		{
			println("[WIFI] Connect failed");
			WIFI_State = WIFI_CONFIGURED;
			WIFI_connected = false;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
		if (strstr(WIFI_readedLine, "ERROR") != NULL)
		{
			println("[WIFI] Connect error");
			WIFI_State = WIFI_CONFIGURED;
			WIFI_connected = false;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
		break;

	case WIFI_READY:
		WIFI_TryGetLine();
		WIFI_ProcessingCommandCallback = 0;
		//receive commands from WIFI clients
		if (strstr(WIFI_readedLine, "+IPD") != NULL && WIFI_ProcessingCommand != WIFI_COMM_TCP_GET_RESPONSE && WIFI_CAT_server_started)
		{
			char *wifi_incoming_link_id = strchr(WIFI_readedLine, ',');
			if (wifi_incoming_link_id == NULL)
				break;
			wifi_incoming_link_id++;

			char *wifi_incoming_length = strchr(wifi_incoming_link_id, ',');
			if (wifi_incoming_length == NULL)
				break;
			*wifi_incoming_length = 0x00;
			wifi_incoming_length++;

			char *wifi_incoming_data = strchr(wifi_incoming_length, ':');
			if (wifi_incoming_data == NULL)
				break;
			*wifi_incoming_data = 0x00;
			wifi_incoming_data++;

			uint32_t wifi_incoming_length_uint = (uint32_t)atoi(wifi_incoming_length);
			uint32_t wifi_incoming_link_id_uint = (uint32_t)atoi(wifi_incoming_link_id);
			if (wifi_incoming_length_uint > 64)
				wifi_incoming_length_uint = 64;
			if (wifi_incoming_length_uint > 0)
				wifi_incoming_length_uint--; //del /n char
			if (wifi_incoming_link_id_uint > 8)
				wifi_incoming_link_id_uint = 8;

			char *wifi_incoming_data_end = wifi_incoming_data + wifi_incoming_length_uint;
			*wifi_incoming_data_end = 0x00;

			if (TRX.Debug_Type == TRX_DEBUG_WIFI)
				println("[WIFI] Command received: ", wifi_incoming_data);
			if (wifi_incoming_length_uint > 0)
				CAT_SetWIFICommand(wifi_incoming_data, wifi_incoming_length_uint, wifi_incoming_link_id_uint);
		}
		if (strstr(WIFI_readedLine, "WIFI DISCONNECT") != NULL)
		{
			println("[WIFI] Disconnected");
			WIFI_State = WIFI_CONFIGURED;
			WIFI_connected = false;
			WIFI_IP_Gotted = false;
			LCD_UpdateQuery.StatusInfoGUI = true;
		}
		break;

	case WIFI_TIMEOUT:
		WIFI_TryGetLine();
		if (WIFI_connected)
			WIFI_State = WIFI_READY;
		else
			WIFI_State = WIFI_CONFIGURED;
		break;

	case WIFI_PROCESS_COMMAND:
		WIFI_TryGetLine();
		if ((HAL_GetTick() - commandStartTime) > WIFI_COMMAND_TIMEOUT)
		{
			WIFI_State = WIFI_TIMEOUT;
			WIFI_ProcessingCommand = WIFI_COMM_NONE;
		}
		else if (strstr(WIFI_readedLine, "OK") != NULL)
		{
			//ListAP Command Ended
			if (WIFI_ProcessingCommand == WIFI_COMM_LISTAP)
			{
				for (uint8_t i = 0; i < WIFI_FOUNDED_AP_MAXCOUNT; i++)
				{
					strcpy((char *)&WIFI_FoundedAP[i], (char *)&WIFI_FoundedAP_InWork[i]);
					WIFI_stop_auto_ap_list = false;
				}
			}
			//Create Server Command Ended
			if (WIFI_ProcessingCommand == WIFI_COMM_CREATESERVER)
			{
				WIFI_SendCommand("AT+CIPSTO=3600\r\n"); //Connection timeout
				WIFI_WaitForOk();
				WIFI_CAT_server_started = true;
				println("[WIFI] CAT Server started on port 6784");
				WIFI_State = WIFI_READY;
			}
			//SNTP Command Ended
			if (WIFI_ProcessingCommand == WIFI_COMM_GETSNTP)
			{
				WIFI_State = WIFI_READY;
			}
			//Get IP Command Ended
			if (WIFI_ProcessingCommand == WIFI_COMM_GETIP)
			{
				WIFI_State = WIFI_READY;
			}
			//TCP connect
			if (WIFI_ProcessingCommand == WIFI_COMM_TCP_CONNECT)
			{
				WIFI_sendHTTPRequest();
				return;
			}
			if (WIFI_ProcessingCommand == WIFI_COMM_TCP_GET_RESPONSE)
			{
				WIFI_getHTTPResponse();
				return;
			}
			//Some stuff
			if (WIFI_ProcessingCommandCallback != NULL)
			{
				WIFI_ProcessingCommandCallback();
			}
			WIFI_ProcessingCommand = WIFI_COMM_NONE;
		}
		else if (strlen(WIFI_readedLine) > 5) //read command output
		{
			if (WIFI_ProcessingCommand == WIFI_COMM_LISTAP) //ListAP Command process
			{
				char *start = strchr(WIFI_readedLine, '"');
				if (start != NULL)
				{
					start = start + 1;
					char *end = strchr(start, '"');
					if (end != NULL)
					{
						*end = 0x00;
						if (strlen(start) > 0)
						{
							strcat((char *)&WIFI_FoundedAP_InWork[WIFI_FoundedAP_Index], start);
							if (WIFI_FoundedAP_Index < (WIFI_FOUNDED_AP_MAXCOUNT - 1))
								WIFI_FoundedAP_Index++;
						}
					}
				}
			}
			else if (WIFI_ProcessingCommand == WIFI_COMM_GETSNTP) //Get and sync SNTP time
			{													  //Mon Jan 18 20:17:56 2021
				char *sntp_str = strchr(WIFI_readedLine, ' ');
				if (sntp_str != NULL)
				{
					sntp_str = sntp_str + 1;
					char *month_str = sntp_str;
					sntp_str = strchr(sntp_str, ' ');
					if (sntp_str != NULL)
					{
						*sntp_str = 0x00;
						sntp_str = sntp_str + 1;
						char *day_str = sntp_str;
						sntp_str = strchr(sntp_str, ' ');
						if (sntp_str != NULL)
						{
							*sntp_str = 0x00;
							sntp_str = sntp_str + 1;
							//hh:mm:ss here
							char *min_str = strchr(sntp_str, ':');
							if (min_str != NULL)
							{
								min_str = min_str + 1;
								char *sec_str = strchr(min_str, ':');
								char *year_str = strchr(min_str, ' ');
								char *end = strchr(sntp_str, ':');
								if (sec_str != NULL && year_str != NULL && end != NULL)
								{
									sec_str = sec_str + 1;
									year_str = year_str + 1;
									*end = 0x00;
									end = strchr(min_str, ':');
									if (end != NULL)
									{
										*end = 0x00;
										end = strchr(sec_str, ' ');
										if (end != NULL)
										{
											*end = 0x00;
											//split strings here
											uint8_t hrs = (uint8_t)atoi(sntp_str);
											uint8_t min = (uint8_t)atoi(min_str);
											uint8_t sec = (uint8_t)atoi(sec_str);
											uint16_t year = (uint16_t)atoi(year_str);
											uint16_t year_short = (uint16_t)atoi(year_str + 2);
											uint16_t month = 1;
											if (strstr(month_str, "Jan") != NULL)
												month = 1;
											if (strstr(month_str, "Feb") != NULL)
												month = 2;
											if (strstr(month_str, "Mar") != NULL)
												month = 3;
											if (strstr(month_str, "Apr") != NULL)
												month = 4;
											if (strstr(month_str, "May") != NULL)
												month = 5;
											if (strstr(month_str, "Jun") != NULL)
												month = 6;
											if (strstr(month_str, "Jul") != NULL)
												month = 7;
											if (strstr(month_str, "Aug") != NULL)
												month = 8;
											if (strstr(month_str, "Sep") != NULL)
												month = 9;
											if (strstr(month_str, "Oct") != NULL)
												month = 10;
											if (strstr(month_str, "Nov") != NULL)
												month = 11;
											if (strstr(month_str, "Dec") != NULL)
												month = 12;
											uint16_t day = (uint16_t)atoi(day_str);
											//save to RTC clock
											if (year > 2018)
											{
												uint32_t currTime = RTC->TR;
												uint8_t currHours = ((currTime >> 20) & 0x03) * 10 + ((currTime >> 16) & 0x0f);
												uint8_t currMinutes = ((currTime >> 12) & 0x07) * 10 + ((currTime >> 8) & 0x0f);
												uint8_t currSeconds = ((currTime >> 4) & 0x07) * 10 + ((currTime >> 0) & 0x0f);
												//clock diff
												if (currHours != hrs || currMinutes != min || currSeconds != sec)
												{
													int16_t secDiff = (currHours - hrs) * 3600 + (currMinutes - min) * 60 + (currSeconds - sec);
													println("[RTC] Current clock error in sec: ", secDiff);

													if (secDiff < -1 || secDiff > 1) //do recalibration
													{
														if (secDiff < 0)
															CALIBRATE.RTC_Calibration--;
														if (secDiff > 0)
															CALIBRATE.RTC_Calibration++;
														if (CALIBRATE.RTC_Calibration < -511)
															CALIBRATE.RTC_Calibration = -511;
														if (CALIBRATE.RTC_Calibration > 511)
															CALIBRATE.RTC_Calibration = 511;
														NeedSaveCalibration = true;

														RTC_Calibration();
														println("[RTC] New RTC Calibration value: ", CALIBRATE.RTC_Calibration);
													}

													//set time
													RTC_TimeTypeDef sTime;
													sTime.TimeFormat = RTC_HOURFORMAT12_PM;
													sTime.SubSeconds = 0;
													sTime.SecondFraction = 0;
													sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
													sTime.StoreOperation = RTC_STOREOPERATION_SET;
													sTime.Hours = hrs;
													sTime.Minutes = min;
													sTime.Seconds = sec;
													RTC_DateTypeDef sDate;
													sDate.Date = day;
													sDate.Month = month;
													sDate.Year = year_short;
													BKPSRAM_Enable();
													HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
													HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
													println("[RTC] New time set")
												}
												TRX_SNTP_Synced = HAL_GetTick();
												println("[WIFI] TIME SYNCED");

												//reset SNTP
												char com_t[64] = {0};
												sprintf(com_t, "AT+CIPSNTPCFG=1,%d,\"0.pool.ntp.org\",\"1.pool.ntp.org\"\r\n", TRX.WIFI_TIMEZONE);
												WIFI_SendCommand(com_t); //configure SNMP
												WIFI_WaitForOk();
											}
										}
									}
								}
							}
						}
					}
				}
			}
			else if (WIFI_ProcessingCommand == WIFI_COMM_GETIP) //GetIP Command process
			{
				char *sep = "_CUR:ip";
				char *istr;
				istr = strstr(WIFI_readedLine, sep);
				if (istr != NULL)
				{
					char *start = strchr(WIFI_readedLine, '"');
					if (start != NULL)
					{
						start = start + 1;
						char *end = strchr(start, '"');
						if (end != NULL)
						{
							*end = 0x00;
							strcpy(WIFI_IP, start);
							println("[WIFI] GOT IP: ", WIFI_IP);
							WIFI_IP_Gotted = true;
							if (LCD_systemMenuOpened)
								LCD_UpdateQuery.SystemMenuRedraw = true;
						}
					}
				}
			}
			else if (WIFI_ProcessingCommand == WIFI_COMM_TCP_GET_RESPONSE) //GetIP Command process
			{
				char *istr;
				istr = strstr(WIFI_readedLine, "+IPD");
				if (istr != NULL)
					WIFI_getHTTPResponse();
			}
		}
		break;

	case WIFI_UNDEFINED:
	case WIFI_NOTFOUND:
	case WIFI_FAIL:
	case WIFI_SLEEP:
		break;
	}
}

bool WIFI_GetSNTPTime(void (*callback)(void))
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_GETSNTP;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_SendCommand("AT+CIPSNTPTIME?\r\n"); //get SNMP time
	return true;
}

bool WIFI_GetIP(void (*callback)(void))
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_GETIP;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_SendCommand("AT+CIPSTA_CUR?\r\n"); //get ip
	return true;
}

bool WIFI_ListAP(void (*callback)(void))
{
	if (WIFI_State != WIFI_READY && WIFI_State != WIFI_CONFIGURED)
		return false;
	if (WIFI_State == WIFI_CONFIGURED && !WIFI_stop_auto_ap_list && WIFI_ProcessingCommand == WIFI_COMM_LISTAP) // stop auto-connection when searching for networks
	{
		WIFI_stop_auto_ap_list = true;
		WIFI_WaitForOk();
	}
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_LISTAP;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_FoundedAP_Index = 0;

	for (uint8_t i = 0; i < WIFI_FOUNDED_AP_MAXCOUNT; i++)
		dma_memset((char *)&WIFI_FoundedAP_InWork[i], 0x00, sizeof WIFI_FoundedAP_InWork[i]);
	WIFI_SendCommand("AT+CWLAP\r\n"); //List AP
	return true;
}

static bool WIFI_ListAP_Sync(void)
{
	WIFI_SendCommand("AT+CWLAP\r\n"); //List AP
	WIFI_FoundedAP_Index = 0;
	for (uint8_t i = 0; i < WIFI_FOUNDED_AP_MAXCOUNT; i++)
		dma_memset((char *)&WIFI_FoundedAP[i], 0x00, sizeof WIFI_FoundedAP[i]);
	uint32_t startTime = HAL_GetTick();
	char *sep = "OK";
	char *istr;

	while ((HAL_GetTick() - startTime) < WIFI_COMMAND_TIMEOUT)
	{
		if (!WIFI_TryGetLine())
		{
			CPULOAD_GoToSleepMode();
			CPULOAD_WakeUp();
			continue;
		}

		istr = strstr(WIFI_readedLine, sep);
		if (istr != NULL)
			return true;

		if (strlen(WIFI_readedLine) > 5) //-V814
		{
			char *start = strchr(WIFI_readedLine, '"');
			if (start != NULL)
			{
				start = start + 1;
				char *end = strchr(start, '"');
				if (end != NULL)
				{
					*end = 0x00;
					strcat((char *)&WIFI_FoundedAP[WIFI_FoundedAP_Index], start);
					if (WIFI_FoundedAP_Index < (WIFI_FOUNDED_AP_MAXCOUNT - 1))
						WIFI_FoundedAP_Index++;
				}
			}
		}
	}
	return false;
}

void WIFI_GoSleep(void)
{
	if (WIFI_State == WIFI_SLEEP)
		return;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_DEEPSLEEP;
	WIFI_SendCommand("AT+GSLP=1000\r\n"); //go sleep
	WIFI_State = WIFI_SLEEP;
	WIFI_connected = false;
	LCD_UpdateQuery.StatusInfoGUI = true;
}

static void WIFI_SendCommand(char *command)
{
	HAL_UART_AbortReceive(&huart6);
	HAL_UART_AbortReceive_IT(&huart6);
	dma_memset(WIFI_AnswerBuffer, 0x00, sizeof(WIFI_AnswerBuffer));
	WIFI_Answer_ReadIndex = 0;
	HAL_UART_Receive_DMA(&huart6, (uint8_t *)WIFI_AnswerBuffer, WIFI_ANSWER_BUFFER_SIZE);
	HAL_UART_Transmit_IT(&huart6, (uint8_t *)command, (uint16_t)strlen(command));
	commandStartTime = HAL_GetTick();
	HAL_Delay(WIFI_COMMAND_DELAY);
	if (TRX.Debug_Type == TRX_DEBUG_WIFI) //DEBUG
		print("WIFI_S: ", command);
}

static bool WIFI_WaitForOk(void)
{
	char *sep = "OK";
	char *sep2 = "ERROR";
	char *istr;
	uint32_t startTime = HAL_GetTick();
	while ((HAL_GetTick() - startTime) < WIFI_COMMAND_TIMEOUT)
	{
		if (WIFI_TryGetLine())
		{
			//OK
			istr = strstr(WIFI_readedLine, sep);
			if (istr != NULL)
				return true;
			//ERROR
			istr = strstr(WIFI_readedLine, sep2);
			if (istr != NULL)
				return false;
		}
		CPULOAD_GoToSleepMode();
		CPULOAD_WakeUp();
	}
	return false;
}

static bool WIFI_TryGetLine(void)
{
	dma_memset(WIFI_readedLine, 0x00, sizeof(WIFI_readedLine));
	dma_memset(tmp, 0x00, sizeof(tmp));

	Aligned_CleanInvalidateDCache_by_Addr((uint32_t)WIFI_AnswerBuffer, sizeof(WIFI_AnswerBuffer));
	uint16_t dma_index = WIFI_ANSWER_BUFFER_SIZE - (uint16_t)__HAL_DMA_GET_COUNTER(huart6.hdmarx);
	if (WIFI_Answer_ReadIndex == dma_index)
		return false;

	if (dma_index < WIFI_Answer_ReadIndex)
	{
		//tail
		uint32_t len = WIFI_ANSWER_BUFFER_SIZE - WIFI_Answer_ReadIndex;
		strncpy(tmp, &WIFI_AnswerBuffer[WIFI_Answer_ReadIndex], len);
		//head
		strncat(tmp, &WIFI_AnswerBuffer[0], dma_index);
	}
	else
	{
		//head
		strncpy(tmp, &WIFI_AnswerBuffer[WIFI_Answer_ReadIndex], dma_index - WIFI_Answer_ReadIndex);
	}

	if (tmp[0] == '\0')
		return false;

	char *istr = strchr(tmp, '\n'); // look for the end of the line
	if (istr == NULL)
		return false;

	uint32_t len = (uint16_t)((uint32_t)istr - (uint32_t)tmp + 1);
	if (len > WIFI_LINE_BUFFER_SIZE)
		return false;
	strncpy(WIFI_readedLine, tmp, len);

	WIFI_Answer_ReadIndex += len;
	if (WIFI_Answer_ReadIndex >= WIFI_ANSWER_BUFFER_SIZE)
	{
		WIFI_Answer_ReadIndex -= WIFI_ANSWER_BUFFER_SIZE;
	}

	if (TRX.Debug_Type == TRX_DEBUG_WIFI) //DEBUG
		print("WIFI_R: ", WIFI_readedLine);

	return true;
}

bool WIFI_StartCATServer(void (*callback)(void))
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_CREATESERVER;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_SendCommand("AT+CIPSERVER=1,6784\r\n"); //Start CAT Server
	return true;
}

bool WIFI_SendCatAnswer(char *data, uint32_t link_id, void (*callback)(void))
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_SENDTCPDATA;
	WIFI_ProcessingCommandCallback = callback;
	char answer[64] = {0};
	sprintf(answer, "AT+CIPSEND=%u,%u\r\n", link_id, strlen(data));
	WIFI_SendCommand(answer); //Send CAT answer
	char answer_data[64] = {0};
	strcat(answer_data, data);
	strcat(answer_data, "\r");
	WIFI_SendCommand(answer_data); //Send CAT answer data
	WIFI_ProcessingCommand = WIFI_COMM_NONE;
	WIFI_State = WIFI_READY;
	return true;
}

bool WIFI_UpdateFW(void (*callback)(void))
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_UPDATEFW;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_SendCommand("AT+CIUPDATE\r\n"); //Start Update Firmware
	//WIFI_WaitForOk();
	return true;
}

static void WIFI_getHTTPResponse(void)
{
	uint32_t readed_body_length = 0;
	char *istr;
	istr = strstr(WIFI_readedLine, "+IPD");
	if (istr != NULL)
	{
		istr += 7;
		char *istr2 = strchr(WIFI_readedLine, ':');
		if (istr2 != NULL)
		{
			*istr2 = 0;
			uint32_t response_length = atoi(istr);
			istr2++;
			strcpy(WIFI_HTTResponseHTML, istr2);
			commandStartTime = HAL_GetTick();

			uint32_t start_time = HAL_GetTick();
			uint32_t len = strlen(WIFI_HTTResponseHTML);
			while (len < response_length && len < sizeof(WIFI_HTTResponseHTML) && (HAL_GetTick() - start_time) < 5000)
			{
				if (WIFI_TryGetLine())
				{
					if (len + strlen(WIFI_readedLine) < sizeof(WIFI_HTTResponseHTML))
						strcat(WIFI_HTTResponseHTML, WIFI_readedLine);
					else
						break;
				}
				len = strlen(WIFI_HTTResponseHTML);
			}
			char *istr3 = WIFI_HTTResponseHTML;
			istr3 += response_length;
			*istr3 = 0;

			//get status
			char *istr4 = strchr(WIFI_HTTResponseHTML, ' ');
			if (istr4 != NULL)
			{
				char *istr5 = istr4 + 4;
				*istr5 = 0;
				WIFI_HTTP_Response_Status = (uint16_t)(atoi(istr4));
				*istr5 = ' ';
			}

			//get content length
			istr4 = strstr(WIFI_HTTResponseHTML, "Content-Length: ");
			if (istr4 != NULL)
			{
				istr4 += 16;
				char *istr5 = strchr(istr4, '\r');
				if (istr5 != NULL)
				{
					*istr5 = 0;
					WIFI_HTTP_Response_ContentLength = (uint32_t)(atoi(istr4));
					*istr5 = ' ';
				}
			}

			//get response body
			char *istr6 = strstr(WIFI_HTTResponseHTML, "\r\n\r\n");
			if (istr6 != NULL)
			{
				istr6 += 4;
				strcpy(WIFI_HTTResponseHTML, istr6);
			}

			//partial callback for image printing
			readed_body_length += strlen(WIFI_HTTResponseHTML);
			if (WIFI_ProcessingCommandCallback == WIFI_printImage_stream_callback)
				WIFI_printImage_stream_partial_callback();

			//may be partial content? continue downloading
			start_time = HAL_GetTick();
			if (readed_body_length < WIFI_HTTP_Response_ContentLength && (HAL_GetTick() - start_time) < 3000)
			{
				while (readed_body_length < WIFI_HTTP_Response_ContentLength && strlen(WIFI_HTTResponseHTML) < sizeof(WIFI_HTTResponseHTML) && (HAL_GetTick() - start_time) < 3000)
				{
					if (WIFI_TryGetLine())
					{
						istr = strstr(WIFI_readedLine, "+IPD");
						if (istr != NULL)
						{
							istr += 7;
							istr2 = strchr(WIFI_readedLine, ':');
							if (istr2 != NULL)
							{
								*istr2 = 0;
								response_length = atoi(istr);
								istr2++;
								strncat(WIFI_HTTResponseHTML, istr2, response_length);

								//partial callback for image printing
								readed_body_length += strlen(WIFI_HTTResponseHTML);
								if (WIFI_ProcessingCommandCallback == WIFI_printImage_stream_callback)
									WIFI_printImage_stream_partial_callback();
							}
						}
					}
				}
			}

			//cut body on content-length
			if (strlen(WIFI_HTTResponseHTML) > WIFI_HTTP_Response_ContentLength)
				WIFI_HTTResponseHTML[WIFI_HTTP_Response_ContentLength] = 0;

			WIFI_ProcessingCommand = WIFI_COMM_NONE;
			WIFI_State = WIFI_READY;
			if (WIFI_ProcessingCommandCallback != NULL)
				WIFI_ProcessingCommandCallback();
		}
	}
}

static void WIFI_sendHTTPRequest(void)
{
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_TCP_GET_RESPONSE;
	dma_memset(WIFI_HTTRequest, 0x00, sizeof(WIFI_HTTRequest));
	strcat(WIFI_HTTRequest, "GET ");
	strcat(WIFI_HTTRequest, WIFI_GETuri);
	strcat(WIFI_HTTRequest, " HTTP/1.1\r\n");
	strcat(WIFI_HTTRequest, "Host: ");
	strcat(WIFI_HTTRequest, WIFI_HOSTuri);
	strcat(WIFI_HTTRequest, "\r\nConnection: close\r\n\r\n");
	char comm_line[64] = {0};
	sprintf(comm_line, "AT+CIPSEND=0,%d\r\n", strlen(WIFI_HTTRequest));
	WIFI_SendCommand(comm_line);
	WIFI_SendCommand(WIFI_HTTRequest);
}

bool WIFI_getHTTPpage(char *host, char *url, void (*callback)(void), bool https)
{
	if (WIFI_State != WIFI_READY)
		return false;
	WIFI_State = WIFI_PROCESS_COMMAND;
	WIFI_ProcessingCommand = WIFI_COMM_TCP_CONNECT;
	WIFI_ProcessingCommandCallback = callback;
	WIFI_HTTP_Response_Status = 0;

	dma_memset(WIFI_HOSTuri, 0x00, sizeof(WIFI_HOSTuri));
	strcat(WIFI_HOSTuri, "AT+CIPSTART=0,");
	if (!https)
		strcat(WIFI_HOSTuri, "\"TCP\"");
	else
		strcat(WIFI_HOSTuri, "\"SSL\"");
	strcat(WIFI_HOSTuri, ",\"");
	strcat(WIFI_HOSTuri, host);

	if (!https)
		strcat(WIFI_HOSTuri, "\",80,10\r\n");
	else
		strcat(WIFI_HOSTuri, "\",443,10\r\n");

	dma_memset(WIFI_GETuri, 0x00, sizeof(WIFI_GETuri));
	strcat(WIFI_GETuri, url);

	WIFI_SendCommand(WIFI_HOSTuri);

	dma_memset(WIFI_HOSTuri, 0x00, sizeof(WIFI_HOSTuri));
	strcat(WIFI_HOSTuri, host);
	return true;
}

static void WIFI_printText_callback(void)
{
	LCDDriver_Fill(BG_COLOR);
	if (WIFI_HTTP_Response_Status == 200)
	{
		LCDDriver_printTextFont(WIFI_HTTResponseHTML, 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
	}
	else
		LCDDriver_printTextFont("Network error", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
}

static void WIFI_printImage_stream_partial_callback(void)
{
	dma_memset(WIFI_RLEStreamBuffer, 0x00, sizeof(WIFI_RLEStreamBuffer));
	//parse hex output from server (convert to bin)
	char *istr = WIFI_HTTResponseHTML;
	char hex[5] = {0};
	WIFI_RLEStreamBuffer_index = 0;
	int16_t val = 0;
	uint32_t len = strlen(WIFI_HTTResponseHTML);
	while (*istr != 0 && (len >= ((WIFI_RLEStreamBuffer_index * 4) + 4)))
	{
		//Get hex
		strncpy(hex, istr, 4);
		val = (int16_t)(strtol(hex, NULL, 16));
		istr += 4;
		//Save
		WIFI_RLEStreamBuffer[WIFI_RLEStreamBuffer_index] = val;
		WIFI_RLEStreamBuffer_index++;
	}

	//send to LCD RLE stream decoder
	LCDDriver_printImage_RLECompressed_ContinueStream(WIFI_RLEStreamBuffer, WIFI_RLEStreamBuffer_index);

	//clean answer
	if (strlen(WIFI_HTTResponseHTML) > (WIFI_RLEStreamBuffer_index * 4)) //part buffer preceed, move to begin
	{
		istr = &WIFI_HTTResponseHTML[(WIFI_RLEStreamBuffer_index * 4)];
		strcpy(WIFI_HTTResponseHTML, istr);
	}
	else
		dma_memset(WIFI_HTTResponseHTML, 0x00, sizeof(WIFI_HTTResponseHTML));
}

static void WIFI_printImage_stream_callback(void)
{
	//image print stream done
}

static void WIFI_printImage_callback(void)
{
	LCDDriver_Fill(BG_COLOR);
	if (WIFI_HTTP_Response_Status == 200)
	{
		char *istr1 = strchr(WIFI_HTTResponseHTML, ',');
		if (istr1 != NULL)
		{
			*istr1 = 0;
			uint32_t filesize = atoi(WIFI_HTTResponseHTML);
			istr1++;
			char *istr2 = strchr(istr1, ',');
			if (istr2 != NULL)
			{
				*istr2 = 0;
				uint16_t width = (uint16_t)(atoi(istr1));
				istr2++;

				uint16_t height = (uint16_t)(atoi(istr2));

				if (filesize > 0 && width > 0 && height > 0)
				{
					LCDDriver_printImage_RLECompressed_StartStream(LCD_WIDTH / 2 - width / 2, LCD_HEIGHT / 2 - height / 2, width, height);
					WIFI_RLEStreamBuffer_part = 0;
					WIFI_getHTTPpage("ua3reo.ru", "/trx_services/propagination.php?part=0", WIFI_printImage_stream_callback, false);
				}
			}
		}
	}
	else
		LCDDriver_printTextFont("Network error", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
}

void WIFI_getRDA(void)
{
	LCDDriver_Fill(BG_COLOR);
	if (WIFI_connected && WIFI_State == WIFI_READY)
		LCDDriver_printTextFont("Loading...", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
	else
	{
		LCDDriver_printTextFont("No connection", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
		return;
	}
	char url[64] = "/trx_services/rda.php?callsign=";
	strcat(url, TRX.CALLSIGN);
	WIFI_getHTTPpage("ua3reo.ru", url, WIFI_printText_callback, false);
}

void WIFI_getPropagination(void)
{
	LCDDriver_Fill(BG_COLOR);
	if (WIFI_connected && WIFI_State == WIFI_READY)
		LCDDriver_printTextFont("Loading...", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
	else
	{
		LCDDriver_printTextFont("No connection", 10, 20, FG_COLOR, BG_COLOR, &FreeSans9pt7b);
		return;
	}
	WIFI_getHTTPpage("ua3reo.ru", "/trx_services/propagination.php", WIFI_printImage_callback, false);
}
