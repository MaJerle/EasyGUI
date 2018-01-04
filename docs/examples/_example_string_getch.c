GUI_Char myStr[] = "EasyGUI\xDF\x8F\xDF\x8F";   /* Source string to check */
GUI_STRING_t s;                                 /* Create string variable */
uint32_t ch;                                    /* Output character */
uint8_t i;                                      /* Number of bytes required for character generation */

/* GUI_CFG_USE_UNICODE = 1: string length = 9;  Length total: 11 */
/* GUI_CFG_USE_UNICODE = 0: string length = 11; Length total: 11 */
gui_string_prepare(&s, myStr);                  /* Prepare string for reading */
while (gui_string_getch(&s, &ch, &i)) {         /* Go through entire string */
    printf("I: %d, ch %c (%d)\r\n", (int)i, ch, (int)ch);   /* Print character by character */
}