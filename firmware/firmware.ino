#include "HX711.h"
#include <DHT.h>

// DHT humidity/temperature sensors
#define DHTPIN_R04 D02     // DHT Sensor output pin connected to Dash GPIO
#define DHTPIN_R05 D03

#define DHTTYPE_R04 DHT22   // DHT 22 (AM2302)
#define DHTTYPE_R05 DHT22   // DHT 22 (AM2302)
//Change this to change how often the DHT22 data is printed and sent to the cloud


// Initialize DHT sensor for normal 16mhz Arduino
DHT dht_pinR04(DHTPIN_R04, DHTTYPE_R04,20);
DHT dht_pinR05(DHTPIN_R05, DHTTYPE_R05,20);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);



//HX711 scale(A1, A0);      //parameter "gain" is ommited; the default value 128 is used by the library
HX711 scale(A02, A01);

float offset = 0;
float scalefactor = 1;

String str_scalefactor = "";
String str_offset = "";

float floatGewicht = 0;
char charGewicht[10];
String stringGewicht = "";

void cloud_sms(const String &sender, const rtc_datetime_t &timestamp, const String &message) {
  Serial.println("CLOUD SMS RECEIVED:");
  Serial.print("SMS SENDER: ");
  Serial.println(sender);
  Serial.print("SMS TIMESTAMP: ");
  Serial.println(timestamp);
  Serial.println("SMS TEXT: ");
  Serial.println(message);

  int firstCommaIndex = message.indexOf(',');
  int secondCommaIndex = message.indexOf(',', firstCommaIndex+1);
  
  str_scalefactor = message.substring(0, firstCommaIndex);
  str_offset = message.substring(firstCommaIndex+1, secondCommaIndex);
  scalefactor = str_scalefactor.toFloat(); 
  offset = str_offset.toFloat();

  Serial.println("Scalefactor: ");
  Serial.println(scalefactor);
  Serial.println("Offset: ");
  Serial.println(offset);

  if(HologramCloud.sendMessage(sender, "SMSRX")) {
    Serial.println("SMS received message sent to cloud.");
  } else {
    Serial.println("Notification send failed.");
    Serial.println("Check failure reason by typing:");
    Serial.println("cloud status");
  }
}

void setup() {

  pinMode(D09, OUTPUT);
  Dash.onLED();
  digitalWrite(D09, HIGH);

  Serial.begin();                                       //Start USB Serial
  HologramCloud.attachHandlerSMS(cloud_sms);            //Handle received SMS

  
  // put your setup code here, to run once:
  Dash.begin();

  //Begin DHT communication
  dht_pinR04.begin();
  dht_pinR05.begin();
  
  

}

void loop() {
  
  
  Dash.pulseLED(1000, 1000);
  digitalWrite(D09, HIGH);
  delay(5000);

  /*Serial.println("Scalefactor: ");
  Serial.println(scalefactor);
  Serial.println("Offset: ");
  Serial.println(offset);
*/

  scale.set_scale(scalefactor);                      //this value is obtained by calibrating the scale with known weights; 
                                                 /*   How to Calibrate Your Scale
                                                      1.Call set_scale() with no parameter.
                                                      2.Call set_tare() with no parameter.
                                                      3.Place a known weight on the scale and call get_units(10).
                                                      4.Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
                                                      5.Adjust the parameter in step 4 until you get an accurate reading. 
                                                  */
   
  scale.power_up();
  HologramCloud.sendMessage(dhtJSON()); // Send JSON DHT data to Hologram Cloud  
  scale.power_down();
  HologramCloud.disconnect();
  digitalWrite(D09, LOW);
  Dash.deepSleepMin(15);
  
}


String dhtJSON() {
  char buf[8];
  dtostrf((scale.get_units(10) - offset),4,2,buf);
  String ret = "&field1=";
  ret.concat(buf);
  dtostrf(dht_pinR04.readTemperature(),4,2,buf);
  ret.concat("&field2=");
  ret.concat(buf);
  dtostrf(dht_pinR04.readHumidity(),4,2,buf);
  ret.concat("&field3=");
  ret.concat(buf);
  dtostrf(dht_pinR05.readTemperature(),4,2,buf);
  ret.concat("&field4=");
  ret.concat(buf);
  //ret.concat(", ");
  dtostrf(dht_pinR05.readHumidity(),4,2,buf);
  ret.concat("&field5=");
  ret.concat(buf);
  dtostrf(Charger.batteryPercentage(),4,2,buf);
  ret.concat("&field6=");
  ret.concat(buf);
 
  return ret;
}

