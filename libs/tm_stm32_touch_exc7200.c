/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
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
#include "tm_stm32_touch_EXC7200.h"

uint8_t TM_TOUCH_EXC7200_Init(TM_TOUCH_t* TS) {
	/* Init I2C */
	TM_I2C_Init(TOUCH_EXC7200_I2C, TOUCH_EXC7200_I2C_PP, 100000);
    
    /* Check connected device */
    if (TM_I2C_IsDeviceConnected(TOUCH_EXC7200_I2C, TOUCH_EXC7200_I2C_DEV) != TM_I2C_Result_Ok) {
        return 1;
    }
	
	/* Set max values */
	TS->MaxX = 639;
	TS->MaxY = 479;
	
	/* Return 0 = OK */
	return 0;
}

uint8_t TM_TOUCH_EXC7200_Read(TM_TOUCH_t* TS) {
	uint8_t DataReceive[11];
	uint16_t X1, Y1;
	
	/* Read data */
	TM_I2C_ReadMultiNoRegister(TOUCH_EXC7200_I2C, TOUCH_EXC7200_I2C_DEV | 1, DataReceive, 10);
	
	/* Format data */
	X1 = (((DataReceive[3] & 0xFF) << 4) | ((DataReceive[2] & 0xF0) >> 4)) << 1;
	Y1 = (((DataReceive[5] & 0xFF) << 4) | ((DataReceive[4] & 0xF0) >> 4)) << 1;
	
	/* Check values */
	if (DataReceive[1] == 0x83) {
		/* At least one is detected */
		TS->NumPresses = 1;
		TS->X[0] = (X1 * TS->MaxX) >> 12;
		TS->Y[0] = (Y1 * TS->MaxY) >> 12;
        
        /* Flush data again */
        TM_I2C_ReadMultiNoRegister(TOUCH_EXC7200_I2C, TOUCH_EXC7200_I2C_DEV, DataReceive, 10);
	} else {
		/* No press detected */
		TS->NumPresses = 0;
	}
	
	/* Everything OK */
	return 0;
}
