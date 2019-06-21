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
    BTN_ENTRY(ID_BTN_MAINWIN_DEBUGBOX, _GT("Debugbox"), ID_WIN_DEBUGBOX, demo_create_feature_debugbox)
    BTN_ENTRY(ID_BTN_MAINWIN_DROPDOWN, _GT("Dropdown"), ID_WIN_DROPDOWN, demo_create_feature_dropdown)
    BTN_ENTRY(ID_BTN_MAINWIN_CHECKBOX, _GT("Checkbox"), ID_WIN_CHECKBOX, demo_create_feature_checkbox)
    BTN_ENTRY(ID_BTN_MAINWIN_RADIO, _GT("Radiobox"), ID_WIN_RADIO, demo_create_feature_radio)
};

static uint8_t  main_win_btn_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result);
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
main_win_btn_callback(gui_handle_p h, gui_widget_evt_t evt, gui_evt_param_t* const param, gui_evt_result_t* const result) {
    uint8_t ret = gui_widget_processdefaultcallback(h, evt, param, result);
    switch (evt) {
        case GUI_EVT_CLICK: {
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
    gui_dim_t x1_s;
    gui_dim_t x1_e;
    gui_dim_t y1_s;
    gui_dim_t y1_e;
    gui_dim_t x2_s;
    gui_dim_t x2_e;
    gui_dim_t y2_s;
    gui_dim_t y2_e;
    
    uint32_t anim_time;
    uint8_t state;
    uint32_t delay_after;
} ts_data_t;

#define TS_ENTRY(state_, x1, y1, x2, y2, delay_after_) {\
    .x1_s = x1,                                         \
    .x1_e = x1,                                         \
    .y1_s = y1,                                         \
    .y1_e = y1,                                         \
    .x2_s = x2,                                         \
    .x2_e = x2,                                         \
    .y2_s = y2,                                         \
    .y2_e = y2,                                         \
    .state = state_,                                    \
    .anim_time = 0,                                     \
    .delay_after = delay_after_                         \
},

#define TS_ENTRY_ANIM(state_, x1_s_, x1_e_, y1_s_, y1_e_, anim_time_, delay_after_)     \
TS_ENTRY_ANIM2(state_, x1_s_, x1_e_, y1_s_, y1_e_, -1, -1, -1, -1, anim_time_, delay_after_)

#define TS_ENTRY_ANIM2(state_, x1_s_, x1_e_, y1_s_, y1_e_, x2_s_, x2_e_, y2_s_, y2_e_, anim_time_, delay_after_) {\
    .x1_s = x1_s_,                                      \
    .x1_e = x1_e_,                                      \
    .y1_s = y1_s_,                                      \
    .y1_e = y1_e_,                                      \
    .x2_s = x2_s_,                                      \
    .x2_e = x2_e_,                                      \
    .y2_s = y2_s_,                                      \
    .y2_e = y2_e_,                                      \
    .state = state_,                                    \
    .anim_time = anim_time_,                            \
    .delay_after = delay_after_                         \
},

#define TS_CLICK(x1, y1, delay_after_)                  \
TS_ENTRY(1, x1, y1, -1, -1, 200)                        \
TS_ENTRY(0, x1, y1, -1, -1, delay_after_)               \

#define TS_DBLCLICK(x1, y1, delay_after_)               \
TS_CLICK(x1, y1, 100)                                   \
TS_CLICK(x1, y1, delay_after_)                          \

static ts_data_t
ts_data[] = {
    /* Scroll left part to top */
    TS_ENTRY_ANIM(1, 10, 10, 10, 300, 500, 0)
    TS_ENTRY(0, -1, -1, -1, -1, 500)

    /* Click on list container */
    TS_CLICK(30, 300, 500)

    /* Animate right container*/
    TS_ENTRY_ANIM(1, 785, 785, 300, 10, 1000, 0)
    TS_ENTRY(0, -1, -1, -1, -1, 100)

    /* Animate right container*/
    TS_ENTRY_ANIM(1, 785, 785, 10, 400, 2000, 0)
    TS_ENTRY(0, -1, -1, -1, -1, 100)
    TS_ENTRY_ANIM(1, 785, 785, 10, 400, 2000, 0)
    TS_ENTRY(0, -1, -1, -1, -1, 100)
    
    ///* Click graph widget */
    //TS_CLICK(100, 150, 1000)

    ///* Move 2 fingers on graph */
    //TS_ENTRY_ANIM(1, 400, 500, 200, 200, 2000, 0)
    //TS_ENTRY(0, -1, -1, -1, -1, 1000)

    ///* Just add second finger and zoom-in */
    //TS_ENTRY_ANIM2(1, 500, 400, 200, 200, 700, 800, 200, 200, 1000, 0)
    //TS_ENTRY(0, -1, -1, -1, -1, 1000)

    ///* Zoom second finger and zoom-out */
    //TS_ENTRY_ANIM2(1, 500, 500, 100, 240, 500, 500, 500, 260, 1000, 0)
    //TS_ENTRY(0, -1, -1, -1, -1, 1000)

    ///* Reset graph zoom */
    //TS_DBLCLICK(400, 200, 500)

    ///* Click and scroll on left side of screen */
    //TS_ENTRY_ANIM(1, 100, 100, 400, 0, 500, 0)
    //TS_ENTRY(0, -1, -1, -1, -1, 500)
    //
    ///* Now click radio button */
    //TS_CLICK(100, 150, 500)
    //
    ///* Now click checkbox button */
    //TS_CLICK(100, 400, 500)
    //
    ///* Now click checkbox widget 4 times */
    //TS_CLICK(400, 40, 500)
    //TS_CLICK(400, 40, 500)
    //TS_CLICK(400, 40, 500)
    //TS_CLICK(400, 40, 500)
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
    gui_delay(1000);
    
    /* Process events */
    for (i = 0, entry = &ts_data[i]; i < GUI_ARRAYSIZE(ts_data); i++, entry = &ts_data[i]) {        
        /* Process animations */
        if (entry->anim_time) {
            float x1_d, y1_d, x2_d, y2_d;
            uint32_t steps, i, step_time = 30;
            
            /* Get number of loops to execute */
            steps = entry->anim_time / step_time;
            
            /* Calculate difference and divide by steps */
            x1_d = (float)(entry->x1_e - entry->x1_s) / (float)steps;
            y1_d = (float)(entry->y1_e - entry->y1_s) / (float)steps;
            x2_d = (float)(entry->x2_e - entry->x2_s) / (float)steps;
            y2_d = (float)(entry->y2_e - entry->y2_s) / (float)steps;
            
            data.status = GUI_TOUCH_STATE_PRESSED;  /* In animation, status is always pressed */
            
            /* Send events to GUI */
            for (i = 0; i < steps; i++) {
                data.count = 1;             /* We have at least first touch */
                data.count += (entry->x2_s >= 0 && entry->y2_s >= 0 && entry->x2_e >= 0 && entry->y2_e >= 0);   /* Check if we have second */
                
                /* Set coordinates */
                data.x[0] = entry->x1_s + (gui_dim_t)(x1_d * (float)i);
                data.y[0] = entry->y1_s + (gui_dim_t)(y1_d * (float)i);
                data.x[1] = entry->x2_s + (gui_dim_t)(x2_d * (float)i);
                data.y[1] = entry->y2_s + (gui_dim_t)(y2_d * (float)i);
                
                gui_input_touchadd(&data);  /* Send input */
                gui_delay(step_time);       /* Delay for step time */
            }
            
            /* Delay rest of the time */
            gui_delay(entry->anim_time - steps * step_time);
        } else {
            data.time = gui_sys_now();      /* Get current time */
            data.status = entry->state ? GUI_TOUCH_STATE_PRESSED : GUI_TOUCH_STATE_RELEASED;/* Find state of touch here */
            
            if (data.status == GUI_TOUCH_STATE_PRESSED) {
                data.count = 1;             /* We have at least first touch */
                data.count += (entry->x2_s >= 0 && entry->y2_s >= 0);   /* Check if we have second */
                
                data.x[0] = entry->x1_s;
                data.y[0] = entry->y1_s;
                data.x[1] = entry->x2_s;
                data.y[1] = entry->y2_s;
            }
            gui_input_touchadd(&data);      /* Send input */
        }
        if (entry->delay_after) {
            gui_delay(entry->delay_after);  /* Delay after */
        }
    }
    
    while (1) {
        gui_delay(1000);
    }
}
