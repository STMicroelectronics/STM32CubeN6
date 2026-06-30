/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_user_services.h
  * @author  MCD Application Team
  * @brief   Header file for usbpd_user_services.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __USBPD_USER_SERVICES_H_
#define __USBPD_USER_SERVICES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Include */
/* USER CODE END Include */

/* Exported typedef ----------------------------------------------------------*/
/* USER CODE BEGIN Typedef */
#if defined(_SNK) || defined(_DRP)
/** @brief  Sink Request characteristics Structure definition
  *
  */
typedef struct
{
  uint32_t RequestedVoltageInmVunits;               /*!< Sink request operating voltage in mV units                  */
  uint32_t MaxOperatingCurrentInmAunits;            /*!< Sink request Max operating current in mA units              */
  uint32_t OperatingCurrentInmAunits;               /*!< Sink request operating current in mA units                  */
  uint32_t MaxOperatingPowerInmWunits;              /*!< Sink request Max operating power in mW units                */
  uint32_t OperatingPowerInmWunits;                 /*!< Sink request operating power in mW units                    */
} USBPD_DPM_SNKPowerRequestDetailsTypeDef;
#endif /* _SNK || _DRP */

/** @brief  USBPD Port PDO Structure definition
  *
  */
typedef struct
{
  uint32_t              *ListOfPDO;                 /*!< Pointer on Power Data Objects list,
                                                         defining port capabilities                                  */
  uint8_t               *NumberOfPDO;               /*!< Number of Power Data Objects defined in ListOfPDO. This
                                                         parameter must be set at max to @ref USBPD_MAX_NB_PDO value */
} USBPD_PortPDOTypeDef;

/** @brief  USBPD PDO Selection method enum definition
  *
  */
typedef enum
{
  PDO_SEL_METHOD_MAX_PWR,
  PDO_SEL_METHOD_MIN_PWR,
  PDO_SEL_METHOD_MAX_VOLT,
  PDO_SEL_METHOD_MIN_VOLT,
  PDO_SEL_METHOD_MAX_CUR,
  PDO_SEL_METHOD_MIN_CUR
} USBPD_USER_SERV_PDO_SelectionMethodTypeDef;
/* USER CODE END Typedef */

/* Exported define -----------------------------------------------------------*/
/* USER CODE BEGIN Define */
/* USER CODE END Define */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN Constant */
/* USER CODE END Constant */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN Macro */
/* USER CODE END Macro */

/* Exported variables --------------------------------------------------------*/
#if defined(_SRC) || defined(_DRP)
USBPD_StatusTypeDef USBPD_USER_SERV_EvaluateReqMess(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject);
#endif /* _SRC || _DRP */

#if defined (_SNK) || defined(_DRP)
void USBPD_USER_SERV_StoreSRCPDO(uint8_t PortNum, uint8_t *Ptr, uint32_t Size);
void USBPD_USER_SERV_EvaluateCapa(uint8_t PortNum,
                                  uint32_t *PtrRequestData,
                                  USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType);
#endif /* _SNK || _DRP */
/* USER CODE BEGIN Private_Variables */

/* USER CODE END Private_Variables */

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __USBPD_USER_SERVICES_H_ */

