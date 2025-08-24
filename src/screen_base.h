/**
 * @file screen_base.h
 * @author amagai
 * @brief スクリーン制御のための基本クラスとスクリーン管理クラス
 * @version 0.1
 * @date 2025-07-20
 * 
 */

#ifndef SCREEN_BASE_H
#define SCREEN_BASE_H
#include "lvgl.h"
#include <vector>

#define SCREEN_ANIM_NONE 0
#define SCREEN_ANIM_LEFT 1
#define SCREEN_ANIM_RIGHT 2

class ScreenManager;

// スクリーン生成のための基本クラス
class ScreenBase
{
    protected:
        lv_obj_t* lv_screen;
        ScreenManager* screen_manager;
        int screen_id;

    public:
        ScreenBase();
        virtual ~ScreenBase();

        virtual void setup();
        virtual void loop() {}

        // スクリーンのロードが行われる直前に呼ばれる
        virtual int on_load(void) { return 0; }
        // スクリーンのアンロードが行われる直前に呼ばれる
        virtual int on_unload(void) { return 0; }

        lv_obj_t * get_lv_screen() { return lv_screen; }

        void set_screen_manager(ScreenManager* manager)
        {
            screen_manager = manager;
        }

        void set_screen_id(int id)
        {
            screen_id = id;
        }

        int get_screen_id() const
        {
            return screen_id;
        }

        int change_screen(int id, int dir = 0);
};


// スクリーン管理クラス
// ScreenManagerは複数のScreenBaseオブジェクトを管理し、現在のスクリーンを切り替える機能を提供。
class ScreenManager
{
    protected:
        ScreenBase* current_screen;
        int current_screen_id;
        // スクリーンのリスト
        std::vector<ScreenBase*> screen_list;

        void set_current_screen(ScreenBase* screen, int dir = 0);

    public:
        ScreenManager() : current_screen(nullptr), current_screen_id(-1) {}

        ~ScreenManager()
        {
            screen_list.clear();
        }


        void loop()
        {
            if (current_screen)
            {
                current_screen->loop();
            }
        }

        void add_screen(int id, ScreenBase* screen);

        ScreenBase* get_screen(int id);
        int change_screen(int id, int dir = 0);
        ScreenBase* get_current_screen() const { return current_screen; }
        int get_current_screen_id() const { return current_screen_id; }
};

#endif // SCREEN_BASE_H
