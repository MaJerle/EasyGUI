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

/* Station IP address */
esp_ip_t esp_ip;

/**
 * \brief           List access points 
 */
void
list_access_points(void) {
    gui_handle_p h;
    
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
 * \brief           Start netconn server
 */
espr_t
start_server(void) {
    /* Start server */
    esp_sys_thread_create(NULL, "esp_server_netconn", (esp_sys_thread_fn)netconn_server_thread, NULL, ESP_SYS_THREAD_SS, ESP_SYS_THREAD_PRIO);
    return espOK;
}

/**
 * \brief           Enable WiFi access point
 */
void
enable_wifi_access_point(void) {
    esp_mac_t mac;
    static char ssid[24];
    
    esp_set_wifi_mode(ESP_MODE_STA_AP, 1);      /* Set mode to station and access point */
    esp_ap_getmac(&mac, 0, 1);
    
    /* Configure SSID name */
    sprintf(ssid, "ESP_%02X%02X%02X%02X%02X%02X", mac.mac[0], mac.mac[1], mac.mac[2], mac.mac[3], mac.mac[4], mac.mac[5]);
    
    /* Configure access point */
    if (esp_ap_configure(ssid, "esp_pwd_123", 13, ESP_ECN_WPA2_PSK, 4, 0, 1, 1) == espOK) {
        printf("Access point configured and enabled\r\n");
    } else {
        printf("Cannot configure access point\r\n");
    }
}

/**
 * \brief           Send data over MQTT protocol
 * \param[in]       data: Data to send
 * \param[in]       len: Data length
 * \return          espOK on success, member of \ref espr_t otherwise
 */
espr_t
mqtt_send_data(const void* data, size_t len) {
    return mqtt_client_publish(mqtt_client, "stm32f7_topic", data, len, MQTT_QOS_EXACTLY_ONCE, 0, NULL);
}

/**
 * \brief           Start MQTT client
 */
void
start_mqtt(void) {
    if (mqtt_client == NULL) {
        mqtt_client = mqtt_client_new(256, 256);/* Create MQTT client */
    }
    if (esp_sta_is_joined()) {
        mqtt_client_connect(mqtt_client, "test.mosquitto.org" /* "mqtt.flespi.io" */, 1883, mqtt_client_evt_fn, &mqtt_client_info);
    } else {
        console_write("MQTT WiFi not connected. Waiting for WIFI network..\r\n");
    }
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
                mqtt_client_subscribe(client, "stm32f7_topic0", MQTT_QOS_EXACTLY_ONCE, NULL);
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
            ESP_UNUSED(result);
            console_write("MQTT subscribe event\r\n");
            break;
        }
        case MQTT_EVT_PUBLISHED: {
            printf("Published!\r\n");
            console_write("MQTT packet published\r\n");
            break;
        }
        case MQTT_EVT_PUBLISH_RECV: {
            char str[20];
            const char* t = evt->evt.publish_recv.payload;
            printf("Publish received!\r\n");
            console_write("MQTT publish received\r\n");
            sprintf(str, "%.*s", evt->evt.publish_recv.payload_len, t);
            console_write(str);
            
            sprintf(str, "%.*s", evt->evt.publish_recv.topic_len, evt->evt.publish_recv.topic);
            //mqtt_client_publish(client, str, evt->evt.publish_recv.payload, evt->evt.publish_recv.payload_len, MQTT_QOS_EXACTLY_ONCE,  0, NULL);
            break;
        }
        case MQTT_EVT_KEEP_ALIVE: {
            console_write("MQTT keep-alive\r\n");
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
            gui_handle_p h;
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
                    if (!strcmp(cb->cb.sta_list_ap.aps[i].ssid, "TilenM_ST")) {
                        esp_sta_join(cb->cb.sta_list_ap.aps[i].ssid, "its private", NULL, 0, 0);
                    }
                }
                gui_widget_invalidate(h);
            }
            console_write("WiFi access points listed\r\n");
            if (cb->cb.sta_list_ap.len == 0) {
                list_access_points();
            }
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
            esp_sta_getip(&esp_ip, NULL, NULL, 0, 0);   /* Get current station IP address */
            break;
        }
        
        /*
         * Wifi disconnected from AP
         */
        case ESP_CB_WIFI_DISCONNECTED: {
            gui_image_setsource(gui_widget_getbyid(GUI_ID_IMAGE_WIFI_STATUS), &image_wifi_off);
            gui_widget_settext(gui_widget_getbyid(GUI_ID_BUTTON_WIFI_CONNECT), _GT("Connect"));
            console_write("WiFi disconnected\r\n");
            lcd_update_ip(NULL);
            break;
        }
        
        case ESP_CB_WIFI_IP_ACQUIRED: {
            char sp[50];
            gui_image_setsource(gui_widget_getbyid(GUI_ID_IMAGE_WIFI_STATUS), &image_wifi_on);
            gui_widget_settext(gui_widget_getbyid(GUI_ID_BUTTON_WIFI_CONNECT), _GT("Disconnect"));
            sprintf(sp, "WiFi got IP address: %d.%d.%d.%d\r\n", esp_ip.ip[0], esp_ip.ip[1], esp_ip.ip[2], esp_ip.ip[3]);
            lcd_update_ip(&esp_ip);
            console_write(sp);
            console_write("MQTT TCP connecting...\r\n");
            start_mqtt();
            break;
        }
        
        /*
         * Event relative to manual connection to access point
         */
        case ESP_CB_STA_JOIN_AP: {
            espr_t status = esp_evt_sta_join_ap_get_status(cb);
            
            switch (status) {
                case espOK: console_write("ESP_JOIN: Joined OK!\r\n"); break;
                case espERRCONNTIMEOUT: console_write("ESP_JOIN: Connection timeout!\r\n"); break;
                case espERRPASS: console_write("ESP_JOIN: Wrong password!\r\n"); break;
                case espERRNOAP: console_write("ESP_JOIN: No access point with desired name!\r\n"); break;
                case espERRCONNFAIL: console_write("ESP_JOIN: Connection fail!\r\n"); break;
                default: console_write("ESP_JOIN: Unknown status!\r\n");
            }
            break;
        }
        default:
            break;
    }
    return espOK;
}
