/**
 * @file scrn_terminal.cpp
 * @author amagai
 * @brief 
 * @version 0.1
 * @date 2025-09-20
 * 
 * @copyright Copyright (c) 2025
 * 
 * 汎用のテキストターミナル画面
 */
#include <string>
#include <cstring>

#include "scrn_main.h"
#include "screen_id.h"
#include "scrn_terminal.h"

LV_FONT_DECLARE(font_head_up_daisy_16);

/**
 * @brief 画面を1行上にスクロールする
 * 
 */
void TermBuffer::roll_up()
{
    int idx_to, idx_from;
    // 1行目からheight-1行目までを1行上にコピー
    idx_to = xy2index(0, 0);
    idx_from = xy2index(0, 1);
    for (int i = 1; i < height; i++)
    {
        memcpy(&buffer[idx_to], &buffer[idx_from], width);
        idx_to = idx_from;
        idx_from += width + 1;
    }
    memset(&buffer[xy2index(0, height - 1)], ' ', width);
    cursor_y = height - 1;
    cursor_x = 0;
}


/**
 * @brief Construct a new Term Buffer:: Term Buffer object
 * 
 * @param w 画面の幅（文字数）
 * @param h 画面の高さ（行数）
 */
TermBuffer::TermBuffer(int w, int h) : width(w), height(h), cursor_x(0), cursor_y(0)
{
    buffer = new char [(width + 1) * height + 1];
    for (int i = 0; i < height; i++)
    {
        memset(&buffer[xy2index(0, i)], ' ', width);
        buffer[xy2index(width, i)] = '\n';
    }
    buffer[sizeof(buffer) - 1] = '\0';
    updated = false;
}


/**
 * @brief Destroy the Term Buffer:: Term Buffer object
 * 
 */
TermBuffer::~TermBuffer()
{
    delete[] buffer;
}


/**
 * @brief 画面をクリアする
 * 
 */
void TermBuffer::clear()
{
    for (int i = 0; i < height; i++)
    {
        memset(&buffer[xy2index(0, i)], ' ', width);
    }
    cursor_x = 0;
    cursor_y = 0;
    updated = true;
}


/**
 * @brief 1文字を画面に表示する
 * 
 * @param c 表示する文字
 */
void TermBuffer::put_char(char c)
{
    if (cursor_x < width && cursor_y < height)
    {
        if( c == '\n' )
        {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= height)
            {
                roll_up();
                updated = true;
            }
            return;
        }
        else if( c == '\r' )
        {
            cursor_x = 0;
            return;
        }
        else if( c == '\t' )
        {
            int spaces = 4 - (cursor_x % 4);
            for( int i = 0; i < spaces; i++ )
            {
                put_char(' ');
            }
            return;
        }
        else if( c < 32 || c > 126 )
        {
            // 非表示文字は無視
            return;
        }

        buffer[xy2index(cursor_x, cursor_y)] = c;
        cursor_x++;
        if (cursor_x >= width)
        {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= height)
            {
                roll_up();
            }
        }
    }
    updated = true;
}


/**
 * @brief 文字列を画面に表示する
 * 
 * @param str 表示する文字列
 */
void TermBuffer::put_string(const char* str)
{
    for (const char* p = str; *p != '\0'; p++)
    {
        put_char(*p);
    }
}


/**
 * @brief 画面の内容を取得する
 * 
 * @return const char* 
 */
const char *TermBuffer::get_content()
{
    return buffer;
}


/**
 * @brief カーソル位置を設定する
 * 
 * @param x X座標
 * @param y Y座標
 * @return int 成功なら0，失敗なら-1
 */
int TermBuffer::set_cursor(int x, int y)
{
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        cursor_x = x;
        cursor_y = y;
        return 0; // 成功
    }
    return -1; // 失敗
}


/**
 * @brief セットアップ
 * 
 */
void ScreenTerminal::setup()
{
    ScreenBase::setup();

    lv_obj_set_style_bg_color(lv_screen, lv_color_make(1, 22, 13), 0);

    label_content = lv_label_create(lv_screen);
    lv_obj_set_width(label_content, 320);
    lv_obj_set_height(label_content, 240);
    lv_obj_set_style_text_color(label_content, lv_color_make(9, 166, 100), 0);
    lv_obj_set_style_text_font(label_content, &font_head_up_daisy_16, 0);
    lv_label_set_long_mode(label_content, LV_LABEL_LONG_CLIP);
    lv_obj_align(label_content, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(label_content, "");

    // スワイプジェスチャーの有効化
    lv_obj_add_event_cb(lv_screen, callback, LV_EVENT_GESTURE, this);

    // ターミナルクリア
    clear();

    // 定期的に画面を更新するタイマーを設定．
    // 100msごとに更新．
    // これ以上の周期で更新してもどうせ読めないので無駄な描画が起きないようにする．
    lv_timer_create(callback_timer, 100, this);

}


void ScreenTerminal::loop()
{
}


ScreenTerminal::ScreenTerminal()
{
    // 35文字x15行．利用するフォントサイズで調整する．
    term_buffer = new TermBuffer(35, 15);
    label_content = nullptr;
}


void ScreenTerminal::print(const char* message)
{
    term_buffer->put_string(message);
}


void ScreenTerminal::println(const char* message)
{
    term_buffer->put_string(message);
    term_buffer->put_char('\n');
}


void ScreenTerminal::printf(const char* format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    
    int size = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // バッファが足りない場合はエラーメッセージを表示
    if (size < 0 || size >= sizeof(buffer))
    {
        snprintf(buffer, sizeof(buffer), "Error formatting string");
    }

    print(buffer);
}


void ScreenTerminal::clear()
{
    term_buffer->clear();
}


void ScreenTerminal::update()
{
    if( term_buffer->is_updated() && label_content != nullptr )
    {
        lv_label_set_text(label_content, term_buffer->get_content());
    }
}

void ScreenTerminal::callback(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED)
    {
        ScreenTerminal *scrn = static_cast<ScreenTerminal *>(lv_event_get_user_data(e));
        scrn->on_button(obj);
    }
    else if (code == LV_EVENT_GESTURE)
    {
        ScreenTerminal *scrn = static_cast<ScreenTerminal *>(lv_event_get_user_data(e));
        lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_get_act());
        scrn->on_swipe(dir);
    }
}


void ScreenTerminal::callback_timer(lv_timer_t *timer)
{
    ScreenTerminal *scrn = static_cast<ScreenTerminal *>(lv_timer_get_user_data(timer));
    scrn->update();
}


void ScreenTerminal::on_button(lv_obj_t *btn)
{
    // ボタンが押されたときの処理
}

void ScreenTerminal::on_swipe(lv_dir_t dir)
{
    if (dir == LV_DIR_LEFT)
    {
        // 左スワイプでMain画面へ
        change_screen(SCREEN_ID_MAIN, SCREEN_ANIM_LEFT);
    }
}

ScreenTerminal::~ScreenTerminal()
{
    delete term_buffer;
}
