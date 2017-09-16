/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_progbar.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define GUI_PROGBAR_FLAG_PERCENT    0x01            /*!< Flag indicating percentages are enabled on view */

#define __GP(x)             ((GUI_PROGBAR_t *)(x))

#define CFG_VALUE           0x01
#define CFG_MIN             0x02
#define CFG_MAX             0x03
#define CFG_PERCENT         0x04

static
uint8_t GUI_PROGBAR_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_Color_t Colors[] = {
    GUI_COLOR_WIN_MIDDLEGRAY,                       /*!< Default background color */
    GUI_COLOR_WIN_DARKGRAY,                         /*!< Default freground color */
    GUI_COLOR_BLACK,                                /*!< Default border color */
};

static const GUI_WIDGET_t Widget = {
    .Name = _GT("PROGBAR"),                         /*!< Widget name */
    .Size = sizeof(GUI_PROGBAR_t),                  /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_PROGBAR_Callback,               /*!< Callback function */
    .Colors = Colors,                               /*!< List of default colors */
    .ColorsCount = GUI_COUNT_OF(Colors),            /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define p           ((GUI_PROGBAR_t *)h)
static
void __SetValue(GUI_HANDLE_p h, int32_t val) {
    if (p->Value != val && val >= p->Min && val <= p->Max) {    /* Value has changed */
        p->Value = val;                             /* Set value */
        __GUI_WIDGET_Invalidate(h);                 /* Redraw widget */
        __GUI_WIDGET_Callback(h, GUI_WC_ValueChanged, NULL, NULL);  /* Process callback */
    }
}

static
uint8_t GUI_PROGBAR_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_PreInit: {
            __GP(h)->Min = 0;
            __GP(h)->Max = 100;
            __GP(h)->Value = 50;
            return 1;
        }
        case GUI_WC_SetParam: {                     /* Set parameter for widget */
            GUI_WIDGET_Param_t* v = (GUI_WIDGET_Param_t *)param;
            int32_t tmp;
            switch (v->Type) {
                case CFG_VALUE:                     /* Set current progress value */
                    __SetValue(h, *(int32_t *)v->Data);
                    break;
                case CFG_MAX:                       /* Set maximal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp > p->Min) {
                        p->Max = tmp;
                        if (p->Value > p->Max) {
                            __SetValue(h, tmp);
                        }
                    }
                    break;
                case CFG_MIN:                       /* Set minimal value */
                    tmp = *(int32_t *)v->Data;
                    if (tmp < p->Max) {
                        p->Min = tmp;
                        if (p->Value < p->Min) {
                            __SetValue(h, tmp);
                        }
                    }
                    break;
                case CFG_PERCENT:                   /* Set percentage mode */
                    if (*(uint8_t *)v->Data) {
                        p->Flags |= GUI_PROGBAR_FLAG_PERCENT;
                    } else {
                        p->Flags &= ~GUI_PROGBAR_FLAG_PERCENT;
                    }
                    break;
                default: break;
            }
            *(uint8_t *)result = 1;                 /* Save result */
            return 1;
        }
        case GUI_WC_Draw: {
            GUI_Dim_t x, y, w, width, height;
            GUI_Display_t* disp = (GUI_Display_t *)param;
    
            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute position on screen */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute position on screen */
            width = __GUI_WIDGET_GetWidth(h);       /* Get widget width */
            height = __GUI_WIDGET_GetHeight(h);     /* Get widget height */
           
            w = ((width - 4) * (p->Value - p->Min)) / (p->Max - p->Min);   /* Get width for active part */
            
            GUI_DRAW_FilledRectangle(disp, x + w + 2, y + 2, width - w - 4, height - 4, __GUI_WIDGET_GetColor(h, GUI_PROGBAR_COLOR_BG));
            GUI_DRAW_FilledRectangle(disp, x + 2, y + 2, w, height - 4, __GUI_WIDGET_GetColor(h, GUI_PROGBAR_COLOR_FG));
            GUI_DRAW_Rectangle3D(disp, x, y, width, height, GUI_DRAW_3D_State_Lowered);
            
            /* Draw text if possible */
            if (__GH(h)->Font) {
                const GUI_Char* text = NULL;
                GUI_Char buff[5];
                
                if (p->Flags & GUI_PROGBAR_FLAG_PERCENT) {
                    sprintf((char *)buff, "%lu%%", (unsigned long)(((p->Value - p->Min) * 100) / (p->Max - p->Min)));
                    text = buff;
                } else if (__GUI_WIDGET_IsFontAndTextSet(h)) {
                    text = __GUI_WIDGET_GetText(h);
                }
                
                if (text) {                         /* If text is valid, print it */
                    GUI_DRAW_FONT_t f;
                    GUI_DRAW_FONT_Init(&f);         /* Init structure */
                    
                    f.X = x + 2;
                    f.Y = y + 2;
                    f.Width = width - 4;
                    f.Height = height - 4;
                    f.Align = GUI_HALIGN_CENTER | GUI_VALIGN_CENTER;
                    f.Color1Width = w ? w - 1 : 0;
                    f.Color1 = __GUI_WIDGET_GetColor(h, GUI_PROGBAR_COLOR_BG);
                    f.Color2 = __GUI_WIDGET_GetColor(h, GUI_PROGBAR_COLOR_FG);
                    GUI_DRAW_WriteText(disp, __GUI_WIDGET_GetFont(h), text, &f);
                }
            }
        }
#if GUI_USE_TOUCH
        case GUI_WC_TouchStart: {
            *(__GUI_TouchStatus_t *)result = touchHANDLEDNOFOCUS;
            return 1;
        }
#endif /* GUI_USE_TOUCH */
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_PROGBAR_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    return (GUI_HANDLE_p)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
}

uint8_t GUI_PROGBAR_SetColor(GUI_HANDLE_p h, GUI_PROGBAR_COLOR_t index, GUI_Color_t color) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetColor(h, (uint8_t)index, color); /* Set color */
}

uint8_t GUI_PROGBAR_SetValue(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_VALUE, &val, 1, 0); /* Set parameter */
}

uint8_t GUI_PROGBAR_SetMin(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_MIN, &val, 1, 0);   /* Set parameter */
}

uint8_t GUI_PROGBAR_SetMax(GUI_HANDLE_p h, int32_t val) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_MAX, &val, 1, 0);   /* Set parameter */
}

uint8_t GUI_PROGBAR_SetPercentMode(GUI_HANDLE_p h, uint8_t enable) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    return __GUI_WIDGET_SetParam(h, CFG_PERCENT, &enable, 1, 0);    /* Set parameter */
}

int32_t GUI_PROGBAR_GetMin(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Min;                             /* Get minimal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

int32_t GUI_PROGBAR_GetMax(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Max;                             /* Get maximal value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}

int32_t GUI_PROGBAR_GetValue(GUI_HANDLE_p h) {
    int32_t val;
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */

    val = __GP(h)->Value;                           /* Get current value */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return val;
}
