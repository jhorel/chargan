/*******************************************************************************
* File Name: IND_RED.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "IND_RED.h"

static IND_RED_BACKUP_STRUCT  IND_RED_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: IND_RED_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet IND_RED_SUT.c usage_IND_RED_Sleep_Wakeup
*******************************************************************************/
void IND_RED_Sleep(void)
{
    #if defined(IND_RED__PC)
        IND_RED_backup.pcState = IND_RED_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            IND_RED_backup.usbState = IND_RED_CR1_REG;
            IND_RED_USB_POWER_REG |= IND_RED_USBIO_ENTER_SLEEP;
            IND_RED_CR1_REG &= IND_RED_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(IND_RED__SIO)
        IND_RED_backup.sioState = IND_RED_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        IND_RED_SIO_REG &= (uint32)(~IND_RED_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: IND_RED_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to IND_RED_Sleep() for an example usage.
*******************************************************************************/
void IND_RED_Wakeup(void)
{
    #if defined(IND_RED__PC)
        IND_RED_PC = IND_RED_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            IND_RED_USB_POWER_REG &= IND_RED_USBIO_EXIT_SLEEP_PH1;
            IND_RED_CR1_REG = IND_RED_backup.usbState;
            IND_RED_USB_POWER_REG &= IND_RED_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(IND_RED__SIO)
        IND_RED_SIO_REG = IND_RED_backup.sioState;
    #endif
}


/* [] END OF FILE */
