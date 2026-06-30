/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   STM32 Controller Driver                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_stm32.h"
#include "ux_utility.h"
#include "ux_device_stack.h"


#if !defined(UX_DEVICE_STANDALONE)

static VOID  _ux_dcd_stm32_transfer_arm(UX_DCD_STM32 *dcd_stm32, UX_SLAVE_ENDPOINT *endpoint,
                                        UX_SLAVE_TRANSFER *transfer_request, UX_DCD_STM32_ED *ed)
{

ULONG                   endpoint_type;


    endpoint_type = endpoint -> ux_slave_endpoint_descriptor.bmAttributes & UX_MASK_ENDPOINT_TYPE;

    if (endpoint_type == UX_ISOCHRONOUS_ENDPOINT)
    {
#if defined(USBD_HAL_ISOINCOMPLETE_CALLBACK)
        if ((ed -> ux_stm32_ed_interval_mask != 0U) &&
            (_ux_dcd_stm32_iso_slot_locked_get(endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress) != 0U))
        {
            _ux_dcd_stm32_iso_submit_pending_set(endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress);
            return;
        }
#endif /* defined(USBD_HAL_ISOINCOMPLETE_CALLBACK) */
    }

    if (transfer_request -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    {
        HAL_PCD_EP_Transmit(dcd_stm32 -> pcd_handle,
                            endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress,
                            transfer_request -> ux_slave_transfer_request_data_pointer,
                            transfer_request -> ux_slave_transfer_request_requested_length);
    }
    else
    {
        HAL_PCD_EP_Receive(dcd_stm32 -> pcd_handle,
                           endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress,
                           transfer_request -> ux_slave_transfer_request_data_pointer,
                           transfer_request -> ux_slave_transfer_request_requested_length);
    }
}

/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_stm32_transfer_request                      PORTABLE C      */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will initiate a transfer to a specific endpoint.      */
/*    The controller is armed according to the transfer direction and     */
/*    endpoint type.                                                      */
/*                                                                        */
/*    For non-control endpoints, the caller then waits for completion     */
/*    from the HAL callback path.                                         */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_stm32                             Pointer to device controller  */
/*    transfer_request                      Pointer to transfer request   */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    HAL_PCD_EP_Transmit                   Transmit data                 */
/*    HAL_PCD_EP_Receive                    Receive data                  */
/*    _ux_utility_semaphore_get             Get semaphore                 */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    STM32 Controller Driver                                             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s), used ST  */
/*                                            HAL library to drive the    */
/*                                            controller,                 */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_stm32_transfer_request(UX_DCD_STM32 *dcd_stm32, UX_SLAVE_TRANSFER *transfer_request)
{

UX_SLAVE_ENDPOINT       *endpoint;
UX_DCD_STM32_ED         *ed;
UINT                    status;


    /* Get the pointer to the logical endpoint from the transfer request.  */
    endpoint =  transfer_request -> ux_slave_transfer_request_endpoint;
    ed =  (UX_DCD_STM32_ED *) endpoint -> ux_slave_endpoint_ed;

    /* Check for transfer direction.  Is this a IN endpoint ? */
    if (transfer_request -> ux_slave_transfer_request_phase == UX_TRANSFER_PHASE_DATA_OUT)
    {
        _ux_dcd_stm32_transfer_arm(dcd_stm32, endpoint, transfer_request, ed);
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore,
                                                (ULONG)transfer_request -> ux_slave_transfer_request_timeout);

            /* Check the completion code. */
            if (status != UX_SUCCESS)
            {
                return(status);
            }

            transfer_request -> ux_slave_transfer_request_actual_length = transfer_request -> ux_slave_transfer_request_requested_length;

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
            {
                return(transfer_request -> ux_slave_transfer_request_completion_code);
            }

            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }
    else
    {
        _ux_dcd_stm32_transfer_arm(dcd_stm32, endpoint, transfer_request, ed);
        /* If the endpoint is a Control endpoint, all this is happening under Interrupt and there is no
           thread to suspend.  */
        if ((endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress & (UINT)~UX_ENDPOINT_DIRECTION) != 0)
        {

            /* We should wait for the semaphore to wake us up.  */
            status =  _ux_utility_semaphore_get(&transfer_request -> ux_slave_transfer_request_semaphore,
                                                (ULONG)transfer_request -> ux_slave_transfer_request_timeout);

            /* Check the completion code. */
            if (status != UX_SUCCESS)
            {
                return(status);
            }

            /* Check the transfer request completion code. We may have had a BUS reset or
               a device disconnection.  */
            if (transfer_request -> ux_slave_transfer_request_completion_code != UX_SUCCESS)
            {
                return(transfer_request -> ux_slave_transfer_request_completion_code);
            }

            /* Return to caller with success.  */
            return(UX_SUCCESS);
        }
    }

    /* Return to caller with success.  */
    return(UX_SUCCESS);
}
#endif
