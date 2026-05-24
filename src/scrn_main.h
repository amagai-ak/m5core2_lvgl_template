
#ifndef SCRN_MAIN_H
#define SCRN_MAIN_H

#include "screen_base.h"

class ScreenMain : public ScreenBase
{
protected:
    lv_obj_t *label_hello;
    lv_obj_t *label_counter;
    lv_obj_t *label_counter_val;
    lv_obj_t *btn_shutdown;
    lv_obj_t *label_timer;
    lv_obj_t *label_timer_val;


public:
    unsigned int loopcounter;
    unsigned int timercounter;
    void setup();
    void loop();
    static void callback(lv_event_t *e);
    void on_button(lv_obj_t *btn);
    void on_swipe(lv_dir_t dir);
    void on_btn_A(void);
    void on_btn_C(void);
    void on_timer(void);
};

#endif // SCRN_MAIN_H
