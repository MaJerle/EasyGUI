/**
 * \file            gui_callbacks.c
 * \brief           GUI callback functions for widgets
 */

#include "app.h"

/**
 * \brief           Window event function
 */
uint8_t
gui_window_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    uint8_t res;
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);

    return res;
}

/**
 * \brief           Container event function
 */
uint8_t
gui_container_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    GUI_Dim_t width;
    
    id = gui_widget_getid(h);                   /* Get widget ID */
    width = gui_widget_getwidth(h);             /* Get widget callback */
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Widget just created? */
            if (GUI_ID_CONTAINER_STATUS != id) {
                gui_container_setcolor(h, GUI_CONTAINER_COLOR_BG, GUI_COLOR_LIGHTGRAY);
            }
            if (GUI_ID_CONTAINER_STATUS == id) {
                gui_textview_create(GUI_ID_TEXTVIEW_CPU_USAGE, 10, 0, 150, 40, h, gui_textview_callback, NULL);
                gui_textview_create(GUI_ID_TEXTVIEW_TIME, width - 50, 0, 46, 40, h, gui_textview_callback, NULL);
                gui_image_create(GUI_ID_IMAGE_WIFI_STATUS, width - 100 + 4, 4, 32, 32, h, gui_image_callback, 0);
                gui_image_create(GUI_ID_IMAGE_CONSOLE, width - 140 + 4, 4, 32, 32, h, gui_image_callback, 0);
                gui_image_create(GUI_ID_IMAGE_LOG, width - 180 + 4, 4, 32, 32, h, gui_image_callback, 0);
            } else if (GUI_ID_CONTAINER_WIFI == id) {
                gui_listview_create(GUI_ID_LISTVIEW_WIFI_APS, 2, 2, 2, 2, h, gui_listview_callback, 0);
                gui_button_create(GUI_ID_BUTTON_WIFI_RELOAD, 2, 2, 2, 2, h, gui_button_callback, 0);
                gui_button_create(GUI_ID_BUTTON_WIFI_CONNECT, 2, 2, 2, 2, h, gui_button_callback, 0);
                gui_edittext_create(GUI_ID_EDITTEXT_WIFI_PASSWORD, 2, 2, 2, 2, h, gui_edittext_callback, 0);
                gui_textview_create(GUI_ID_TEXTVIEW_IP_ADDR, 2, 2, 2, 2, h, gui_textview_callback, NULL);
            } else if (GUI_ID_CONTAINER_LOG == id) {
                gui_debugbox_create(GUI_ID_DEBUGBOX_LOG, 2, 2, 2, 2, h, gui_debugbox_callback, 0);
            }
            break;
        }
            
        default:
            break;
    }
    
    return res;
}

/**
 * \brief           Image event function
 */
uint8_t
gui_image_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    id = gui_widget_getid(h);                   /* Get widget ID */
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Init widget callback */
            if (GUI_ID_IMAGE_WIFI_STATUS == id) {
                gui_image_setsource(h, &image_wifi_off);
            } else if (GUI_ID_IMAGE_CONSOLE == id) {
                gui_image_setsource(h, &image_console);
            } else if (GUI_ID_IMAGE_LOG == id) {
                gui_image_setsource(h, &image_log);
            }
            break;
        }
        case GUI_WC_Click: {
            if (GUI_ID_IMAGE_WIFI_STATUS == id) {
                open_container(GUI_ID_CONTAINER_WIFI);
            } else if (GUI_ID_IMAGE_CONSOLE == id) {
                open_container(GUI_ID_CONTAINER_CONSOLE);
            } else if (GUI_ID_IMAGE_LOG == id) {
                open_container(GUI_ID_CONTAINER_LOG);
            }
            break;
        }
        default:
            break;
    }
    return res;
}

/**
 * \brief           Text view event function
 */
uint8_t
gui_textview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    id = gui_widget_getid(h);                   /* Get widget ID */
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Init widget callback */
            if (id == GUI_ID_TEXTVIEW_TIME) {
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_CENTER);
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_RIGHT);
                gui_textview_setcolor(h, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_TRANS);
                gui_widget_settext(h, _GT("15:55"));
            } else if (id == GUI_ID_TEXTVIEW_CPU_USAGE) {
                gui_textview_setvalign(h, GUI_TEXTVIEW_VALIGN_CENTER);
                gui_textview_sethalign(h, GUI_TEXTVIEW_HALIGN_LEFT);
                gui_textview_setcolor(h, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_TRANS);
                gui_widget_settext(h, _GT("EasyGUI by TM"));
            } else if (id == GUI_ID_TEXTVIEW_IP_ADDR) {
                gui_textview_setcolor(h, GUI_TEXTVIEW_COLOR_BG, GUI_COLOR_TRANS);
                gui_widget_setpositionpercent(h, 50.5f, 45.5f);
                gui_widget_setsizepercent(h, 49.0f, 10.0f);
                gui_widget_alloctextmemory(h, 24);
                gui_widget_settext(h, _GT("IP: not connected"));
            }
        }
        default:
            break;
    }
    return res;
}

/**
 * \brief           Listview event function
 */
uint8_t
gui_listview_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    GUI_HANDLE_p tmp;
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    id = gui_widget_getid(h);                   /* Get widget ID */
    
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Init widget callback */
            if (GUI_ID_LISTVIEW_WIFI_APS == id) {
                gui_widget_setpositionpercent(h, 0.5f, 0.5f);
                gui_widget_setsizepercent(h, 49.0f, 99.0f);
                
                /* Set columns */
                gui_listview_addcolumn(h, _GT("Access point"), 300);
                gui_listview_addcolumn(h, _GT("Security"), 100);
            }
            break;
        }
        case GUI_WC_SelectionChanged: {
            int16_t sel = gui_listview_getselection(h);
            if (sel >= 0 && id == GUI_ID_LISTVIEW_WIFI_APS) {
                esp_ecn_t ecn = access_points[sel].ecn;
                
                tmp = gui_widget_getbyid(GUI_ID_EDITTEXT_WIFI_PASSWORD);    /* Get password edit text */
                if (tmp != NULL) {
                    if (ecn != ESP_ECN_OPEN) {  /* If there is no encryption */
                        gui_widget_show(tmp);
                    } else {
                        gui_widget_hide(tmp);
                    }
                }
            }
        }
        default:
            break;
    }
    return res;
}

/**
 * \brief           Debugbox event function
 */
uint8_t
gui_debugbox_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    
    id = gui_widget_getid(h);                   /* Get widget ID */
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Widget just created? */
            if (id == GUI_ID_DEBUGBOX_LOG) {
                gui_widget_setpositionpercent(h, 0.0f, 0.0f);
                gui_widget_setsizepercent(h, 100.0f, 100.0f);
                gui_widget_setfont(h, &GUI_Font_Arial_Bold_18);
                
                gui_debugbox_setmaxitems(h, 50);
                gui_debugbox_addstring(h, _GT("Logging widget"));
            }
            break;
        }
        default:
            break;
    }
    return res;
}

/**
 * \brief           Button event function
 */
uint8_t
gui_button_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    GUI_HANDLE_p tmp;
    
    id = gui_widget_getid(h);                   /* Get widget ID */
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Init widget callback */
            if (id == GUI_ID_BUTTON_WIFI_RELOAD) {
                gui_widget_setpositionpercent(h, 50.5f, 30.5f);
                gui_widget_setsizepercent(h, 49.0f, 14.0f);
                gui_widget_settext(h, _GT("Scan access points"));
            } else if (id == GUI_ID_BUTTON_WIFI_CONNECT) {
                gui_widget_setpositionpercent(h, 50.5f, 15.5f);
                gui_widget_setsizepercent(h, 49.0f, 14.0f);
                if (esp_sta_is_joined()) {
                    gui_widget_settext(h, _GT("Disconnect"));
                } else {
                    gui_widget_settext(h, _GT("Connect"));
                }
            }
            break;
        }
        case GUI_WC_Click: {
            if (id == GUI_ID_BUTTON_WIFI_RELOAD) {
                list_access_points();           /* List access points */
                console_write("Listing access points...\r\n");
            } else if (id == GUI_ID_BUTTON_WIFI_CONNECT) {
                static GUI_Char password[24] = {0}, ssid[24] = {0};
                int16_t selection = -1;
                
                if (!esp_sta_is_joined()) {     /* If there is no AP, try to join */
                    /* Get selection from listview of access points */
                    tmp = gui_widget_getbyid(GUI_ID_LISTVIEW_WIFI_APS);
                    if (tmp != NULL) {
                        selection = gui_listview_getselection(tmp);
                    }
                    if (selection >= 0) {
                        /* Get SSID name */
                        gui_listview_getitemvalue(tmp, selection, 0, ssid, sizeof(ssid) - 1);
                        
                        /* Check for password */
                        tmp = gui_widget_getbyid(GUI_ID_EDITTEXT_WIFI_PASSWORD);
                        if (tmp != NULL) {
                            gui_widget_gettextcopy(tmp, password, sizeof(password) - 1);
                        }
                        
                        /* Start join process */
                        esp_sta_join((char *)ssid, (char *)password, NULL, 0, 0);
                        console_write("Connecting to access point...\r\n");
                    }
                } else {
                    esp_sta_quit(0);            /* Disconnect from AP */
                    console_write("Quiting from access point...\r\n");
                }
            }
            break;
        }
        default:
            break;
    }
    return res;
}

/**
 * \brief           Edit text event function
 */
uint8_t
gui_edittext_callback(GUI_HANDLE_p h, GUI_WC_t ctrl, GUI_WIDGET_PARAM_t* param, GUI_WIDGET_RESULT_t* result) {
    GUI_ID_t id;
    uint8_t res;
    
    id = gui_widget_getid(h);                   /* Get widget ID */
    
    res = gui_widget_processdefaultcallback(h, ctrl, param, result);
    switch (ctrl) {
        case GUI_WC_Init: {                     /* Init widget callback */
            if (id == GUI_ID_EDITTEXT_WIFI_PASSWORD) {
                gui_widget_setpositionpercent(h, 50.5f, 0.5f);
                gui_widget_setsizepercent(h, 49.0f, 14.0f);
                gui_widget_alloctextmemory(h, 128);
                gui_widget_settext(h, _GT("majerle_internet"));
            }
            break;
        }
        default:
            break;
    }
    return res;
}

