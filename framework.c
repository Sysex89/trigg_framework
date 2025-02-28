#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/time.h"

/// ==== LED pins =============================================================
#define TRG_LED_L 28
#define TRG_LED_R 4

/// ==== Button pins ==========================================================
#define TRG_BTN_W 5
#define TRG_BTN_A 6
#define TRG_BTN_S 7
#define TRG_BTN_D 8

#define TRG_BTN_I 12
#define TRG_BTN_J 13
#define TRG_BTN_K 14
#define TRG_BTN_L 15

/// ==== Screen pins ==========================================================
#define TRG_SCR_CS	  20
#define TRG_SCR_RESET 26
#define TRG_SCR_DC	  22
#define TRG_SCR_MOSI  19
#define TRG_SCR_SCK	  18
#define TRG_SCR_LITE  17

#define TRG_SCR_WIDTH  160
#define TRG_SCR_HEIGHT 128

/// ==== SPI instance =========================================================
#define SPI_PORT spi0

/// ==== Common TFT LCD commands ==============================================
#define CMD_SLEEP_OUT	   0x11
#define CMD_DISPLAY_ON	   0x29
#define CMD_COLUMN_ADDRESS 0x2A
#define CMD_PAGE_ADDRESS   0x2B
#define CMD_MEMORY_WRITE   0x2C
#define CMD_MACTL		   0x36	 // Memory Access Control
#define CMD_PIXELFORMAT	   0x3A

/// ==== Color definitions (RGB565 format) ====================================
#define COLOR_BLACK	  0x0000
#define COLOR_BLUE	  0x001F
#define COLOR_RED	  0xF800
#define COLOR_GREEN	  0x07E0
#define COLOR_CYAN	  0x07FF
#define COLOR_MAGENTA 0xF81F
#define COLOR_YELLOW  0xFFE0
#define COLOR_WHITE	  0xFFFF
#define COLOR_PURPLE  0x780F

/// ==== Return codes =========================================================
#define TRG_OK 0

/// ==== Function prototypes ==================================================

int trg_led_init(void);
int trg_buttons_init(void);

void tft_init(void);
void tft_write_command(uint8_t cmd);
void tft_write_data(uint8_t data);
void tft_write_data16(uint16_t data);
void tft_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void tft_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void tft_fill_screen(uint16_t color);

// void test_buttons(uint16_t *x, uint16_t *y);
void blink_twice(void);

int main(void) {
	stdio_init_all();

	// Initialize the LEDs
	int rc = trg_led_init();
	hard_assert(rc == TRG_OK);

	// Initialize the buttons
	rc = trg_buttons_init();
	hard_assert(rc == TRG_OK);

	// Blink the LEDs twice to indicate that the program has started
	blink_twice();

	// Initialize the TFT display
	tft_init();

	// Fill the screen with purple color
	tft_fill_screen(COLOR_GREEN);

	uint16_t x = TRG_SCR_WIDTH / 2;
	uint16_t y = TRG_SCR_HEIGHT / 2;

	while (true) {
		if (!gpio_get(TRG_BTN_W)) {
			if (y < TRG_SCR_HEIGHT)
				y++;
		}
		if (!gpio_get(TRG_BTN_A)) {
			if (x > 0)
				x--;
		}
		if (!gpio_get(TRG_BTN_S)) {
			if (y > 0)
				y--;
		}
		if (!gpio_get(TRG_BTN_D)) {
			if (x < TRG_SCR_WIDTH)
				x++;
		}
		tft_draw_pixel(x, y, COLOR_RED);
		sleep_ms(50);
	}
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
