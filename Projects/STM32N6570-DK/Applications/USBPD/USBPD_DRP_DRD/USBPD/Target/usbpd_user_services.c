/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usbpd_user_services.c
  * @author  MCD Application Team
  * @brief   USBPD user services code
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

#define USBPD_USER_SERVICES_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbpd_core.h"
#include "usbpd_user_services.h"
#include "usbpd_dpm_conf.h"
#include "usbpd_pwr_if.h"
#if defined(_TRACE)
#include "usbpd_trace.h"
#include "stdio.h"
#endif /* _TRACE */
#include "string.h"
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */
/* USER CODE END Private_Typedef */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_Define */
#define DPM_NO_SRC_PDO_FOUND   0xFFU /*!< No match found between Received SRC PDO and SNK capabilities */
/* USER CODE END Private_Define */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */
/* USER CODE END Private_Macro */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */

/* Method used to find the "best" PDO */
USBPD_USER_SERV_PDO_SelectionMethodTypeDef USER_SERV_PDO_Sel_Method = PDO_SEL_METHOD_MAX_PWR;

extern USBPD_ParamsTypeDef              DPM_Params[USBPD_PORT_COUNT];
extern USBPD_SettingsTypeDef            DPM_Settings[USBPD_PORT_COUNT];
extern USBPD_USER_SettingsTypeDef       DPM_USER_Settings[USBPD_PORT_COUNT];
/* USER CODE END Private_Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */
#if defined (_SNK) || defined(_DRP)
static uint32_t USER_SERV_SNK_EvaluateMatchWithSRCPDO(uint8_t PortNum,
                                                      uint32_t SrcPDO,
                                                      uint32_t *PtrRequestedVoltage,
                                                      uint32_t *PtrRequestedPower);

static uint32_t USER_SERV_FindVoltageIndex(uint32_t PortNum,
                                           USBPD_DPM_SNKPowerRequestDetailsTypeDef *PtrRequestPowerDetails,
                                           uint8_t Method);

static void     USER_SERV_SNK_BuildRDOfromSelectedPDO(uint8_t PortNum,
                                                      uint8_t IndexSrcPDO,
                                                      USBPD_DPM_SNKPowerRequestDetailsTypeDef *PtrRequestPowerDetails,
                                                      USBPD_SNKRDO_TypeDef *Rdo,
                                                      USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject);
#endif /* _SNK || _DRP */
/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS_Prototypes */

/* Exported functions ------- ------------------------------------------------*/
/* USER CODE BEGIN USBPD_USER_EXPORTED_FUNCTIONS */
#if defined(_DRP) || defined(_SRC)
/**
  * @brief  Evaluate received Request Message from Sink port
  * @param  PortNum Port number
  * @param  PtrPowerObject  Pointer on the power data object
  * @retval USBPD status : USBPD_ACCEPT, USBPD_REJECT, USBPD_WAIT, USBPD_GOTOMIN
  */
USBPD_StatusTypeDef USBPD_USER_SERV_EvaluateReqMess(uint8_t PortNum, USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_PDO_TypeDef pdo;
  uint32_t pdomaxcurrent = 0U;
  uint32_t rdomaxcurrent = 0U, rdoopcurrent = 0U, rdoobjposition = 0U;
  uint32_t rdovoltage;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];

  rdo.d32 = pdhandle->DPM_RcvRequestDOMsg;
  rdoobjposition  = rdo.GenericRDO.ObjectPosition;
#if defined(USBPD_REV30_SUPPORT) && defined(_UNCHUNKED_SUPPORT)
  /* Set unchuncked bit if supported by ports */
  DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
  if ((USBPD_TRUE == rdo.GenericRDO.UnchunkedExtendedMessage)
      && (USBPD_TRUE == DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport))
  {
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
  }
#endif /* USBPD_REV30_SUPPORT && _UNCHUNKED_SUPPORT */
  pdhandle->DPM_RDOPosition = 0U;

  /* Check if RDP can be met within the supported PDOs by the Source port */
  /* USBPD_USER_SERV_EvaluateReqMess: Evaluate Sink Request\r */
  /* USBPD_USER_SERV_EvaluateReqMess: Check if RDP can be met within the supported PDOs by the Source port\r */

  /* Search PDO in Port Source PDO list, that corresponds to Position provided in Request RDO */
  if (USBPD_PWR_IF_SearchRequestedPDO(PortNum, rdoobjposition, &pdo.d32) != USBPD_OK)
  {
    /* Invalid PDO index */
    /* USBPD_USER_SERV_EvaluateReqMess: Invalid PDOs index */
    return USBPD_REJECT;
  }

  switch (pdo.GenericPDO.PowerObject)
  {
    case USBPD_CORE_PDO_TYPE_FIXED:
    {
      pdomaxcurrent = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
      rdomaxcurrent = rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits;
      rdoopcurrent  = rdo.FixedVariableRDO.OperatingCurrentIn10mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 10U;
      rdovoltage    = pdo.SRCFixedPDO.VoltageIn50mVunits * 50U;

      if (rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        /* USBPD_USER_SERV_EvaluateReqMess: Sink requests too much operating current*/
        return USBPD_REJECT;
      }

      if (rdomaxcurrent > pdomaxcurrent)
      {
        /* Sink requests too much maximum operating current */
        /* USBPD_USER_SERV_EvaluateReqMess: Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
#if _PPS
    case USBPD_CORE_PDO_TYPE_APDO:
    {
      uint32_t pdominvoltage, pdomaxvoltage;
      pdomaxcurrent                           = pdo.SRCSNKAPDO.MaxCurrentIn50mAunits;
      rdoopcurrent                            = rdo.ProgRDO.OperatingCurrentIn50mAunits;
      DPM_Ports[PortNum].DPM_RequestedCurrent = rdoopcurrent * 50U;
      if (rdoopcurrent > pdomaxcurrent)
      {
        /* Sink requests too much operating current */
        return USBPD_REJECT;
      }
      pdominvoltage = pdo.SRCSNKAPDO.MinVoltageIn100mV * 100U;
      pdomaxvoltage = pdo.SRCSNKAPDO.MaxVoltageIn100mV * 100U;
      rdovoltage    = rdo.ProgRDO.OutputVoltageIn20mV * 20U;

      if ((rdovoltage < pdominvoltage) || (rdovoltage > pdomaxvoltage))
      {
        /* Sink requests too much maximum operating current */
        return USBPD_REJECT;
      }
    }
    break;
#endif /* _PPS */
    case USBPD_CORE_PDO_TYPE_BATTERY:
    case USBPD_CORE_PDO_TYPE_VARIABLE:
    default:
    {
      return USBPD_REJECT;
    }
  }

  /* Set RDO position and requested voltage in DPM port structure */
  DPM_Ports[PortNum].DPM_RequestedVoltage = rdovoltage;
  pdhandle->DPM_RDOPositionPrevious = pdhandle->DPM_RDOPosition;
  pdhandle->DPM_RDOPosition = rdoobjposition;

  /* Save the power object */
  *PtrPowerObject = pdo.GenericPDO.PowerObject;

  /* Accept the requested power */
  /* USBPD_USER_SERV_EvaluateReqMess: Sink requested %d mV %d mA for operating current from %d to %d mA\r",
               pdo.SRCFixedPDO.VoltageIn50mVunits * 50, pdo.SRCFixedPDO.MaxCurrentIn10mAunits * 10,
               rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits * 10,
               rdo.FixedVariableRDO.OperatingCurrentIn10mAunits * 10 */
  /* USBPD_USER_SERV_EvaluateReqMess: Source accepts the requested power */
  return USBPD_ACCEPT;
}
#endif /* _DRP || _SRC */

#if defined(_SNK) || defined(_DRP)
/**
  * @brief  Store the received source PDO
  * @param  PortNum Port number
  * @param  Ptr     Pointer on the data
  * @param  Size    Nb of bytes to be updated in DPM
  * @retval None
  */
void USBPD_USER_SERV_StoreSRCPDO(uint8_t PortNum, uint8_t *Ptr, uint32_t Size)
{
  /*!< Storage of Received Source PDO values */
  if (Size <= (USBPD_MAX_NB_PDO * 4U))
  {
    uint8_t *rdo;
    DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO = (Size / 4U);
    /* Copy PDO data in DPM Handle field */
    for (uint32_t index = 0; index < (Size / 4U); index++)
    {
      rdo = (uint8_t *)&DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO[index];
      (void)memcpy(rdo, (Ptr + (index * 4U)), (4U * sizeof(uint8_t)));
    }
  }
}

/**
  * @brief  Evaluate received Capabilities Message from Source port and prepare the request message
  * @param  PortNum         Port number
  * @param  PtrRequestData  Pointer on selected request data object
  * @param  PtrPowerObjectType  Pointer on the power data object
  * @retval None
  */
void USBPD_USER_SERV_EvaluateCapa(uint8_t PortNum,
                                  uint32_t *PtrRequestData,
                                  USBPD_CORE_PDO_Type_TypeDef *PtrPowerObjectType)
{
  USBPD_PDO_TypeDef  fixed_pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];
  USBPD_DPM_SNKPowerRequestDetailsTypeDef snkpowerrequestdetails;
  uint32_t pdoindex;
  uint32_t size;
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  snkpowerrequestdetails.RequestedVoltageInmVunits = 0;
  snkpowerrequestdetails.OperatingCurrentInmAunits = 0;

  /* Find the Pdo index for the requested voltage, depending on the wanted method */
  pdoindex = USER_SERV_FindVoltageIndex(PortNum, &snkpowerrequestdetails, USER_SERV_PDO_Sel_Method);

  /* Initialize RDO */
  rdo.d32 = 0;

  /* If no valid SNK PDO or if no SRC PDO match found
      (index>=nb of valid received SRC PDOs or function returned DPM_NO_SRC_PDO_FOUND */
  if (pdoindex >= pdhandle->DPM_NumberOfRcvSRCPDO)
  {
#if defined(_TRACE)
    uint8_t msg[] = "USBPD_USER_SERV_EvaluateCapa: could not find desired voltage";
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)msg, sizeof(msg));
#endif /* _TRACE */
    fixed_pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];
    /* Read SNK PDO list for retrieving useful data to fill in RDO */
    USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t *)&snkpdolist[0], &size);
    /* Store value of 1st SNK PDO (Fixed) in local variable */
    snk_fixed_pdo.d32 = snkpdolist[0];
    rdo.FixedVariableRDO.ObjectPosition = 1U;
    rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  =  fixed_pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits =  fixed_pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.CapabilityMismatch = 1U;
    rdo.FixedVariableRDO.USBCommunicationsCapable = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
    DPM_Ports[PortNum].DPM_RequestedCurrent = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;

    pdhandle->DPM_RequestDOMsg = rdo.d32;
    *PtrPowerObjectType = USBPD_CORE_PDO_TYPE_FIXED;
    *PtrRequestData = rdo.d32;
    pdhandle->DPM_RequestedVoltage = 5000U;
    return;
  }

  USER_SERV_SNK_BuildRDOfromSelectedPDO(PortNum, pdoindex, &snkpowerrequestdetails, &rdo, PtrPowerObjectType);

  *PtrRequestData = pdhandle->DPM_RequestDOMsg;
}
#endif /* _SNK || _DRP */
/* USER CODE END USBPD_USER_EXPORTED_FUNCTIONS */

/* USER CODE BEGIN USBPD_USER_PRIVATE_FUNCTIONS */

#if defined(_SNK) || defined(_DRP)
/**
  * @brief  Examinate a given SRC PDO to check if matching with SNK capabilities.
  * @param  PortNum             Port number
  * @param  SrcPDO              Selected SRC PDO (32 bits)
  * @param  PtrRequestedVoltage Pointer on Voltage value that could be reached if SRC
                                PDO is requested (only valid if USBPD_TRUE is returned) in mV
  * @param  PtrRequestedPower   Pointer on Power value that could be reached if SRC PDO is requested
                                (only valid if USBPD_TRUE is returned) in mW
  * @retval USBPD_FALSE of USBPD_TRUE (USBPD_TRUE returned in SRC PDO is considered matching with SNK profile)
  */
static uint32_t USER_SERV_SNK_EvaluateMatchWithSRCPDO(uint8_t PortNum,
                                                      uint32_t SrcPDO,
                                                      uint32_t *PtrRequestedVoltage,
                                                      uint32_t *PtrRequestedPower)
{
  USBPD_PDO_TypeDef srcpdo;
  USBPD_PDO_TypeDef snkpdo;
  uint32_t match = USBPD_FALSE;
  uint32_t nbsnkpdo;
  uint32_t snkpdo_array[USBPD_MAX_NB_PDO];
  uint16_t srcvoltage50mv;
  uint16_t srcmaxcurrent10ma;
  uint16_t snkvoltage50mv;
  uint16_t snkopcurrent10ma;
  uint32_t maxrequestedpower;
  uint32_t currentrequestedpower;
  uint32_t maxrequestedvoltage;
  uint32_t currentrequestedvoltage;

  /* Retrieve SNK PDO list from PWR_IF storage : PDO values + nb of u32 written by PWR_IF (nb of PDOs) */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t *)snkpdo_array, &nbsnkpdo);

  if (0 == nbsnkpdo)
  {
    return (USBPD_FALSE);
  }

  /* Set default output values */
  maxrequestedpower    = 0;
  maxrequestedvoltage  = 0;

  /* Check SRC PDO value according to its type */
  srcpdo.d32 = SrcPDO;
  switch (srcpdo.GenericPDO.PowerObject)
  {
    /* SRC Fixed Supply PDO */
    case USBPD_CORE_PDO_TYPE_FIXED:
    {
      srcvoltage50mv = srcpdo.SRCFixedPDO.VoltageIn50mVunits;
      srcmaxcurrent10ma = srcpdo.SRCFixedPDO.MaxCurrentIn10mAunits;

      /* Loop through SNK PDO list */
      for (uint16_t i = 0; i < nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch (snkpdo.GenericPDO.PowerObject)
        {
          /* SNK Fixed Supply PDO */
          case USBPD_CORE_PDO_TYPE_FIXED:
          {
            snkvoltage50mv = snkpdo.SNKFixedPDO.VoltageIn50mVunits;
            snkopcurrent10ma = snkpdo.SNKFixedPDO.OperationalCurrentIn10mAunits;

            /* Match if :
                 SNK Voltage = SRC Voltage
                 &&
                 SNK Op Current <= SRC Max Current

               Requested Voltage: SNK Voltage
               Requested Op Current: SNK Op Current
               Requested Max Current: SNK Op Current
            */
            if ((snkvoltage50mv == srcvoltage50mv) && (snkopcurrent10ma <= srcmaxcurrent10ma))
            {
              currentrequestedpower = (snkvoltage50mv * snkopcurrent10ma) / 2U; /* To get value in mW */
              currentrequestedvoltage = snkvoltage50mv;
            }
            break;
          }
          /* SNK Augmented Power Data Object (APDO) */
          case USBPD_CORE_PDO_TYPE_APDO:
            break;

          default:
            break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
    }
    break;

    /* Augmented Power Data Object (APDO) */
    case USBPD_CORE_PDO_TYPE_APDO:
    {
      uint16_t srcmaxvoltage100mv;
      uint16_t srcminvoltage100mv;
      uint16_t srcmaxcurrent50ma;
      srcmaxvoltage100mv = srcpdo.SRCSNKAPDO.MaxVoltageIn100mV;
      srcminvoltage100mv = srcpdo.SRCSNKAPDO.MinVoltageIn100mV;
      srcmaxcurrent50ma = srcpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

      /* Loop through SNK PDO list */
      for (uint16_t i = 0; i < nbsnkpdo; i++)
      {
        currentrequestedpower = 0;
        currentrequestedvoltage = 0;

        /* Retrieve SNK PDO value according to its type */
        snkpdo.d32 = snkpdo_array[i];
        switch (snkpdo.GenericPDO.PowerObject)
        {
          case USBPD_CORE_PDO_TYPE_FIXED:
            /* No match */
            break;

          /* SNK Augmented Power Data Object (APDO) */
          case USBPD_CORE_PDO_TYPE_APDO:
          {
            uint16_t snkmaxvoltage100mv;
            uint16_t snkminvoltage100mv;
            uint16_t snkmaxcurrent50ma;

            snkminvoltage100mv = snkpdo.SRCSNKAPDO.MinVoltageIn100mV;
            snkmaxvoltage100mv = snkpdo.SRCSNKAPDO.MaxVoltageIn100mV;
            snkmaxcurrent50ma = snkpdo.SRCSNKAPDO.MaxCurrentIn50mAunits;

            /* Match if SNK APDO voltage overlaps with the SRC APDO voltage range */
            if (((srcminvoltage100mv <= snkmaxvoltage100mv) && (srcminvoltage100mv >= snkminvoltage100mv)) ||
                ((snkminvoltage100mv <= srcmaxvoltage100mv) && (snkminvoltage100mv >= srcminvoltage100mv)))
            {
              if (snkmaxcurrent50ma <= srcmaxcurrent50ma)
              {
                *PtrRequestedVoltage = MIN(PWR_DECODE_100MV(srcmaxvoltage100mv),
                                           PWR_DECODE_100MV(snkmaxvoltage100mv));
                currentrequestedpower = (*PtrRequestedVoltage * PWR_DECODE_50MA(snkmaxcurrent50ma)) / 1000U; /* mW */
                currentrequestedvoltage = (*PtrRequestedVoltage / 50U);
              }
            }
          }
          break;

          default:
            break;
        }

        if (currentrequestedpower > maxrequestedpower)
        {
          match = USBPD_TRUE;
          maxrequestedpower   = currentrequestedpower;
          maxrequestedvoltage = currentrequestedvoltage;
        }
      }
    }
    break;

    default:
      return (USBPD_FALSE);
  }

  if (maxrequestedpower > 0)
  {
    *PtrRequestedPower   = maxrequestedpower;
    *PtrRequestedVoltage = maxrequestedvoltage * 50U; /* value in mV */
  }
  return (match);
}

/**
  * @brief  Find PDO index depending on SNK capabilities and selection method: max/min power, voltage, or current.
  * @param  PortNum Port number
  * @param  PtrRequestPowerDetails  Sink requested power details structure pointer
  * @param  Method  Method used to find the "best" PDO. This parameter can be one of the following values:
  *         @arg @ref PDO_SEL_METHOD_MAX_PWR
  *         @arg @ref PDO_SEL_METHOD_MIN_PWR
  *         @arg @ref PDO_SEL_METHOD_MAX_VOLT
  *         @arg @ref PDO_SEL_METHOD_MIN_VOLT
  *         @arg @ref PDO_SEL_METHOD_MAX_CUR
  *         @arg @ref PDO_SEL_METHOD_MIN_CUR
  * @retval Index of PDO within source capabilities message (DPM_NO_SRC_PDO_FOUND indicating not found)
  */
static uint32_t USER_SERV_FindVoltageIndex(uint32_t PortNum,
                                           USBPD_DPM_SNKPowerRequestDetailsTypeDef *PtrRequestPowerDetails,
                                           uint8_t Method)
{
  uint32_t *ptpdoarray;
  USBPD_PDO_TypeDef  pdo;
  uint32_t voltage;
  uint32_t reqvoltage;
  uint32_t nbpdo;
  uint32_t allowablepower;
  uint32_t selpower;
  uint32_t allowablecurrent;
  uint32_t selcurrent;
  uint32_t curr_index = DPM_NO_SRC_PDO_FOUND;
  uint32_t temp_index;
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];

  allowablepower = 0;
  selpower       = 0;
  reqvoltage     = 0;
  voltage        = 0;
  selcurrent     = 0;

  /* Search PDO index among Source PDO of Port */
  nbpdo = DPM_Ports[PortNum].DPM_NumberOfRcvSRCPDO;
  ptpdoarray = DPM_Ports[PortNum].DPM_ListOfRcvSRCPDO;

  /* search the best PDO in the list of source PDOs */
  for (temp_index = 0; temp_index < nbpdo; temp_index++)
  {
    pdo.d32 = ptpdoarray[temp_index];

    /* Check if the received source PDO is matching any of the SNK PDO */
    allowablepower = 0;
    if (USBPD_TRUE == USER_SERV_SNK_EvaluateMatchWithSRCPDO(PortNum, pdo.d32, &voltage, &allowablepower))
    {
      allowablecurrent = (allowablepower / voltage) * 1000U;

      /* Choose the best PDO depending on the user preferences */
      switch (Method)
      {
        case PDO_SEL_METHOD_MAX_PWR:
          if (allowablepower > selpower)
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        case PDO_SEL_METHOD_MIN_PWR:
          if ((allowablepower < selpower) || (selpower == 0))
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        case PDO_SEL_METHOD_MAX_VOLT:
          if (voltage > reqvoltage)
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        case PDO_SEL_METHOD_MIN_VOLT:
          if ((voltage < reqvoltage) || (reqvoltage == 0))
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        case PDO_SEL_METHOD_MAX_CUR:
          if (allowablecurrent > selcurrent)
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        case PDO_SEL_METHOD_MIN_CUR:
          if ((allowablecurrent < selcurrent) || (selcurrent == 0))
          {
            /* Consider the current PDO the best one until now */
            curr_index = temp_index;
            selpower   = allowablepower;
            reqvoltage = voltage;
            selcurrent = allowablecurrent;
          }
          break;

        default:
          /* Default behavior: last PDO is selected */
          curr_index = temp_index;
          selpower   = allowablepower;
          reqvoltage = voltage;
          selcurrent = allowablecurrent;
      }
    }
  }

  /* If a suitable PDO was found */
  if (curr_index != DPM_NO_SRC_PDO_FOUND)
  {
    /* Fill the request power details */
    PtrRequestPowerDetails->MaxOperatingCurrentInmAunits = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;
    PtrRequestPowerDetails->OperatingCurrentInmAunits    = (1000U * selpower) / reqvoltage;
    PtrRequestPowerDetails->MaxOperatingPowerInmWunits   = puser->DPM_SNKRequestedPower.MaxOperatingPowerInmWunits;
    PtrRequestPowerDetails->OperatingPowerInmWunits      = selpower;
    PtrRequestPowerDetails->RequestedVoltageInmVunits    = reqvoltage;
  }

  return curr_index;
}

/**
  * @brief  Build RDO to be used in Request message according to selected PDO from received SRC Capabilities
  * @param  PortNum           Port number
  * @param  IndexSrcPDO       Index on the selected SRC PDO (value between 0 to 6)
  * @param  PtrRequestPowerDetails  Sink requested power details structure pointer
  * @param  Rdo               Pointer on the RDO
  * @param  PtrPowerObject    Pointer on the selected power object
  * @retval None
  */
static void USER_SERV_SNK_BuildRDOfromSelectedPDO(uint8_t PortNum,
                                                  uint8_t IndexSrcPDO,
                                                  USBPD_DPM_SNKPowerRequestDetailsTypeDef *PtrRequestPowerDetails,
                                                  USBPD_SNKRDO_TypeDef *Rdo,
                                                  USBPD_CORE_PDO_Type_TypeDef *PtrPowerObject)
{
  uint32_t mv = 0;
  uint32_t ma = 0;
  uint32_t size;
  USBPD_PDO_TypeDef  pdo;
  USBPD_SNKRDO_TypeDef rdo;
  USBPD_HandleTypeDef *pdhandle = &DPM_Ports[PortNum];
  USBPD_USER_SettingsTypeDef *puser = (USBPD_USER_SettingsTypeDef *)&DPM_USER_Settings[PortNum];
  uint32_t snkpdolist[USBPD_MAX_NB_PDO];
  USBPD_PDO_TypeDef snk_fixed_pdo;

  /* Initialize RDO */
  rdo.d32 = 0;

  /* Read SNK PDO list for retrieving useful data to fill in RDO */
  USBPD_PWR_IF_GetPortPDOs(PortNum, USBPD_CORE_DATATYPE_SNK_PDO, (uint8_t *)&snkpdolist[0], &size);

  /* Store value of 1st SNK PDO (Fixed) in local variable */
  snk_fixed_pdo.d32 = snkpdolist[0];

  /* Set common fields in RDO */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[0];
  rdo.GenericRDO.USBCommunicationsCapable     = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
  if (USBPD_SPECIFICATION_REV2 < DPM_Params[PortNum].PE_SpecRevision)
  {
#if defined(USBPDCORE_UNCHUNCKED_MODE)
    rdo.FixedVariableRDO.UnchunkedExtendedMessage = DPM_Settings[PortNum].PE_PD3_Support.d.PE_UnchunkSupport;
    DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_FALSE;
    /* Set unchuncked bit if supported by port partner;*/
    if (USBPD_TRUE == pdo.SRCFixedPDO.UnchunkedExtendedMessage)
    {
      DPM_Params[PortNum].PE_UnchunkSupport   = USBPD_TRUE;
    }
#endif /* USBPDCORE_UNCHUNCKED_MODE */
  }

  /* If no valid SNK PDO or if no SRC PDO match found (index>=nb of valid received SRC PDOs */
  if ((size < 1) || (IndexSrcPDO >= pdhandle->DPM_NumberOfRcvSRCPDO))
  {
    /* USBPD_DPM_EvaluateCapabilities: Mismatch, could not find desired pdo index */
#ifdef _TRACE
    uint8_t msg[] = "USER_SERV_SNK_BuildRDOfromSelectedPDO: Pb in SRC PDO selection";
    USBPD_TRACE_Add(USBPD_TRACE_DEBUG, PortNum, 0, (uint8_t *)msg, sizeof(msg));
#endif /* _TRACE */
    rdo.FixedVariableRDO.ObjectPosition = 1U;
    rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = pdo.SRCFixedPDO.MaxCurrentIn10mAunits;
    rdo.FixedVariableRDO.CapabilityMismatch           = 1U;
    rdo.FixedVariableRDO.USBCommunicationsCapable     = snk_fixed_pdo.SNKFixedPDO.USBCommunicationsCapable;
    DPM_Ports[PortNum].DPM_RequestedCurrent           = puser->DPM_SNKRequestedPower.MaxOperatingCurrentInmAunits;
    /* USBPD_DPM_EvaluateCapabilities: Mismatch, could not find desired pdo index */

    pdhandle->DPM_RequestDOMsg = rdo.d32;
    return;
  }

  /* Set the Object position */
  rdo.GenericRDO.ObjectPosition               = IndexSrcPDO + 1U;
  rdo.GenericRDO.NoUSBSuspend                 = 0;

  /* Extract power information from Power Data Object */
  pdo.d32 = pdhandle->DPM_ListOfRcvSRCPDO[IndexSrcPDO];

  *PtrPowerObject = pdo.GenericPDO.PowerObject;

  /* Retrieve request details from SRC PDO selection */
  mv = PtrRequestPowerDetails->RequestedVoltageInmVunits;
  ma = PtrRequestPowerDetails->OperatingCurrentInmAunits;

  switch (pdo.GenericPDO.PowerObject)
  {
    case USBPD_CORE_PDO_TYPE_FIXED:
    {
      DPM_Ports[PortNum].DPM_RequestedCurrent           = ma;
      rdo.FixedVariableRDO.OperatingCurrentIn10mAunits  = ma / 10U;
      rdo.FixedVariableRDO.MaxOperatingCurrent10mAunits = ma / 10U;
    }
    break;

    case USBPD_CORE_PDO_TYPE_APDO:
    {
      DPM_Ports[PortNum].DPM_RequestedCurrent    = ma;
      rdo.ProgRDO.ObjectPosition                 = IndexSrcPDO + 1U;
      rdo.ProgRDO.OperatingCurrentIn50mAunits    = ma / 50U;
      rdo.ProgRDO.OutputVoltageIn20mV            = mv / 20U;
    }
    break;

    default:
      break;
  }

  pdhandle->DPM_RequestDOMsg = rdo.d32;
  pdhandle->DPM_RDOPosition  = rdo.GenericRDO.ObjectPosition;

  Rdo->d32 = pdhandle->DPM_RequestDOMsg;
  /* Get the requested voltage */
  pdhandle->DPM_RequestedVoltage = mv;
}
#endif /* _SNK || _DRP */
/* USER CODE END USBPD_USER_PRIVATE_FUNCTIONS */
