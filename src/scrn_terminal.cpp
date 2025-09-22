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
 * @param vh 表示する行数（h以下）。指定しない場合はhと同じ
 */
TermBuffer::TermBuffer(int w, int h, int vh)
{
    width = w;
    height = h;
    if( vh > 0 && vh < h )
    {
        visible_height = vh;
    }
    else
    {
        visible_height = height;
    }
    cursor_x = 0;
    cursor_y = 0;
    ymax = 0;
    buffer = new char [(width + 1) * height];
    view_buffer = new char [(width + 1) * visible_height + 1];
    for (int i = 0; i < height; i++)
    {
        memset(&buffer[xy2index(0, i)], ' ', width);
        buffer[xy2index(width, i)] = '\n';
    }
    for( int i = 0; i < visible_height; i++ )
    {
        view_buffer[i * (width + 1) + width] = '\n';
    }
    view_buffer[visible_height * (width + 1)] = '\0';
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
    ymax = 0;
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
    if( cursor_y > ymax )
        ymax = cursor_y;
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
const char *TermBuffer::get_content(int start_line)
{
    if( start_line > ymax - visible_height +1 ) 
        start_line = ymax - visible_height +1;
    if( start_line < 0 ) 
        start_line = 0;
    int idx_to = 0;
    int idx_from = xy2index(0, start_line);
    for( int i = 0; i < visible_height; i++ )
    {
        memcpy( &view_buffer[idx_to], &buffer[idx_from], width + 1 );
        idx_to += width + 1;
        idx_from += width + 1;
    }
    return view_buffer;
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


///////////////////////////////////////////////////////////////
// ScreenTerminal
///////////////////////////////////////////////////////////////

/**
 * @brief Construct a new Screen Terminal:: Screen Terminal object
 * 
 */
ScreenTerminal::ScreenTerminal()
{
    // 35文字x14行．利用するフォントサイズで調整する．
    visible_lines = 14; // 表示可能な行数
    buffer_lines = 30;  // バッファの行数
    scroll_start_line = buffer_lines - visible_lines;
    touch_dy = 0;
    term_buffer = new TermBuffer(35, buffer_lines, visible_lines);
    label_content = nullptr;
}


/**
 * @brief セットアップ
 * 
 */
void ScreenTerminal::setup()
{
    ScreenBase::setup();

    lv_obj_set_style_bg_color(lv_screen, lv_color_make(1, 22, 13), 0);

    // さらに行数があることを示すラベル
    label_more_lines = lv_label_create(lv_screen);
    lv_obj_set_style_text_color(label_more_lines, lv_color_make(0, 128, 128), 0);
    lv_obj_set_style_text_font(label_more_lines, &lv_font_montserrat_14, 0);
    lv_label_set_text(label_more_lines, LV_SYMBOL_DOWN);
    lv_obj_align(label_more_lines, LV_ALIGN_BOTTOM_MID, 0, -2);
    lv_obj_add_flag(label_more_lines, LV_OBJ_FLAG_HIDDEN); // 最初は非表示

    label_content = lv_label_create(lv_screen);
    lv_obj_set_width(label_content, 320);
    lv_obj_set_height(label_content, 240);
    lv_obj_set_style_text_color(label_content, lv_color_make(9, 166, 100), 0);
    lv_obj_set_style_text_font(label_content, &font_head_up_daisy_16, 0);
    lv_label_set_long_mode(label_content, LV_LABEL_LONG_CLIP);
    lv_obj_align(label_content, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_label_set_text(label_content, "");
    // ラベルへのタッチイベントの有効化．デフォルトではラベルはタッチを無視する．
    lv_obj_add_flag(label_content, LV_OBJ_FLAG_CLICKABLE);

    font_height = lv_font_get_line_height(&font_head_up_daisy_16);
    touch_dy = 0;

    // スワイプジェスチャーの有効化
    lv_obj_add_event_cb(lv_screen, callback, LV_EVENT_GESTURE, this);

    // ラベルをドラッグ操作でスクロールできるようにする
    lv_obj_add_event_cb(label_content, callback, LV_EVENT_PRESSED, this);
    lv_obj_add_event_cb(label_content, callback, LV_EVENT_PRESSING, this);

    // ターミナルクリア
    clear();

    // 定期的に画面を更新するタイマーを設定．
    // 100msごとに更新．
    // これ以上の周期で更新してもどうせ読めないので無駄な描画が起きないようにする．
    lv_timer_create(callback_timer, 100, this);

    // printf("Font height: %d\n", font_height);
}


void ScreenTerminal::loop()
{
}


/**
 * @brief 画面に文字列を表示する
 * 
 * @param message 表示する文字列
 */
void ScreenTerminal::print(const char* message)
{
    term_buffer->put_string(message);
}


/**
 * @brief 画面に文字列を表示する．改行付き．
 * 
 * @param message 表示する文字列
 */
void ScreenTerminal::println(const char* message)
{
    term_buffer->put_string(message);
    term_buffer->put_char('\n');
}


/**
 * @brief 画面にフォーマットされた文字列を表示する
 * 
 * @param format フォーマット文字列
 * @param ... 可変長引数
 */
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
        lv_label_set_text(label_content, term_buffer->get_content(scroll_start_line));
    }
}


void ScreenTerminal::callback(lv_event_t *e)
{
    lv_obj_t *obj = (lv_obj_t *)lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);
    ScreenTerminal *scrn = static_cast<ScreenTerminal *>(lv_event_get_user_data(e));
    if (code == LV_EVENT_CLICKED)
    {
        scrn->on_button(obj);
    }
    else if (code == LV_EVENT_PRESSED)
    {
        lv_point_t pos;
        lv_indev_get_point(lv_indev_get_act(), &pos);
        scrn->on_press(obj, &pos);
    }
    else if (code == LV_EVENT_PRESSING)
    {
        lv_point_t vec;
        lv_indev_get_vect(lv_indev_get_act(), &vec);
        scrn->on_pressing(obj, &vec);
    }
    else if (code == LV_EVENT_GESTURE)
    {
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


/**
 * @brief タッチ開始時の処理
 * 
 * @param obj 
 * @param pos 
 */
void ScreenTerminal::on_press(lv_obj_t *obj, lv_point_t *pos)
{
    if( obj == label_content )
    {
        touch_dy = 0;
    }
}


/**
 * @brief ドラッグ操作中の処理
 * 
 * @param obj 
 * @param vec 
 */
void ScreenTerminal::on_pressing(lv_obj_t *obj, lv_point_t *vec)
{
    if( obj == label_content )
    {
        touch_dy += vec->y;
        if( touch_dy > font_height )
        {
            // 下方向にスクロール
            if( scroll_start_line > 0 )
            {
                scroll_start_line--;
                lv_label_set_text(label_content, term_buffer->get_content(scroll_start_line));
            }
            touch_dy = 0;
        }
        else if( touch_dy < -font_height )
        {
            // 上方向にスクロール
            if( scroll_start_line < term_buffer->get_line_count() - visible_lines )
            {
                scroll_start_line++;
                lv_label_set_text(label_content, term_buffer->get_content(scroll_start_line));
            }
            touch_dy = 0;
        }
        // 下端が見えていない場合には矢印を表示する
        if( scroll_start_line < term_buffer->get_line_count() - visible_lines )
            lv_obj_clear_flag(label_more_lines, LV_OBJ_FLAG_HIDDEN);
        else
            lv_obj_add_flag(label_more_lines, LV_OBJ_FLAG_HIDDEN);
    }
}


/**
 * @brief スワイプ操作の処理
 * 
 * @param dir スワイプ方向
 */
void ScreenTerminal::on_swipe(lv_dir_t dir)
{
    if (dir == LV_DIR_LEFT)
    {
        // 左スワイプでMain画面へ
        change_screen(SCREEN_ID_MAIN, SCREEN_ANIM_LEFT);
    }
}


/**
 * @brief Destroy the Screen Terminal object
 */
ScreenTerminal::~ScreenTerminal()
{
    delete term_buffer;
}
