#include "lvgl/lvgl.h"
#include "lv_drivers/wayland/wayland.h"
#include "lv_demos/lv_demo.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux wayland device init*/
    wayland_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[WAYLAND_HOR_RES * WAYLAND_VER_RES];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, WAYLAND_HOR_RES * WAYLAND_VER_RES);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = wayland_flush;
    disp_drv.hor_res    = WAYLAND_HOR_RES;
    disp_drv.ver_res    = WAYLAND_VER_RES;
    lv_disp_drv_register(&disp_drv);

    /*Add keyboard*/
    lv_indev_drv_t indev_drv_kb;
    lv_indev_drv_init(&indev_drv_kb);
    indev_drv_kb.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_kb.read_cb = wayland_keyboard_read;
    lv_indev_drv_register(&indev_drv_kb);

    /*Add touchscreen*/
    lv_indev_drv_t indev_drv_touch;
    lv_indev_drv_init(&indev_drv_touch);
    indev_drv_touch.type = LV_INDEV_TYPE_POINTER;
    indev_drv_touch.read_cb = wayland_touch_read;
    lv_indev_drv_register(&indev_drv_touch);

    /*Add mouse*/
    lv_indev_drv_t indev_drv_mouse;
    lv_indev_drv_init(&indev_drv_mouse);
    indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
    indev_drv_mouse.read_cb = wayland_pointer_read;
    lv_indev_drv_register(&indev_drv_mouse);

    /*Create a Demo*/
    lv_demo_widgets();

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
    }

    return 0;
}

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
