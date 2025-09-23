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

/**
 * @brief 画面遷移アニメーションの種類
 */
enum {
    SCREEN_ANIM_NONE  = 0,  ///< アニメーションなし
    SCREEN_ANIM_LEFT  = 1,  ///< 左へ動くアニメーション
    SCREEN_ANIM_RIGHT = 2   ///< 右へ動くアニメーション
};


class ScreenManager;

/**
 * @brief スクリーン生成のための基本クラス
 * 
 */
class ScreenBase
{
    protected:
        lv_obj_t* lv_screen;                ///< LVGLスクリーンオブジェクト
        ScreenManager* screen_manager;      ///< スクリーンマネージャー
        int screen_id;                      ///< スクリーンID

    public:
        ScreenBase();
        virtual ~ScreenBase();

        virtual void setup();
        /** @brief スクリーンがアクティブである間，繰り返し呼ばれる */
        virtual void loop() {}

        /** @brief スクリーンのロードが行われる直前に呼ばれる */
        virtual int on_load(void) { return 0; }

        /** @brief スクリーンのアンロードが行われる直前に呼ばれる */
        virtual int on_unload(void) { return 0; }

        /** @brief LVGLスクリーンオブジェクトを取得する */
        lv_obj_t * get_lv_screen() const { return lv_screen; }

        /** @brief スクリーンマネージャーを設定する */
        void set_screen_manager(ScreenManager* manager)
        {
            screen_manager = manager;
        }

        /** @brief スクリーンIDを設定する */
        void set_screen_id(int id)
        {
            screen_id = id;
        }

        /** @brief スクリーンIDを取得する */
        int get_screen_id() const
        {
            return screen_id;
        }

        int change_screen(int id, int dir = 0);

        /** @brief このスクリーンが現在アクティブかどうかを返す */
        bool is_active() const
        {
            return get_lv_screen() == lv_scr_act();
        }
};


/**
 * @brief スクリーン管理クラス
 * 
 * ScreenManagerは複数のScreenBaseオブジェクトを管理し、現在のスクリーンを切り替える機能を提供。
 */
class ScreenManager
{
    protected:
        ScreenBase* current_screen;         ///< 現在のスクリーン
        int current_screen_id;              ///< 現在のスクリーンID
        std::vector<ScreenBase*> screen_list; ///< スクリーンのリスト

        void set_current_screen(ScreenBase* screen, int dir = 0);

    public:
        ScreenManager() : current_screen(nullptr), current_screen_id(-1) {}

        ~ScreenManager()
        {
            screen_list.clear();
        }

        /// 現在表示中のスクリーンのloop処理を呼び出す
        void loop()
        {
            if (current_screen)
            {
                current_screen->loop();
            }
        }

        void add_screen(int id, ScreenBase* screen);

        ScreenBase* get_screen(int id);     ///< スクリーンIDからScreenBaseオブジェクトを取得
        int change_screen(int id, int dir = 0);
        ScreenBase* get_current_screen() const { return current_screen; }   ///< 現在のScreenBaseを取得
        int get_current_screen_id() const { return current_screen_id; }     ///< 現在のスクリーンIDを取得
};

#endif // SCREEN_BASE_H
