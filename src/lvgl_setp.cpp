#include <M5Unified.h>
#include <lvgl.h>
#include <esp_timer.h>

#include "lvgl_setup.h"

static lv_display_t *display;
static lv_indev_t *indev;

// 画面解像度
constexpr int32_t HOR_RES=320;
constexpr int32_t VER_RES=240;

// 1ピクセルあたりのバイト数
constexpr int32_t BYTES_PER_PIXEL = (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565));

// 描画バッファの分割数．大きいほど省メモリになるが，描画が遅くなる．
constexpr int32_t DISPBUF_DIVIDE = 10;


static uint32_t my_tick_function() 
{
    return (esp_timer_get_time() / 1000LL);
}


static void my_display_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) 
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    lv_draw_sw_rgb565_swap(px_map, w*h);
    M5.Display.pushImageDMA<uint16_t>(area->x1, area->y1, w, h, (uint16_t *)px_map);
    lv_disp_flush_ready(disp);
}

static void my_touchpad_read(lv_indev_t * drv, lv_indev_data_t * data) 
{
    M5.update();
    auto count = M5.Touch.getCount();

    if ( count == 0 ) 
    {
        data->state = LV_INDEV_STATE_RELEASED;
    } 
    else 
    {
        auto touch = M5.Touch.getDetail(0);
        data->state = LV_INDEV_STATE_PRESSED; 
        data->point.x = touch.x;
        data->point.y = touch.y;
    }
}


void lvgl_setup()
{
    lv_init();
    lv_tick_set_cb(my_tick_function);

    display = lv_display_create(HOR_RES, VER_RES);
    lv_display_set_flush_cb(display, my_display_flush);

    alignas(4) static uint8_t buf1[HOR_RES * VER_RES / DISPBUF_DIVIDE * BYTES_PER_PIXEL]; 
    lv_display_set_buffers(display, buf1, nullptr, sizeof(buf1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
}

