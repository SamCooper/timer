void create_footer(lv_obj_t* footer) {
  if (lv_obj_get_child_count(footer) == 0) {
    tab_btn_create(footer, "Mode");
    tab_btn_create(footer, "Home");
    tab_btn_create(footer, "Boost");

    lv_obj_t* indicator = lv_obj_create(footer);
    lv_obj_remove_style(indicator, NULL, 0);
    lv_obj_set_style_bg_opa(indicator, LV_OPA_40, 0);
    lv_subject_add_observer_obj(&current_tab_subject, tab_indicator_observer_cb, indicator, NULL);
    lv_obj_set_height(indicator, 10);
    lv_obj_align(indicator, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_flag(indicator, LV_OBJ_FLAG_IGNORE_LAYOUT);

    /*Be sure the indicator has the correct size*/
    lv_obj_update_layout(indicator);
  }
}

void build_display(void) {
  lv_subject_init_int(&current_tab_subject, 0);
  init_page_styles();

  lv_obj_t* main_cont = lv_obj_create(lv_screen_active());
  lv_obj_remove_style_all(main_cont);
  lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
  lv_obj_set_style_pad_all(main_cont, 0, 0);
  lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);

  lv_obj_t* cont = page_cont = lv_obj_create(main_cont);
  lv_obj_remove_style_all(cont);
  lv_obj_set_flex_grow(cont, 1);
  lv_obj_set_style_pad_all(cont, 8, 0);
  lv_obj_set_width(cont, lv_pct(100));
  lv_subject_add_observer_obj(&current_tab_subject, cont_observer_cb, cont, NULL);
  lv_obj_set_scroll_dir(cont, LV_DIR_VER);

  lv_obj_t* footer = footer_cont = lv_obj_create(main_cont);
  lv_obj_remove_style_all(footer);
  lv_obj_set_style_pad_column(footer, 8, 0);
  lv_obj_set_style_pad_all(footer, 8, 0);
  lv_obj_set_flex_flow(footer, LV_FLEX_FLOW_ROW);
  lv_obj_set_size(footer, lv_pct(100), 60);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);

  create_footer(footer);

  lv_subject_notify(&current_tab_subject);
  lv_subject_set_int(&current_tab_subject, 1);
}

void tab_btn_create(lv_obj_t* parent, const char* text) {
  lv_obj_t* btn = lv_button_create(parent);
  lv_obj_set_flex_grow(btn, 1);
  lv_obj_set_height(btn, lv_pct(100));
  lv_obj_set_style_radius(btn, 0, 0);
  lv_subject_add_observer_obj(&current_tab_subject, tab_btn_observer_cb, btn, NULL);
  lv_obj_add_event_cb(btn, tab_btn_click_event_cb, LV_EVENT_SHORT_CLICKED, NULL);
  lv_obj_add_event_cb(btn, tab_btn_long_press_event_cb, LV_EVENT_LONG_PRESSED, NULL);

  lv_obj_t* label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_center(label);
}

void tab_btn_click_event_cb(lv_event_t* e) {
  lv_obj_t* btn = lv_event_get_target_obj(e);
  uint32_t idx = lv_obj_get_index(btn);
  lv_subject_set_int(&current_tab_subject, idx);
}

void tab_btn_long_press_event_cb(lv_event_t* e) {
  lv_obj_t* btn = lv_event_get_target_obj(e);
  uint32_t idx = lv_obj_get_index(btn);

  if (idx == 1 && 1 == lv_subject_get_int(&current_tab_subject)) {
    show_confinguration_page(page_cont, footer_cont, 1);
  }
}

void tab_btn_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  int32_t prev_v = lv_subject_get_previous_int(subject);
  int32_t cur_v = lv_subject_get_int(subject);

  lv_obj_t* btn = (lv_obj_t*)lv_observer_get_target(observer);
  int32_t idx = (int32_t)lv_obj_get_index(btn);

  if (idx == prev_v) lv_obj_remove_state(btn, LV_STATE_CHECKED);
  if (idx == cur_v) lv_obj_add_state(btn, LV_STATE_CHECKED);
}

void tab_indicator_observer_cb(lv_observer_t* observer, lv_subject_t* subject) {
  int32_t cur_v = lv_subject_get_int(subject);
  lv_obj_t* indicator = (lv_obj_t*)lv_observer_get_target(observer);

  lv_obj_t* footer = lv_obj_get_parent(indicator);
  lv_obj_t* btn_act = lv_obj_get_child(footer, cur_v);
  lv_obj_set_width(indicator, lv_obj_get_width(btn_act));

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, anim_set_x_cb);
  lv_anim_set_duration(&a, 300);
  lv_anim_set_path_cb(&a, lv_anim_path_ease_in_out);
  lv_anim_set_var(&a, indicator);
  lv_anim_set_values(&a, lv_obj_get_x(indicator), lv_obj_get_x(btn_act));
  lv_anim_start(&a);
}
