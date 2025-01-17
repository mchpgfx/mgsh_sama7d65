/*******************************************************************************
  Application for Screen - Help

  Company:
    Microchip Technology Inc.

  File Name:
    app_screen_help.c

  Summary:
    This file defines all the events on the screen 'Help' in MGS Composer.

  Description:
    Events are typically called by the graphics library in response to user 
    input on a screen. They can be enabled by selecting a widget that supports   
    it and selecting check-boxes in the 'Events' section of the Object Editor.
    Events enabled in composer are declared automatically, 
    but must be defined by the user as shown below. 
    Here, we simply switch the screen in response to a button click.
 *******************************************************************************/

#include "gfx/legato/generated/le_gen_init.h"

void event_Help_ButtonWidgetGoHome_OnReleased(leButtonWidget* btn)
{
    legato_showScreen(screenID_Home);
}
