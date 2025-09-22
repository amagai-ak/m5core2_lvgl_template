/**
 * @file scrn_terminal.h
 * @author amagai
 * @brief 
 * @version 0.1
 * @date 2025-09-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef SCRN_TERMINAL_H
#define SCRN_TERMINAL_H
#include "screen_base.h"
#include <cstdarg>

class TermBuffer
{
protected:
    int width;
    int height;
    int visible_height;
    char *buffer;
    char *view_buffer;
    int cursor_x;
    int cursor_y;
    int ymax;
    bool updated;

    void roll_up();
    int xy2index(int x, int y) { return y * (width + 1) + x; }

public:
    TermBuffer(int w, int h, int vh = -1);
    ~TermBuffer();
    void clear();
    void put_char(char c);
    void put_string(const char* str);
    const char *get_content(int start_line = 0);
    bool is_updated() { bool u = updated; updated = false; return u; }
    int set_cursor(int x, int y);
    int get_cursor_x() { return cursor_x; };
    int get_cursor_y() { return cursor_y; }
    int get_width() { return width; };
    int get_height() { return height; };
    int get_line_count() { return ymax; };
};


class ScreenTerminal : public ScreenBase {
protected:
    lv_obj_t *label_content;
    lv_obj_t *label_more_lines;
    TermBuffer *term_buffer;
    int scroll_start_line;
    int touch_dy;
    int font_height;
    int visible_lines;
    int buffer_lines;

public:
    ScreenTerminal();
    ~ScreenTerminal();
    static void callback(lv_event_t *e);
    static void callback_timer(lv_timer_t *timer);
    void setup();
    void loop();
    void update();
    void print(const char* message);
    void println(const char* message);
    void printf(const char* format, ...);
    int get_width() { return term_buffer->get_width(); }
    int get_height() { return term_buffer->get_height(); }
    void clear();
    void on_swipe(lv_dir_t dir);
    void on_button(lv_obj_t *btn);
    void on_press(lv_obj_t *obj, lv_point_t *pos);
    void on_pressing(lv_obj_t *obj, lv_point_t *vec);
};

#endif // SCRN_TERMINAL_H
