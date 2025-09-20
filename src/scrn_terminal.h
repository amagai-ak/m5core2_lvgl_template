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
#include <sstream>

class TermBuffer
{
protected:
    int width;
    int height;
    char **buffer;
    int cursor_x;
    int cursor_y;
    bool updated;
    std::string oss;

    void roll_up();

public:
    TermBuffer(int w, int h);
    ~TermBuffer();
    void clear();
    void put_char(char c);
    void put_string(const char* str);
    const char *get_content();
    bool is_updated() { bool u = updated; updated = false; return u; }
    int set_cursor(int x, int y);
};


class ScreenTerminal : public ScreenBase {
protected:
    lv_obj_t *label_content;
    TermBuffer *term_buffer;

public:
    ScreenTerminal();
    ~ScreenTerminal();
    static void callback(lv_event_t *e);
    void setup();
    void loop();
    void print(const char* message);
    void clear();
    void on_swipe(lv_dir_t dir);
    void on_button(lv_obj_t *btn);
};

#endif // SCRN_TERMINAL_H
