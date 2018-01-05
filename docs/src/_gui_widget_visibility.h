/**
 * \addtogroup      GUI_WIDGETS_VISIBILITY
 * \{
 *
 * \section         sect_gui_widget_transparency Widget transparency
 *
 * Library support transparency setup on widget. For this, 
 * special blending function must be enabled (best if support on hardware).
 *
 * \image html image_transparent_buttons.png Transparent of 0x80 on button over opaque button
 *
 * When transparency on widget should be used, additional RAM is used:
 *
 * \f$\ size=WidgetVisibleWidth*WidgetVisibleHeight*BytesPerPixel\f$
 *
 * The additional RAM usage is used if widget has transparency and its children has also custom transparency.
 * In this case, RAM used is for each widget separatelly and depends on number of tree length of widgets of transparency values.
 *
 * \par             Blending technique for transparency
 *
 * Imagine this setup:
 *   - Desktop
 *      - <b>Widget 1</b>, children of desktop
 *         - <b>Widget 2</b>, children of Widget 1
 *            - <b>Widget 3</b>, children of Widget 2
 *
 * At some time, Widget1 has transparency = 0x80 (0.5) and Widget 3 has transparency = 0x40 (0.25)
 *
 * Drawing process would go like this:
 *
 * 1. Draw desktop widget on default drawing frame buffer
 * 2. Allocate memory for visible widget part for widget 1 and set RAM as drawing buffer
 * 3. Draw widget 1 on drawing buffer (allocated RAM)
 * 4. Draw widget 2 on drawing buffer (still allocated RAM of previous widget)
 * 5. Allocate memory for visible widget part for widget 3 and set RAM as drawing buffer
 * 6. Draw widget 3 on drawing buffer (latest allocated RAM of widget 3)
 * 7. With blending technique, merge widget 3 frame buffer with widget 1 frame buffer
 * 8. Set frame buffer back to widget 1 allocated RAM
 * 9. With blending technique, merge widget 1 frame buffer with desktop frame buffer
 * 10. Set frame buffer back to default drawing frame buffer
 *
 * \}
 */