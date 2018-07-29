/* Todo: Update code to working example! */

gui_char myStr[] = "EasyGUI\xDF\x8F\xDF\x8F";   /* Source string to check */
gui_string_t s;                                 /* Create string variable */
uint32_t ch;                                    /* Output character */
uint8_t i;                                      /* Number of bytes required for character generation */

/* GUI_CFG_USE_UNICODE = 1: string length = 9;  Length total: 11 */
/* GUI_CFG_USE_UNICODE = 0: string length = 11; Length total: 11 */
gui_string_prepare(&s, myStr);                  /* Prepare string for reading */
gui_string_gotoend(&ptr);                       /* Go to last character of string */
while (gui_string_getchreverse(&s, &ch, &i)) {  /* Go through entire string */
    printf("I: %d, ch %c (%d)\r\n", (int)i, ch, (int)ch);   /* Print character by character */
}