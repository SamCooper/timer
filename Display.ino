// Get the Touchscreen data
void touchscreen_read(lv_indev_t* indev, lv_indev_data_t* data) {
  touch_coords_t touch_coords;

  if (get_touch_coords(&touch_coords)) {
    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = touch_coords.x;
    data->point.y = touch_coords.y;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

int32_t anim_get_x_cb(lv_anim_t* a) {
  return lv_obj_get_x_aligned((lv_obj_t*)a->var);
}

void anim_set_x_cb(void* obj, int32_t v) {
  lv_obj_set_x((lv_obj_t*)obj, v);
}

int32_t anim_get_y_cb(lv_anim_t* a) {
  return lv_obj_get_y_aligned((lv_obj_t*)a->var);
}

void anim_set_y_cb(void* obj, int32_t v) {
  lv_obj_set_y((lv_obj_t*)obj, v);
}

void create_main_page(lv_obj_t* cont, int32_t cur_v) {
  /*Create the widgets according to the current value*/
  if (cur_v == 0) {
    create_mode_page(cont);
  }
  if (cur_v == 1) {
    create_home_page(cont);
  }
  if (cur_v == 2) {
    create_boost_page(cont);
  }
}

void cont_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  int32_t prev_v = lv_subject_get_previous_int(subject);
  int32_t cur_v = lv_subject_get_int(subject);
  lv_obj_t* cont = (lv_obj_t*)lv_observer_get_target(observer);

  /*Animate out the previous content*/
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_duration(&a, 300);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&a, anim_set_x_cb);
  lv_anim_set_get_value_cb(&a, anim_get_x_cb);
  lv_anim_set_completed_cb(&a, lv_obj_delete_anim_completed_cb);

  uint32_t i;
  uint32_t delay = 0;
  uint32_t child_cnt_prev = lv_obj_get_child_count(cont);
  for (i = 0; i < child_cnt_prev; i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    if (prev_v < cur_v) {
      lv_anim_set_values(&a, 0, -20);
    } else {
      lv_anim_set_values(&a, 0, 20);
    }
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_out(child, 200, delay);
    delay += 50;
  }

  create_main_page(cont, cur_v);

  /*Animate in the new widgets*/
  lv_anim_set_completed_cb(&a, NULL);
  for (i = child_cnt_prev; i < lv_obj_get_child_count(cont); i++) {
    lv_obj_t* child = lv_obj_get_child(cont, i);
    lv_anim_set_var(&a, child);
    if (prev_v < cur_v) {
      lv_anim_set_values(&a, 20, 0);
    } else {
      lv_anim_set_values(&a, -20, -0);
    }
    lv_anim_set_delay(&a, delay);
    lv_anim_start(&a);
    lv_obj_fade_in(child, 200, delay);
    delay += 50;
  }
}

void updateTime() {
  unsigned long rawTime = timeClient.getEpochTime();
  lv_subject_set_int(&curr_time_subject, rawTime);
}

void wake_up_screen(lv_event_t* e) {
  lv_obj_clean(lv_layer_top());
  updateTime();

  set_backlight_level(lv_subject_get_int(&conf_brightness_subject));
  isIdle = false;
}

void switch_off_screen(void) {
  close_set_time_panel();

  uint32_t child_cnt_prev = lv_obj_get_child_count(lv_layer_top());
  for (int i = 0; i < child_cnt_prev; i++) {
    lv_obj_delete(lv_obj_get_child(lv_layer_top(), i));
  }

  lv_obj_t* overlay_cont = lv_obj_create(lv_layer_top());

  lv_obj_set_style_bg_color(overlay_cont, lv_color_black(), 0);
  lv_obj_set_style_radius(overlay_cont, 0, 0);
  lv_obj_set_style_border_width(overlay_cont, 0, 0);
  lv_obj_set_style_pad_all(overlay_cont, 0, 0);
  lv_obj_set_size(overlay_cont, lv_pct(100), lv_pct(100));
  lv_obj_align(overlay_cont, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_event_cb(overlay_cont, wake_up_screen, LV_EVENT_CLICKED, NULL);

  set_backlight_level(0);

  exit_conf_page(page_cont, footer_cont);
  lv_subject_set_int(&current_tab_subject, 1);
}
