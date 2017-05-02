/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2017 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#define GUI_INTERNAL
#include "gui_math.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/

/******************************************************************************/
/******************************************************************************/
/***                                Public API                               **/
/******************************************************************************/
/******************************************************************************/
uint8_t GUI_MATH_Sqrt(float x, float* result) {
    if (x > 0) {                                    /* Check valid input */
        *result = sqrt(x);                          /* Calculate on positive number */
        return 1;
    }
    return 0;
}

uint8_t GUI_MATH_RSqrt(float x, float* result) {
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

uint8_t GUI_MATH_DistanceBetweenXY(float x1, float y1, float x2, float y2, float* result) {
    return GUI_MATH_Sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2), result);    /* Calculate distance between 2 points with X and Y value */
}

uint8_t GUI_MATH_CenterOfXY(float x1, float y1, float x2, float y2, float* resultX, float* resultY) {
    *resultX = (x1 + x2) / 2.0f;                    /* Calculate middle point for X position */
    *resultY = (y1 + y2) / 2.0f;                    /* Calculate middle point for Y position */
    
    return 1;
}
