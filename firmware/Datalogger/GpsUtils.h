#ifndef __GPS_UTILS_H__
#define __GPS_UTILS_H__

#define GGA_TIME_COL 1
#define GGA_LAT_COL 2
#define GGA_NS_COL 3
#define GGA_LON_COL 4
#define GGA_EW_COL 5
#define GGA_QUALITY_COL 6
#define RMC_TIME_COL 1
#define RMC_STATUS_COL 2
#define RMC_LAT_COL 3
#define RMC_DATE_COL 9

const String PREFIX_GNGGA = "$GNGGA";
const String PREFIX_GNRMC = "$GNRMC";

String getStringEntry(String string, uint8_t entry_number) {
  int next_comma_index = 0;
  int last_comma_index;
  
  for (uint8_t i = 0; i < entry_number; i++) {
    last_comma_index = next_comma_index;
    next_comma_index = string.indexOf(',', last_comma_index+1);
  }

  last_comma_index = next_comma_index;
  next_comma_index = string.indexOf(',', last_comma_index+1);

  return string.substring(last_comma_index+1, next_comma_index);
}

bool stringIsGgaData(String gps_string) {
  return gps_string.startsWith(PREFIX_GNGGA);
}

bool stringIsRmcData(String gps_string) {
  return gps_string.startsWith(PREFIX_GNRMC);
}

String getDateTimeFromRmcString(String rmc_string) {
  String time_str = getStringEntry(rmc_string, RMC_TIME_COL);
  String time_str_trim = time_str.substring(0,3);
  String date_str = getStringEntry(rmc_string, RMC_DATE_COL);
  String date_time_str = date_str + time_str_trim;
  return date_time_str;
}

String getDateFromRmcString(String rmc_string) {
  String date_str = getStringEntry(rmc_string, RMC_DATE_COL);
  return date_str;
}

bool gpsIsSynchronized(String gps_data) {
  if (!stringIsGgaData(gps_data)) {
    return false;
  }

  String quality_string = getStringEntry(gps_data, GGA_QUALITY_COL);
  long quality = quality_string.toInt();

  return quality > 0;
}

#endif
