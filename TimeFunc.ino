void setupTime() {
  Serial.println("Config time...");
  configTime(0, 3600, "pool.ntp.org");
  Serial.println("Config time done");
}

void updateTime() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int rawTime = (timeinfo.tm_hour * 3600) + (timeinfo.tm_min * 60) + timeinfo.tm_sec;
    lv_subject_set_int(&curr_time_subject, rawTime);
  }
}

int getHoursAndMinutes() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return (timeinfo.tm_hour * 60) + timeinfo.tm_min;
}
