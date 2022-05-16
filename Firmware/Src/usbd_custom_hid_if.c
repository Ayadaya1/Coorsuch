/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  * 1. Redistributions of source code must retain the above copyright notice,
  * this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  * this list of conditions and the following disclaimer in the documentation
  * and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of its contributors
  * may be used to endorse or promote products derived from this software
  * without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
*/
unsigned char report[64];
/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"
#include "ssd1306.h"
extern unsigned int ADC_T(void);
extern unsigned int ADC_V(void);
extern void RGB_POWER(unsigned int R,unsigned int G,unsigned int B);
extern void PlayMCMelody(uint8_t line);
extern void PlayNeverMeant(uint8_t line);
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
0x06, 0x00, 0xff, // USAGE_PAGE (Generic Desktop)
0x09, 0x01, // USAGE (Vendor Usage 1)
0xa1, 0x01, // COLLECTION (Application)
0x15, 0x00,                         //   LOGICAL_MINIMUM (0)
0x26, 0xff, 0x00,                   //   LOGICAL_MAXIMUM (255)
// 12 bytes
// button read (8 bit, but only 4 used)
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x01,                         //   REPORT_COUNT (1)
0x85, 0x01,     //   REPORT_ID (0x01)
0x09, 0x01,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,                         //   FEATURE (Data,Var,Abs)
// 22 bytes
// RGB LED read/write ( 3*16 bit)
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x06,                         //   REPORT_COUNT (1)
0x85, 0x02,     //   REPORT_ID (0x02)
0x09, 0x02,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,
// 32 bytes
// slider read (16 bit)
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x02,                         //   REPORT_COUNT (1)
0x85, 0x03,     //   REPORT_ID (0x03)
0x09, 0x03,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,
// 42 bytes
// display set pixel x(8 bit, y(8bit), color (8bit, but if 0 then black, else white)
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x03,                         //   REPORT_COUNT (1)
0x85, 0x04,     //   REPORT_ID (0x04)
0x09, 0x04,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,
// 52 bytes
// Play melody
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x02,                         //   REPORT_COUNT (1)
0x85, 0x05,     //   REPORT_ID (0x04)
0x09, 0x05,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,
// Play melody
0x75, 0x08,                         //   REPORT_SIZE (8)
0x95, 0x02,                         //   REPORT_COUNT (1)
0x85, 0x06,     //   REPORT_ID (0x04)
0x09, 0x06,                         //   USAGE (Vendor Usage 1)
0xb1, 0x02,
/* this for example, how to implement simple reports. All changes need to REcalculate descriptor size!!!!
// And need to realize  workaround for all types of reports (!)

// OUTPUT REPORT ->
0x85, 0x01, // REPORT_ID

0x09, 0x01,

// USAGE
0x75, 0x08, // REPORT_SIZE (8)

0x95, 0x3f, // REPORT_COUNT

0x91, 0x82, // OUTPUT (Data,Var,Abs,Vol)

// + 8

// INPUT REPORT ->
0x85, 0x02, // REPORT_ID

0x09, 0x02, // USAGE

0x75, 0x08, // REPORT_SIZE (8)

0x95, (64-1), // REPORT_COUNT

0x81, 0x82, // INPUT (Data,Var,Abs,Vol)

*/
// + 1
0xC0    //     END_COLLECTION	             
//53 bytes at all
};

/* USER CODE BEGIN PRIVATE_VARIABLES */
/* USER CODE END PRIVATE_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_IF_Exported_Variables
  * @{
  */
  extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE BEGIN EXPORTED_VARIABLES */
/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @{
  */
static int8_t CUSTOM_HID_Init_FS     (void);
static int8_t CUSTOM_HID_DeInit_FS   (void);
static int8_t CUSTOM_HID_OutEvent_FS (uint8_t event_idx, uint8_t* state);
static int8_t CustomHID_SetFeature(uint8_t event_idx, uint8_t* buffer);
static int8_t CustomHID_GetFeature(uint8_t event_idx, uint8_t* buffer, uint16_t* length);
uint16_t CustomHID_FeatureReportLength(uint8_t event_idx);

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
   CUSTOM_HID_ReportDesc_FS,
   CUSTOM_HID_Init_FS,
   CUSTOM_HID_DeInit_FS,
   CUSTOM_HID_OutEvent_FS,
   CustomHID_SetFeature,
   CustomHID_GetFeature
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CUSTOM_HID_Init_FS
  *         Initializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (0);
  /* USER CODE END 4 */
}

/**
  * @brief  CUSTOM_HID_DeInit_FS
  *         DeInitializes the CUSTOM HID media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (0);
  /* USER CODE END 5 */
}

/**
  * @brief  CUSTOM_HID_OutEvent_FS
  *         Manage the CUSTOM HID class events
  * @param  event_idx: event index
  * @param  state: event state
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS  (uint8_t event_idx, uint8_t* state)
{
   switch(event_idx)
   {
   case 0x01:
      // write command to FIFO from where it will be sent later
      //FIFO_Write(&irsnd_fifo, (fifo_entry_t*)buffer);
      break;

   default: /* Report does not exist */
      break;
   }

   return (USBD_OK);
}

/* USER CODE BEGIN 7 */
/**
  * @brief  USBD_CUSTOM_HID_SendReport_FS
  *         Send the report to the Host
  * @param  report: the report to be sent
  * @param  len: the report length
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */

static int8_t USBD_CUSTOM_HID_SendReport_FS ( uint8_t *report,uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}

/**
 * @brief  CustomHID_SetFeature
 *         Manage the CUSTOM HID SetFeature request.
 *         Host -> Device
 * @param  event_idx: Report Number
 * @param  buffer: Received Data
 * @retval USBD_OK
 */
uint16_t RGB[3];
static int8_t CustomHID_SetFeature(uint8_t event_idx, uint8_t* buffer)
{
    

   switch(event_idx) // проверяем reportID
   {
   case 0x02:
        memcpy(&RGB[0],&buffer[0],6); // копируем массив байт в массив интов, быстро, можно юнион еще, но.
        RGB_POWER(RGB[0],RGB[1],RGB[2]); // вызываем ф-ю задающую яркость светодиодов (1-1000) по трем цветам
        break;
   case 0x04:
        ssd1306_DrawPixel(buffer[0], buffer[1], buffer[2]); // рисуем пиксель X,Y,color
        break;
	 case 0x05:
				PlayMCMelody(buffer[0]);
				break;
	 case 0x06:
				PlayNeverMeant(buffer[0]);
				break;
   default: /* Report does not exist */
      break;
   }

   return (USBD_OK);
}

/**
 * @brief  CustomHID_GetFeature
 *         Manage the CUSTOM HID GetFeature request.
 *         Device -> Host
 * @param  event_idx: Requested Report Number
 * @param  buffer: Data to transmit including ReportID
 * @param  length: Length of the buffer
 * @retval length: Number of bytes to send
 * @retval USBD_OK
 */
static int8_t CustomHID_GetFeature(uint8_t event_idx, uint8_t* buffer, uint16_t* length)
{
    uint16_t tmpInt;
    switch(event_idx)
    {
    case 0x01:     // собираем состояние 4-х пинов в один байт
        {
            buffer[0]=0;
            if (HAL_GPIO_ReadPin ( GPIOB,GPIO_PIN_4)) { buffer[0] +=  0x01;}; 
            if (HAL_GPIO_ReadPin ( GPIOA,GPIO_PIN_7)) { buffer[0] +=  0x02;}; 
            if (HAL_GPIO_ReadPin ( GPIOA,GPIO_PIN_6)) { buffer[0] +=  0x04;}; 
            if (HAL_GPIO_ReadPin ( GPIOB,GPIO_PIN_5)) { buffer[0] +=  0x08;}; 
        break;
        }
    case 0x03: // замеряем напряжение на переменном сопротивлении
        {
            tmpInt=ADC_V();
            memcpy(&buffer[0], &tmpInt,2);
            break;
        }
    default: /* Report does not exist */
        {
            return (USBD_FAIL);
            break;
        }
   }

   *length = CustomHID_FeatureReportLength(event_idx);
   return (USBD_OK);
}

/**
 * @brief  CustomHID_FeatureReportLength
 *         Get length of a specific feature report (data without ReportID).
 * @param  event_idx: Requested Report Number
 * @retval length: Number of bytes of the according report.
 */
uint16_t CustomHID_FeatureReportLength(uint8_t event_idx)
{
   uint16_t length = 0;

   // Get length of corresponding data
   switch (event_idx)
   {
    case 0x01:
      length = 0x01;
      break;
    case 0x02:
      length = 0x06;
      break;
    case 0x03:
      length = 0x02;
      break;
    case 0x04:
      length = 0x03;
      break;
		case 0x05:
			length = 0x02;
			break;
		case 0x06:
			length = 0x02;
			break;

   default:
      break;
   }

   return length;
}



/**
  * @}
  */

/**
  * @}
  */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
