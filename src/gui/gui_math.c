/**	
 * \file            gui_math.c
 * \brief           Mathematical functions
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
#include "gui/gui_math.h"

/**
 * \brief           Calculate square of input value
 *
 *                  \f$\ y=sqrt{(x)}\f$
 *
 * \param[in]       x: Number to calculate square from
 * \param[out]      result: Pointer to float variable to store result to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_math_sqrt(float x, float* const result) {
    if (x > 0) {                                    /* Check valid input */
        *result = GUI_FLOAT(sqrt(x));               /* Calculate on positive number */
        return 1;
    }
    return 0;
}

/**
 * \brief           Calculate reverse square of input value
 *
 *                  \f$\ y=\frac{1}{sqrt{(x)}}\f$
 *
 * \param[in]       x: Number to calculate reverse square from
 * \param[out]      result: Pointer to float variable to store result to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_math_rsqrt(float x, float* const result) {
    int32_t i;
	float x2, y;
	const float th = 1.5f;

	x2 = x * 0.5f;
	y = x;
	i = *(int32_t *)&y;                             /* Read float number memory representation as long integer */
	i = 0x5F3759dF - (i >> 1);                      /* Subtract integer number divided by 2 from magic number */
	y = *(float *)&i;                               /* Read new integer value as float again */
	y = y * (th - (x2 * y * y));                    /* Calculate using iterations */
	y = y * (th - (x2 * y * y));                    /* Calculate using iterations */

    *result = y;                                    /* Save number */
    
    return 1;                                       /* Return status */
}

/**
 * \brief           Calculate distance between 2 XY points
 *
 *                  \f$\ y=sqrt{((x_1-x_2)^2+(y_1-y_2)^2)}\f$
 *
 * \param[in]       x1: X position of point 1
 * \param[in]       y1: Y position of point 1
 * \param[in]       x2: X position of point 2
 * \param[in]       y2: Y position of point 2
 * \param[out]      result: Pointer to float variable to store result to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_math_distancebetweenxy(float x1, float y1, float x2, float y2, float* const result) {
    return gui_math_sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2), result);    /* Calculate distance between 2 points with X and Y value */
}

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
 * \param[out]      resultx: Pointer to float variable to store result about X position to
 * \param[out]      resulty: Pointer to float variable to store result about Y position to
 * \return          `1` on success, `0` otherwise
 */
uint8_t
gui_math_centerofxy(float x1, float y1, float x2, float y2, float* const resultx, float* const resulty) {
    *resultx = (x1 + x2) / 2.0f;                    /* Calculate middle point for X position */
    *resulty = (y1 + y2) / 2.0f;                    /* Calculate middle point for Y position */
    
    return 1;
}
