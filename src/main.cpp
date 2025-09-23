/**
 * @file main.cpp
 * @author amagai
 * @brief Core2でVLGLを使うための雛型
 * @version 0.1
 * @date 2025-08-24
 * 
 */

/**
 * @mainpage M5Core2 + LVGL Template
 * @author amagai
 * @version 0.1
 * @copyright Copyright (c) 2025
 * 
 * @section 概要
 * M5Stack Core2でLVGLを使うための雛型プロジェクトです．
 * LVGLの他に，ArduinoフレームワークとM5Unifiedライブラリを使用しています．
 * 
 * LVGLのバージョンは9.2.2を使用しています．https://docs.lvgl.io/9.2/
 */

#include <Arduino.h>
#include <M5Unified.h>
#include <time.h>

// #define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>

#include "lvgl_setup.h"

#include "scrn_main.h"
#include "scrn_shutdown.h"
#include "scrn_terminal.h"
#include "screen_id.h"

static const char* time_zone  = "JST-9";

// 各スクリーンのインスタンスを生成
static ScreenMain scrn_main;
static ScreenShutdown scrn_shutdown;
static ScreenTerminal scrn_terminal;

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

    Serial.setRxBufferSize(1024);
    Serial.begin(115200);
    setenv("TZ", time_zone, 1);
    tzset();

    // LVGLの初期化
    lvgl_setup();

    // 各スクリーンのセットアップ
    scrn_main.setup();
    scrn_shutdown.setup();
    scrn_terminal.setup();

    // スクリーンマネージャにスクリーンを追加
    // 最初に追加したスクリーンが最初に表示されるスクリーンになる
    scrn_manager.add_screen(SCREEN_ID_MAIN, &scrn_main);
    scrn_manager.add_screen(SCREEN_ID_SHUTDOWN, &scrn_shutdown);
    scrn_manager.add_screen(SCREEN_ID_TERMINAL, &scrn_terminal);

    scrn_terminal.print("Terminal Screen Initialized.\n");
    scrn_terminal.print("0123456789012345678901234567890123456789\n");
}


void loop() 
{
    static uint32_t prev_sec = 0;
    uint32_t sec;

    M5.update();

    // ここに各種処理を追加

    sec = millis() / 1000;
    if (sec != prev_sec)
    {
        // 1秒ごとの処理をここに追加
        prev_sec = sec;
        scrn_terminal.printf("Uptime: %u sec\n", sec);
    }

    // 現在表示されているスクリーンのループ処理
    scrn_manager.loop();

    // LVGLのタスクハンドラを呼び出す
    lv_task_handler();

    delay(5);
}

