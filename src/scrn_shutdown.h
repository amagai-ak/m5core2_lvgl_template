
#ifndef SCRN_SHUTDOWN_H
#define SCRN_SHUTDOWN_H

#include "screen_base.h"
#include "screen_id.h"

class ScreenShutdown : public ScreenBase
{
protected:
    lv_obj_t *label_goodbye;
    lv_obj_t *btn_resume;
    lv_obj_t *btn_poweroff;

public:
    void setup();
    void loop();
    static void callback(lv_event_t *e);
    void on_button(lv_obj_t *btn);
    void on_swipe(lv_dir_t dir);
};

#endif // SCRN_SHUTDOWN_H
