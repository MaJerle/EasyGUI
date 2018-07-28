#ifndef __DEMO_H
#define __DEMO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/gui.h"
#include "gui/gui_keyboard.h"
#include "widget/gui_button.h"
#include "widget/gui_window.h"
#include "widget/gui_edittext.h"
#include "widget/gui_graph.h"
#include "widget/gui_progbar.h"
#include "widget/gui_led.h"
#include "widget/gui_list_container.h"
#include "widget/gui_container.h"
#include "widget/gui_checkbox.h"
#include "widget/gui_radio.h"

/**
 * \brief           Window data structure
 */
typedef struct {
    gui_dim_t id;
    const gui_char* title;
    void(*feature_create_fn)(gui_handle_p parent);
} win_data_t;

/* List of widget IDs */
#define ID_BTN_BASIC                        (GUI_ID_USER + 0x100)
#define ID_BTN_MAINWIN_BUTTONS              (ID_BTN_BASIC + 0x001)
#define ID_BTN_MAINWIN_EDITTEXT             (ID_BTN_BASIC + 0x002)
#define ID_BTN_MAINWIN_GRAPH                (ID_BTN_BASIC + 0x003)
#define ID_BTN_MAINWIN_PROGBAR              (ID_BTN_BASIC + 0x004)
#define ID_BTN_MAINWIN_LED                  (ID_BTN_BASIC + 0x005)
#define ID_BTN_MAINWIN_LIST_CONTAINER       (ID_BTN_BASIC + 0x006)
#define ID_BTN_MAINWIN_WINDOW               (ID_BTN_BASIC + 0x007)

/* List of windows IDs */
#define ID_WIN_BASIC                        (GUI_ID_USER + 0x200)
#define ID_WIN_BUTTONS				        (ID_WIN_BASIC + 0x001)
#define ID_WIN_EDITTEXT				        (ID_WIN_BASIC + 0x002)
#define ID_WIN_GRAPH				        (ID_WIN_BASIC + 0x003)
#define ID_WIN_PROGBAR				        (ID_WIN_BASIC + 0x004)
#define ID_WIN_LED				            (ID_WIN_BASIC + 0x005)
#define ID_WIN_LIST_CONTAINER			    (ID_WIN_BASIC + 0x006)
#define ID_WIN_WINDOW                       (ID_WIN_BASIC + 0x007)

/* Graph ID */
#define ID_WIDGET_BASIC                     (GUI_ID_USER + 0x300)
#define ID_GRAPH                            (ID_WIDGET_BASIC + 0x001)

/* List of fonts */
extern gui_const gui_font_t GUI_Font_Calibri_Bold_8;
extern gui_const gui_font_t GUI_Font_Arial_Bold_18;

/* Functions for export */
void    demo_init(void);
void    demo_create_feature(win_data_t* data);

void    demo_create_feature_window(gui_handle_p parent);
void    demo_create_feature_list_container(gui_handle_p parent);
void    demo_create_feature_button(gui_handle_p parent);
void    demo_create_feature_edit_text(gui_handle_p parent);
void    demo_create_feature_progbar(gui_handle_p parent);
void    demo_create_feature_graph(gui_handle_p parent);
void    demo_create_feature_dialog(gui_handle_p parent);
void    demo_create_feature_image(gui_handle_p parent);
void    demo_create_feature_listview(gui_handle_p parent);
void    demo_create_feature_listbox(gui_handle_p parent);
void    demo_create_feature_radio(gui_handle_p parent);
void    demo_create_feature_slider(gui_handle_p parent);
void    demo_create_feature_textview(gui_handle_p parent);
void    demo_create_feature_led(gui_handle_p parent);
void    demo_create_feature_checkbox(gui_handle_p parent);
void    demo_create_feature_dropdown(gui_handle_p parent);

#ifdef __cplusplus
};
#endif

#endif /* __DEMO_H */
