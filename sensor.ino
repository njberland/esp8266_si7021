// almost all of it copied from adafruit and other git-hub repos
// make sure to install libraries for Si7021 and ESP8266
// tested with Adafruit Huzzah Feather and NodeMCU

#include "Adafruit_Si7021.h"
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi parameters
#define WLAN_SSID       "myssid"
#define WLAN_PASS       "verysecret"

// Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "gollum"
#define AIO_KEY         "verylongkeyfromadafruit"

Adafruit_Si7021 sensor = Adafruit_Si7021();

WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Setup feeds for temperature & humidity
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/bytemp");
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/byhumid");

/*************************** Sketch Code ************************************/

void setup() {

  // Init sensor

  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();

  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // connect to adafruit io
  connect();

  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true);
  }
}

void loop() {

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  // Grab the current state of the sensor
  //float humidity_data = sensor.readHumidity();
  //float temperature_data = sensor.readTemperature();

  // By default, the temperature report is in Celsius, for Fahrenheit uncomment
  //    following line.
  // temperature_data = temperature_data*(9.0/5.0) + 32.0;

  
  Serial.print(F("Humidity  "));
  Serial.print(sensor.readHumidity(),2);
  Serial.print(F(" Temp "));
  Serial.print(sensor.readTemperature(),2);
  Serial.println();

  // Publish data
  if (! temperature.publish(sensor.readTemperature()))
    Serial.println(F("Failed to publish temperature"));
    
  else
    Serial.println(F("Temperature published!"));

  if (! humidity.publish(sensor.readHumidity()))
    Serial.println(F("Failed to publish humi  dity"));
  else
    Serial.println(F("Humidity published!"));

  // Repeat every 10 minuttes
  delay(1000*60*10);
}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
