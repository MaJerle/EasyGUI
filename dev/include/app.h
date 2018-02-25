/**
 * \file            app.h
 * \brief           Application header
 */

/*
 * Copyright (c) 2018 Tilen Majerle
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
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef __APP_H
#define __APP_H

#include "cmsis_os.h"

/* GUI library */
#include "gui/gui.h"
#include "widget/gui_window.h"
#include "widget/gui_button.h"
#include "widget/gui_led.h"
#include "widget/gui_progbar.h"
#include "widget/gui_graph.h"
#include "widget/gui_edittext.h"
#include "widget/gui_checkbox.h"
#include "widget/gui_radio.h"
#include "widget/gui_listbox.h"
#include "widget/gui_listview.h"
#include "widget/gui_textview.h"
#include "widget/gui_dropdown.h"
#include "widget/gui_dialog.h"
#include "widget/gui_image.h"
#include "widget/gui_slider.h"
#include "widget/gui_container.h"
#include "widget/gui_debugbox.h"
#include "widget/gui_list_container.h"
#include "gui/gui_keyboard.h"
#include "gui/gui_lcd.h"
#include "netconn_server.h"

#include "tm_stm32_touch.h"

/* ESP library */
#include "esp/esp.h"
#include "esp/apps/esp_mqtt_client.h"

#include "gui_app.h"
#include "esp_app.h"

extern TM_TOUCH_t TS;

/* Thread definitions */
void user_thread(void const * arg);
void init_thread(void const * arg);

/* Read touch function prototype */
void read_touch(void);

/* Application functions */
void console_write(const char* str);

/* ESP callback function */
espr_t  esp_cb_func(esp_cb_t* cb);

/* GUI APP based functions */
void    create_desktop(void);
void    open_container(GUI_ID_t cont_id);

/* Widget event functions */
uint8_t gui_window_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_container_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result); 
uint8_t gui_image_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_button_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_textview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_listview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_debugbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);
uint8_t gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result);

/* Access points visible to ESP device */
extern esp_ap_t access_points[10];
extern size_t access_points_count;

/* ESP APP based functions */
void    list_access_points(void);
void    enable_wifi_access_point(void);
espr_t  mqtt_send_data(const void* data, size_t len);
espr_t  start_server(void);
void    lcd_update_up(esp_ip_t* ip);

/* MQTT client */
extern mqtt_client_t* mqtt_client;

/* List of GUI widget IDs */
#define GUI_ID_CONTAINER_STATUS             (GUI_ID_USER + 0x0001)
#define GUI_ID_CONTAINER_WIFI               (GUI_ID_USER + 0x0002)
#define GUI_ID_CONTAINER_CONSOLE            (GUI_ID_USER + 0x0003)
#define GUI_ID_CONTAINER_LOG                (GUI_ID_USER + 0x0004)

#define GUI_ID_IMAGE_WIFI_STATUS            (GUI_ID_USER + 0x0101)
#define GUI_ID_IMAGE_CONSOLE                (GUI_ID_USER + 0x0102)
#define GUI_ID_IMAGE_LOG                    (GUI_ID_USER + 0x0103)

#define GUI_ID_TEXTVIEW_TIME                (GUI_ID_USER + 0x0201)
#define GUI_ID_TEXTVIEW_CPU_USAGE           (GUI_ID_USER + 0x0202)
#define GUI_ID_TEXTVIEW_IP_ADDR             (GUI_ID_USER + 0x0203)

#define GUI_ID_LISTVIEW_WIFI_APS            (GUI_ID_USER + 0x0301)

#define GUI_ID_BUTTON_WIFI_RELOAD           (GUI_ID_USER + 0x0401)
#define GUI_ID_BUTTON_WIFI_CONNECT          (GUI_ID_USER + 0x0402)

#define GUI_ID_EDITTEXT_WIFI_PASSWORD       (GUI_ID_USER + 0x0501)

#define GUI_ID_DEBUGBOX_LOG                 (GUI_ID_USER + 0x0601)

/* Image descriptors */
extern const GUI_IMAGE_DESC_t image_wifi_on;
extern const GUI_IMAGE_DESC_t image_wifi_off;
extern const GUI_IMAGE_DESC_t image_console;
extern const GUI_IMAGE_DESC_t image_log;

/* List of fonts */
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Bold_18;
extern GUI_Const GUI_FONT_t GUI_Font_Comic_Sans_MS_Regular_22;
extern GUI_Const GUI_FONT_t GUI_Font_Arial_Narrow_Italic_22;
extern GUI_Const GUI_FONT_t GUI_Font_Calibri_Bold_8;

#endif /* __APP_H */
