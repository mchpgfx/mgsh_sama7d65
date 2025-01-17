#ifndef LE_GEN_SCREEN_HELP_H
#define LE_GEN_SCREEN_HELP_H

#include "gfx/legato/legato.h"

#include "gfx/legato/generated/le_gen_scheme.h"
#include "gfx/legato/generated/le_gen_assets.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
extern "C" {
#endif
// DOM-IGNORE-END

// screen member widget declarations
extern leWidget* Help_BackgroundPanel;
extern leImageWidget* Help_ImageWidgetMCHPLogo;
extern leLabelWidget* Help_LabelWidgetHelp;
extern leImageWidget* Help_ImageWidgetQRCode;
extern leImageWidget* Help_ImageWidgetMGSLogo;
extern leButtonWidget* Help_ButtonWidgetGoHome;

// event handlers
// !!THESE MUST BE IMPLEMENTED IN THE APPLICATION CODE!!
void event_Help_ButtonWidgetGoHome_OnReleased(leButtonWidget* btn);

// screen lifecycle functions
// DO NOT CALL THESE DIRECTLY
leResult screenInit_Help(void); // called when Legato is initialized
leResult screenShow_Help(void); // called when screen is shown
void screenHide_Help(void); // called when screen is hidden
void screenDestroy_Help(void); // called when Legato is destroyed
void screenUpdate_Help(void); // called when Legato is updating

leWidget* screenGetRoot_Help(uint32_t lyrIdx); // gets a root widget for this screen

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // LE_GEN_SCREEN_HELP_H
