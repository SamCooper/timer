#include <Preferences.h>
#include <WiFi.h>
#include "time.h"
#include <WiFiClient.h>
#include <WebServer.h>
#include <Update.h>
#include "lv_conf.h"
#include "User_Setup.h"
#include <lvgl.h>
#include <TFT_eSPI.h>
#include "ft6336u.h"
#include "Backlight.hpp"


// wifi_secrets.h must contain the following
//const char *ssid = "xxxx";   // Change this to your WiFi SSID
//const char *password = "xxxx";  // Change this to your WiFi password
#include "wifi_secrets.h"

#define SCREEN_WIDTH LCD_HOR_RES
#define SCREEN_HEIGHT LCD_VER_RES
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))

#define HEATING_GPIO 32

// GPIO to HIGH(1) to switch heating off
typedef enum {
  GPIO_HEATING_ON = 0,
  GPIO_HEATING_OFF = 1
} gpio_heating_truth_t;

typedef enum {
  HEATING_MODE_OFF,
  HEATING_MODE_TIMER,
  HEATING_MODE_ON
} heating_mode_t;

typedef enum {
  HEATING_BOOST_MODE_OFF,
  HEATING_BOOST_MODE_NEXT,
  HEATING_BOOST_MODE_HOURS
} heating_boost_mode_t;

typedef void (*set_time_cb_t)(int value);

typedef struct {
  lv_subject_t* start_time;
  lv_subject_t* end_time;
  set_time_cb_t set_start_time_cb;
  set_time_cb_t set_end_time_cb;
} time_pair_t;

void set_morning_start(int value);
void set_morning_end(int value);
void set_evening_start(int value);
void set_evening_end(int value);

Preferences preferences;
WebServer server(80);

uint32_t draw_buf[DRAW_BUF_SIZE / 4];

lv_subject_t current_tab_subject;
lv_subject_t conf_brightness_subject;
lv_subject_t heating_mode_subject;
lv_subject_t heating_on_subject;
lv_subject_t curr_time_subject;
lv_subject_t time_str_subject;
lv_subject_t next_time_str_subject;
lv_subject_t timer_morning_start_subject;
lv_subject_t timer_morning_end_subject;
lv_subject_t timer_evening_start_subject;
lv_subject_t timer_evening_end_subject;
lv_subject_t heating_boost_mode_subject;
lv_subject_t heating_boost_next_subject;
lv_subject_t heating_boost_hours_subject;
unsigned long heating_boost_hours_end_time = 0;

time_pair_t morning_time_pair{ .start_time = &timer_morning_start_subject, .end_time = &timer_morning_end_subject, .set_start_time_cb = set_morning_start, .set_end_time_cb = set_morning_end };
time_pair_t evening_time_pair{ .start_time = &timer_evening_start_subject, .end_time = &timer_evening_end_subject, .set_start_time_cb = set_evening_start, .set_end_time_cb = set_evening_end };


lv_obj_t* page_cont;
lv_obj_t* footer_cont;
unsigned long lastSecond = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(19200);

  lv_init();

  setupWiFi();
  setupOta();
  setupTime();

  // Start the SPI for the touchscreen and init the touchscreen
  touch_init(TP_SDA, TP_SCL, TP_RST, TP_INT);
  backlight_init();

  pinMode(HEATING_GPIO, OUTPUT);
  digitalWrite(HEATING_GPIO, GPIO_HEATING_OFF);

  lv_display_t* disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_90);

  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t* indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);

  init_subjects();
  build_display();

  lastSecond = lastTime = millis();
}

bool isIdle = false;
bool isOTA = false;
void loop() {
  if (isOTA) {
    loopOta();
  }

  unsigned long nowTime = millis();

  lv_task_handler();                // let the GUI do its work
  lv_tick_inc(nowTime - lastTime);  // tell LVGL how much time has passed
  lastTime = nowTime;
  uint32_t inactiveTime = lv_display_get_inactive_time(NULL);

  bool isNewSecond = false;

  if ((nowTime - lastSecond) > 1000) {
    lastSecond += ((nowTime - lastSecond) / 1000) * 1000;
    isNewSecond = true;
  }

  if (!isIdle) {
    if (isNewSecond) {
      updateTime();
    }

    if (!isOTA && inactiveTime > 10000) {
      isIdle = true;
      switch_off_screen();
    }
  }

  if (isNewSecond) {
    updateHeating();
  }

  delay(5);
}
