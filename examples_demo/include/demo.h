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

/* List of widget IDs */
#define ID_BTN_BASIC                        (GUI_ID_USER + 0x100)
#define ID_BTN_MAINWIN_BUTTONS              (ID_BTN_BASIC + 0x001)
#define ID_BTN_MAINWIN_EDITTEXT             (ID_BTN_BASIC + 0x002)
#define ID_BTN_MAINWIN_GRAPH                (ID_BTN_BASIC + 0x003)
#define ID_BTN_MAINWIN_PROGBAR              (ID_BTN_BASIC + 0x004)
#define ID_BTN_MAINWIN_LED                  (ID_BTN_BASIC + 0x005)

/* List of windows IDs */
#define ID_WIN_BASIC                        (GUI_ID_USER + 0x200)
#define ID_WIN_BUTTONS				        (ID_WIN_BASIC + 0x001)
#define ID_WIN_EDITTEXT				        (ID_WIN_BASIC + 0x002)
#define ID_WIN_GRAPH				        (ID_WIN_BASIC + 0x003)
#define ID_WIN_PROGBAR				        (ID_WIN_BASIC + 0x004)
#define ID_WIN_LED				            (ID_WIN_BASIC + 0x005)

/* List of fonts */
extern gui_const gui_font_t GUI_Font_Calibri_Bold_8;
extern gui_const gui_font_t GUI_Font_Arial_Bold_18;

/* Functions for export */
void    demo_init(void);
void    demo_create_feature_window(gui_id_t win_id);

#ifdef __cplusplus
};
#endif

#endif /* __DEMO_H */