/*******************************************************************************
 Module for Microchip Legato Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    le_gen_assets.h

  Summary:
    Header file containing a list of asset specifications for use with the
    Legato Graphics Stack.


  Description:
    Header file containing a list of asset specifications for use with the
    Legato Graphics Stack.

*******************************************************************************/


// DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C)  Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

// DOM-IGNORE-END

#ifndef LE_GEN_ASSETS_H
#define LE_GEN_ASSETS_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

#include "gfx/legato/legato.h"

extern const lePalette leGlobalPalette;

/*****************************************************************************
 * Legato Graphics Image Assets
 *****************************************************************************/
/*********************************
 * Legato Image Asset
 * Name:   icnHomePressed
 * Size:   40x40 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage icnHomePressed;

/*********************************
 * Legato Image Asset
 * Name:   icnHomeReleased
 * Size:   40x40 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage icnHomeReleased;

/*********************************
 * Legato Image Asset
 * Name:   mchpBlackLogo
 * Size:   206x48 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage mchpBlackLogo;

/*********************************
 * Legato Image Asset
 * Name:   mgsLogo
 * Size:   240x67 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage mgsLogo;

/*********************************
 * Legato Image Asset
 * Name:   imgButton1
 * Size:   240x67 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage imgButton1;

/*********************************
 * Legato Image Asset
 * Name:   imgButton2
 * Size:   240x67 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage imgButton2;

/*********************************
 * Legato Image Asset
 * Name:   QR
 * Size:   256x256 pixels
 * Type:   RGB Data
 * Format: RGBA_8888
 ***********************************/
extern leImage QR;

/*****************************************************************************
 * Legato Graphics Font Assets
 *****************************************************************************/
/*********************************
 * Legato Font Asset
 * Name:         bigFont
 * Height:       19
 * Baseline:     61
 * Style:        Antialias
 * Glyph Count:  95
 * Range Count:  8
 * Glyph Ranges: 0x20-0x7E
***********************************/
extern leRasterFont bigFont;

/*********************************
 * Legato Font Asset
 * Name:         smallFont
 * Height:       19
 * Baseline:     27
 * Style:        Antialias
 * Glyph Count:  95
 * Range Count:  9
 * Glyph Ranges: 0x20-0x7E
***********************************/
extern leRasterFont smallFont;

/*********************************
 * Legato Font Asset
 * Name:         extraSmallFont
 * Height:       19
 * Baseline:     21
 * Style:        Antialias
 * Glyph Count:  95
 * Range Count:  11
 * Glyph Ranges: 0x20-0x7E
***********************************/
extern leRasterFont extraSmallFont;

/*****************************************************************************
 * Legato String Table
 * Encoding        ASCII
 * Language Count: 1
 * String Count:   6
 *****************************************************************************/

// language IDs
#define language_Default    0

// string IDs
#define stringID_strQuickstart    0
#define stringID_srtHelp    1
#define stringID_strSmart    2
#define stringID_strBack    3
#define stringID_strFast    4
#define stringID_strEasy    5

extern const leStringTable stringTable;


// string list
extern leTableString string_strQuickstart;
extern leTableString string_srtHelp;
extern leTableString string_strSmart;
extern leTableString string_strBack;
extern leTableString string_strFast;
extern leTableString string_strEasy;

void initializeStrings(void);
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* LE_GEN_ASSETS_H */
