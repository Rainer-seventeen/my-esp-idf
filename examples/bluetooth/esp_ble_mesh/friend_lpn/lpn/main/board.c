#include <stdio.h>
#include "esp_log.h"
#include "iot_button.h"

#define TAG "BOARD"

#define BUTTON_IO_NUM           9
#define BUTTON_ACTIVE_LEVEL     0

extern void example_ble_mesh_send_frnd_poll(void);

static void button_tap_cb(void* arg)
{
    example_ble_mesh_send_frnd_poll();
}

static void board_button_init(void)
{
    button_handle_t btn_handle = iot_button_create(BUTTON_IO_NUM, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_RELEASE, button_tap_cb, "RELEASE");
    }
}

void board_init(void)
{
    board_button_init();
}