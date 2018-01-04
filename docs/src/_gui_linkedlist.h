/**
 * \addtogroup      GUI_LINKEDLIST
 * \{
 *
 * \par             Doubly linked list
 *
 * Library operates with <b>doubly</b> linked list type since each element in list has next and previous element known. This allows loop from first to last and from last to first easily.
 *
 * Linked list link structure is statically allocated which does not add addditional overhead.
 *
 * \par             Multi linked list
 *
 * Second support is <b>multi</b> linked list. This approach allows single element to be part of multiple linked lists.
 * 
 * In relation to first, this approach needs dynamic link memory allocation for each linked list it exists.
 *
 * \note            This functions can only be used inside GUI library
 *
 * \}
 */