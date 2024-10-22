/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ALARM_PERIOD_IN_SECONDS 120
#define RESPONSE_BUFFER_SIZE 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RTC_HandleTypeDef hrtc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void sendATCommand(char* command);
void InitialConfigBg95();
void receiveResponse(char* buffer, uint16_t bufferSize);
void ConfigPdpContext();
void ConfigMqttContext();
void ActivePdp();
void MqttConnectAndSubscribe();
void MqttConfigBeforeConnection();
void EnterSleepMode();
void InitFlags();
void SetNextAlarm();
uint8_t responseBuffer[RESPONSE_BUFFER_SIZE];
volatile uint8_t responseReceived = 0;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //configurando rtc apos reset
  	__HAL_RCC_PWR_CLK_ENABLE();
  	__HAL_RCC_RTC_ENABLE();
  	InitFlags();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /*
	        while (!responseReceived) {
	        	sendATCommand();
	        	HAL_Delay(100);
	        	HAL_UART_Receive_IT(&huart2, responseBuffer,
	        		    	RESPONSE_BUFFER_SIZE);
	        }
	        responseReceived = 0;
	*/

		InitialConfigBg95();
		ConfigPdpContext();
		ActivePdp();
		ConfigMqttContext();
		MqttConfigBeforeConnection();
		MqttConnectAndSubscribe();
		HAL_Delay(10000);

		EnterSleepMode();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void sendATCommand(char* command) {
    HAL_UART_Transmit(&huart2, (uint8_t*)command, strlen(command), HAL_MAX_DELAY);
}
void receiveResponse(char* buffer, uint16_t bufferSize) {
    HAL_UART_Receive(&huart2, (uint8_t*)buffer, bufferSize, HAL_MAX_DELAY);
}


void InitialConfigBg95() {
    char response[128];   // Buffer temporário para respostas AT
    char ip[16];          // Buffer para armazenar o endereço IP
    char command[128];    // Buffer para os comandos AT
    sendATCommand("AT+CCID\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+CCID\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Comando AT+CIMI
    sendATCommand("AT+CIMI\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CIMI\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;

    // Comando AT+CFUN=0
    sendATCommand("AT+CFUN=0\r\n");
    while (!responseReceived) {
        	sendATCommand("AT+CFUN=0\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;

    // Comando AT+QCFG="nwscanmode",0,1
    sendATCommand("AT+QCFG=\"nwscanmode\",0,1\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+QCFG=\"nwscanmode\",0,1\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;

    // Comando AT+QCFG="nwscanseq",020103,1
    sendATCommand("AT+QCFG=\"nwscanseq\",020103,1\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+QCFG=\"nwscanseq\",020103,1\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;

    // Comando AT+QCFG="band",0,0
    sendATCommand("AT+QCFG=\"band\",0,0\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+QCFG=\"band\",0,0\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;


    // Comando AT+COPS=0
    sendATCommand("AT+COPS=0\r\n");
    while (!responseReceived) {
        	sendATCommand("AT+COPS=0\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;

    // Configurar o APN, username e password para a Vivo
    sendATCommand("AT+CGDCONT=1,\"IP\",\"inlog.vivo.com.br\",\"datatem\",\"datatem\"\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+CGDCONT=1,\"IP\",\"inlog.vivo.com.br\",\"datatem\",\"datatem\"\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;

    // Comando AT+CFUN=1
    sendATCommand("AT+CFUN=1\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+CFUN=1\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;

    // Comando AT+CREG=1;+CGREG=1;+CEREG=1
    sendATCommand("AT+CREG=1;+CGREG=1;+CEREG=1\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CREG=1;+CGREG=1;+CEREG=1\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;

    // Comando AT+COPS?
    sendATCommand("AT+COPS?\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+COPS?\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;

    // Comando AT+QCSQ
    sendATCommand("AT+QCSQ\r\n");
    while (!responseReceived) {
        	sendATCommand("AT+QCSQ\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;

    // Comando AT+CREG?;+CEREG?;+CGREG?
    sendATCommand("AT+CREG?;+CEREG?;+CGREG?\r\n");
    while (!responseReceived) {
    	 	sendATCommand("AT+CREG?;+CEREG?;+CGREG?\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;

    // Comando AT+CGATT=1 (Conectar à rede)
    sendATCommand("AT+CGATT=1\r\n");
    while (!responseReceived) {
        		sendATCommand("AT+CGATT=1\r\n");
           		HAL_Delay(100);
           		HAL_UART_Receive_IT(&huart2, responseBuffer,
           		RESPONSE_BUFFER_SIZE);
           	}
           	responseReceived = 0;

    // Verificar o endereço IP alocado
    sendATCommand("AT+CGPADDR\r\n");
	while (!responseReceived) {
		sendATCommand("AT+CGPADDR\r\n");

		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Extrair o endereço IP da resposta
    if (sscanf(responseBuffer, "+CGPADDR: 1,\"%15[^\"]", ip) == 1) {
        printf("Endereço IP alocado: %s\n", ip); // Exibir o IP
    } else {
        printf("Falha ao obter o endereço IP.\n");
    }

    // Realizar um teste de ping com o IP obtido
    snprintf(command, sizeof(command), "AT+QPING=1,\"%s\"\r\n", ip);
    sendATCommand(command);
    while (!responseReceived) {
    		sendATCommand(command);


    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
}


void ConfigPdpContext() {
    char command[128];
    char response[256];

    // Configurar APN com o contexto CID 1, com login e senha
    snprintf(command, sizeof(command), "AT+CGDCONT=1,\"IP\",\"inlog.vivo.com.br\",\"\",0,0\r\n");
    sendATCommand(command);
	while (!responseReceived) {
	    sendATCommand(command);

		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Ativar o contexto PDP
    sendATCommand("AT+CGACT=1,1\r\n");
    while (!responseReceived) {
        	sendATCommand("AT+CGACT=1,1\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
    // Verificar se o contexto PDP está ativo
    sendATCommand("AT+CGPADDR=1\r\n");
    while (!responseReceived) {
        		sendATCommand("AT+CGPADDR=1\r\n");
        		HAL_Delay(100);
        		HAL_UART_Receive_IT(&huart2, responseBuffer,
        		RESPONSE_BUFFER_SIZE);
        	}
        	responseReceived = 0;
    // Receber a resposta e verificar se contém o IP
  //  HAL_UART_Receive(&huart2, (uint8_t*)response, sizeof(response), HAL_MAX_DELAY);
    if (strstr(responseBuffer, "0.0.0.0") == NULL) {
        // Contexto PDP ativado com sucesso, IP foi alocado corretamente
        snprintf(command, sizeof(command), "PDP context ativado com sucesso. APN: inlog.vivo.com.br, IP: %s\r\n", response);
       // sendATCommand(command); // Transmitir a mensagem de sucesso pela UART
    } else {
        // Erro ao ativar o contexto PDP
        //sendATCommand("Erro ao ativar o contexto PDP\r\n");
    }
}


void ConfigMqttContext() {
    // Configurar a versão MQTT como 3.1.1
    sendATCommand("AT+QMTCFG=\"version\",0,4\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+QMTCFG=\"version\",0,4\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Associar o contexto PDP ao cliente MQTT
    sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
    // Habilitar SSL para o cliente MQTT
    sendATCommand("AT+QMTCFG=\"ssl\",0,1,0\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+QMTCFG=\"ssl\",0,1,0\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;
    // Configurar keepalive para 3600 segundos
    sendATCommand("AT+QMTCFG=\"keepalive\",0,3600\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+QMTCFG=\"keepalive\",0,3600\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Configurar o "Will Message" do MQTT
   // sendATCommand("AT+QMTCFG=\"will\",0,1,0,1,\"/test/will\",\"Client disconnected unexpectedly\"\r\n");
}

void ActivePdp() {
    // Verificar se o APN está configurado corretamente
    sendATCommand("AT+CGDCONT?\r\n");
    while (!responseReceived) {
        	sendATCommand("AT+CGDCONT?\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
    // Ativar o contexto PDP
    sendATCommand("AT+CGACT=1,1\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CGACT=1,1\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;
    // Verificar se um endereço IP foi atribuído
    sendATCommand("AT+CGPADDR=1\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CGPADDR=1\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;
}
void MqttConnectAndSubscribe() {
    // Conectar ao broker MQTT
    sendATCommand("AT+QMTOPEN=0,1883\r\n");
	while (!responseReceived) {
		sendATCommand("AT+QMTOPEN=0,1883\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Conectar ao broker MQTT com ID e credenciais
    sendATCommand("AT+QMTCONN=0,\"1\",\"pixtest\",\"pixtest\"\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+QMTCONN=0,\"1\",\"pixtest\",\"pixtest\"\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
    // Inscrever-se no tópico desejado
    sendATCommand("AT+QMTSUB=0,\"pixtest\",1\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+QMTSUB=0,\"pixtest\",1\r\n");
      		HAL_Delay(100);
      		HAL_UART_Receive_IT(&huart2, responseBuffer,
      		RESPONSE_BUFFER_SIZE);
      	}
      	responseReceived = 0;
}

void MqttConfigBeforeConnection() {

    // Verificar se o dispositivo está registrado na rede
    sendATCommand("AT+CREG?\r\n");
	while (!responseReceived) {
	    sendATCommand("AT+CREG?\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
    // Verificar o status do contexto PDP
    sendATCommand("AT+CGACT?\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CGACT?\r\n");
    		HAL_Delay(100);
    		HAL_UART_Receive_IT(&huart2, responseBuffer,
    		RESPONSE_BUFFER_SIZE);
    	}
    	responseReceived = 0;
    // Verificar a qualidade do sinal
    sendATCommand("AT+CSQ\r\n");
    while (!responseReceived) {
    		sendATCommand("AT+CSQ\r\n");
       		HAL_Delay(100);
       		HAL_UART_Receive_IT(&huart2, responseBuffer,
       		RESPONSE_BUFFER_SIZE);
       	}
       	responseReceived = 0;
    // Configurar o cntexto PDP para o canal MQTT
    sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
	while (!responseReceived) {
		sendATCommand("AT+QMTCFG=\"pdpcid\",0,1\r\n");
		HAL_Delay(100);
		HAL_UART_Receive_IT(&huart2, responseBuffer,
		RESPONSE_BUFFER_SIZE);
	}
	responseReceived = 0;
}


void EnterSleepMode() {
	SetNextAlarm();
	 HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);
	HAL_PWREx_EnterSHUTDOWNMode();

}

void SetNextAlarm() {
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	RTC_AlarmTypeDef sAlarm;

// Obter o tempo e data atuais
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

// Configurar o tempo para o próximo alarme
	uint32_t totalSeconds = ALARM_PERIOD_IN_SECONDS; // Definir o período em segundos
	uint32_t hours = totalSeconds / 3600;
	uint32_t minutes = (totalSeconds % 3600) / 60;
	uint32_t seconds = totalSeconds % 60;

// Configurar os valores no alarme
	sAlarm.Alarm = RTC_ALARM_A;
	sAlarm.AlarmTime.Hours = hours;
	sAlarm.AlarmTime.Minutes = minutes;
	sAlarm.AlarmTime.Seconds = seconds;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

	sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY; // Ignorar a data, repetir alarme diariamente
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 1;  // Não utilizado devido à máscara

// Desativar alarme anterior antes de configurar o novo
	HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);

// Configurar novo alarme com interrupção
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
		printf("Erro ao configurar o alarme RTC\n");
		while (1)
			; // Travar em caso de erro crítico
	}


}

void InitFlags() {
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);  // clear the flag
		HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);  // disable PA0
	} else {
		MX_RTC_Init();
	}

}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
