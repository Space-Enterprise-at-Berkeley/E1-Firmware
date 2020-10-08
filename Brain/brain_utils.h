String make_packet(struct sensorInfo snesor);
uint16_t Fletcher16(uint8_t *data, int count);

/*
 * Data structure to allow the conversion of bytes to floats and vice versa.
 */
union floatArrToBytes {
  char buffer[24];
  float sensorReadings[6];
} farrbconvert;

/*
 * Data structure to store all information relevant to a specific sensor type.
 */
struct sensorInfo {
  String sensor_name;
  int board_address;
  byte sensor_id;
  int overall_id;
  int clock_freq;
  sensorInfo(String n, int b, byte s, int o, int f) : sensor_name(n), board_address(b), sensor_id(s), overall_id(o), clock_freq(f) {}
};

/*
 * Constructs packet in the following format: 
 * {<sensor_ID>, <data1>, <data2>, ..., <dataN>|checksum}
 */
String make_packet(struct sensorInfo sensor) {
  String packet_content = (String)sensor.overall_id;
  packet_content += ",";
  for (int i=0; i<6; i++) {
    float reading = farrbconvert.sensorReadings[i];
    if (reading > 0) {
      packet_content += (String)reading;
      packet_content += ",";
    } else {
      packet_content.remove(packet_content.length()-1);
      int count = packet_content.length();
      char const *data = packet_content.c_str();
      uint16_t checksum = Fletcher16((uint8_t *) data, count);
      packet_content += "|";
      packet_content += (String)checksum;
      break;
    }
  }
  String packet = "{" + packet_content + "}";
  return packet;
}

/*
 * Calculates checksum for key values being sent to ground station:
 * sensor_ID and it's corresponding data points
 */
uint16_t Fletcher16(uint8_t *data, int count) {
  
  uint16_t sum1 = 0;
  uint16_t sum2 = 0;

  for (int index=0; index<count; index++) {
    if (data[index] > 0) {
      sum1 = (sum1 + data[index]) % 255;
      sum2 = (sum2 + sum1) % 255;
    }
  }
  return (sum2 << 8) | sum1;
}
