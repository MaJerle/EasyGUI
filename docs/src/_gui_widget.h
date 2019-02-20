/**
 * \addtogroup      GUI_WIDGETS
 * \{
 *
 * \section         sect_widgets Widget organization
 *
 * Widgets are organized in multi depth linked list of parent widgets where each widget has reference to:
 *  - Next widget
 *  - Previous widget
 *  - Parent widget
 *  - Children first widget
 *  - Children last widget
 *
 * \par             Next widget link
 *
 * Next widget in linked list is used to get next widget for drawing operation.
 * Widget which is at the end of linked list is later visible on top on LCD.
 *
 * \par             Previous widget link
 *
 * Previous link is used to get widget which is "behind" current widget. 
 * This link is used for touch management where widget at the end of linked list is first checked for events (touch, mouse, etc) because it is visible on top
 *
 * \par             Parent widget
 * 
 * Link to parent widget is always required to calculate currect widget position on LCD and for other tasks (like percentage width, etc)
 *
 * \par             Children first widget
 *
 * Children links are supported on widgets which implement \ref GUI_HANDLE_ROOT_t structure to handle children widgets (such as windows).
 * It holds link to first children widget on the list.
 *
 * \par             Children last widget
 *
 * Children last widget is used for events processing (touch, mouse, etc)
 *
 * <hr />
 * Any operation on widget structure (change Z-index, put on front, etc) is only moving widgets on linked list.
 * With this, you can change visible order and also order how events are processed (touch, keyboard).
 * Linked list is most important part of GUI library therefore it is designed in robust way.
 *
 * \}
 */