#include "framework.h"

void move_player(uint8_t keycode);

int main(void) {
	stdio_init_all();

	int rc = trg_led_init();
	hard_assert(rc == TRG_OK);
	rc = trg_buttons_init();
	hard_assert(rc == TRG_OK);
	rc = trg_audio_init();
	hard_assert(rc == TRG_OK);
	// Blink the LEDs twice to indicate that the program has started
	blink_twice();
	tft_init();
	tft_fill_screen(COLOR_GREEN);

	init_keypress_event_loop(true);
	reg_keypress_callback(move_player);

	while (true) {
		uint8_t button_state = trg_get_button();
		for (int i = 7; i >= 0; i--) {
			stdio_printf("%d", (button_state >> i) & 1);
		}
		stdio_printf("\n");
		sleep_ms(50);
	}

	stop_keypress_event_loop();
}

uint16_t g_x = TRG_SCR_WIDTH / 2;
uint16_t g_y = TRG_SCR_HEIGHT / 2;

/// ==== Audio globals ========================================================
static trg_audio_t g_audio = {0};

void move_player(uint8_t keycode) {
	static uint8_t delay = 100;
	if (keycode & KEY_W) {
		if (g_y < TRG_SCR_HEIGHT - 1)
			g_y++;
	}
	if (keycode & KEY_A) {
		if (g_x > 0)
			g_x--;
	}
	if (keycode & KEY_S) {
		if (g_y > 0)
			g_y--;
	}
	if (keycode & KEY_D) {
		if (g_x < TRG_SCR_WIDTH - 1)
			g_x++;
	}
	if (keycode & KEY_I) {
		if (delay > 20)
			delay -= 10;
	}
	if (keycode & KEY_K) {
		if (delay < 200)
			delay += 10;
	}
	if (keycode & KEY_J) {
		trg_audio_play_tone(440.0f, 100);
	}
	if (keycode & KEY_L) {
		trg_audio_play_tone(880.0f, 100);
	}
	tft_draw_pixel(g_x, g_y, COLOR_RED);
	sleep_ms(delay);
}

/// ==== Function definitions =================================================

int trg_led_init(void) {
	gpio_init(TRG_LED_L);
	gpio_set_dir(TRG_LED_L, GPIO_OUT);
	gpio_init(TRG_LED_R);
	gpio_set_dir(TRG_LED_R, GPIO_OUT);
	return TRG_OK;
}

int trg_buttons_init(void) {
	//     W
	gpio_init(TRG_BTN_W);
	gpio_set_dir(TRG_BTN_W, GPIO_IN);
	gpio_pull_up(TRG_BTN_W);

	//     A
	gpio_init(TRG_BTN_A);
	gpio_set_dir(TRG_BTN_A, GPIO_IN);
	gpio_pull_up(TRG_BTN_A);

	//     S
	gpio_init(TRG_BTN_S);
	gpio_set_dir(TRG_BTN_S, GPIO_IN);
	gpio_pull_up(TRG_BTN_S);

	//     D
	gpio_init(TRG_BTN_D);
	gpio_set_dir(TRG_BTN_D, GPIO_IN);
	gpio_pull_up(TRG_BTN_D);

	//     I
	gpio_init(TRG_BTN_I);
	gpio_set_dir(TRG_BTN_I, GPIO_IN);
	gpio_pull_up(TRG_BTN_I);

	//     J
	gpio_init(TRG_BTN_J);
	gpio_set_dir(TRG_BTN_J, GPIO_IN);
	gpio_pull_up(TRG_BTN_J);

	//     K
	gpio_init(TRG_BTN_K);
	gpio_set_dir(TRG_BTN_K, GPIO_IN);
	gpio_pull_up(TRG_BTN_K);

	//     L
	gpio_init(TRG_BTN_L);
	gpio_set_dir(TRG_BTN_L, GPIO_IN);
	gpio_pull_up(TRG_BTN_L);
	return TRG_OK;
}

// void test_buttons(uint16_t *x, uint16_t *y) {
// 	if (!gpio_get(TRG_BTN_W) || !gpio_get(TRG_BTN_A) || !gpio_get(TRG_BTN_S) ||
// 		!gpio_get(TRG_BTN_D)) {
// 		gpio_put(TRG_LED_L, 1);
// 		sleep_ms(100);
// 		stdio_printf("WASD\n");
// 		(*x)++;
// 	}
// 	if (!gpio_get(TRG_BTN_I) || !gpio_get(TRG_BTN_J) || !gpio_get(TRG_BTN_K) ||
// 		!gpio_get(TRG_BTN_L)) {
// 		gpio_put(TRG_LED_R, 1);
// 		sleep_ms(100);
// 		stdio_printf("IJKL\n");
// 		(*y)++;
// 	}
// 	gpio_put(TRG_LED_L, 0);
// 	gpio_put(TRG_LED_R, 0);
// }

void blink_twice(void) {
	for (int i = 0; i < 2; i++) {
		gpio_put(TRG_LED_L, 1);
		gpio_put(TRG_LED_R, 1);
		sleep_ms(500);
		gpio_put(TRG_LED_L, 0);
		gpio_put(TRG_LED_R, 0);
		sleep_ms(500);
	}
}

void tft_init(void) {
	// Initialize SPI port at 20 MHz
	spi_init(SPI_PORT, 20000000);

	// Set SPI pins
	gpio_set_function(TRG_SCR_SCK, GPIO_FUNC_SPI);
	gpio_set_function(TRG_SCR_MOSI, GPIO_FUNC_SPI);

	// Set up control pins as GPIO
	gpio_init(TRG_SCR_CS);
	gpio_init(TRG_SCR_DC);
	gpio_init(TRG_SCR_RESET);
	gpio_init(TRG_SCR_LITE);

	// Set pins as outputs
	gpio_set_dir(TRG_SCR_CS, GPIO_OUT);
	gpio_set_dir(TRG_SCR_DC, GPIO_OUT);
	gpio_set_dir(TRG_SCR_RESET, GPIO_OUT);
	gpio_set_dir(TRG_SCR_LITE, GPIO_OUT);

	// Set initial pin states
	gpio_put(TRG_SCR_CS, 1);	// Chip select high (inactive)
	gpio_put(TRG_SCR_DC, 1);	// Data mode
	gpio_put(TRG_SCR_LITE, 1);	// Backlight on

	// Reset the display
	gpio_put(TRG_SCR_RESET, 1);
	sleep_ms(5);
	gpio_put(TRG_SCR_RESET, 0);
	sleep_ms(20);
	gpio_put(TRG_SCR_RESET, 1);
	sleep_ms(150);

	// Send initialization commands

	// Exit sleep mode
	tft_write_command(CMD_SLEEP_OUT);
	sleep_ms(120);	// Sleep out needs some time

	// Set pixel format to 16 bits per pixel (RGB565)
	tft_write_command(CMD_PIXELFORMAT);
	tft_write_data(0x05);

	// Set memory access control (orientation)
	tft_write_command(CMD_MACTL);
	tft_write_data(0x30);

	// Turn on the display
	tft_write_command(CMD_DISPLAY_ON);

	stdio_printf("TFT initialized successfully!\n");
}

void tft_write_command(uint8_t cmd) {
	gpio_put(TRG_SCR_DC, 0);  // Command mode
	gpio_put(TRG_SCR_CS, 0);  // Select display

	spi_write_blocking(SPI_PORT, &cmd, 1);

	gpio_put(TRG_SCR_CS, 1);  // Deselect display
}

void tft_write_data(uint8_t data) {
	gpio_put(TRG_SCR_DC, 1);  // Data mode
	gpio_put(TRG_SCR_CS, 0);  // Select display

	spi_write_blocking(SPI_PORT, &data, 1);

	gpio_put(TRG_SCR_CS, 1);  // Deselect display
}

void tft_write_data16(uint16_t data) {
	gpio_put(TRG_SCR_DC, 1);  // Data mode
	gpio_put(TRG_SCR_CS, 0);  // Select display

	uint8_t data_bytes[2] = {(uint8_t) (data >> 8), (uint8_t) data};
	spi_write_blocking(SPI_PORT, data_bytes, 2);

	gpio_put(TRG_SCR_CS, 1);  // Deselect display
}

void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1,
							uint16_t y1) {
	// Set column address range
	tft_write_command(CMD_COLUMN_ADDRESS);
	tft_write_data16(x0);
	tft_write_data16(x1);

	// Set row address range
	tft_write_command(CMD_PAGE_ADDRESS);
	tft_write_data16(y0);
	tft_write_data16(y1);

	// Ready to receive pixel data
	tft_write_command(CMD_MEMORY_WRITE);
}

void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
	// Bounds check to prevent writing outside the display area
	if (x >= TRG_SCR_WIDTH || y >= TRG_SCR_HEIGHT) {
		return;
	}

	// Set the address window to just this one pixel
	tft_set_address_window(x, y, x, y);

	// Write the color data
	gpio_put(TRG_SCR_DC, 1);  // Data mode
	gpio_put(TRG_SCR_CS, 0);  // Select display

	// Split the 16-bit color into two 8-bit values
	uint8_t high_byte = (uint8_t) (color >> 8);
	uint8_t low_byte = (uint8_t) color;

	// Send the color data
	spi_write_blocking(SPI_PORT, &high_byte, 1);
	spi_write_blocking(SPI_PORT, &low_byte, 1);

	gpio_put(TRG_SCR_CS, 1);  // Deselect display
}

void tft_fill_screen(uint16_t color) {
	tft_set_address_window(0, 0, TRG_SCR_WIDTH - 1, TRG_SCR_HEIGHT - 1);

	gpio_put(TRG_SCR_DC, 1);  // Data mode
	gpio_put(TRG_SCR_CS, 0);  // Select display

	uint8_t high_byte = (uint8_t) (color >> 8);
	uint8_t low_byte = (uint8_t) color;

	for (int i = 0; i < TRG_SCR_WIDTH * TRG_SCR_HEIGHT; i++) {
		spi_write_blocking(SPI_PORT, &high_byte, 1);
		spi_write_blocking(SPI_PORT, &low_byte, 1);
	}

	gpio_put(TRG_SCR_CS, 1);  // Deselect display
}

/// ==== Audio function implementations ======================================

int trg_audio_init(void) {
	if (g_audio.initialized) {
		return TRG_OK;
	}

	// Initialize audio format
	g_audio.format = (audio_format_t) {
		.format = AUDIO_BUFFER_FORMAT_PCM_S16,
		.sample_freq = TRG_AUD_SAMPLE_RATE,
		.channel_count = 1,
	};

	// Create audio buffer pool
	static audio_buffer_pool_t *pool = NULL;
	audio_buffer_format_t audio_buffer_format = {
		.format = &g_audio.format,
		.sample_stride = 2
	};

	pool = audio_new_buffer_pool(&audio_buffer_format, 
								 TRG_AUD_BUFFER_COUNT, 
								 TRG_AUD_BUFFER_SIZE);
	if (!pool) {
		return -1;
	}

	// Configure I2S
	audio_i2s_config_t audio_i2s_config = {
		.data_pin = TRG_AUD_DIN,
		.clock_pin_base = TRG_AUD_BCLK,
		.dma_channel = 0,
		.pio_sm = 0,
	};

	if (!audio_i2s_setup(&g_audio.format, &audio_i2s_config)) {
		return -1;
	}

	if (!audio_i2s_connect(pool)) {
		return -1;
	}

	g_audio.pool = pool;
	g_audio.initialized = true;
	audio_i2s_set_enabled(true);

	stdio_printf("Audio system initialized successfully!\n");
	return TRG_OK;
}

void trg_audio_cleanup(void) {
	if (!g_audio.initialized) {
		return;
	}

	audio_i2s_set_enabled(false);
	g_audio.initialized = false;
	stdio_printf("Audio system cleaned up\n");
}

audio_buffer_t *trg_audio_get_buffer(void) {
	if (!g_audio.initialized) {
		return NULL;
	}
	return take_audio_buffer(g_audio.pool, false);
}

void trg_audio_play_buffer(audio_buffer_t *buffer) {
	if (!g_audio.initialized || !buffer) {
		return;
	}
	give_audio_buffer(g_audio.pool, buffer);
}

void trg_audio_play_tone(float frequency, uint32_t duration_ms) {
	if (!g_audio.initialized) {
		return;
	}

	audio_buffer_t *buffer = trg_audio_get_buffer();
	if (!buffer) {
		return;
	}

	int16_t *samples = (int16_t *)buffer->buffer->bytes;
	uint32_t sample_count = buffer->max_sample_count;
	
	// Generate sine wave
	for (uint32_t i = 0; i < sample_count; i++) {
		float t = (float)i / TRG_AUD_SAMPLE_RATE;
		if (t * 1000.0f > duration_ms) {
			samples[i] = 0;
		} else {
			samples[i] = (int16_t)(sinf(2.0f * M_PI * frequency * t) * 16384.0f);
		}
	}
	
	buffer->sample_count = sample_count;
	trg_audio_play_buffer(buffer);
}

void trg_audio_play_pcm(int16_t *samples, size_t sample_count) {
	if (!g_audio.initialized || !samples) {
		return;
	}

	audio_buffer_t *buffer = trg_audio_get_buffer();
	if (!buffer) {
		return;
	}

	int16_t *buffer_samples = (int16_t *)buffer->buffer->bytes;
	size_t copy_count = sample_count < buffer->max_sample_count ? 
						sample_count : buffer->max_sample_count;
	
	for (size_t i = 0; i < copy_count; i++) {
		buffer_samples[i] = samples[i];
	}
	
	buffer->sample_count = copy_count;
	trg_audio_play_buffer(buffer);
}
