handle = gui_led_create(0, 10, 80, 30, 30, 0, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE, 0);
gui_led_set(handle, 1, 0);

handle = gui_led_create(0, 50, 80, 30, 30, 0, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE, 0);

handle = gui_led_create(0, 90, 80, 30, 30, 0, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT, 0);
gui_led_set(handle, 1, 0);

handle = gui_led_create(0, 130, 80, 30, 30, 0, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT, 0);