#include <iostream>
#include <string>
#include <sstream>

#include "scrn_main.h"
#include "screen_id.h"

LV_FONT_DECLARE(myrica_m_24);

void ScreenMain::callback(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        ScreenMain *scrn = static_cast<ScreenMain *>(lv_event_get_user_data(e));
        scrn->on_button(obj);
    }
    else if (code == LV_EVENT_GESTURE)
    {
        ScreenMain *scrn = static_cast<ScreenMain *>(lv_event_get_user_data(e));
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        scrn->on_swipe(dir);
    }
}


void ScreenMain::setup()
{
    // Setup code for the main screen
    ScreenBase::setup();

    lv_obj_set_style_bg_color(lv_screen, lv_color_make(0, 0, 0), 0);

    label_hello = lv_label_create(lv_screen);
    lv_label_set_text(label_hello, "Hello, World!");
    lv_obj_set_style_text_color(label_hello, lv_color_make(255, 255, 255), 0);
    lv_obj_align(label_hello, LV_ALIGN_CENTER, 0, 0);

    // シャットダウンボタン
    btn_shutdown = lv_btn_create(lv_screen);
    lv_obj_set_size(btn_shutdown, 200, 50);
    lv_obj_align(btn_shutdown, LV_ALIGN_CENTER, 0, 40);
    lv_obj_add_event_cb(btn_shutdown, callback, LV_EVENT_CLICKED, this);

    lv_obj_t *label = lv_label_create(btn_shutdown);
    lv_label_set_text(label, "Shutdown");
    lv_obj_center(label);

    // カウンタ
    label_counter = lv_label_create(lv_screen);
    lv_obj_set_style_text_font(label_counter, &myrica_m_24, 0);
    lv_label_set_text(label_counter, "Counter: ");
    lv_obj_set_style_text_color(label_counter, lv_color_make(255, 255, 255), 0);
    lv_obj_align(label_counter, LV_ALIGN_CENTER, -100, 80);

    label_counter_val = lv_label_create(lv_screen);
    lv_obj_set_style_text_font(label_counter_val, &myrica_m_24, 0);
    lv_label_set_text(label_counter_val, "0");
    lv_obj_set_style_text_color(label_counter_val, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_text_align(label_counter_val, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(label_counter_val, LV_ALIGN_CENTER, 0, 80);

    // スワイプジェスチャーの有効化
    lv_obj_add_event_cb(lv_screen, callback, LV_EVENT_GESTURE, this);

    counter = 0;
}


void ScreenMain::loop()
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", ++counter);
    lv_label_set_text(label_counter_val, buf);
}


void ScreenMain::on_button(lv_obj_t *btn)
{
    if (btn == btn_shutdown)
    {
        // Shutdown button pressed
        change_screen(SCREEN_ID_SHUTDOWN, SCREEN_ANIM_RIGHT);
    }
}


void ScreenMain::on_swipe(lv_dir_t dir)
{
    if (dir == LV_DIR_LEFT)
    {
        // 左スワイプでシャットダウン画面へ
        change_screen(SCREEN_ID_SHUTDOWN, SCREEN_ANIM_LEFT);
    }
    else if (dir == LV_DIR_RIGHT)
    {
        // 右スワイプでシャットダウン画面へ
        change_screen(SCREEN_ID_SHUTDOWN, SCREEN_ANIM_RIGHT);
    }
}
