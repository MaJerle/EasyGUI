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
static BUFFER_t TSBuffer;
static GUI_Byte_t TSBufferData[TOUCH_BUFFER_SIZE * sizeof(GUI_TouchData_t) + 1];
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
static BUFFER_t KBBuffer;
static GUI_Byte_t KBBufferData[TOUCH_BUFFER_SIZE * sizeof(GUI_KeyboardData_t) + 1];
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
void GUI_INPUT_AddTouch(const GUI_TouchData_t* Data) {
    BUFFER_Write(&TSBuffer, Data, sizeof(*Data));  /* Write data to buffer */
}

GUI_Byte_t __GUI_INPUT_ReadTouch(GUI_TouchData_t* Data) {
    if (BUFFER_GetFull(&TSBuffer) >= sizeof(*Data)) {
        return (GUI_Byte_t)BUFFER_Read(&TSBuffer, Data, sizeof(*Data)); /* Read data fro mbuffer */
    }
    return 0;
}
#endif /* GUI_USE_TOUCH */


#if GUI_USE_KEYBOARD
void GUI_INPUT_AddKey(const GUI_KeyboardData_t* Data) {
    BUFFER_Write(&KBBuffer, Data, sizeof(*Data));   /* Write data to buffer */
}

GUI_Byte_t __GUI_INPUT_ReadKey(GUI_KeyboardData_t* Data) {
    if (BUFFER_GetFull(&KBBuffer) >= sizeof(*Data)) {
        return (GUI_Byte_t)BUFFER_Read(&KBBuffer, Data, sizeof(*Data)); /* Read data fro mbuffer */
    }
    return 0;
}
#endif /* GUI_USE_KEYBOARD */

void __GUI_INPUT_Init(void) {
#if GUI_USE_TOUCH
    BUFFER_Init(&TSBuffer, sizeof(TSBufferData), TSBufferData);
#endif /* GUI_USE_TOUCH */
#if GUI_USE_KEYBOARD
    BUFFER_Init(&KBBuffer, sizeof(KBBufferData), KBBufferData);
#endif /* GUI_USE_KEYBOARD */
}
