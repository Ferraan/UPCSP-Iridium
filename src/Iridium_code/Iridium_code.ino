#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <IridiumSBD.h>

#define Sleep_pin 8
#define Ring_pin 7
#define Input_pin 6
#define LED_pin 5
static const int RXgps = 4, TXgps = 3;
static const uint32_t GPSBaud = 9600;
static const int RXIridium = 10, TXIridium = 9;
static const uint32_t IridiumBaud = 19200;


// The serial connection to the GPS device
SoftwareSerial ssgps(RXgps, TXgps);
SoftwareSerial ssIridium(RXIridium, TXIridium);
//Iridium object
IridiumSBD modem(ssIridium,Sleep_pin,Ring_pin);
// The TinyGPSPlus object
TinyGPSPlus gps;
//String to contain the iridium message
String gpsStr;
//Time between Iridium messages
int msgperiod = 90;


void setup() {
  int err;
  Serial.begin(115200);
  // Start the serial port connected to the gps
  ssgps.begin(GPSBaud);
  // Start the serial port connected to the satellite modem
  ssIridium.begin(IridiumBaud);
  // Begin satellite modem operation
  Serial.println("Starting modem...");
  err = modem.begin();
  if (err != ISBD_SUCCESS) {
    Serial.print("Begin failed: error ");
    Serial.println(err);
    if (err == ISBD_NO_MODEM_DETECTED)
      Serial.println("No modem detected: check wiring.");
    return;
  }
  //Define input pin
  pinMode(Input_pin, INPUT);
  pinMode(LED_pin, OUTPUT);
  Serial.println(F("Beginning execution:"));
}

void loop() {
  
  if (gps.location.isValid() && digitalRead(Input_pin)==HIGH) {
    gpsStr = getGpsStr(gps.time, gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.location.isValid());
    Serial.println(gpsStr);
    digitalWrite(LED_pin, HIGH);
  } else {
    digitalWrite(LED_pin, LOW);
  }
  smartDelay(30000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ssgps.available())
      gps.encode(ssgps.read());
  } while (millis() - start < ms);
}

String getGpsStr(TinyGPSTime &t, float lat, float lng, float alt, bool valid) {
  String message;
  if (!valid) {
    String errmsg = "No location";
    message.concat(errmsg);
  } else {
    String latStr = String(lat, 5);
    String lngStr = String(lng, 5);
    String altStr = String(alt, 5);
    String hour = String(t.hour());
    String minute = String(t.minute());
    String second = String(t.second());
    message.concat(hour);
    message.concat(":");
    message.concat(minute);
    message.concat(":");
    message.concat(second);
    message.concat(",");
    message.concat(latStr);
    message.concat(",");
    message.concat(lngStr);
    message.concat(",");
    message.concat(altStr);
  }
  return (message);
}
