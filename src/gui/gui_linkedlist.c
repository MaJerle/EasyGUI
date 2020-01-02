/**	
 * \file            gui_linkedlist.c
 * \brief           Linked list manager
 */
 
/*
 * Copyright (c) 2020 Tilen MAJERLE
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of EasyGUI library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         $_version_$
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_linkedlist.h"
#include "widget/gui_widget.h"

/**
 * \brief           Print linked list from root element
 * \param[in]       root: Root linked list element
 */
static void
print_list(gui_handle_p root) {
    static uint8_t depth = 0;
    gui_handle_p h;
    gui_linkedlistroot_t* list;
    
    depth++;
    if (root != NULL) {
        list = &root->root_list;
    } else {
        list = &GUI.root;
    }
    for (h = list->first; h != NULL; h = h->list.next) {
        GUI_DEBUG("%*d: W: %s; A: 0x%p, Hidden: %lu; Redraw: %lu; Remove: %lu\r\n",
            2 * depth, depth,
            (const char *)h->widget->name,
            h,
            (unsigned long)!!(h->flags & GUI_FLAG_HIDDEN),
            (unsigned long)!!(h->flags & GUI_FLAG_REDRAW),
            (unsigned long)!!(h->flags & GUI_FLAG_REMOVE)
        );
        if (guii_widget_haschildren(h)) {
            print_list(h);
        }
    }
    depth--;
}

/**
 * \brief           Add element to doubly linked list
 * \note            Element added to linkedlist must have \ref gui_linkedlist_t as top structure
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to add to base linked list 
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
void
gui_linkedlist_add_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    element->prev = root->last;                 /* Previous element of new entry is currently last element */
    element->next = NULL;                       /* There is no next element */
    if (root->first == NULL || root->last == NULL) {/* First widget is about to be created */
        root->first = element;                  /* Set as first widget */
    } else {
        ((gui_linkedlist_t *)root->last)->next = element;   /* Previous's element next element is current element */
    }
    root->last = element;                       /* Add new element as last */
}
    
/**
 * \brief           Remove element from doubly linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to remove from base linked list
 * \return          Removed element handle
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
gui_linkedlist_t *
gui_linkedlist_remove_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    gui_linkedlist_t *prev, *next;
    
    if (element == NULL) {
        return 0;
    }

    prev = element->prev;                           /* Get previous element of current */
    next = element->next;                           /* Get next element of current */
    
    if (prev != NULL) {                             /* If current element has previous elemnet */
        prev->next = next;                          /* Set new next element to previous element */
    }
    if (next != NULL) {                             /* If current element has next element */
        next->prev = prev;                          /* Set new previous element to next element */
    }
    if (root->first == element) {                   /* If current is the same as first */
        root->first = next;                         /* Set next element as first */
    }
    if (root->last == element) {                    /* If current is the same as last */
        root->last = prev;                          /* Set previous as last element */
    }
    
    element->prev = NULL;                           /*!< Clear element pointer */
    element->next = NULL;                           /*!< Clear element pointer */
    return element;
}

/**
 * \brief           Get next element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Next element of <b>element</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>First element in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to get next element of
 * \return          Next element handle on success, NULL otherwise
 * \sa              gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
gui_linkedlist_t *
gui_linkedlist_getnext_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    if (element == NULL) { 
        if (root != NULL) {
            return root->first;
        } else {
            return NULL;
        }
    }
    return element->next;                           /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Previous element of <b>element</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>Last element in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to get next element of
 * \return          Previous element handle on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
gui_linkedlist_t *
gui_linkedlist_getprev_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    if (element == NULL) { 
        if (root != NULL) {
            return GUI_VP(root->last);
        } else {
            return NULL;
        }
    }
    return element->prev;                           /* Get next widget of current in linked list */
}

/**
 * \brief           Move widget down for one on doubly linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to move down on base linked list
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_movedown_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    gui_linkedlist_t *nextnext = NULL, *prev = NULL, *next = NULL;
    
    if (element == NULL || element == root->last) { /* Check if move is available */
        return 0;                                   /* Could not move */
    }
    
    prev = element->prev;                           /* Get previous element */
    next = element->next;                           /* Get next element */
    if (next != NULL) {                             /* Check if next is available */
        nextnext = next->next;                      /* Get next element of next element */
    }
    
    if (nextnext != NULL) {                         /* If there is available memory */
        nextnext->prev = element;                   /* Previous element of next next element is current element */
    } else {
        root->last = element;                       /* There is no next next element so we will be last element after move */
    }
    
    if (next != NULL) {                             /* If there is next element */
        next->next = element;                       /* Next element will become previous and set next element as current */
        next->prev = prev;                          /* Set previous element to next element as current previous */
    }
    
    element->next = nextnext;                       /* Set next next element to current next element */
    element->prev = next;                           /* Set next element as previous (swap current and next elements) */
    
    if (prev != NULL) {                             /* Check if next exists */
        prev->next = next;                          /* Set previous element to next */
    }
    
    if (root->first == element) {                   /* Check for current element */
        root->first = next;                         /* Set new element as first in linked list */
    }
    
    return 1;                                       /* Move was successful */
}

/**
 * \brief           Move widget up for one on doubly linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to move up on base linked list
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_moveup_gen(gui_linkedlistroot_t* const root, gui_linkedlist_t* const element) {
    gui_linkedlist_t *prevprev = NULL, *prev = NULL, *next = NULL;
    
    if (element == NULL || element == root->first) {/* Check if move is available */
        return 0;                                   /* Could not move */
    }
    
    prev = element->prev;                           /* Get previous element */
    next = element->next;                           /* Get next element */
    if (prev != NULL) {                             /* Check if previous is available */
        prevprev = prev->prev;                      /* Get previous element of previous element */
    }
    
    if (prevprev != NULL) {                         /* If there is available memory */
        prevprev->next = element;                   /* Next element of previous previous element is current element */
    } else {
        root->first = element;                      /* There is no previous previous element so we will be first element after move */
    }
    
    if (prev != NULL) {                             /* If there is previous element */
        prev->prev = element;                       /* Previous element will become next and set previous element as current */
        prev->next = next;                          /* Set next element to previous element as current previous */
    }
    
    element->prev = prevprev;                       /* Set previous previous element to current previous element */
    element->next = prev;                           /* Set previous element as next (swap current and previous elements) */
    
    if (next != NULL) {                             /* Check if previous exists */
        next->prev = prev;                          /* Set next element to previous */
    }
    
    if (root->last == element) {                    /* Check for current element */
        root->last = prev;                          /* Set new last as first in linked list */
    }
    
    return 1;                                       /* Move was successful */
}

/**
 * \brief           Get item from linked list by index
 * \note            Index value is the number of item in list order
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       index: Number in list to get item
 * \return          Item handle on success, `NULL` otherwise
 */
gui_linkedlist_t *
gui_linkedlist_getnext_byindex_gen(gui_linkedlistroot_t* const root, uint16_t index) {
    gui_linkedlist_t* item = gui_linkedlist_getnext_gen(root, NULL);    /* Get first element */
    while (index-- && item != NULL) {               /* Scroll to next elements */
        item = gui_linkedlist_getnext_gen(NULL, (gui_linkedlist_t *)item);
    }
    return item;                                    /* Get that item */
}

/**
 * \brief           Add element to multi linked list
 * \note            Element can be any type since \ref gui_linkedlistmulti_t structure is dynamicall allocated
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to add to base linked list
 * \return          Multi linked list handle on success, NULL otherwise
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
gui_linkedlistmulti_t *
gui_linkedlist_multi_add_gen(gui_linkedlistroot_t* const root, void* const element) {
    gui_linkedlistmulti_t* ptr;
    
    ptr = GUI_MEMALLOC(sizeof(gui_linkedlistmulti_t));  /* Create memory for linked list */
    if (ptr != NULL) {
        ptr->element = element;                     /* Save pointer to our element */
        gui_linkedlist_add_gen(root, &ptr->list);   /* Add element to linked list */
    }
    
    return ptr;
}
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlist_t element to remove from base linked list
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen
 */
uint8_t
gui_linkedlist_multi_remove_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* element) {
    if (element == NULL) {                          /* Check valid element */
        return 0;
    }
    gui_linkedlist_remove_gen(root, (gui_linkedlist_t *)element); /* Remove element from linked list */
    GUI_MEMFREE(element);
    return 1;
}

/**
 * \brief           Get next element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getnext_gen(root, element)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Next element of <b>element</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>First element in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlistmulti_t element to get next element of
 * \return          Next element on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getprev_gen
 */
gui_linkedlistmulti_t *
gui_linkedlist_multi_getnext_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element) {
    if (element == NULL) { 
        if (root != NULL) {
            return GUI_VP(root->first);
        } else {
            return NULL;
        }
    }
    return GUI_VP(element->list.next);          /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getprev_gen(root, element)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Previous element of <b>element</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>Last element in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlistmulti_t element to get next element of
 * \return          Previous element on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen
 */
gui_linkedlistmulti_t *
gui_linkedlist_multi_getprev_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element) {
    if (element == NULL) { 
        if (root != NULL) {
            return GUI_VP(root->last);
        } else {
            return NULL;
        }
    }
    return GUI_VP(element->list.prev);                  /* Get next widget of current in linked list */
}

/**
 * \brief           Move widget down for one on multi linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlistmulti_t element to move down on base linked list
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen
 */
uint8_t
gui_linkedlist_multi_movedown_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element) {
    return gui_linkedlist_movedown_gen(root, &element->list); /* Move down element */
}

/**
 * \brief           Move widget up for one on multi linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Pointer to \ref gui_linkedlistmulti_t element to move up on base linked list
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_multi_moveup_gen(gui_linkedlistroot_t* const root, gui_linkedlistmulti_t* const element) {
    return gui_linkedlist_moveup_gen(root, &element->list);   /* Move down element */
}
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Delete by element pointer, not by list item. It searches in all entries for element pointers and deletes all links with that element pointer
 *
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       root: Pointer to \ref gui_linkedlistroot_t structure as base element
 * \param[in]       element: Memory address in data part of linked list entry
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
uint8_t
gui_linkedlist_multi_find_remove(gui_linkedlistroot_t* const root, void* const element) {
    gui_linkedlistmulti_t* link;
    uint8_t ret = 0;
    
    GUI_ASSERTPARAMS(root);
    
    for (link = gui_linkedlist_multi_getnext_gen(root, NULL); link != NULL;
        link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
        if (GUI_VP(gui_linkedlist_multi_getdata(link)) == element) {/* Check match */
            gui_linkedlist_multi_remove_gen(root, link);  /* Remove element from linked list */
            ret = 1;
        }
    }
    return ret;
}

/**
 * \brief           Add new widget to linked list of parent widget
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       parent: Pointer to parent widget where to put this widget on list
 * \param[in]       h: Widget handle to add to list 
 * \sa              gui_linkedlist_widgetremove
 */
void
gui_linkedlist_widgetadd(gui_handle_p parent, gui_handle_p h) {    
    if (parent != NULL) {
        gui_linkedlist_add_gen(&parent->root_list, GUI_VP(h));
    } else {
        gui_linkedlist_add_gen(&GUI.root, GUI_VP(h));
    }
    gui_linkedlist_widgetmovetotop(h);              /* Reset by moving to top */
    gui_linkedlist_widgetmovetobottom(h);           /* Reset by moving to bottom with reorder */
}

/**
 * \brief           Remove widget from linked list of parent widget
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle to remove from linked list 
 * \sa              gui_linkedlist_widgetadd
 */
void
gui_linkedlist_widgetremove(gui_handle_p h) {    
    if (guii_widget_hasparent(h)) {
        gui_linkedlist_remove_gen(&(guii_widget_getparent(h)->root_list), GUI_VP(h));
    } else {
        gui_linkedlist_remove_gen(&GUI.root, GUI_VP(h));
    }
}

/**
 * \brief           Move widget up for one in linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle to move up
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_widgetmovedown
 */
uint8_t
gui_linkedlist_widgetmoveup(gui_handle_p h) {
    if (guii_widget_hasparent(h)) {
        return gui_linkedlist_moveup_gen(&(guii_widget_getparent(h)->root_list), GUI_VP(h));
    }
    return gui_linkedlist_moveup_gen(&GUI.root, GUI_VP(h));
}

/**
 * \brief           Move widget down for one in linked list
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget handle to move down
 * \return          `1` on success, `0` otherwise
 * \sa              gui_linkedlist_widgetmoveup
 */
uint8_t
gui_linkedlist_widgetmovedown(gui_handle_p h) {
    if (guii_widget_hasparent(h)) {
        return gui_linkedlist_movedown_gen(&(guii_widget_getparent(h)->root_list), GUI_VP(h));
    }
    return gui_linkedlist_movedown_gen(&GUI.root, GUI_VP(h));
}

/**
 * \brief           Get next widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h       <th>gui_linkedlist_widgetgetnext(parent, h)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Next widget of <b>h</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>First widget in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Next widget of <b>h</b> if exists, first widget of <b>parent</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 *
 * \include         _example_linkedlist_widgetgetnext.c 
 *
 * \param[in]       parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \return          Previous widget on linked list on success, NULL otherwise
 * \sa              gui_linkedlist_widgetgetprev
 */
gui_handle_p
gui_linkedlist_widgetgetnext(gui_handle_p parent, gui_handle_p h) {
    if (h == NULL) {                                /* Get first widget on list */
        if (parent != NULL) {                       /* If parent exists... */
            return parent->root_list.first;         /* ...get first widget on parent */
        } else {
            return GUI.root.first;                  /* Get first widget in GUI */
        }
    }
    return h->list.next;                            /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h       <th>gui_linkedlist_widgetgetprev(parent, h)
 *                      <tr><td>NULL    <td>NULL    <td>NULL
 *                      <tr><td>NULL    <td>!= NULL <td>Previous widget of <b>h</b> if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>NULL    <td>Last widget in <b>parent</b> list if exists, <b>NULL</b> otherwise
 *                      <tr><td>!= NULL <td>!= NULL <td>Previous widget of <b>h</b> if exists, last widget of <b>parent</b> if exists, <b>NULL</b> otherwise
 *                  </table>
 *
 * \note            This function is private and may be called only when OS protection is active
 *
 * \include         _example_linkedlist_widgetgetprev.c 
 *
 * \param[in]       parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \return          Next widget on linked list on success, NULL otherwise
 * \sa              gui_linkedlist_widgetgetnext
 */
gui_handle_p
gui_linkedlist_widgetgetprev(gui_handle_p parent, gui_handle_p h) {
    if (h == NULL) {                                /* Get last widget on list */
        if (parent != NULL) {                       /* If parent exists... */
            return parent->root_list.last;          /* ...get last widget on parent */
        } else {
            return GUI.root.last;                   /* Get last widget in GUI */
        }
    }
    return h->list.prev;                            /* Get next widget of current in linked list */
}

/*
 * Widget linked list order:
 *
 * 1. Normal widgets, with automatic or fixed z-index
 * 2. Widgets with children support
 * 3. Widgets as dialog base elements
 */

/**
 * \brief           Move widget to bottom in linked list of parent widget
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget to move to bottom
 * \return          Number of moves on linked list, `0` if move not available
 * \sa              gui_linkedlist_widgetmovetotop
 */
uint8_t
gui_linkedlist_widgetmovetobottom(gui_handle_p h) {
    uint32_t cnt = 0;
    if (h->list.next == NULL) {
        return 0;
    }
    while (h->list.next != NULL) {                  /* While device has next element */
        if (guii_widget_isdialogbase(h)) {          /* Widget is dialog base element */
            if (!gui_linkedlist_widgetmovedown(h)) {/* Move down */
                break;
            }
            cnt++;
        } else if (guii_widget_allowchildren(h)) { /* Widget supports children widgets, go to the end of the list if necessary */
            if (!guii_widget_isdialogbase(h->list.next)) { /* Go down till dialog base is reached */
                if (h->zindex >= __GH(h->list.next)->zindex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmovedown(h)) {/* Move down */
                        break;
                    }
                    cnt++;
                } else {
                    break;
                }
            } else {
                break;
            }
        } else {                                    /* Our widget does not allow sub widgets */
            if (!guii_widget_allowchildren(h->list.next)) {  /* Allow moving down only if next widget does not allow sub widgets */
                if (h->zindex >= __GH(h->list.next)->zindex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmovedown(h)) {/* Move down */
                        break;
                    }
                    cnt++;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    return cnt;
}

/**
 * \brief           Move widget to top in linked list of parent widget
 * \note            This function is private and may be called only when OS protection is active
 * \param[in]       h: Widget to move to top
 * \return          Number of moves on linked list, `0` if move not available
 * \sa              gui_linkedlist_widgetmovetobottom
 */
uint8_t
gui_linkedlist_widgetmovetotop(gui_handle_p h) {
    uint32_t cnt = 0;
    if (h->list.prev == NULL) {
        return 0;
    }
    while (h->list.prev != NULL) {                  /* While device has previous element */
        if (guii_widget_isdialogbase(h)) {          /* Widget is dialog base element */
            if (guii_widget_isdialogbase(h->list.prev)) {   /* If previous widget is dialog base too */
                if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up widget */
                    break;
                }
                cnt++;
            } else {
                break;                              /* Stop execution */
            }
        } else if (guii_widget_allowchildren(h)) {  /* If moving widget allows children elements */
            if (guii_widget_allowchildren(h->list.prev)) {  /* If previous widget allows children too */
                if (h->zindex <= __GH(h->list.prev)->zindex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up */
                        break;
                    }
                    cnt++;
                } else {
                    break;
                }
            } else {
                break;                              /* Stop execution */
            }
        } else {                                    /* Normal widget */
            if (h->zindex <= __GH(h->list.prev)->zindex) {  /* Check if z-index allows move */
                if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up */
                    break;
                }
                cnt++;
            } else {
                break;
            }
        }
    }
    return cnt;
}

/**
 * \brief           Prints widget structure from root tree
 */
void
gui_linkedlist_printlist(void) {
    print_list(NULL);
}
