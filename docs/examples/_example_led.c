handle = gui_led_create(0, 10, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);
gui_led_set(handle, 1);

handle = gui_led_create(0, 50, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_CIRCLE);

handle = gui_led_create(0, 90, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT);
gui_led_set(handle, 1);

handle = gui_led_create(0, 130, 80, 30, 30, 0, 0, 0);
gui_led_settype(handle, GUI_LED_TYPE_RECT);