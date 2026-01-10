// lv_subject_t heating_boost_mode_subject;
// lv_subject_t heating_boost_next_subject;
// lv_subject_t heating_boost_hours_subject;

// plus/minus buttons
void heating_boost_next_minus_btn_clicked_event_cb(lv_event_t* e) {
  int x = lv_subject_get_int(&heating_boost_next_subject);
  x--;

  if (x < 0) {
    x = 3;
  }

  lv_subject_set_int(&heating_boost_next_subject, x);
}

void heating_boost_next_plus_btn_clicked_event_cb(lv_event_t* e) {
  int x = lv_subject_get_int(&heating_boost_next_subject);
  x++;

  if (x > 3) {
    x = 0;
  }

  lv_subject_set_int(&heating_boost_next_subject, x);
}

void heating_boost_hours_minus_btn_clicked_event_cb(lv_event_t* e) {
  int x = lv_subject_get_int(&heating_boost_hours_subject);
  x -= 3600;

  heating_boost_mode_t boost_mode = (heating_boost_mode_t)lv_subject_get_int(&heating_boost_mode_subject);
  if (x < 3600 && boost_mode != HEATING_BOOST_MODE_HOURS) {
    x = 3600;
  } else if (x < 0) {
    x = 0;
  }

  lv_subject_set_int(&heating_boost_hours_subject, x);
}

void heating_boost_hours_plus_btn_clicked_event_cb(lv_event_t* e) {
  int x = lv_subject_get_int(&heating_boost_hours_subject);
  x += 3600;

  if (x > (12 * 3600)) {
    x = (12 * 3600);
  }

  lv_subject_set_int(&heating_boost_hours_subject, x);
}

// button label update observers

void heating_boost_next_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  lv_obj_t* label = lv_observer_get_target_obj(observer);
  int x = lv_subject_get_int(subject);

  lv_subject_t* next_subject;
  const char* prefix;

  if (0 == x) {
    prefix = "Off until ";
    next_subject = &timer_morning_start_subject;
  } else if (1 == x) {
    prefix = "On until ";
    next_subject = &timer_morning_end_subject;
  } else if (2 == x) {
    prefix = "Off until ";
    next_subject = &timer_evening_start_subject;
  } else if (3 == x) {
    prefix = "On until ";
    next_subject = &timer_evening_end_subject;
  }

  int minVal = lv_subject_get_int(next_subject);

  lv_label_set_text_fmt(label, "%s %02d:%02d", prefix, minVal / 60, minVal % 60);
}

void heating_boost_hours_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  lv_obj_t* label = lv_observer_get_target_obj(observer);
  int x = lv_subject_get_int(subject);

  heating_boost_mode_t boost_mode = (heating_boost_mode_t)lv_subject_get_int(&heating_boost_mode_subject);
  if (boost_mode == HEATING_BOOST_MODE_HOURS) {
    int hours = (x / 3600);
    int mins = (x % 3600) / 60;
    int secs = (x % 3600) % 60;
    lv_label_set_text_fmt(label, "On for %02d:%02d:%02d", hours, mins, secs);
  } else {
    lv_label_set_text_fmt(label, "On for %d hours", (x / 3600));
  }
}

// button mode selected callbacks
void heating_boost_next_select_btn_clicked_event_cb(lv_event_t* e) {
  lv_subject_set_int(&heating_boost_mode_subject, HEATING_BOOST_MODE_NEXT);
  lv_subject_set_int(&heating_boost_hours_subject, 3600);
}

void heating_boost_hours_select_btn_clicked_event_cb(lv_event_t* e) {
  lv_subject_set_int(&heating_boost_mode_subject, HEATING_BOOST_MODE_HOURS);
}

void heating_boost_off_select_btn_clicked_event_cb(lv_event_t* e) {
  resetBoost();
}

void resetBoost() {
  lv_subject_set_int(&heating_boost_mode_subject, HEATING_BOOST_MODE_OFF);
  lv_subject_set_int(&heating_boost_hours_subject, 3600);
}

void create_boost_page(lv_obj_t* cont) {
  lv_obj_t* btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 40, 30);
  lv_obj_set_size(btn, 53, 53);
  lv_obj_add_event_cb(btn, heating_boost_next_minus_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, LV_SYMBOL_MINUS);
  lv_obj_center(label);

  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 110, 30);
  lv_obj_set_size(btn, 260, 53);
  lv_obj_add_event_cb(btn, heating_boost_next_select_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_bind_state_if_eq(btn, &heating_boost_mode_subject, (lv_state_t)(LV_STATE_CHECKED | LV_STATE_DISABLED), HEATING_BOOST_MODE_NEXT);

  label = lv_label_create(btn);
  // lv_label_set_text(label, LV_SYMBOL_MINUS);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_30, 0);
  lv_obj_center(label);
  lv_subject_add_observer_obj(&heating_boost_next_subject, heating_boost_next_observer_cb, label, NULL);

  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 387, 30);
  lv_obj_set_size(btn, 53, 53);
  lv_obj_add_event_cb(btn, heating_boost_next_plus_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, LV_SYMBOL_PLUS);
  lv_obj_center(label);


  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 40, 100);
  lv_obj_set_size(btn, 53, 53);
  lv_obj_add_event_cb(btn, heating_boost_hours_minus_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, LV_SYMBOL_MINUS);
  lv_obj_center(label);

  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 110, 100);
  lv_obj_set_size(btn, 260, 53);
  lv_obj_add_event_cb(btn, heating_boost_hours_select_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_bind_state_if_eq(btn, &heating_boost_mode_subject, (lv_state_t)(LV_STATE_CHECKED | LV_STATE_DISABLED), HEATING_BOOST_MODE_HOURS);

  label = lv_label_create(btn);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_30, 0);
  lv_obj_center(label);
  lv_subject_add_observer_obj(&heating_boost_hours_subject, heating_boost_hours_observer_cb, label, NULL);

  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 387, 100);
  lv_obj_set_size(btn, 53, 53);
  lv_obj_add_event_cb(btn, heating_boost_hours_plus_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  label = lv_label_create(btn);
  lv_label_set_text(label, LV_SYMBOL_PLUS);
  lv_obj_center(label);


  btn = lv_button_create(cont);
  lv_obj_set_pos(btn, 110, 185);
  lv_obj_set_size(btn, 260, 53);
  lv_obj_add_event_cb(btn, heating_boost_off_select_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_bind_state_if_eq(btn, &heating_boost_mode_subject, LV_STATE_DISABLED, HEATING_BOOST_MODE_OFF);

  label = lv_label_create(btn);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_30, 0);
  lv_obj_center(label);
  lv_label_set_text(label, "Reset/Clear");
}

void updateBoostState(heating_mode_t heating_mode) {
  bool need_reset = false;

  if (heating_mode != HEATING_MODE_ON) {
    heating_boost_mode_t boost_mode = (heating_boost_mode_t)lv_subject_get_int(&heating_boost_mode_subject);

    if (boost_mode == HEATING_BOOST_MODE_NEXT) {
      int morn_start_time = lv_subject_get_int(&timer_morning_start_subject);
      int morn_end_time = lv_subject_get_int(&timer_morning_end_subject);
      int eve_start_time = lv_subject_get_int(&timer_evening_start_subject);
      int eve_end_time = lv_subject_get_int(&timer_evening_end_subject);
      int now_time = (timeClient.getHours() * 60) + timeClient.getMinutes();
      int adv = lv_subject_get_int(&heating_boost_next_subject);

      if (adv == 0 && (now_time == morn_start_time)) {
        need_reset = true;
      }
      if (adv == 1 && (now_time == morn_end_time)) {
        need_reset = true;
      }
      if (adv == 2 && (now_time == eve_start_time)) {
        need_reset = true;
      }
      if (adv == 3 && (now_time == eve_end_time)) {
        need_reset = true;
      }
    } else if (boost_mode == HEATING_BOOST_MODE_HOURS) {
      int end_time = lv_subject_get_int(&heating_boost_hours_subject);
      --end_time;

      if (end_time <= 0) {
        need_reset = true;
      } else {
        lv_subject_set_int(&heating_boost_hours_subject, end_time);
      }
    }
  } else {
    need_reset = true;
  }

  if (need_reset) {
    resetBoost();
  }
}
