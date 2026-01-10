static lv_subject_t hour_subject;
static lv_subject_t minute_subject;
// static lv_subject_t *time_group_array_subject[] = { &hour_subject, &minute_subject };
const char *hour24_options =
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23";
const char *minute_options =
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59";

lv_obj_t *time_set_btn;
lv_obj_t *time_set_cont;

void set_btn_clicked_event_cb(lv_event_t *e);
void close_clicked_event_cb(lv_event_t *e);
void time_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

void create_row(lv_obj_t *cont, time_pair_t *set_pair, int y) {
  lv_obj_t *on_label = lv_label_create(cont);
  lv_obj_set_style_text_font(on_label, &lv_font_montserrat_30, 0);
  lv_obj_set_pos(on_label, 20, y + 6);
  lv_label_set_text(on_label, "On at");

  lv_obj_t *set_btn = lv_button_create(cont);
  lv_obj_set_pos(set_btn, 110, y);
  lv_obj_set_size(set_btn, 115, 53);
  lv_obj_add_event_cb(set_btn, set_start_btn_clicked_event_cb, LV_EVENT_CLICKED, set_pair);

  lv_obj_t *time_label = lv_label_create(set_btn);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_30, 0);
  lv_obj_center(time_label);
  lv_subject_add_observer_obj(set_pair->start_time, time_observer_cb, time_label, NULL);

  on_label = lv_label_create(cont);
  lv_obj_set_style_text_font(on_label, &lv_font_montserrat_30, 0);
  lv_obj_set_pos(on_label, 250, y + 6);
  lv_label_set_text(on_label, "until");

  set_btn = lv_button_create(cont);
  lv_obj_set_pos(set_btn, 322, y);
  lv_obj_set_size(set_btn, 115, 53);
  lv_obj_add_event_cb(set_btn, set_end_btn_clicked_event_cb, LV_EVENT_CLICKED, set_pair);

  time_label = lv_label_create(set_btn);
  lv_obj_set_style_text_font(time_label, &lv_font_montserrat_30, 0);
  lv_obj_center(time_label);
  lv_subject_add_observer_obj(set_pair->end_time, time_observer_cb, time_label, NULL);
}

void create_set_timer_page(lv_obj_t *cont) {
  /*Initialize the subjects.*/
  lv_subject_init_int(&hour_subject, 7);
  lv_subject_init_int(&minute_subject, 45);

  /*Create the UI*/
  create_row(cont, &morning_time_pair, 24);
  create_row(cont, &evening_time_pair, 84);


  lv_obj_t *exit_btn = lv_button_create(cont);
  lv_obj_set_pos(exit_btn, 340, 200);
  lv_obj_add_event_cb(exit_btn, exit_conf_btn_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *exit_label = lv_label_create(exit_btn);
  lv_label_set_text(exit_label, LV_SYMBOL_CLOSE);

  /*Update some subjects to see if the UI is updated as well*/
  lv_subject_notify(&timer_morning_start_subject);
  lv_subject_notify(&timer_morning_end_subject);
  lv_subject_notify(&timer_evening_start_subject);
  lv_subject_notify(&timer_evening_end_subject);
}

void close_set_time_panel() {
  if (time_set_cont) {
    lv_obj_delete(time_set_cont);
    time_set_cont = NULL;
  }

  if (time_set_btn) {
    lv_obj_remove_state(time_set_btn, LV_STATE_DISABLED);
    time_set_btn = NULL;
  }
}

void set_start_btn_clicked_event_cb(lv_event_t *e) {
  time_pair_t *set_pair = (time_pair_t *)lv_event_get_user_data(e);
  lv_obj_t *src_btn = lv_event_get_target_obj(e);

  set_btn_clicked_event_cb(src_btn, set_pair->start_time, set_pair, set_start_value_clicked_event_cb);
}

void set_end_btn_clicked_event_cb(lv_event_t *e) {
  time_pair_t *set_pair = (time_pair_t *)lv_event_get_user_data(e);
  lv_obj_t *src_btn = lv_event_get_target_obj(e);

  set_btn_clicked_event_cb(src_btn, set_pair->end_time, set_pair, set_end_value_clicked_event_cb);
}

void set_btn_clicked_event_cb(lv_obj_t *src_btn, lv_subject_t *set_subject, time_pair_t *set_pair, lv_event_cb_t set_event_cb) {
  close_set_time_panel();

  lv_obj_add_state(src_btn, LV_STATE_DISABLED);
  time_set_btn = src_btn;

  int minVal = lv_subject_get_int(set_subject);
  lv_subject_set_int(&hour_subject, minVal / 60);
  lv_subject_set_int(&minute_subject, minVal % 60);

  lv_obj_t *cont = time_set_cont = lv_obj_create(lv_layer_top());
  lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
  lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);

  lv_obj_t *hour_roller = lv_roller_create(cont);
  lv_obj_add_flag(hour_roller, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
  lv_roller_set_options(hour_roller, hour24_options, LV_ROLLER_MODE_NORMAL);
  lv_roller_bind_value(hour_roller, &hour_subject);
  lv_obj_set_pos(hour_roller, 0, 0);

  lv_obj_t *min_roller = lv_roller_create(cont);
  lv_roller_set_options(min_roller, minute_options, LV_ROLLER_MODE_NORMAL);
  lv_roller_bind_value(min_roller, &minute_subject);
  lv_obj_set_pos(min_roller, 64, 0);

  lv_obj_t *set_btn = lv_button_create(cont);
  lv_obj_align(set_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
  lv_obj_add_event_cb(set_btn, set_event_cb, LV_EVENT_CLICKED, set_pair);

  lv_obj_t *set_label = lv_label_create(set_btn);
  lv_label_set_text(set_label, LV_SYMBOL_OK);

  lv_obj_t *close_btn = lv_button_create(cont);
  lv_obj_align(close_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_add_event_cb(close_btn, close_clicked_event_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t *close_label = lv_label_create(close_btn);
  lv_label_set_text(close_label, LV_SYMBOL_CLOSE);
}

void set_start_value_clicked_event_cb(lv_event_t *e) {
  time_pair_t *set_pair = (time_pair_t *)lv_event_get_user_data(e);

  int hourVal = lv_subject_get_int(&hour_subject);
  int minVal = lv_subject_get_int(&minute_subject);
  int timeToSet = toMinutes(hourVal, minVal);
  int otherTime = lv_subject_get_int(set_pair->end_time);

  if (otherTime < timeToSet) {
    timeToSet = otherTime;
  }

  set_pair->set_start_time_cb(timeToSet);

  close_set_time_panel();
}

void set_end_value_clicked_event_cb(lv_event_t *e) {
  time_pair_t *set_pair = (time_pair_t *)lv_event_get_user_data(e);

  int hourVal = lv_subject_get_int(&hour_subject);
  int minVal = lv_subject_get_int(&minute_subject);
  int timeToSet = toMinutes(hourVal, minVal);
  int otherTime = lv_subject_get_int(set_pair->start_time);

  if (otherTime > timeToSet) {
    timeToSet = otherTime;
  }

  set_pair->set_end_time_cb(timeToSet);

  close_set_time_panel();
}

void close_clicked_event_cb(lv_event_t *e) {
  close_set_time_panel();
}

/*Watch all related subject to display the current time correctly*/
void time_observer_cb(lv_observer_t *observer, lv_subject_t *subject) {
  int minVal = lv_subject_get_int(subject);

  lv_obj_t *label = (lv_obj_t *)lv_observer_get_target(observer);
  lv_label_set_text_fmt(label, "%02d:%02d", minVal / 60, minVal % 60);
}
