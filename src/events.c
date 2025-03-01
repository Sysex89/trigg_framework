#include "framework.h"

uint8_t trg_get_button(void) {
	uint8_t button_state = 0;
	button_state |= !gpio_get(TRG_BTN_W) << 0;
	button_state |= !gpio_get(TRG_BTN_A) << 1;
	button_state |= !gpio_get(TRG_BTN_S) << 2;
	button_state |= !gpio_get(TRG_BTN_D) << 3;
	button_state |= !gpio_get(TRG_BTN_I) << 4;
	button_state |= !gpio_get(TRG_BTN_J) << 5;
	button_state |= !gpio_get(TRG_BTN_K) << 6;
	button_state |= !gpio_get(TRG_BTN_L) << 7;
	return button_state;
}

void init_keypress_event_loop(bool callback_trigger_always) {
	g_keypress_event_loop_running = true;
	g_keypress_callback_trigger_always = callback_trigger_always;
	multicore_launch_core1(keypress_event_loop);
	stdio_printf("[Core1] :: Keypress event loop started\n");
}

void stop_keypress_event_loop(void) {
	g_keypress_event_loop_running = false;
	stdio_printf("[Core1] :: Keypress event loop stopped\n");
	multicore_reset_core1();
}

void reg_keypress_callback(keypress_callback_t callback) {
	if (callback != NULL) {
		g_keypress_callback = callback;
	}
}

void keypress_event_loop(void) {
	uint8_t button_state = 0;
	uint8_t last_button_state = 0;
	while (g_keypress_event_loop_running) {
		button_state = trg_get_button();
		if (button_state != last_button_state ||
			g_keypress_callback_trigger_always) {
			if (g_keypress_callback != NULL) {
				g_keypress_callback(button_state);
			}
			last_button_state = button_state;
		}
	}
}
