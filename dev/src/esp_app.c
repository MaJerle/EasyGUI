/**
 * \file            esp_app.c
 * \brief           ESP application
 */
#include "app.h"

/* Access points visible to ESP device */
esp_ap_t access_points[10];
size_t access_points_count;

/* MQTT client related */
mqtt_client_t* mqtt_client;
static void mqtt_client_evt_fn(mqtt_client_t* client, mqtt_evt_t* evt);
const static mqtt_client_info_t
mqtt_client_info = {
    .id = "stm32f7_esp_device",
    .keep_alive = 10,
};

uint8_t is_publisher;

/**
 * \brief           List access points 
 */
void
list_access_points(void) {
    GUI_HANDLE_p h;
    
    h = gui_widget_getbyid(GUI_ID_LISTVIEW_WIFI_APS);
    if (h != NULL) {   
        gui_listview_removerows(h);             /* Remove all rows from listview */
        gui_listview_setitemstring(h, gui_listview_addrow(h), 0, _GT("Loading access points.."));
        gui_widget_invalidate(h);               /* Force widget invalidation */
    }
    
    /* Start scan command */
    esp_sta_list_ap(NULL, access_points, ESP_ARRAYSIZE(access_points), &access_points_count, 0);
}

/**
 * \brief           Enable WiFi access point
 */
void
enable_wifi_access_point(void) {
    uint8_t mac[6];
    char ssid[24];
    
    esp_set_wifi_mode(ESP_MODE_STA_AP, 1);      /* Set mode to station and access point */
    esp_ap_getmac(mac, 0, 1);
    
    /* Configure SSID name */
    sprintf(ssid, "ESP_%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    
    is_publisher = mac[3] == 0x99;
    
    /* Configure access point */
    esp_ap_configure(ssid, "esp_pwd_123", 13, ESP_ECN_WPA2_PSK, 4, 0, 1, 1);
}

/**
 * \brief           Start MQTT client
 */
void
start_mqtt(void) {
    if (mqtt_client == NULL) {
        mqtt_client = mqtt_client_new(256, 256);/* Create MQTT client */
    }
    mqtt_client_connect(mqtt_client, "193.193.165.37" /* "mqtt.flespi.io" */, 1883, mqtt_client_evt_fn, &mqtt_client_info);
}

/**
 * \brief           MQTT client info callback function
 */
void
mqtt_client_evt_fn(mqtt_client_t* client, mqtt_evt_t* evt) {
    switch (evt->type) {
        case MQTT_EVT_CONNECT: {
            mqtt_conn_status_t status = evt->evt.connect.status;
            
            if (status == MQTT_CONN_STATUS_ACCEPTED) {
                console_write("MQTT connection accepted\r\n");
                mqtt_client_subscribe(client, "stm32f7_topic", MQTT_QOS_EXACTLY_ONCE, NULL);
            } else if (status == MQTT_CONN_STATUS_TCP_FAILED) {
                console_write("MQTT TCP failed. Reconnecting...\r\n");
                start_mqtt();
            }
            break;
        }
        case MQTT_EVT_DISCONNECT: {
            console_write("MQTT TCP disconnected. Reconnecting...\r\n");
            start_mqtt();
            break;
        }
        case MQTT_EVT_SUBSCRIBE: {
            espr_t result = evt->evt.sub_unsub_scribed.res;
            console_write("MQTT subscribed\r\n");
            if (result == espOK) {
                if (is_publisher) {
                    mqtt_client_publish(client, "stm32f7_topic", "stm32f7_topic_data", 18, MQTT_QOS_EXACTLY_ONCE, 0, NULL);
                }
            }
            break;
        }
        case MQTT_EVT_PUBLISHED: {
            printf("Published!\r\n");
            console_write("MQTT packet published\r\n");
            if (is_publisher) {
                mqtt_client_publish(client, "stm32f7_topic", "stm32f7_topic_data", 18, MQTT_QOS_EXACTLY_ONCE, 0, NULL);
            }
            break;
        }
        case MQTT_EVT_PUBLISH_RECV: {
            printf("Publish received!\r\n");
            console_write("MQTT publish received\r\n");
            break;
        }
        default:
            break;
    }
}

/**
 * \brief           ESP callback function for global events
 * \param[in]       cb: Callback event data
 * \return          espOK on success, member of \ref espr_t otherwise
 */
espr_t
esp_cb_func(esp_cb_t* cb) {
    switch (cb->type) {
        /*
         * Wifi list access points finished
         */
        case ESP_CB_STA_LIST_AP: {
            size_t i;
            GUI_HANDLE_p h;
            GUI_LISTVIEW_ROW_p row;
            
            h = gui_widget_getbyid(GUI_ID_LISTVIEW_WIFI_APS);   /* Get list view to hold access points */
            if (h != NULL) {
                gui_listview_removerows(h);
                for (i = 0; i < cb->cb.sta_list_ap.len; i++) {
                    row = gui_listview_addrow(h);
                    if (row == NULL) {
                        break;
                    }
                    gui_listview_setitemstring(h, row, 0, _GT(cb->cb.sta_list_ap.aps[i].ssid));
                    switch (cb->cb.sta_list_ap.aps[i].ecn) {
                        case ESP_ECN_OPEN: gui_listview_setitemstring(h, row, 1, _GT("Open")); break;
                        case ESP_ECN_WPA_PSK: gui_listview_setitemstring(h, row, 1, _GT("WPA")); break;
                        case ESP_ECN_WPA2_PSK: gui_listview_setitemstring(h, row, 1, _GT("WPA2")); break;
                        case ESP_ECN_WPA_WPA2_PSK: gui_listview_setitemstring(h, row, 1, _GT("WPA/2")); break;
                        default: break;
                    }
                }
                gui_widget_invalidate(h);
            }
            console_write("WiFi access points listed\r\n");
            break;
        }
        
        /*
         * Wifi connected
         */
        case ESP_CB_WIFI_CONNECTED: {
            console_write("WiFi connected\r\n");
            break;
        }
        
        /*
         * Wifi has an IP address
         */
        case ESP_CB_WIFI_GOT_IP: {
            gui_image_setsource(gui_widget_getbyid(GUI_ID_IMAGE_WIFI_STATUS), &image_wifi_on);
            gui_widget_settext(gui_widget_getbyid(GUI_ID_BUTTON_WIFI_CONNECT), _GT("Disconnect"));
            start_mqtt();
            console_write("WiFi got IP address\r\n");
            console_write("MQTT TCP connecting...\r\n");
            break;
        }
        
        /*
         * Wifi disconnected from AP
         */
        case ESP_CB_WIFI_DISCONNECTED: {
            gui_image_setsource(gui_widget_getbyid(GUI_ID_IMAGE_WIFI_STATUS), &image_wifi_off);
            gui_widget_settext(gui_widget_getbyid(GUI_ID_BUTTON_WIFI_CONNECT), _GT("Connect"));
            console_write("WiFi disconnected\r\n");
            break;
        }
        default:
            break;
    }
    return espOK;
}
