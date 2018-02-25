/**	
 * \file            gui_linkedlist.c
 * \brief           Linked list manager
 */
 
/*
 * Copyright (c) 2017 Tilen Majerle
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
 * Author:          Tilen Majerle <tilen@majerle.eu>
 */
#define GUI_INTERNAL
#include "gui/gui_private.h"
#include "gui/gui_linkedlist.h"

/**
 * \brief           Print linked list from root element
 * \param[in]       root: Root linked list element
 */
static void
print_list(GUI_HANDLE_ROOT_t* root) {
    static uint8_t depth = 0;
    GUI_HANDLE_ROOT_t* h;
    GUI_LinkedListRoot_t* list;
    
    depth++;
    if (root) {
        list = &root->RootList;
    } else {
        list = &GUI.Root;
    }
    for (h = (GUI_HANDLE_ROOT_t *)list->First; h != NULL; h = h->Handle.List.Next) {
        GUI_DEBUG("%*d: W: %s; A: 0x%08X; R: %lu; D: %lu\r\n", depth, depth, (const char *)h->Handle.Widget->Name, (unsigned int)h, (unsigned long)!!(h->Handle.Flags & GUI_FLAG_REDRAW), (unsigned long)!!(h->Handle.Flags & GUI_FLAG_REMOVE));
        if (gui_widget_allowchildren__(h)) {
            print_list(h);
        }
    }
    depth--;
}

/**
 * \brief           Add element to doubly linked list
 * \note            Element added to linkedlist must have \ref GUI_LinkedList_t as top structure
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to add to base linked list 
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
void
gui_linkedlist_add_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    if (!root->First || !root->Last) {              /* First widget is about to be created */
        element->Prev = NULL;                       /* There is no previous element */
        element->Next = NULL;                       /* There is no next element */
        root->First = element;                      /* Set as first widget */
        root->Last = element;                       /* Set as last widget */
    } else {
        element->Next = NULL;                       /* Next element of last is not known */
        element->Prev = root->Last;                 /* Previous element of new entry is currently last element */
        ((GUI_LinkedList_t *)root->Last)->Next = element;   /* Previous's element next element is current element */
        root->Last = element;                       /* Add new element as last */
    }
}
    
/**
 * \brief           Remove element from doubly linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \return          Removed element handle
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_multi_add_gen, gui_linkedlist_multi_remove_gen
 */
GUI_LinkedList_t*
gui_linkedlist_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    GUI_LinkedList_t* prev;
    GUI_LinkedList_t* next;
    
    if (!element) {                                 /* Maybe element is not even valid? */
        return 0;
    }

    prev = element->Prev;                           /* Get previous element of current */
    next = element->Next;                           /* Get next element of current */
    
    if (prev) {                                     /* If current element has previous elemnet */
        prev->Next = next;                          /* Set new next element to previous element */
    }
    if (next) {                                     /* If current element has next element */
        next->Prev = prev;                          /* Set new previous element to next element */
    }
    if (root->First == element) {                   /* If current is the same as first */
        root->First = next;                         /* Set next element as first */
    }
    if (root->Last == element) {                    /* If current is the same as last */
        root->Last = prev;                          /* Set previous as last element */
    }
    
    element->Prev = NULL;                           /*!< Clear element pointer */
    element->Next = NULL;                           /*!< Clear element pointer */
    return element;
}

/**
 * \brief           Get next element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Next element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>First element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to get next element of
 * \return          Next element handle on success, NULL otherwise
 * \sa              gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedList_t*
gui_linkedlist_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    if (!element) { 
        if (root) {
            return (void *)root->First;
        } else {
            return 0;
        }
    }
    return element->Next;                           /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous element structure from doubly linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Previous element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>Last element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to get next element of
 * \return          Previous element handle on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_multi_getnext_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedList_t*
gui_linkedlist_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    if (!element) { 
        if (root) {
            return (void *)root->Last;
        } else {
            return 0;
        }
    }
    return element->Prev;                           /* Get next widget of current in linked list */
}

/**
 * \brief           Move widget down for one on doubly linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to move down on base linked list
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    GUI_LinkedList_t* Prev = 0;
    GUI_LinkedList_t* Next = 0;
    GUI_LinkedList_t* NextNext = 0;
    
    if (!element || element == root->Last) {        /* Check if move is available */
        return 0;                                   /* Could not move */
    }
    
    Prev = element->Prev;                           /* Get previous element */
    Next = element->Next;                           /* Get next element */
    if (Next) {                                     /* Check if next is available */
        NextNext = Next->Next;                      /* Get next element of next element */
    }
    
    if (NextNext) {                                 /* If there is available memory */
        NextNext->Prev = element;                   /* Previous element of next next element is current element */
    } else {
        root->Last = element;                       /* There is no next next element so we will be last element after move */
    }
    
    if (Next) {                                     /* If there is next element */
        Next->Next = element;                       /* Next element will become previous and set next element as current */
        Next->Prev = Prev;                          /* Set previous element to next element as current previous */
    }
    
    element->Next = NextNext;                       /* Set next next element to current next element */
    element->Prev = Next;                           /* Set next element as previous (swap current and next elements) */
    
    if (Prev) {                                     /* Check if next exists */
        Prev->Next = Next;                          /* Set previous element to next */
    }
    
    if (root->First == element) {                   /* Check for current element */
        root->First = Next;                         /* Set new element as first in linked list */
    }
    
    return 1;                                       /* Move was successful */
}

/**
 * \brief           Move widget up for one on doubly linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to move up on base linked list
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedList_t* element) {
    GUI_LinkedList_t* PrevPrev = 0;
    GUI_LinkedList_t* Prev = 0;
    GUI_LinkedList_t* Next = 0;
    
    if (!element || element == root->First) {       /* Check if move is available */
        return 0;                                   /* Could not move */
    }
    
    Prev = element->Prev;                           /* Get previous element */
    Next = element->Next;                           /* Get next element */
    if (Prev) {                                     /* Check if previous is available */
        PrevPrev = Prev->Prev;                      /* Get previous element of previous element */
    }
    
    if (PrevPrev) {                                 /* If there is available memory */
        PrevPrev->Next = element;                   /* Next element of previous previous element is current element */
    } else {
        root->First = element;                      /* There is no previous previous element so we will be first element after move */
    }
    
    if (Prev) {                                     /* If there is previous element */
        Prev->Prev = element;                       /* Previous element will become next and set previous element as current */
        Prev->Next = Next;                          /* Set next element to previous element as current previous */
    }
    
    element->Prev = PrevPrev;                       /* Set previous previous element to current previous element */
    element->Next = Prev;                           /* Set previous element as next (swap current and previous elements) */
    
    if (Next) {                                     /* Check if previous exists */
        Next->Prev = Prev;                          /* Set next element to previous */
    }
    
    if (root->Last == element) {                    /* Check for current element */
        root->Last = Prev;                          /* Set new last as first in linked list */
    }
    
    return 1;                                       /* Move was successful */
}

/**
 * \brief           Get item from linked list by index
 * \note            Index value is the number of item in list order
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       index: Number in list to get item
 * \return          Item handle on success, NULL otherwise
 */
GUI_LinkedList_t*
gui_linkedlist_getnext_byindex_gen(GUI_LinkedListRoot_t* root, uint16_t index) {
    GUI_LinkedList_t* item = gui_linkedlist_getnext_gen(root, 0); /* Get first element */
    while (index-- && item) {                       /* Scroll to next elements */
        item = gui_linkedlist_getnext_gen(NULL, (GUI_LinkedList_t *)item);
    }
    return item;                                    /* Get that item */
}

/**
 * \brief           Add element to multi linked list
 * \note            Element can be any type since \ref GUI_LinkedListMulti_t structure is dynamicall allocated
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to add to base linked list
 * \return          Multi linked list handle on success, NULL otherwise
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
GUI_LinkedListMulti_t*
gui_linkedlist_multi_add_gen(GUI_LinkedListRoot_t* root, void* element) {
    GUI_LinkedListMulti_t* ptr;
    ptr = GUI_MEMALLOC(sizeof(GUI_LinkedListMulti_t));  /* Create memory for linked list */
    if (ptr != NULL) {
        ptr->Element = element;                     /* Save pointer to our element */
        gui_linkedlist_add_gen(root, &ptr->List);   /* Add element to linked list */
    }
    
    return ptr;
}
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedList_t element to remove from base linked list
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_add_gen, gui_linkedlist_remove_gen, gui_linkedlist_multi_add_gen
 */
uint8_t
gui_linkedlist_multi_remove_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element) {
    if (!element) {                                 /* Check valid element */
        return 0;
    }
    gui_linkedlist_remove_gen(root, (GUI_LinkedList_t *)element); /* Remove element from linked list */
    GUI_MEMFREE(element);                           /* Free memory */
    return 1;
}

/**
 * \brief           Get next element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getnext_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Next element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>First element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Next element of <b>element</b> if exists, first element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedListMulti_t element to get next element of
 * \return          Next element on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getprev_gen
 */
GUI_LinkedListMulti_t*
gui_linkedlist_multi_getnext_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element) {
    if (!element) { 
        if (root) {
            return (GUI_LinkedListMulti_t *)root->First;
        } else {
            return 0;
        }
    }
    return (GUI_LinkedListMulti_t *)element->List.Next; /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous element structure from multi linked list
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>root    <th>element <th>gui_linkedlist_multi_getprev_gen(root, element)
 *                      <tr><td>0       <td>0       <td>0
 *                      <tr><td>0       <td>> 0     <td>Previous element of <b>element</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0       <td>Last element in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0     <td>Previous element of <b>element</b> if exists, last element of <b>root</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedListMulti_t element to get next element of
 * \return          Previous element on success, NULL otherwise
 * \sa              gui_linkedlist_getnext_gen, gui_linkedlist_getprev_gen, gui_linkedlist_multi_getnext_gen
 */
GUI_LinkedListMulti_t*
gui_linkedlist_multi_getprev_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element) {
    if (!element) { 
        if (root) {
            return (GUI_LinkedListMulti_t *)root->Last;
        } else {
            return 0;
        }
    }
    return (GUI_LinkedListMulti_t *)element->List.Prev; /* Get next widget of current in linked list */
}

/**
 * \brief           Move widget down for one on multi linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedListMulti_t element to move down on base linked list
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_moveup_gen
 */
uint8_t
gui_linkedlist_multi_movedown_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element) {
    return gui_linkedlist_movedown_gen(root, &element->List); /* Move down elemenet */
}

/**
 * \brief           Move widget up for one on multi linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Pointer to \ref GUI_LinkedListMulti_t element to move up on base linked list
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_moveup_gen, gui_linkedlist_movedown_gen, gui_linkedlist_multi_movedown_gen
 */
uint8_t
gui_linkedlist_multi_moveup_gen(GUI_LinkedListRoot_t* root, GUI_LinkedListMulti_t* element) {
    return gui_linkedlist_moveup_gen(root, &element->List);   /* Move down elemenet */
}
    
/**
 * \brief           Remove element from linked list in multi linked list
 * \note            Delete by element pointer, not by list item. It searches in all entries for element pointers and deletes all links with that element pointer
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to \ref GUI_LinkedListRoot_t structure as base element
 * \param[in]       element: Memory address in data part of linked list entry
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_remove_gen, gui_linkedlist_multi_remove_gen
 */
uint8_t
gui_linkedlist_multi_find_remove(GUI_LinkedListRoot_t* root, void* element) {
    GUI_LinkedListMulti_t* link;
    uint8_t ret = 0;
    
    __GUI_ASSERTPARAMS(root);                       /* Check input parameters */
    
    for (link = gui_linkedlist_multi_getnext_gen(root, NULL); link; link = gui_linkedlist_multi_getnext_gen(NULL, link)) {
        if ((void *)gui_linkedlist_multi_getdata(link) == element) {/* Check match */
            gui_linkedlist_multi_remove_gen(root, link);  /* Remove element from linked list */
            ret = 1;
        }
    }
    return ret;
}

/**
 * \brief           Add new widget to linked list of parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       root: Pointer to parent widget where to put this widget on list
 * \param[in]       h: Widget handle to add to list 
 * \sa              gui_linkedlist_widgetremove
 */
void
gui_linkedlist_widgetadd(GUI_HANDLE_ROOT_t* root, GUI_HANDLE_p h) {    
    if (root != NULL) {
        gui_linkedlist_add_gen(&root->RootList, (GUI_LinkedList_t *)h);
    } else {
        gui_linkedlist_add_gen(&GUI.Root, (GUI_LinkedList_t *)h);
    }
    gui_linkedlist_widgetmovetotop(h);              /* Reset by moving to top */
    gui_linkedlist_widgetmovetobottom(h);           /* Reset by moving to bottom with reorder */
}

/**
 * \brief           Remove widget from linked list of parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle to remove from linked list 
 * \sa              gui_linkedlist_widgetadd
 */
void
gui_linkedlist_widgetremove(GUI_HANDLE_p h) {    
    if (__GH(h)->Parent != NULL) {
        gui_linkedlist_remove_gen(&((GUI_HANDLE_ROOT_t *)__GH(h)->Parent)->RootList, (GUI_LinkedList_t *)h);
    } else {
        gui_linkedlist_remove_gen(&GUI.Root, (GUI_LinkedList_t *)h);
    }
}

/**
 * \brief           Move widget up for one in linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle to move up
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_widgetmovedown
 */
uint8_t
gui_linkedlist_widgetmoveup(GUI_HANDLE_p h) {
    if (__GH(h)->Parent != NULL) {
        return gui_linkedlist_moveup_gen(&__GHR(__GH(h)->Parent)->RootList, (GUI_LinkedList_t *)h);
    }
    return gui_linkedlist_moveup_gen(&GUI.Root, (GUI_LinkedList_t *)h);
}

/**
 * \brief           Move widget down for one in linked list
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget handle to move down
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_widgetmoveup
 */
uint8_t
gui_linkedlist_widgetmovedown(GUI_HANDLE_p h) {
    if (__GH(h)->Parent != NULL) {
        return gui_linkedlist_movedown_gen(&__GHR(__GH(h)->Parent)->RootList, (GUI_LinkedList_t *)h);
    }
    return gui_linkedlist_movedown_gen(&GUI.Root, (GUI_LinkedList_t *)h);
}

/**
 * \brief           Get next widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>gui_linkedlist_widgetgetnext(parent, h)
 *                      <tr><td>0       <td>0   <td>0
 *                      <tr><td>0       <td>> 0 <td>Next widget of <b>h</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0   <td>First widget in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0 <td>Next widget of <b>h</b> if exists, first widget of <b>parent</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \include         _example_linkedlist_widgetgetnext.c 
 *
 * \param[in]       parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \return          Previous widget on linked list on success, NULL otherwise
 * \sa              gui_linkedlist_widgetgetprev
 */
GUI_HANDLE_p
gui_linkedlist_widgetgetnext(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h) {
    if (h == NULL) {                                /* Get first widget on list */
        if (parent != NULL) {                       /* If parent exists... */
            return (GUI_HANDLE_p)parent->RootList.First;    /* ...get first widget on parent */
        } else {
            return (GUI_HANDLE_p)GUI.Root.First;    /* Get first widget in GUI */
        }
    }
    return (GUI_HANDLE_p)__GH(h)->List.Next;        /* Get next widget of current in linked list */
}

/**
 * \brief           Get previous widget from current widget handle
 * \note            With combination of input parameters, function acts different:
 *                  <table>
 *                      <tr><th>parent  <th>h   <th>gui_linkedlist_widgetgetprev(parent, h)
 *                      <tr><td>0       <td>0   <td>0
 *                      <tr><td>0       <td>> 0 <td>Previous widget of <b>h</b> if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>0   <td>Last widget in <b>parent</b> list if exists, <b>0</b> otherwise
 *                      <tr><td>> 0     <td>> 0 <td>Previous widget of <b>h</b> if exists, last widget of <b>parent</b> if exists, <b>0</b> otherwise
 *                  </table>
 *
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 *
 * \include         _example_linkedlist_widgetgetprev.c 
 *
 * \param[in]       parent: Parent widget object for which you want to get first widget
 * \param[in]       h: Widget handle for which you want next widget
 * \return          Next widget on linked list on success, NULL otherwise
 * \sa              gui_linkedlist_widgetgetnext
 */
GUI_HANDLE_p
gui_linkedlist_widgetgetprev(GUI_HANDLE_ROOT_t* parent, GUI_HANDLE_p h) {
    if (h == NULL) {                                /* Get last widget on list */
        if (parent != NULL) {                       /* If parent exists... */
            return (GUI_HANDLE_p)parent->RootList.Last; /* ...get last widget on parent */
        } else {
            return (GUI_HANDLE_p)GUI.Root.Last;     /* Get last widget in GUI */
        }
    }
    return (GUI_HANDLE_p)__GH(h)->List.Prev;        /* Get next widget of current in linked list */
}

/***
 * Widget linked list order:
 *
 * 1. Normal widgets, with automatic or fixed z-index
 * 2. Widgets with children support
 * 3. Widgets as dialog base elements
 */
/**
 * \brief           Move widget to bottom in linked list of parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget to move to bottom
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_widgetmovetotop
 */
uint8_t
gui_linkedlist_widgetmovetobottom(GUI_HANDLE_p h) {
    if (__GH(h)->List.Next == NULL) {
        return 0;
    }
    while (__GH(h)->List.Next != NULL) {            /* While device has next element */
        if (gui_widget_isdialogbase__(h)) {         /* Widget is dialog base element */
            if (!gui_linkedlist_widgetmovedown(h)) {    /* Move down */
                return 0;
            }
        } else if (gui_widget_allowchildren__(h)) { /* Widget supports children widgets, go to the end of the list if necessary */
            if (!gui_widget_isdialogbase__(__GH(__GH(h)->List.Next))) { /* Go down till dialog base is reached */
                if (__GH(h)->ZIndex >= __GH(__GH(h)->List.Next)->ZIndex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmovedown(h)) {    /* Move down */
                        return 0;
                    }
                } else {
                    break;
                }
            } else {
                break;
            }
        } else {                                    /* Our widget does not allow sub widgets */
            if (!gui_widget_allowchildren__(__GH(__GH(h)->List.Next))) {    /* Allow moving down only if next widget does not allow sub widgets */
                if (__GH(h)->ZIndex >= __GH(__GH(h)->List.Next)->ZIndex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmovedown(h)) {    /* Move down */
                        return 0;
                    }
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }
    return 1;
}

/**
 * \brief           Move widget to top in linked list of parent widget
 * \note            The function is private and can be called only when GUI protection against multiple access is activated
 * \param[in]       h: Widget to move to top
 * \return          1 on success, 0 otherwise
 * \sa              gui_linkedlist_widgetmovetobottom
 */
uint8_t
gui_linkedlist_widgetmovetotop(GUI_HANDLE_p h) {
    if (__GH(h)->List.Prev == NULL) {
        return 0;
    }
    while (__GH(h)->List.Prev != NULL) {            /* While device has previous element */
        if (gui_widget_isdialogbase__(h)) {         /* Widget is dialog base element */
            if (gui_widget_isdialogbase__(__GH(__GH(h)->List.Prev))) {  /* If previous widget is dialog base too */
                if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up widget */
                    return 0;
                }
            } else {
                break;                              /* Stop execution */
            }
        } else if (gui_widget_allowchildren__(h)) { /* If moving widget allows children elements */
            if (gui_widget_allowchildren__(__GH(__GH(h)->List.Prev))) { /* If previous widget allows children too */
                if (__GH(h)->ZIndex <= __GH(__GH(h)->List.Prev)->ZIndex) {  /* Check if z-index allows move */
                    if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up */
                        return 0;
                    }
                } else {
                    break;
                }
            } else {
                break;                              /* Stop execution */
            }
        } else {                                    /* Normal widget */
            if (__GH(h)->ZIndex <= __GH(__GH(h)->List.Prev)->ZIndex) {  /* Check if z-index allows move */
                if (!gui_linkedlist_widgetmoveup(h)) {  /* Move up */
                    return 0;
                }
            } else {
                break;
            }
        }
    }
    return 1;
}

/**
 * \brief           Prints widget structure from root tre 
 */
void
gui_linkedlist_printlist(void) {
    print_list(NULL);
}
