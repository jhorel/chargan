/*******************************************************************************
* File Name: button.h  
* Version 2.10
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_button_H) /* Pins button_H */
#define CY_PINS_button_H

#include "cytypes.h"
#include "cyfitter.h"
#include "button_aliases.h"


/***************************************
*        Function Prototypes             
***************************************/    

void    button_Write(uint8 value) ;
void    button_SetDriveMode(uint8 mode) ;
uint8   button_ReadDataReg(void) ;
uint8   button_Read(void) ;
uint8   button_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define button_DRIVE_MODE_BITS        (3)
#define button_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - button_DRIVE_MODE_BITS))

#define button_DM_ALG_HIZ         (0x00u)
#define button_DM_DIG_HIZ         (0x01u)
#define button_DM_RES_UP          (0x02u)
#define button_DM_RES_DWN         (0x03u)
#define button_DM_OD_LO           (0x04u)
#define button_DM_OD_HI           (0x05u)
#define button_DM_STRONG          (0x06u)
#define button_DM_RES_UPDWN       (0x07u)

/* Digital Port Constants */
#define button_MASK               button__MASK
#define button_SHIFT              button__SHIFT
#define button_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define button_PS                     (* (reg32 *) button__PS)
/* Port Configuration */
#define button_PC                     (* (reg32 *) button__PC)
/* Data Register */
#define button_DR                     (* (reg32 *) button__DR)
/* Input Buffer Disable Override */
#define button_INP_DIS                (* (reg32 *) button__PC2)


#if defined(button__INTSTAT)  /* Interrupt Registers */

    #define button_INTSTAT                (* (reg32 *) button__INTSTAT)

#endif /* Interrupt Registers */


/***************************************
* The following code is DEPRECATED and 
* must not be used.
***************************************/

#define button_DRIVE_MODE_SHIFT       (0x00u)
#define button_DRIVE_MODE_MASK        (0x07u << button_DRIVE_MODE_SHIFT)


#endif /* End Pins button_H */


/* [] END OF FILE */
