/**
 * \addtogroup      GUI_IMAGE
 * \{
 *
 * Image widget can display images of different color depth (ARGB8888, RGB888 or RGB565)
 *
 * \image html image_widget_image.png Image widget of 3 images. On the left is ARGB8888 (TrueColor with alpha), middle one is RGB888 and right one format is RGB565
 *
 * \note            When image has transparent alpha channel then on each redraw, parent widget must be redrawn. 
 *                  In other words, each image with alpha channel which is not maximum (fully visible) must use more resources to redraw image widget by redrawing parent first.
 *                  On above image is clearly visible how left image has transparency (source data) over button.
 *
 * Example code for example above
 * 
 * \include         _example_image.c
 *
 * \}
 */