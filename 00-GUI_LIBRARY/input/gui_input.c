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
#include "gui_input.h"

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
#if GUI_USE_TOUCH
static GUI_BUFFER_t TSBuffer;
static GUI_Byte_t TSBufferData[GUI_TOUCH_BUFFER_SIZE * sizeof(GUI_TouchData_t) + 1];
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
static GUI_BUFFER_t KBBuffer;
static GUI_Byte_t KBBufferData[GUI_TOUCH_BUFFER_SIZE * sizeof(GUI_KeyboardData_t) + 1];
#endif /* GUI_USE_KEYBOARD */

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
#if GUI_USE_TOUCH
uint8_t GUI_INPUT_TouchAdd(GUI_TouchData_t* ts) {
    ts->Time = GUI.Time;                            /* Set event time */
    return GUI_BUFFER_Write(&TSBuffer, ts, sizeof(*ts)) ? 1 : 0;    /* Write data to buffer */
}

uint8_t __GUI_INPUT_TouchRead(GUI_TouchData_t* ts) {
    if (GUI_BUFFER_GetFull(&TSBuffer) >= sizeof(*ts)) {
        return (uint8_t)GUI_BUFFER_Read(&TSBuffer, ts, sizeof(*ts)); /* Read data fro mbuffer */
    }
    return 0;
}

uint8_t __GUI_INPUT_TouchAvailable(void) {
    return GUI_BUFFER_GetFull(&TSBuffer) > 0;       /* Check if any available touch */
}
#endif /* GUI_USE_TOUCH */


#if GUI_USE_KEYBOARD
uint8_t GUI_INPUT_KeyAdd(GUI_KeyboardData_t* kb) {
    kb->Time = GUI.Time;                            /* Set event time */
    return GUI_BUFFER_Write(&KBBuffer, kb, sizeof(*kb)) ? 1 : 0;    /* Write data to buffer */
}

uint8_t __GUI_INPUT_KeyRead(GUI_KeyboardData_t* kb) {
    if (GUI_BUFFER_GetFull(&KBBuffer) >= sizeof(*kb)) {
        return (uint8_t)GUI_BUFFER_Read(&KBBuffer, kb, sizeof(*kb)); /* Read data fro mbuffer */
    }
    return 0;
}
#endif /* GUI_USE_KEYBOARD */

void __GUI_INPUT_Init(void) {
#if GUI_USE_TOUCH
    GUI_BUFFER_Init(&TSBuffer, sizeof(TSBufferData), TSBufferData);
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    GUI_BUFFER_Init(&KBBuffer, sizeof(KBBufferData), KBBufferData);
#endif /* GUI_USE_KEYBOARD */
}
