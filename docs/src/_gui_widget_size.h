/**
 * \addtogroup  GUI_WIDGET_SIZE
 * \{
 * 
 * Widget size can be set either in `pixels` directly or in `percents`, relative to parent widget
 *
 * \par         Fixed size
 *
 * When fixed size is applied (size in pixels) widget has known size and is not dependant to its parent widget size.
 *
 * \note        Take a look at percentage size for variable size
 * 
 * \par         Percentage size
 * 
 * When size is in units of pixels, value is directly reflected to the screen.
 * However, when value is in percent, it has to be calculated first to pixels.
 * Reference for calculation is size of parent widget size
 * (if parent is also in percent, we take its parent, and so on).
 *
 * Imagine there is parent widget with pixels dimensions `width = 800; height = 400`.
 * Inside this widget, we have second widget with percent dimensions `width = 20%; height = 30%`
 *
 * Calculation for pixels consists of:
 *
 *  \f$\ width_{pixels} = \frac{width_{percent} * parentwidth_{pixels}}{100} \f$
 *
 *  \f$\ height_{pixels} = \frac{height_{percent} * parentheight_{pixels}}{100} \f$
 *
 * \note        When parent widget changes size, child widget will be changed too
 *
 * \par         Original size
 *
 * Some functions have suffix `original` which applies to set or get original widget size.
 *
 * Each widget may have size independent between each other,
 * meaning that width may be in percents while height is in fixed value or opposite.
 *
 * Original size will simply set/get actual register value for current widget. Consider example below:
 *
 * \code{c}
h = create_widget(...);

//Set width in fixed units and height to percentage width
gui_widget_setwidth(h, 100);
gui_widget_setheightpercent(h, 50);

//If we try to get fixed value available on screen
//Now we have both values in units of pixels
//Value of height is currently not know as it depends on its parent widget
widget_screen_width = gui_widget_getwidth(h);
widget_screen_height = gui_widget_getheight(h);

//If we use original approach
//Now width will return 100 while height will return 50
//Because these were our initial values we put for widget
widget_orig_width = gui_widget_getwidthoriginal(h, NULL);
widget_orig_height = gui_widget_getheightoriginal(h, NULL);

//If we now try to set values in original units
//Now width is set to 150 pixels and height to 30 percent
//Because current units for height are percents
gui_widget_setwidthoriginal(h, 150);
gui_widget_setheightoriginal(h, 30);
//This will do the same as 2 functions above
gui_widget_setsizeoriginal(h, 150, 30);

//To force size to specific values, use dedicated functions
//Now our widget has 40% width relative to parent
//and fixed height of 70 pixels
gui_widget_setwidthpercent(h, 40);
gui_widget_setheight(h, 70);
\endcode
 *
 * An example when to use functions with `original` suffix would be when we want
 * to keep same size format and just (let's say) make widget bigger by `50%`
 *
 * \code{c}
h = create_widget(...);

//Let's set widget width and height to some desired values
//Our widget is 50% width relative to parent and has fixed size of 30 pixels
//We need percentage width because we want that our widget changes width
//according to parent widget width value
gui_widget_setwidthpercent(h, 50);
gui_widget_setheight(h, 30);

//Now, somewhere in the code we want to increase widget both direction
//We do not care what is size format, just use current one (original one)
gui_widget_setsizeoriginal(h,
    gui_widget_getwidthoriginal(h, NULL) * 1.5f,
    gui_widget_getheightoriginal(h, NULL) * 1.5f
);
\endcode
 *
 * \}
 */