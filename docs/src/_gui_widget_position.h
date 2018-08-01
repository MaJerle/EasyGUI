/**
 * \addtogroup  GUI_WIDGET_POSITION
 * \{
 * 
 * Widget position can be set either with percentage to parent or with fixed dimension.
 *
 * \note        Widget position is always relative to parent widget position
 *
 * \note        Please refer to section \ref GUI_WIDGET_SIZE to see an idea of different types
 *
 * \par         Example
 *
 * In this example, widget is set to `y = 10 pixels` relative to parent and `x = 50%` relative to parent.
 * Parent widget is on screen at absolute position `x = 200, y = 100 pixels` and its size is `width = 320, height = 240 pixels`.
 *
 * Now, to calculate relative `Y` position in pixels, we have to:
 *
 *  - Y position is known and is `10` pixels relative to parent.
 *  - Absolute position on screen is thus `y = 110 pixels`.
 *
 * To calculate `X` position, we need first relative position:
 *
 *  \f$\ x_{rel} = \frac{x_{rel_{percent}} * parentwidth_{pixels}}{100} = \frac{50 * 320}{100} = 160 \f$
 *
 *  \f$\ x_{abs} = x_{rel} + x_{abs_{parent}} = 160 + 200 = 360 \f$
 *
 * Now we know absolute position of widget, `x = 360, y = 110`.
 *
 * \note        If parent width is also in percent, we have to calculate its pixels width first.
 *              We do so until we reach top widget which for sure has fixed width (LCD width)
 *
 * \}
 */