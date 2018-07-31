#include "demo.h"

/* Define list of all buttons on main screen */
typedef struct {
    gui_id_t id;
    const uint8_t* label;
    gui_id_t win_id;
    win_data_t data;
} main_win_btn_t;

#define BTN_ENTRY(vid, vlabel, vwin_id, create_fn)            { .id = vid, .label = vlabel, .win_id = vwin_id, .data = { .feature_create_fn = create_fn } },

/* Create list of all buttons */
static main_win_btn_t
btns[] = {
    BTN_ENTRY(ID_BTN_MAINWIN_WINDOW, _GT("Window"), ID_WIN_WINDOW, demo_create_feature_window)
    BTN_ENTRY(ID_BTN_MAINWIN_BUTTONS, _GT("Buttons"), ID_WIN_BUTTONS, demo_create_feature_button)
    BTN_ENTRY(ID_BTN_MAINWIN_EDITTEXT, _GT("Edit text"), ID_WIN_EDITTEXT, demo_create_feature_edit_text)
    BTN_ENTRY(ID_BTN_MAINWIN_GRAPH, _GT("Graph"), ID_WIN_GRAPH, demo_create_feature_graph)
    BTN_ENTRY(ID_BTN_MAINWIN_PROGBAR, _GT("Progress bar"), ID_WIN_PROGBAR, demo_create_feature_progbar)
    BTN_ENTRY(ID_BTN_MAINWIN_LED, _GT("Led"), ID_WIN_LED, demo_create_feature_led)
    BTN_ENTRY(ID_BTN_MAINWIN_LIST_CONTAINER, _GT("ListContainer"), ID_WIN_LIST_CONTAINER, demo_create_feature_list_container)
    BTN_ENTRY(ID_BTN_MAINWIN_TEXTVIEW, _GT("Text view"), ID_WIN_TEXTVIEW, demo_create_feature_text_view)
    BTN_ENTRY(ID_BTN_MAINWIN_SLIDER, _GT("Slider"), ID_WIN_SLIDER, demo_create_feature_slider)
    BTN_ENTRY(ID_BTN_MAINWIN_LISTBOX, _GT("Listbox"), ID_WIN_LISTBOX, demo_create_feature_listbox)
    BTN_ENTRY(ID_BTN_MAINWIN_LISTVIEW, _GT("Listview"), ID_WIN_LISTVIEW, demo_create_feature_listview)
    BTN_ENTRY(ID_BTN_MAINWIN_DROPDOWN, _GT("Dropdown"), ID_WIN_DROPDOWN, demo_create_feature_dropdown)
    BTN_ENTRY(ID_BTN_MAINWIN_CHECKBOX, _GT("Checkbox"), ID_WIN_CHECKBOX, demo_create_feature_checkbox)
    BTN_ENTRY(ID_BTN_MAINWIN_RADIO, _GT("Radiobox"), ID_WIN_RADIO, demo_create_feature_radio)
};

static uint8_t  main_win_btn_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result);
static void     touch_demo_thread(void * const arg);

/**
 * \brief           Initialize demonstration
 */
void
demo_init(void) {
    size_t i;
    gui_handle_p h, parent;
                                                
    gui_protect(1);
    
    gui_widget_setfontdefault(&GUI_Font_Arial_Bold_18);     /* Set default font */
    gui_keyboard_create();
    
    parent = gui_window_getdesktop();           /* Get desktop widget */

    /* Create list container on left side for all buttons */
    parent = gui_listcontainer_create(0, 0, 0, 10, 10, parent, NULL, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE);
    gui_widget_setpositionpercent(parent, 1, 1);
    gui_widget_setsizepercent(parent, 28, 98);
    gui_listcontainer_setcolor(parent, GUI_LISTCONTAINER_COLOR_BG, GUI_COLOR_DARKGRAY);
    gui_listcontainer_setmode(parent, GUI_LISTCONTAINER_MODE_VERTICAL);
    gui_widget_setpadding(parent, 5);
    gui_widget_setignoreinvalidate(parent, 0, 1);

    /* Print all buttons on container */
    for (i = 0; i < GUI_COUNT_OF(btns); i++) {
        h = gui_button_create(btns[i].id,
            0, 0, 0, 0,
            parent, main_win_btn_callback, GUI_FLAG_WIDGET_CREATE_IGNORE_INVALIDATE
        );
        gui_widget_setpositionpercent(h, 0, GUI_FLOAT(i * 10));
        gui_widget_setsizepercent(h, 100, 8.5f);
        gui_widget_settext(h, _GT(btns[i].label));
        btns[i].data.id = btns[i].win_id;
        btns[i].data.title = btns[i].label;
        gui_widget_setuserdata(h, (void *)&btns[i].data);
        gui_widget_setignoreinvalidate(h, 0, 1);
    }

    gui_unprotect(1);
    
    //gui_sys_thread_create(NULL, "touch_demo", touch_demo_thread, NULL, GUI_SYS_THREAD_SS, GUI_SYS_THREAD_PRIO);
}

/**
 * \brief           Callback function for main window buttons
 */
static uint8_t
main_win_btn_callback(gui_handle_p h, gui_wc_t wc, gui_widget_param_t* param, gui_widget_result_t* result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, wc, param, result);
    switch (wc) {
        case GUI_WC_Click: {
            demo_create_feature(gui_widget_getuserdata(h), 0);      /* Create feature */
            break;
        }
        default: break;
    }
    return ret;
}


/**
 * \brief           This part is for test only :)
 */

typedef struct {
    gui_dim_t x1;
    gui_dim_t y1;
    gui_dim_t x2;
    gui_dim_t y2;
    uint8_t state;
    uint32_t delay_after;
} ts_data_t;

#define TS_ENTRY(state, x1, y1, x2, y2, delay_after) {x1, y1, x2, y2, state, delay_after},

static ts_data_t
ts_data[] = {
    /* Click and scroll on left side of screen */
    TS_ENTRY(1, 100, 400, -1, -1, 100)
    TS_ENTRY(1, 100, 375, -1, -1, 100)
    TS_ENTRY(1, 100, 350, -1, -1, 100)
    TS_ENTRY(1, 100, 325, -1, -1, 100)
    TS_ENTRY(1, 100, 300, -1, -1, 100)
    TS_ENTRY(1, 100, 275, -1, -1, 100)
    TS_ENTRY(1, 100, 250, -1, -1, 100)
    TS_ENTRY(1, 100, 225, -1, -1, 100)
    TS_ENTRY(1, 100, 200, -1, -1, 100)
    TS_ENTRY(1, 100, 175, -1, -1, 100)
    TS_ENTRY(1, 100, 150, -1, -1, 100)
    TS_ENTRY(1, 100, 125, -1, -1, 100)
    TS_ENTRY(1, 100, 100, -1, -1, 100)
    TS_ENTRY(1, 100,  75, -1, -1, 100)
    TS_ENTRY(1, 100,  50, -1, -1, 100)
    TS_ENTRY(1, 100,  25, -1, -1, 100)
    TS_ENTRY(1, 100,   0, -1, -1, 100)
    TS_ENTRY(0, 0, 0, 0, 0, 1000)
    
    /* Now click radio button */
    TS_ENTRY(1, 100, 450, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 1000)
    
    /* Now click checkbox button */
    TS_ENTRY(1, 100, 400, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 1000)
    
    /* Now click checkbox widget */
    TS_ENTRY(1, 400, 40, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 500)
    TS_ENTRY(1, 400, 40, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 500)
    TS_ENTRY(1, 400, 40, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 500)
    TS_ENTRY(1, 400, 40, -1, -1, 100)
    TS_ENTRY(0, -1, -1, -1, -1, 500)
};

/**
 * \brief           Fake input data
 */
static void
touch_demo_thread(void * const arg) {
    size_t i;
    gui_touch_data_t data;
    ts_data_t* entry;
    
    /* Initial delay */
    gui_delay(2000);
    
    /* Process events */
    for (i = 0, entry = &ts_data[i]; i < GUI_ARRAYSIZE(ts_data); i++, entry = &ts_data[i]) {
        data.time = gui_sys_now();          /* Get current time */
        data.status = entry->state ? GUI_TOUCH_STATE_PRESSED : GUI_TOUCH_STATE_RELEASED;
        if (entry->state) {
            data.count = 1;                 /* At least one is valid */
            data.count += (entry->x2 >= 0 && entry->y2 >= 0);   /* Check if second is valid too */
            
            data.x[0] = entry->x1;
            data.x[1] = entry->x2;
            data.y[0] = entry->y1;
            data.y[1] = entry->y2;
        }
        gui_input_touchadd(&data);          /* Send input */
        gui_delay(entry->delay_after);      /* Delay after */
    }
    
    while (1) {
        gui_delay(1000);
    }
}
