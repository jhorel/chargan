/*******************************************************************************
* File Name: IND_BLU.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_IND_BLU_H) /* Pins IND_BLU_H */
#define CY_PINS_IND_BLU_H

#include "cytypes.h"
#include "cyfitter.h"
#include "IND_BLU_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} IND_BLU_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   IND_BLU_Read(void);
void    IND_BLU_Write(uint8 value);
uint8   IND_BLU_ReadDataReg(void);
#if defined(IND_BLU__PC) || (CY_PSOC4_4200L) 
    void    IND_BLU_SetDriveMode(uint8 mode);
#endif
void    IND_BLU_SetInterruptMode(uint16 position, uint16 mode);
uint8   IND_BLU_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void IND_BLU_Sleep(void); 
void IND_BLU_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(IND_BLU__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define IND_BLU_DRIVE_MODE_BITS        (3)
    #define IND_BLU_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - IND_BLU_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the IND_BLU_SetDriveMode() function.
         *  @{
         */
        #define IND_BLU_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define IND_BLU_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define IND_BLU_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define IND_BLU_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define IND_BLU_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define IND_BLU_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define IND_BLU_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define IND_BLU_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define IND_BLU_MASK               IND_BLU__MASK
#define IND_BLU_SHIFT              IND_BLU__SHIFT
#define IND_BLU_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in IND_BLU_SetInterruptMode() function.
     *  @{
     */
        #define IND_BLU_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define IND_BLU_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define IND_BLU_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define IND_BLU_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(IND_BLU__SIO)
    #define IND_BLU_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(IND_BLU__PC) && (CY_PSOC4_4200L)
    #define IND_BLU_USBIO_ENABLE               ((uint32)0x80000000u)
    #define IND_BLU_USBIO_DISABLE              ((uint32)(~IND_BLU_USBIO_ENABLE))
    #define IND_BLU_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define IND_BLU_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define IND_BLU_USBIO_ENTER_SLEEP          ((uint32)((1u << IND_BLU_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << IND_BLU_USBIO_SUSPEND_DEL_SHIFT)))
    #define IND_BLU_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << IND_BLU_USBIO_SUSPEND_SHIFT)))
    #define IND_BLU_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << IND_BLU_USBIO_SUSPEND_DEL_SHIFT)))
    #define IND_BLU_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(IND_BLU__PC)
    /* Port Configuration */
    #define IND_BLU_PC                 (* (reg32 *) IND_BLU__PC)
#endif
/* Pin State */
#define IND_BLU_PS                     (* (reg32 *) IND_BLU__PS)
/* Data Register */
#define IND_BLU_DR                     (* (reg32 *) IND_BLU__DR)
/* Input Buffer Disable Override */
#define IND_BLU_INP_DIS                (* (reg32 *) IND_BLU__PC2)

/* Interrupt configuration Registers */
#define IND_BLU_INTCFG                 (* (reg32 *) IND_BLU__INTCFG)
#define IND_BLU_INTSTAT                (* (reg32 *) IND_BLU__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define IND_BLU_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(IND_BLU__SIO)
    #define IND_BLU_SIO_REG            (* (reg32 *) IND_BLU__SIO)
#endif /* (IND_BLU__SIO_CFG) */

/* USBIO registers */
#if !defined(IND_BLU__PC) && (CY_PSOC4_4200L)
    #define IND_BLU_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define IND_BLU_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define IND_BLU_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define IND_BLU_DRIVE_MODE_SHIFT       (0x00u)
#define IND_BLU_DRIVE_MODE_MASK        (0x07u << IND_BLU_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins IND_BLU_H */


/* [] END OF FILE */
