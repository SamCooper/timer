char curr_time_str[255];
char next_time_str[255];

lv_style_t style_mode_btn;

int toMinutes(int hour, int minute) {
  return (60 * hour) + minute;
}

void init_page_styles() {
  lv_style_init(&style_mode_btn);
  lv_style_set_bg_color(&style_mode_btn, lv_color_hex(0x009900));
}

void exit_conf_btn_clicked_event_cb(lv_event_t *e) {
  exit_conf_page(page_cont, footer_cont);
}

void brightness_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  int32_t v = lv_subject_get_int(subject);
  set_brightness(v);
  set_backlight_level(v);
}

void firmware_btn_click_event_cb(lv_event_t* e) {
  lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
  lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
  lv_label_set_text_fmt(label, "IP: %s", WiFi.localIP().toString().c_str());
}

void firmware_btn_long_click_event_cb(lv_event_t* e) {
  lv_obj_t* btn = (lv_obj_t*)lv_event_get_target_obj(e);
  lv_obj_t* label = (lv_obj_t*)lv_event_get_user_data(e);
  lv_label_set_text_fmt(label, "IP: %s", WiFi.localIP().toString().c_str());
  lv_obj_add_state(btn, LV_STATE_CHECKED);
  isOTA = true;
}

void create_system_config_page(lv_obj_t* cont) {
  lv_obj_t* slider = lv_slider_create(cont);
  lv_slider_set_range(slider, 1, LCD_BL_MAX);
  lv_slider_bind_value(slider, &conf_brightness_subject);
  lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 40);

  lv_obj_t* btn = lv_button_create(cont);
  lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 80);
  lv_obj_set_style_radius(btn, 4, 8);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, "Update firmware");
  lv_obj_center(label);
  lv_obj_add_event_cb(btn, firmware_btn_click_event_cb, LV_EVENT_SHORT_CLICKED, label);
  lv_obj_add_event_cb(btn, firmware_btn_long_click_event_cb, LV_EVENT_LONG_PRESSED, label);

  lv_obj_t *exit_btn = lv_button_create(cont);
  lv_obj_set_pos(exit_btn, 340, 200);
  lv_obj_add_event_cb(exit_btn, exit_conf_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *exit_label = lv_label_create(exit_btn);
  lv_label_set_text(exit_label, LV_SYMBOL_CLOSE);
}

void heating_mode_on_btn_click_event_cb(lv_event_t* e) {
  set_heating_mode(HEATING_MODE_ON);
}

void heating_mode_timer_btn_click_event_cb(lv_event_t* e) {
  set_heating_mode(HEATING_MODE_TIMER);
}

void heating_set_timer_btn_click_event_cb(lv_event_t* e) {
  show_confinguration_page(page_cont, footer_cont, 0);
}

void heating_mode_off_btn_click_event_cb(lv_event_t* e) {
  set_heating_mode(HEATING_MODE_OFF);
}

lv_obj_t* create_heating_mode_btn(lv_obj_t* cont, int i, const char* str) {
  lv_obj_t* btn = lv_button_create(cont);
  lv_obj_add_style(btn, &style_mode_btn, LV_STATE_CHECKED);
  lv_obj_set_style_radius(btn, 16, 0);
  lv_obj_set_size(btn, 120, 120);
  lv_obj_align(btn, LV_ALIGN_CENTER, 160 * i, -20);

  lv_obj_t* label = lv_label_create(btn);
  lv_obj_center(label);
  lv_label_set_text(label, str);

  return btn;
}

void create_mode_page(lv_obj_t* cont) {
  lv_obj_t* btn = create_heating_mode_btn(cont, -1, "On");
  lv_obj_bind_state_if_eq(btn, &heating_mode_subject, LV_STATE_CHECKED, HEATING_MODE_ON);
  lv_obj_add_event_cb(btn, heating_mode_on_btn_click_event_cb, LV_EVENT_CLICKED, NULL);

  btn = create_heating_mode_btn(cont, 0, "Timer");
  lv_obj_bind_state_if_eq(btn, &heating_mode_subject, LV_STATE_CHECKED, HEATING_MODE_TIMER);
  lv_obj_add_event_cb(btn, heating_mode_timer_btn_click_event_cb, LV_EVENT_CLICKED, NULL);

  btn = create_heating_mode_btn(cont, 0, "Set");
  lv_obj_add_event_cb(btn, heating_set_timer_btn_click_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_set_size(btn, 120, 40);
  lv_obj_align(btn, LV_ALIGN_CENTER, 0, 70);

  btn = create_heating_mode_btn(cont, 1, "Off");
  lv_obj_bind_state_if_eq(btn, &heating_mode_subject, LV_STATE_CHECKED, HEATING_MODE_OFF);
  lv_obj_add_event_cb(btn, heating_mode_off_btn_click_event_cb, LV_EVENT_CLICKED, NULL);
}

void curr_time_home_cb(lv_observer_t* observer, lv_subject_t* subject) {
  unsigned long rawTime = timeClient.getEpochTime();

  unsigned long hours = (rawTime % 86400L) / 3600;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  unsigned long minutes = (rawTime % 3600) / 60;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  String timestamp = hoursStr + ":" + minuteStr;

  lv_subject_copy_string(&time_str_subject, timestamp.c_str());
}

void create_home_page(lv_obj_t* cont) {
  lv_obj_t* label = lv_label_create(cont);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
  lv_label_bind_text(label, &heating_on_subject, "Heating: %d");

  label = lv_label_create(cont);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 50);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_40, 0);
  lv_label_bind_text(label, &time_str_subject, "%s");
  lv_subject_add_observer_obj(&curr_time_subject, curr_time_home_cb, label, NULL);

  label = lv_label_create(cont);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 100);
  lv_label_bind_text(label, &next_time_str_subject, "%s");
}

void show_confinguration_page(lv_obj_t* cont, lv_obj_t* footer, int index) {
  /*Animate out the previous content*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_duration(&a, 300);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&a, anim_set_y_cb);
  lv_anim_set_get_value_cb(&a, anim_get_y_cb);
  lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);

  uint32_t i;
  uint32_t delay = 0;
  uint32_t child_cnt_prev = lv_obj_get_child_count(cont);
  for (i = 0; i < child_cnt_prev; i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, 0, 20);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_out(child, 200, delay);
    delay += 50;
  }
  child_cnt_prev = lv_obj_get_child_count(footer);
  for (i = 0; i < child_cnt_prev; i++) {
    lv_obj_t* child = lv_obj_get_child(footer, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, 0, 20);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_out(child, 200, delay);
    delay += 50;
  }

  if (index == 0) {
    create_set_timer_page(cont);
  } else if (index == 1) {
    create_system_config_page(cont);
  }

  /*Animate in the new widgets*/
  lv_anim_set_completed_cb(&a, NULL);
  for (i = child_cnt_prev; i < lv_obj_get_child_count(cont); i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, -20, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_in(child, 200, delay);
    delay += 50;
  }
}

void exit_conf_page(lv_obj_t* cont, lv_obj_t* footer) {
  isOTA = false;

  /*Animate out the previous content*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_duration(&a, 300);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&a, anim_set_y_cb);
  lv_anim_set_get_value_cb(&a, anim_get_y_cb);
  lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);

  uint32_t i;
  uint32_t delay = 0;
  uint32_t child_cnt_prev = lv_obj_get_child_count(cont);
  for (i = 0; i < child_cnt_prev; i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, 0, -20);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_out(child, 200, delay);
    delay += 50;
  }

  create_main_page(cont, 1);
  create_footer(footer);

  /*Animate in the new widgets*/
  lv_anim_set_completed_cb(&a, NULL);
  for (i = child_cnt_prev; i < lv_obj_get_child_count(cont); i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, 20, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_in(child, 200, delay);
    delay += 50;
  }
  for (i = child_cnt_prev; i < lv_obj_get_child_count(footer); i++) {
    lv_obj_t* child = lv_obj_get_child(footer, i);
    lv_anim_set_var(&a, child);
    lv_anim_set_values(&a, 20, 0);
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_in(child, 200, delay);
    delay += 1;
  }

  lv_subject_notify(&current_tab_subject);
}
