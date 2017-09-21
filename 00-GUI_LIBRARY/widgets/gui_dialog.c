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
    GUI_LinkedList_t list;                          /*!< Linked list entry element, must always be first on list */
    GUI_ID_t id;                                    /*!< Dialog ID */
    GUI_HANDLE_p h;                                 /*!< Pointer to dialog address */
    volatile int status;                            /*!< Status on dismissed call */
#if GUI_OS
    gui_sys_sem_t sem;                              /*!< Semaphore handle for blocking */
    uint8_t ib;                                     /*!< Indication if dialog is blocking */
#endif /* GUI_OS */
} DDList_t;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define __GW(x)             ((GUI_WINDOW_t *)(x))

static uint8_t gui_dialog_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result);

static
GUI_LinkedListRoot_t DDList;                        /*!< List of dismissed dialog elements which are not read yet */

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
static const GUI_WIDGET_t Widget = {
    .Name = _GT("DIALOG"),                          /*!< Widget name */
    .Size = sizeof(GUI_DIALOG_t),                   /*!< Size of widget for memory allocation */
    .Flags = GUI_FLAG_WIDGET_ALLOW_CHILDREN | GUI_FLAG_WIDGET_DIALOG_BASE,  /*!< List of widget flags */
    .Callback = gui_dialog_callback,                /*!< Control function */
    .Colors = NULL,                                 /*!< Pointer to colors array */
    .ColorsCount = 0,                               /*!< Number of colors */
};

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/* Add widget to active dialogs (not yet dismissed) */
static DDList_t*
add_to_active_dialogs(GUI_HANDLE_p h) {
    DDList_t* l;
    
    l = __GUI_MEMALLOC(sizeof(*l));                 /* Allocate memory for dismissed dialog list */
    if (l) {
        l->h = h;
        l->id = gui_widget_getid__(h);
        gui_linkedlist_add_gen(&DDList, &l->list);  /* Add entry to linked list */
    }
    return l;
}

/* Remove and free memory from linked list */
static void
remove_from_active_dialogs(DDList_t* l) {
    gui_linkedlist_remove_gen(&DDList, &l->list);   /* Remove entry from linked list first */
    
    __GUI_MEMFREE(l);                               /* Free memory */
}

/* Get entry from linked list for specific dialog */
static DDList_t*
get_dialog(GUI_HANDLE_p h) {
    DDList_t* l = NULL;
    GUI_ID_t id;
    
    id = gui_widget_getid__(h);                     /* Get id of widget */
    
    for (l = (DDList_t *)gui_linkedlist_getnext_gen((GUI_LinkedListRoot_t *)&DDList, NULL); l;
        l = (DDList_t *)gui_linkedlist_getnext_gen(NULL, (GUI_LinkedList_t *)l)) {
        if (l->h == h && l->id == id) {             /* Check match for handle and id */
            break;
        }
    }
    return l;
}

/* Default dialog callback */
static uint8_t
gui_dialog_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, void* param, void* result) {
    __GUI_ASSERTPARAMS(h && __GH(h)->Widget == &Widget);    /* Check input parameters */
    switch (ctrl) {                                 /* Handle control function if required */
        case GUI_WC_Remove: {
            return 1;
        }
        case GUI_WC_PreInit: {
            return 1;
        }
        case GUI_WC_OnDismiss: {
            int dv = *(int *)param;                 /* Get dismiss parameter value */
            __GUI_UNUSED(dv);                       /* Unused parameters */
            return 1;
        }
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
GUI_HANDLE_p
gui_dialog_create(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_WIDGET_CreateFunc_t func, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_HANDLE_p ptr;
    if (!func) {                                    /* Check create function */
        return NULL;
    }
    
    ptr = func(id, x, y, width, height, NULL, cb, flags | GUI_FLAG_WIDGET_CREATE_PARENT_DESKTOP);   /* Create desired widget */
    if (ptr) {
        __GUI_ENTER();                              /* Enter GUI */
        gui_widget_setflag__(ptr, GUI_FLAG_WIDGET_DIALOG_BASE); /* Add dialog base flag to widget */
        gui_linkedlist_widgetmovetobottom(ptr);     /* Move to bottom on linked list make it on top now with flag set as dialog */
        add_to_active_dialogs(ptr);                 /* Add this dialog to active dialogs */
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return (GUI_HANDLE_p)ptr;
}

#if GUI_OS
int
gui_dialog_createblocking(GUI_ID_t id, GUI_iDim_t x, GUI_iDim_t y, GUI_Dim_t width, GUI_Dim_t height, GUI_WIDGET_CreateFunc_t func, GUI_WIDGET_CALLBACK_t cb, uint16_t flags) {
    GUI_HANDLE_p ptr;
    int resp = -1;                                  /* Dialog not created error */
    
    ptr = gui_dialog_create(id, x, y, width, height, func, cb, flags);  /* Create dialog first */
    if (ptr) {                                      /* Widget created */
        DDList_t* l;
        
        __GUI_ENTER();                              /* Enter GUI */
        l = get_dialog(ptr);                        /* Get entry from active dialogs */
        if (l) {                                    /* Check if successfully added widget to active dialogs */
            l->ib = 1;                              /* Blocking entry */
            if (gui_sys_sem_create(&l->sem, 0)) {   /* Create semaphore and lock it immediatelly */
                __GUI_SYS_UNPROTECT();              /* Disable protection while waiting for semaphore */
                gui_sys_sem_wait(&l->sem, 0);       /* Wait for semaphore again, should be released from dismiss function */
                __GUI_SYS_PROTECT();                /* Protect back before continuing */
                gui_sys_sem_delete(&l->sem);        /* Delete system semaphore */
                resp = l->status;                   /* Get new status */
                remove_from_active_dialogs(l);      /* Remove from active dialogs */
            } else {
                gui_widget_remove__(ptr);           /* Remove widget */
            }
        } else {
            gui_widget_remove__(ptr);               /* Remove widget */
        }
        __GUI_LEAVE();                              /* Leave GUI */
    }
    
    return resp;
}
#endif /* GUI_OS */

uint8_t
gui_dialog_dismiss(GUI_HANDLE_p h, int status) {
    DDList_t* l;
    uint8_t ret = 0;
    
    /* Do not check widget type as it is not dialog type but create function widget type */
    __GUI_ASSERTPARAMS(h);                          /* Check input parameters */
    __GUI_ENTER();                                  /* Enter GUI */
    
    l = get_dialog(h);                              /* Get entry from list */
    if (l) {
        l->status = status;                         /* Save status for later */
        gui_widget_callback__(h, GUI_WC_OnDismiss, (int *)&l->status, 0);   /* Process callback */
#if GUI_OS
        if (l->ib && gui_sys_sem_isvalid(&l->sem)) {/* Check if semaphore is valid */
            gui_sys_sem_release(&l->sem);           /* Release locked semaphore */
        } else 
#endif /* GUI_OS */
        {
            remove_from_active_dialogs(l);          /* Remove from active dialogs */
        }
        gui_widget_remove__(h);                     /* Remove widget */
    }
    
    __GUI_LEAVE();                                  /* Leave GUI */
    return ret;
}
