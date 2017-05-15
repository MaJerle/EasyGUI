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
#include "gui_image.h"

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
#define __GI(x)             ((GUI_IMAGE_t *)(x))

static
uint8_t GUI_IMAGE_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("IMAGE"),                            /*!< Widget name */
    .Size = sizeof(GUI_IMAGE_t),                    /*!< Size of widget for memory allocation */
    .Flags = 0,                                     /*!< List of widget flags */
    .Callback = GUI_IMAGE_Callback,                 /*!< Callback function */
    .Colors = 0,                                    /*!< List of default colors */
    .ColorsCount = 0,                               /*!< Define number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
#define o       ((GUI_IMAGE_t *)(h))
static
uint8_t GUI_IMAGE_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_Dim_t x, y;

            x = __GUI_WIDGET_GetAbsoluteX(h);       /* Get absolute X coordinate */
            y = __GUI_WIDGET_GetAbsoluteY(h);       /* Get absolute Y coordinate */
            
            GUI_DRAW_Image(disp, x, y, o->Image);
            return 1;
        }
        case GUI_WC_FocusIn: {
            __GUI_WIDGET_InvalidateWithParent(h);   /* Invalidate parent too because of possible transparent sections */
            return 1;
        }
        case GUI_WC_FocusOut: {
            __GUI_WIDGET_InvalidateWithParent(h);   /* Invalidate parent too because of possible transparent sections */
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}
#undef o

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_IMAGE_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_IMAGE_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_IMAGE_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags);  /* Allocate memory for basic widget */
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

uint8_t GUI_IMAGE_SetSource(GUI_HANDLE_p h, const GUI_IMAGE_DESC_t* img) {
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget && img); /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    __GI(h)->Image = img;                           /* Set image */
    __GUI_WIDGET_Invalidate(h);                     /* Invalidate widget */

    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}
