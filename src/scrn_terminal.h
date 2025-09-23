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
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

/**
 * @brief 端末用バッファクラス
 * 
 */
class TermBuffer
{
protected:
    int width;                  ///< 画面の幅（文字数）
    int height;                 ///< 画面の高さ（行数）
    int visible_height;        ///< 表示領域の高さ（行数）
    char *buffer;               ///< バッファ本体
    char *view_buffer;          ///< 表示用バッファ
    int cursor_x;              ///< カーソルのX座標
    int cursor_y;              ///< カーソルのY座標
    int ymax;                  ///< バッファの最大Y座標
    bool updated;              ///< 更新フラグ

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
    int get_line_count() { return (ymax + 1); };
};


/**
 * @brief テキストターミナル画面クラス
 * 
 */
class ScreenTerminal : public ScreenBase {
protected:
    lv_obj_t *label_content;    ///< ターミナル表示用ラベル
    lv_obj_t *label_more_lines; ///< さらに行数があることを示すラベル
    TermBuffer *term_buffer;     ///< ターミナル用バッファ
    int scroll_start_line;      ///< スクロール開始行
    int touch_dy;               ///< タッチドラッグ量
    int font_height;            ///< フォントの高さ
    int visible_lines;          ///< 表示領域の行数
    int buffer_lines;           ///< バッファ全体の行数
    bool auto_follow;          ///< 自動スクロールフラグ
    SemaphoreHandle_t mutex;   ///< ミューテックス

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
