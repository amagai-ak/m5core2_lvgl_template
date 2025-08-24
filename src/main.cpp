/**
 * @file main.cpp
 * @author amagai
 * @brief Core2でVLGLを使うための雛型
 * @version 0.1
 * @date 2025-08-24
 * 
 */

#include <Arduino.h>
#include <M5Unified.h>
#include <time.h>

// #define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>

#include "lvgl_setup.h"

#include "scrn_main.h"
#include "scrn_shutdown.h"
#include "screen_id.h"

static const char* time_zone  = "JST-9";

// 各スクリーンのインスタンスを生成
static ScreenMain scrn_main;
static ScreenShutdown scrn_shutdown;

// スクリーンマネージャのインスタンスを生成
static ScreenManager scrn_manager;


/**
 * @brief システム初期化
 * 
 */
void setup() 
{
    auto cfg = M5.config();
    M5.begin(cfg);

    Serial.begin(115200);
    setenv("TZ", time_zone, 1);
    tzset();

    // LVGLの初期化
    lvgl_setup();

    // 各スクリーンのセットアップ
    scrn_main.setup();
    scrn_shutdown.setup();

    // スクリーンマネージャにスクリーンを追加
    // 最初に追加したスクリーンが最初に表示されるスクリーンになる
    scrn_manager.add_screen(SCREEN_ID_MAIN, &scrn_main);
    scrn_manager.add_screen(SCREEN_ID_SHUTDOWN, &scrn_shutdown);
}


void loop() 
{
    M5.update();

    // ここに各種処理を追加

    // LVGLのタスクハンドラを呼び出す
    lv_task_handler();
    scrn_manager.loop();

    delay(5);
}

