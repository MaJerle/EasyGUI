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
#include "gui_dialog.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct DDList_t {
    GUI_LinkedList_t List;                          /*!< Linked list entry element, must always be first on list */
    GUI_ID_t ID;                                    /*!< Dialog ID */
    int Status;                                     /*!< Status on dismissed call */
} DDList_t;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GW(x)             ((GUI_WINDOW_t *)(x))

static
uint8_t GUI_DIALOG_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

static
GUI_LinkedListRoot_t DDList;                        /*!< List of dismissed dialog elements which are not read yet */

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
const static GUI_WIDGET_t Widget = {
    .Name = _T("Dialog"),                           /*!< Widget name */
    .Size = sizeof(GUI_DIALOG_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN | GUI_FLAG_WIDGET_DIALOG_BASE,  /*!< List of widget flags */
    .Callback = GUI_DIALOG_Callback,                /*!< Control function */
    .Colors = NULL,                                 /*!< Pointer to colors array */
    .ColorsCount = 0,                               /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
static
uint8_t GUI_DIALOG_Callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Draw: {
            GUI_Display_t* disp = (GUI_Display_t *)param;
            GUI_DRAW_FilledRectangle(disp,
                __GUI_WIDGET_GetAbsoluteX(h), __GUI_WIDGET_GetAbsoluteY(h),
                __GUI_WIDGET_GetWidth(h), __GUI_WIDGET_GetHeight(h),
                GUI_COLOR_LIGHTCYAN
            );
            return 1;
        }
        default:                                    /* Handle default option */
            __GUI_UNUSED3(h, param, result);        /* Unused elements to prevent compiler warnings */
            return 0;                               /* Command was not processed */
    }
}

/* Check if dialog with specific ID is dismissed */
static
uint8_t __GUI_DIALOG_IsDismissed(GUI_ID_t id) {
    DDList_t* list;
    for (list = __GUI_LINKEDLIST_GETNEXT_GEN(&DDList, 0); list; list = __GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)list)) {
        if (list->ID == id) {                       /* Check if there is entry from dismissed values */
            return 1;
        }
    }
    return 0;
}

/* Get dismiss status from dialog */
static
int __GUI_DIALOG_GetDismissedStatus(GUI_ID_t id) {
    DDList_t* list;
    for (list = __GUI_LINKEDLIST_GETNEXT_GEN(&DDList, 0); list; list = __GUI_LINKEDLIST_GETNEXT_GEN(0, (GUI_LinkedList_t *)list)) {
        if (list->ID == id) {                       /* Check if there is entry from dismissed values */
            break;
        }
    }
    if (list) {                                     /* Entry found */
        int status = list->Status;                  /* Save return status */
        __GUI_LINKEDLIST_REMOVE_GEN(&DDList, (GUI_LinkedList_t *)list); /* Remove entry from linked list */
        __GUI_MEMFREE(list);                        /* Free allocated memory */
        return status;                              /* Return dismissed status */
    }
    return -1;
}    

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
GUI_HANDLE_p GUI_DIALOG_Create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_DIALOG_t* ptr;
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = (GUI_DIALOG_t *)__GUI_WIDGET_Create(&Widget, id, x, y, width, height, parent, cb, flags | GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP); /* Allocate memory for basic widget */

    while (__GUI_DIALOG_IsDismissed(id)) {          /* Clear dismissed status first */
        __GUI_DIALOG_GetDismissedStatus(id);        /* Make dummy read */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return __GH(ptr);
}

int GUI_DIALOG_CreateBlocking(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_HANDLE_p parent, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    int resp = -1;                                  /* Dialog not created error */
    
    __GUI_ENTER();                                  /* Enter GUI */
    
    GUI_HANDLE_p res = GUI_DIALOG_Create(id, x, y, width, height, parent, cb, flags);   /* Create dialog first */
    if (res) {                                      /* Widget created */
        while (__GUI_DIALOG_IsDismissed(id)) {      /* Clear dismissed status first */
            __GUI_DIALOG_GetDismissedStatus(id);    /* Make dummy read */
        }
        
        do {
            GUI_Process();                          /* Process GUI */
        } while (!__GUI_DIALOG_IsDismissed(id));    /* Wait dialog to be dismissed */
        resp = __GUI_DIALOG_GetDismissedStatus(id);     /* Get dismissed status */
    }
    __GUI_LEAVE();                                  /* Leave GUI */
    return resp;
}

uint8_t GUI_DIALOG_Dismiss(GUI_HANDLE_p h, int status) {
    DDList_t* ptr;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    ptr = __GUI_MEMALLOC(sizeof(*ptr));             /* Allocate memory for dismissed status */
    if (ptr) {
        ptr->ID = __GUI_WIDGET_GetId(h);            /* Get widget ID */
        ptr->Status = status;                       /* Save status for dismiss */
        __GUI_LINKEDLIST_ADD_GEN(&DDList, (GUI_LinkedList_t *)ptr); /* Add element to linked list */
        
        __GUI_WIDGET_Callback(h, GUI_WC_OnDismiss, &status, 0); /* Process callback */
        
        __GUI_WIDGET_Remove(h);                     /* Remove widget and all its children from memory */
        
        ret = 1;
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}
