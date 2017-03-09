#include "HX711.h"

#include <DHT.h>
#define Serial SerialUSB
#define DHTPIN R04     // DHT Sensor output pin connected to Dash GPIO
#define DHTTYPE DHT22   // DHT 22
//Change this to change how often the DHT22 data is printed and sent to the cloud

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

//HX711.DOUT    - pin #A1
//HX711.PD_SCK  - pin #A0

//HX711 scale(A1, A0);      //parameter "gain" is ommited; the default value 128 is used by the library
HX711 scale(A02, A01);

float offset = 299.7;
float scalefactor = 28075.5;
float floatGewicht = 0;
char charGewicht[10];
String stringGewicht = "";

void setup() {
  // put your setup code here, to run once:
  Dash.begin();  
  SerialUSB.begin(9600);  
  scale.set_scale(scalefactor);                      //this value is obtained by calibrating the scale with known weights; 
                                                 /*   How to Calibrate Your Scale
                                                      1.Call set_scale() with no parameter.
                                                      2.Call set_tare() with no parameter.
                                                      3.Place a known weight on the scale and call get_units(10).
                                                      4.Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
                                                      5.Adjust the parameter in step 4 until you get an accurate reading. 
                                                  */

}

void loop() {
  
  SerialCloud.begin(115200); 
  //SerialUSB.println("Waiting for GSM network");
  //delay(30000); //Delay to make connection to GSM network
  Dash.snooze(30000);
 
  scale.power_up();
  //SerialUSB.print("Average weight from 10 messures:\t");
  //float floatGewicht = (scale.get_units(10) - offset);
  //SerialUSB.println(floatGewicht);
  //dhtPrint(); // Print serial reading of DHT reading
  //dtostrf(floatGewicht, 6, 2, charGewicht);
  //stringGewicht = charGewicht;  
  //SerialCloud.println("&field1="+stringGewicht);
  SerialCloud.println(dhtJSON()); // Send JSON DHT data to Hologram Cloud  
  scale.power_down();
  Dash.deepSleepMin(60);
  //Dash.snooze(3530000);


}

/*void dhtPrint() {
  Serial.print("Humidity: ");
  Serial.print(dht.readHumidity());
  Serial.println(" %\t");
  Serial.println("Temperature: ");
  Serial.print(dht.readTemperature());
  Serial.print(" *C ");
  Serial.print(dht.readTemperature(true));
  Serial.println(" *F\t");
  Serial.println("SOC: ");
  Serial.print(Dash.batteryPercentage());
  Serial.println(" %\t");
}
*/
String dhtJSON() {
  char buf[8];
  dtostrf((scale.get_units(10) - offset),4,2,buf);
  String ret = "&field1=";
  ret.concat(buf);
  dtostrf(dht.readTemperature(),4,2,buf);
  ret.concat("&field2=");
  ret.concat(buf);
  //ret.concat(", ");
  dtostrf(dht.readHumidity(),4,2,buf);
  ret.concat("&field3=");
  ret.concat(buf);
  //ret.concat(", ");
  dtostrf(Charger.batteryPercentage(),4,2,buf);
  ret.concat("&field4=");
  ret.concat(buf);
  //ret.concat("}");
  //Serial.println(ret);
  return ret;
}

