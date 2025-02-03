#include "gfx/legato/generated/screen/le_gen_screen_Help.h"

// screen member widget declarations
static leWidget* root0;

leWidget* Help_BackgroundPanel;
leImageWidget* Help_ImageWidgetMCHPLogo;
leLabelWidget* Help_LabelWidgetHelp;
leImageWidget* Help_ImageWidgetQRCode;
leImageWidget* Help_ImageWidgetMGSLogo;
leButtonWidget* Help_ButtonWidgetGoHome;

static leBool initialized = LE_FALSE;
static leBool showing = LE_FALSE;

leResult screenInit_Help(void)
{
    if(initialized == LE_TRUE)
        return LE_FAILURE;

    initialized = LE_TRUE;

    return LE_SUCCESS;
}

leResult screenShow_Help(void)
{
    if(showing == LE_TRUE)
        return LE_FAILURE;

    // layer 0
    root0 = leWidget_New();
    root0->fn->setSize(root0, LE_DEFAULT_SCREEN_WIDTH, LE_DEFAULT_SCREEN_HEIGHT);
    root0->fn->setBackgroundType(root0, LE_WIDGET_BACKGROUND_NONE);
    root0->fn->setMargins(root0, 0, 0, 0, 0);
    root0->flags |= LE_WIDGET_IGNOREEVENTS;
    root0->flags |= LE_WIDGET_IGNOREPICK;

    Help_BackgroundPanel = leWidget_New();
    Help_BackgroundPanel->fn->setPosition(Help_BackgroundPanel, 0, 0);
    Help_BackgroundPanel->fn->setSize(Help_BackgroundPanel, 1280, 800);
    Help_BackgroundPanel->fn->setScheme(Help_BackgroundPanel, &WhiteScheme);
    root0->fn->addChild(root0, (leWidget*)Help_BackgroundPanel);

    Help_ImageWidgetMCHPLogo = leImageWidget_New();
    Help_ImageWidgetMCHPLogo->fn->setPosition(Help_ImageWidgetMCHPLogo, 1058, 16);
    Help_ImageWidgetMCHPLogo->fn->setSize(Help_ImageWidgetMCHPLogo, 206, 48);
    Help_ImageWidgetMCHPLogo->fn->setBackgroundType(Help_ImageWidgetMCHPLogo, LE_WIDGET_BACKGROUND_NONE);
    Help_ImageWidgetMCHPLogo->fn->setBorderType(Help_ImageWidgetMCHPLogo, LE_WIDGET_BORDER_NONE);
    Help_ImageWidgetMCHPLogo->fn->setImage(Help_ImageWidgetMCHPLogo, (leImage*)&mchpBlackLogo);
    root0->fn->addChild(root0, (leWidget*)Help_ImageWidgetMCHPLogo);

    Help_LabelWidgetHelp = leLabelWidget_New();
    Help_LabelWidgetHelp->fn->setPosition(Help_LabelWidgetHelp, 360, 100);
    Help_LabelWidgetHelp->fn->setSize(Help_LabelWidgetHelp, 560, 50);
    Help_LabelWidgetHelp->fn->setBackgroundType(Help_LabelWidgetHelp, LE_WIDGET_BACKGROUND_NONE);
    Help_LabelWidgetHelp->fn->setString(Help_LabelWidgetHelp, (leString*)&string_srtHelp);
    root0->fn->addChild(root0, (leWidget*)Help_LabelWidgetHelp);

    Help_ImageWidgetQRCode = leImageWidget_New();
    Help_ImageWidgetQRCode->fn->setPosition(Help_ImageWidgetQRCode, 512, 272);
    Help_ImageWidgetQRCode->fn->setSize(Help_ImageWidgetQRCode, 256, 256);
    Help_ImageWidgetQRCode->fn->setBackgroundType(Help_ImageWidgetQRCode, LE_WIDGET_BACKGROUND_NONE);
    Help_ImageWidgetQRCode->fn->setBorderType(Help_ImageWidgetQRCode, LE_WIDGET_BORDER_NONE);
    Help_ImageWidgetQRCode->fn->setImage(Help_ImageWidgetQRCode, (leImage*)&QR);
    root0->fn->addChild(root0, (leWidget*)Help_ImageWidgetQRCode);

    Help_ImageWidgetMGSLogo = leImageWidget_New();
    Help_ImageWidgetMGSLogo->fn->setPosition(Help_ImageWidgetMGSLogo, 16, 717);
    Help_ImageWidgetMGSLogo->fn->setSize(Help_ImageWidgetMGSLogo, 240, 67);
    Help_ImageWidgetMGSLogo->fn->setBackgroundType(Help_ImageWidgetMGSLogo, LE_WIDGET_BACKGROUND_NONE);
    Help_ImageWidgetMGSLogo->fn->setBorderType(Help_ImageWidgetMGSLogo, LE_WIDGET_BORDER_NONE);
    Help_ImageWidgetMGSLogo->fn->setImage(Help_ImageWidgetMGSLogo, (leImage*)&mgsLogo);
    root0->fn->addChild(root0, (leWidget*)Help_ImageWidgetMGSLogo);

    Help_ButtonWidgetGoHome = leButtonWidget_New();
    Help_ButtonWidgetGoHome->fn->setPosition(Help_ButtonWidgetGoHome, 1024, 717);
    Help_ButtonWidgetGoHome->fn->setSize(Help_ButtonWidgetGoHome, 240, 67);
    Help_ButtonWidgetGoHome->fn->setBackgroundType(Help_ButtonWidgetGoHome, LE_WIDGET_BACKGROUND_NONE);
    Help_ButtonWidgetGoHome->fn->setBorderType(Help_ButtonWidgetGoHome, LE_WIDGET_BORDER_NONE);
    Help_ButtonWidgetGoHome->fn->setString(Help_ButtonWidgetGoHome, (leString*)&string_strBack);
    Help_ButtonWidgetGoHome->fn->setPressedImage(Help_ButtonWidgetGoHome, (leImage*)&imgButton1);
    Help_ButtonWidgetGoHome->fn->setReleasedImage(Help_ButtonWidgetGoHome, (leImage*)&imgButton2);
    Help_ButtonWidgetGoHome->fn->setImagePosition(Help_ButtonWidgetGoHome, LE_RELATIVE_POSITION_BEHIND);
    Help_ButtonWidgetGoHome->fn->setPressedOffset(Help_ButtonWidgetGoHome, 0);
    Help_ButtonWidgetGoHome->fn->setReleasedEventCallback(Help_ButtonWidgetGoHome, event_Help_ButtonWidgetGoHome_OnReleased);
    root0->fn->addChild(root0, (leWidget*)Help_ButtonWidgetGoHome);

    leAddRootWidget(root0, 0);
    leSetLayerColorMode(0, LE_COLOR_MODE_RGB_565);

    showing = LE_TRUE;

    return LE_SUCCESS;
}

void screenUpdate_Help(void)
{
    root0->fn->setSize(root0, root0->rect.width, root0->rect.height);
}

void screenHide_Help(void)
{

    leRemoveRootWidget(root0, 0);
    leWidget_Delete(root0);
    root0 = NULL;

    Help_BackgroundPanel = NULL;
    Help_ImageWidgetMCHPLogo = NULL;
    Help_LabelWidgetHelp = NULL;
    Help_ImageWidgetQRCode = NULL;
    Help_ImageWidgetMGSLogo = NULL;
    Help_ButtonWidgetGoHome = NULL;


    showing = LE_FALSE;
}

void screenDestroy_Help(void)
{
    if(initialized == LE_FALSE)
        return;

    initialized = LE_FALSE;
}

leWidget* screenGetRoot_Help(uint32_t lyrIdx)
{
    if(lyrIdx >= LE_LAYER_COUNT)
        return NULL;

    switch(lyrIdx)
    {
        case 0:
        {
            return root0;
        }
        default:
        {
            return NULL;
        }
    }
}

