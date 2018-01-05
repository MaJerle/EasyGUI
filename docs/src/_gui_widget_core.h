/**
 * \addtogroup      GUI_WIDGETS_CORE
 * \{
 *
 * User can use all function which do not end with <b>__GUI</b> which indicate private functions.
 * 
 * Core widget functions are common functions to all widgets. They can perform:
 *  - Set text to widget (Static text from non-volatile memory)
 *  - Preallocate memory for text on widget (dynamic text)
 *  - Set font type for drawing
 *  - Set dimentions and positions
 *  - Get dimensions and positions relative to parent or absolute on screen
 *  - Set or get Z index of widget
 *  - Set transparency
 *  - Set widget as visible or hidden
 *  - Delete widget
 *  - Manually widget invalidation
 *  - ..and more
 *
 * In general, each widget also uses custom based functions. These are provided in widget specific section.
 * \}
 */