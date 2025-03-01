#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/multicore.h"
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

/// ==== Keys =================================================================
typedef enum {
	KEY_W = 0x1,
	KEY_A = 0x2,
	KEY_S = 0x4,
	KEY_D = 0x8,
	KEY_I = 0x10,
	KEY_J = 0x20,
	KEY_K = 0x40,
	KEY_L = 0x80
} KeyCode;

/// Keypress callback function prototype
typedef void (*keypress_callback_t)(uint8_t key);

static keypress_callback_t g_keypress_callback = NULL;
static bool g_keypress_event_loop_running = false;
static bool g_keypress_callback_trigger_always = false;

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

/// ==== Event loop ===========================================================
uint8_t trg_get_button(void);
void init_keypress_event_loop(bool callback_trigger_always);
void stop_keypress_event_loop(void);
void reg_keypress_callback(keypress_callback_t callback);
void keypress_event_loop(void);
