/*******************************************************************************
* File Name: IND_RED.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_IND_RED_ALIASES_H) /* Pins IND_RED_ALIASES_H */
#define CY_PINS_IND_RED_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define IND_RED_0			(IND_RED__0__PC)
#define IND_RED_0_PS		(IND_RED__0__PS)
#define IND_RED_0_PC		(IND_RED__0__PC)
#define IND_RED_0_DR		(IND_RED__0__DR)
#define IND_RED_0_SHIFT	(IND_RED__0__SHIFT)
#define IND_RED_0_INTR	((uint16)((uint16)0x0003u << (IND_RED__0__SHIFT*2u)))

#define IND_RED_INTR_ALL	 ((uint16)(IND_RED_0_INTR))


#endif /* End Pins IND_RED_ALIASES_H */


/* [] END OF FILE */
