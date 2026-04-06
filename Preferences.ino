void init_stored_subjects() {
  preferences.begin("Timer", false);

  lv_subject_init_int(&heating_mode_subject, get_initial_int("heating_mode", HEATING_MODE_OFF));
  lv_subject_init_int(&timer_morning_start_subject, get_initial_int("morning_start", toMinutes(6, 30)));
  lv_subject_init_int(&timer_morning_end_subject, get_initial_int("morning_end", toMinutes(10, 30)));
  lv_subject_init_int(&timer_evening_start_subject, get_initial_int("evening_start", toMinutes(17, 15)));
  lv_subject_init_int(&timer_evening_end_subject, get_initial_int("evening_end", toMinutes(23, 0)));

  int initial_brightness = get_initial_int("brightness", 7);
  lv_subject_init_int(&conf_brightness_subject, initial_brightness);

  preferences.end();

  lv_subject_add_observer(&conf_brightness_subject, brightness_observer_cb, NULL);
  set_backlight_level(initial_brightness);
}

void init_page_subjects() {
  lv_subject_init_int(&heating_on_subject, 0);
  lv_subject_init_int(&curr_time_subject, 0);
  lv_subject_init_string(&time_str_subject, curr_time_str, NULL, 255, "");
  lv_subject_init_string(&next_time_str_subject, next_time_str, NULL, 255, "Next ON at 18:53");

  lv_subject_init_int(&heating_boost_mode_subject, HEATING_BOOST_MODE_OFF);
  lv_subject_init_int(&heating_boost_next_subject, 0);
  lv_subject_init_int(&heating_boost_hours_subject, 3600);
}

void init_subjects() {
  init_stored_subjects();
  init_page_subjects();
}

int get_initial_int(const char* key, int defaultValue) {
  if (preferences.isKey(key)) {
    // Serial.print("Pref: ");
    // Serial.print(key);
    // Serial.print("  : ");
    int i = preferences.getInt(key);
    // Serial.println(i);
    return i;
  } else {
    preferences.putInt(key, defaultValue);
    // Serial.print("Pref: ");
    // Serial.print(key);
    // Serial.print("  : Using default value");
    return defaultValue;
  }
}

void pref_set_int(const char* key, int value) {
  preferences.begin("Timer", false);
  preferences.putInt(key, value);
  preferences.end();
}

void pref_subject_set_int(lv_subject_t* subject, const char* key, int value) {
  pref_set_int(key, value);
  lv_subject_set_int(subject, value);
}

void set_brightness(int value) {
  pref_set_int("brightness", value);
}

void set_heating_mode(heating_mode_t value) {
  pref_subject_set_int(&heating_mode_subject, "heating_mode", value);
}

void set_morning_start(int value) {
  pref_subject_set_int(&timer_morning_start_subject, "morning_start", value);
}

void set_morning_end(int value) {
  pref_subject_set_int(&timer_morning_end_subject, "morning_end", value);
}

void set_evening_start(int value) {
  pref_subject_set_int(&timer_evening_start_subject, "evening_start", value);
}

void set_evening_end(int value) {
  pref_subject_set_int(&timer_evening_end_subject, "evening_end", value);
}
