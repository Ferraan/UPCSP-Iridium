#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPSPlus object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

String gpsStr;

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  pinMode(6,OUTPUT);
  Serial.println(F("Beginning"));
  
}

void loop()
{
  gpsStr=getGpsStr(gps.time, gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.location.isValid());
  Serial.println(gpsStr);
  if(gps.location.isValid()){
    digitalWrite(6,HIGH);
  }
  else{
    digitalWrite(6,LOW);
  }
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

String getGpsStr(TinyGPSTime &t, float lat, float lng, float alt, bool valid){
  String message;
  if(!valid){
    String errmsg="No location";
    message.concat(errmsg);
  }  
  else{
  String latStr=String(lat,5);
  String lngStr=String(lng,5);
  String altStr=String(alt,5);
  String hour=String(t.hour());
  String minute=String(t.minute());
  String second=String(t.second());
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
  return(message);
}
