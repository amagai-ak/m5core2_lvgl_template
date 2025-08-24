/**
 * @file screen_base.cpp
 * @author amagai
 * @brief スクリーン制御のための基本クラスとスクリーン管理クラス
 * @version 0.1
 * @date 2025-07-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "screen_base.h"


/////////////////////// ScreenBase Implementation ///////////////////////

/**
 * @brief Construct a new Screen Base:: Screen Base object
 * 
 */
ScreenBase::ScreenBase()
{
    lv_screen = NULL;
    screen_manager = NULL;
    screen_id = -1;
}


/**
 * @brief Destroy the Screen Base:: Screen Base object
 * 
 */
ScreenBase::~ScreenBase()
{
    if (lv_screen)
    {
        lv_obj_del(lv_screen);
        lv_screen = NULL;
    }
}


/**
 * @brief スクリーンのセットアップ．スクリーンの親となるオブジェクトを生成する．
 * 
 */
void ScreenBase::setup()
{
    lv_screen = lv_obj_create(NULL);
    return;
}


/**
 * @brief スクリーンを変更する
 * 
 * @param id スクリーンID
 * @param dir アニメーションの方向（0:なし、1:左から右、2:右から左）
 * @return int 成功した場合は0、失敗した場合は-1
 */
int ScreenBase::change_screen(int id, int dir)
{
    int rtn;

    if (screen_manager)
    {
        rtn = screen_manager->change_screen(id, dir);
    }
    else
    {
        rtn = -1;
    }
    return rtn;
}



/////////////////////// ScreenManager Implementation ///////////////////////

/**
 * @brief 指定されたIDのスクリーンを取得する
 * 
 * @param id スクリーンID
 * @return ScreenBase* スクリーンが見つかった場合はそのポインタ、見つからない場合はnullptr
 */
ScreenBase* ScreenManager::get_screen(int id)
{
    for (auto& screen : screen_list)
    {
        if (screen->get_screen_id() == id)
        {
            return screen;
        }
    }
    return nullptr; // 見つからない場合はnullptrを返す
}


/**
 * @brief スクリーンIDを指定して表示スクリーンを変更する
 * 
 * @param id スクリーンID
 * @param dir アニメーションの方向（0:なし、1:左から右、2:右から左）
 * @return int 成功した場合は0、失敗した場合は-1
 */
int ScreenManager::change_screen(int id, int dir)
{
    ScreenBase* screen = get_screen(id);
    if (screen)
    {
        set_current_screen(screen, dir);
        current_screen_id = id;
    }
    else
    {
        return -1; // スクリーンが見つからない場合はエラー
    }
    return 0;
}


/**
 * @brief スクリーンを追加する
 * 
 * @param id スクリーンID
 * @param screen ScreenBaseのポインタ
 */
void ScreenManager::add_screen(int id, ScreenBase* screen)
{
    screen_list.push_back(screen);
    screen->set_screen_manager(this);
    screen->set_screen_id(id);
    if (current_screen == nullptr)
    {
        set_current_screen(screen); // 最初のスクリーンを設定
        current_screen_id = id;
    }
}


/**
 * @brief 表示対象スクリーンを設定する
 * 
 * @param screen 表示対象とするスクリーンオブジェクト
 * @param dir アニメーションの方向（0:なし、1:左から右、2:右から左）
 */
void ScreenManager::set_current_screen(ScreenBase* screen, int dir)
{
    if( current_screen == screen )
        return; // 既に同じスクリーンが設定されている場合は何もしない

    if (current_screen)
    {
        current_screen->on_unload(); // 現在のスクリーンをアンロード
    }
    current_screen = screen;
    if (current_screen)
    {
        current_screen->on_load();
        switch( dir )
        {
            case SCREEN_ANIM_RIGHT: // 左から右
                lv_scr_load_anim(current_screen->get_lv_screen(), LV_SCR_LOAD_ANIM_MOVE_RIGHT, 300, 0, false);
                break;
            case SCREEN_ANIM_LEFT: // 右から左
                lv_scr_load_anim(current_screen->get_lv_screen(), LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
                break;
            default: // アニメーションなし
                lv_scr_load(current_screen->get_lv_screen());
                break;
        }
    }
}
