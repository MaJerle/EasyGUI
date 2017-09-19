/**
 * \author  Tilen Majerle <tilen@majerle.eu>
 * \brief   GUI mathematical functions
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2017 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef GUI_MATH_H
#define GUI_MATH_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup      GUI_UTILS
 * \{
 */
#include "gui_utils.h"
#include "math.h"
#include "errno.h"

/**
 * \defgroup        GUI_MATH Mathematical
 * \brief           Common mathematical functions used by GUI
 * \{
 */

/**
 * \brief           Calculate square of input value
 *
 *                  \f$\ y=sqrt{(x)}\f$
 *
 * \param[in]       x: Number to calculate square from
 * \param[out]      *result: Pointer to float variable to store result to
 * \retval          1: Function succedded, result is valid
 * \retval          0: Function failed, result is not valid
 */
uint8_t gui_math_sqrt(float x, float* result);

/**
 * \brief           Calculate reverse square of input value
 *
 *                  \f$\ y=\frac{1}{sqrt{(x)}}\f$
 *
 * \param[in]       x: Number to calculate reverse square from
 * \param[out]      *result: Pointer to float variable to store result to
 * \retval          1: Function succedded, result is valid
 * \retval          0: Function failed, result is not valid
 */
uint8_t gui_math_rsqrt(float x, float* result);

/**
 * \brief           Calculate distance between 2 XY points
 *
 *                  \f$\ y=sqrt{((x_1-x_2)^2+(y_1-y_2)^2)}\f$
 *
 * \param[in]       x1: X position of point 1
 * \param[in]       y1: Y position of point 1
 * \param[in]       x2: X position of point 2
 * \param[in]       y2: Y position of point 2
 * \param[out]      *result: Pointer to float variable to store result to
 * \retval          1: Function succedded, result is valid
 * \retval          0: Function failed, result is not valid
 */
uint8_t gui_math_distancebetweenxy(float x1, float y1, float x2, float y2, float* result);

/**
 * \brief           Calculate center XY coordinate between 2 XY points
 *
 *                  \f$\ c_x=\frac{x_1+x_2}{2}\f$
 *
 *                  \f$\ c_y=\frac{y_1+y_2}{2}\f$
 *
 * \param[in]       x1: X position of point 1
 * \param[in]       y1: Y position of point 1
 * \param[in]       x2: X position of point 2
 * \param[in]       y2: Y position of point 2
 * \param[out]      *resultX: Pointer to float variable to store result about X position to
 * \param[out]      *resultY: Pointer to float variable to store result about Y position to
 * \retval          1: Function succedded, results are valid
 * \retval          0: Function failed, results are not valid
 */
uint8_t gui_math_centerofxy(float x1, float y1, float x2, float y2, float* resultX, float* resultY);
    
/**
 * \}
 */

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
