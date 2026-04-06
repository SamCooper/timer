bool isTimerHeatingOn() {
  int morn_start_time = lv_subject_get_int(&timer_morning_start_subject);
  int morn_end_time = lv_subject_get_int(&timer_morning_end_subject);
  int eve_start_time = lv_subject_get_int(&timer_evening_start_subject);
  int eve_end_time = lv_subject_get_int(&timer_evening_end_subject);
  int now_time = (timeClient.getHours() * 60) + timeClient.getMinutes();

  if (((morn_start_time <= now_time) && (now_time < morn_end_time)) || ((eve_start_time <= now_time) && (now_time < eve_end_time))) {
    return true;
  }

  return false;
}

bool isBoostOn() {
  heating_boost_mode_t boost_mode = (heating_boost_mode_t)lv_subject_get_int(&heating_boost_mode_subject);

  return boost_mode != HEATING_BOOST_MODE_OFF;
}

bool isBoostHeatingOn() {
  heating_boost_mode_t boost_mode = (heating_boost_mode_t)lv_subject_get_int(&heating_boost_mode_subject);
  bool isBoosting = false;

  if (boost_mode == HEATING_BOOST_MODE_NEXT) {
    int adv = lv_subject_get_int(&heating_boost_next_subject);
    isBoosting = ((adv == 1) || (adv == 3));
  } else if (boost_mode == HEATING_BOOST_MODE_HOURS) {
    isBoosting = true;
  }

  return isBoosting;
}

void updateHeating() {
  bool prev_heating_on = lv_subject_get_int(&heating_on_subject);
  bool heating_on = false;
  bool check_boost = false;

  heating_mode_t heating_mode = (heating_mode_t)lv_subject_get_int(&heating_mode_subject);

  updateBoostState(heating_mode);

  if (heating_mode == HEATING_MODE_OFF) {
    check_boost = true;
  } else if (heating_mode == HEATING_MODE_TIMER) {
    heating_on = isTimerHeatingOn();
    check_boost = true;
  } else if (heating_mode == HEATING_MODE_ON) {
    heating_on = true;
  }

  bool boost_on = isBoostOn();
  if (check_boost && boost_on) {
    heating_on = isBoostHeatingOn();
  }

  if (heating_on != prev_heating_on) {
    // Serial.print("Heating: ");
    // Serial.print(prev_heating_on);
    // Serial.print("  to: ");
    // Serial.println(heating_on);
    lv_subject_set_int(&heating_on_subject, heating_on);
    digitalWrite(HEATING_GPIO, heating_on ? GPIO_HEATING_ON : GPIO_HEATING_OFF);
  }

  updateHeatingStatus(heating_mode, boost_on);
}

void updateHeatingStatus(heating_mode_t heating_mode, bool boost_on) {
  if (heating_mode == HEATING_MODE_ON) {
    lv_subject_copy_string(&next_time_str_subject, "Mode: ON");
  } else {
    if (heating_mode == HEATING_MODE_TIMER) {
      if (boost_on) {
        lv_subject_copy_string(&next_time_str_subject, "Mode: TIMER (Boost override)");
      } else {
        lv_subject_copy_string(&next_time_str_subject, "Mode: TIMER");
      }
    } else {
      if (boost_on) {
        lv_subject_copy_string(&next_time_str_subject, "Mode: OFF (Boost override)");
      } else {
        lv_subject_copy_string(&next_time_str_subject, "Mode: OFF");
      }
    }
  }
}