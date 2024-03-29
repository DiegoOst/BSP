/**
  ******************************************************************************
  * @file    x_nucleo_nfc04a1_nfctag.c
  * @author  MMY Application Team
  * @version $Revision: 3306 $
  * @date    $Date: 2017-01-13 11:18:15 +0100 (Fri, 13 Jan 2017) $
  * @brief   This file provides a set of functions needed to manage a nfc dual 
  *          interface eeprom memory.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
  *
  * Licensed under ST MYLIBERTY SOFTWARE LICENSE AGREEMENT (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/myliberty  
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied,
  * AND SPECIFICALLY DISCLAIMING THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "x_nucleo_nfc04a1_nfctag.h"
//#include <cstddef.h>

/** @addtogroup BSP
 * @{
 */

/** @addtogroup X_NUCLEO_NFC04A1
 * @{
 */

/** @defgroup X_NUCLEO_NFC04A1_NFCTAG
 * @{
 */
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Private_Defines
 * @{
 */
/*#ifndef NULL
#define NULL      (void *) 0
#endif*/
/**
 * @}
 */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/ 
/* Global variables ----------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Private_Variables
 * @{
 */
static NFCTAG_DrvTypeDef *Nfctag_Drv = NULL;
static uint8_t NfctagInitialized = 0;
/**
 * @}
 */
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
/** @defgroup X_NUCLEO_NFC04A1_NFCTAG_Public_Functions
 * @{
 */
/**
  * @brief  Initializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_Init(I2C* mi2cChannel, DigitalOut * mLPD)
{ 
  uint8_t nfctag_id;
  
  if( !NfctagInitialized )
  {
    /* ST25DV Init */
    if( St25Dv_i2c_Drv.Init(mi2cChannel, mLPD) != NFCTAG_OK )
    {
      return NFCTAG_ERROR;
    }


    St25Dv_i2c_Drv.ReadID(&nfctag_id, mi2cChannel);
	
    /* Check if it is the wanted chip */
    if( (nfctag_id == I_AM_ST25DV04) || (nfctag_id == I_AM_ST25DV64) )
    {
      NfctagInitialized = 1;
      Nfctag_Drv = &St25Dv_i2c_Drv;
      Nfctag_Drv->pData = &St25Dv_i2c_ExtDrv;
    }
    else
    {
     Nfctag_Drv = NULL;
      NfctagInitialized = 0;
      return NFCTAG_ERROR;
    }
  }
  
  return NFCTAG_OK;
}

/**
  * @brief  Deinitializes peripherals used by the I2C NFCTAG driver
  * @param  None
  * @retval None
  */
void BSP_NFCTAG_DeInit( void )
{ 
  Nfctag_Drv->pData = NULL;
  Nfctag_Drv = NULL;
  NfctagInitialized = 0;
}

/**
  * @brief  Check if the nfctag is initialized
  * @param  None
  * @retval 0 if the nfctag is not initialized, 1 if the nfctag is already initialized
  */
uint8_t BSP_NFCTAG_isInitialized( void )
{
  return NfctagInitialized;
}

/**
  * @brief  Read the ID of the nfctag
  * @param  wai_id : the pointer where the who_am_i of the device is stored
  * @param mi2cChannel : I2C channel
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_ReadID( uint8_t * const wai_id, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->ReadID == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadID( wai_id, mi2cChannel );
}

/**
  * @brief  Return the size of the nfctag
  * @param mi2cChannel : I2C channel
  * @retval Size of the NFCtag in Bytes
  */
uint32_t BSP_NFCTAG_GetByteSize( I2C * mi2cChannel )
{
  ST25DV_MEM_SIZE mem_size;
  ((NFCTAG_ExtDrvTypeDef *)Nfctag_Drv->pData)->ReadMemSize( &mem_size, mi2cChannel );
  return (mem_size.BlockSize+1) * (mem_size.Mem_Size+1);
}

/**
  * @brief  Check if the nfctag is available
  * @param  Trials : Number of trials
  * @param mi2cChannel : I2C channel
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_IsDeviceReady( const uint32_t Trials, I2C * mi2cChannel, const uint16_t TarAddr )
{
  if ( Nfctag_Drv->IsReady == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->IsReady( Trials, mi2cChannel);
}

/**
  * @brief  Configure nfctag interrupt
  * @param  ITConfig : store interrupt to configure
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_ConfigIT( const uint16_t ITConfig, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->ConfigIT == NULL )
  {
    return NFCTAG_ERROR;
  }
  return Nfctag_Drv->ConfigIT( ITConfig, mi2cChannel );
}

/**
  * @brief  Get nfctag interrupt configutration
  * @param  ITConfig : store interrupt configuration
  *                  - 0x01 => RF BUSY
  *                  - 0x02 => WIP
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_GetITStatus( uint16_t * const ITConfig, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->GetITStatus == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->GetITStatus( ITConfig, mi2cChannel );
}

/**
  * @brief  Reads data in the nfctag at specific address
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C data memory address to read
  * @param  Size : Size in bytes of the value to be read
  * @param mi2cChannel : I2C channel
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_ReadData( uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->ReadData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadData( pData, TarAddr, Size, mi2cChannel );
}

/**
  * @brief  Writes data in the nfctag at specific address
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C data memory address to write
  * @param  Size : Size in bytes of the value to be written
  * @param mi2cChannel : I2C channel
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_WriteData( const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->WriteData == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->WriteData( pData, TarAddr, Size, mi2cChannel );
}

/**
  * @brief  Reads nfctag Register
  * @param  pData : pointer to store read data
  * @param  TarAddr : I2C register address to read
  * @param  Size : Size in bytes of the value to be read
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_ReadRegister( uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size, I2C * mi2cChannel )
{
  if ( Nfctag_Drv->ReadRegister == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  return Nfctag_Drv->ReadRegister( pData, TarAddr, Size, mi2cChannel );
}

/**
  * @brief  Writes nfctag Register
  * @param  pData : pointer to the data to write
  * @param  TarAddr : I2C register address to write
  * @param  Size : Size in bytes of the value to be written
  * @param mi2cChannel : I2C channel
  * @retval NFCTAG enum status
  */
NFCTAG_StatusTypeDef BSP_NFCTAG_WriteRegister( const uint8_t * const pData, const uint16_t TarAddr, const uint16_t Size, I2C * mi2cChannel )
{
  NFCTAG_StatusTypeDef ret_value;
  if ( Nfctag_Drv->WriteRegister == NULL )
  {
    return NFCTAG_ERROR;
  }
  
  ret_value = Nfctag_Drv->WriteRegister( pData, TarAddr, Size, mi2cChannel );
  if( ret_value == NFCTAG_OK )
  {
    while( BSP_NFCTAG_IsDeviceReady( 1, mi2cChannel, TarAddr ) != NFCTAG_OK ) {};
      return NFCTAG_OK;
  }
  
  return ret_value;
}

/**
  * @brief  Give extended features for component
  * @param  None
  * @retval address of the Extended Component Structure
  */
NFCTAG_ExtDrvTypeDef *BSP_NFCTAG_GetExtended_Drv( void )
{
  return (NFCTAG_ExtDrvTypeDef *)Nfctag_Drv->pData;
}

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/******************* (C) COPYRIGHT 2017 STMicroelectronics *****END OF FILE****/
