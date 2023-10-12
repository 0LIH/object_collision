/*
*  Object Detection App
*  CGS Semester 2
*  Task 2
*  Author: OJH
*/

#include <math.h>
#include <WiFi.h>
#include <aREST.h>


// DHT11 sensor pins
#define OBSENSOR 18
#define DHTTYPE DHT11


// Create aREST instance
aREST rest = aREST();



// WiFi parameters
const char* ssid = "Proxima";
const char* password = "centauri";
//Static IP address configuration
// P connections 
#define LISTEN_PORT           80

// Create an instance of the server
WiFiServer server(LISTEN_PORT);

// Variables to be exposed to the API

int lastState = HIGH;
int currentState;
char* location = "Al Fresco";
int timer = 72000;
bool value = false;

// Declare functions to be exposed to the API
int ledControl(String command);

void setup(void)
{  
  // Start Serial
  Serial.begin(9600);
  
  // Init DHT 
  //dht.begin();

  // initialize the ESP32's pin as an input
  pinMode(OBSENSOR, INPUT);
  
  // Init variables and expose them to REST API
  rest.variable("currentState",&currentState);
  rest.variable("lastState",&lastState);
  rest.variable("location",&location);
  rest.variable("value",&value);
    
  // Give name and ID to device
  rest.set_id("129");
  rest.set_name("alpha-129");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 1, 129); //set static ip
  IPAddress gateway(192, 168, 1, 1); //set getteway
  Serial.print(F("Setting static ip to : "));
  Serial.println(ip);
  IPAddress subnet(255, 255, 255, 0);//set subnet
  WiFi.config(ip, gateway, subnet);

  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
  
  // Print the IP address
  Serial.println(WiFi.localIP());
  
}

void loop() {
  //26
  // Reading temperature and humidity
  // temperature = dht.readTemperature();
  // humidity = dht.readHumidity();

  // // Prints the temperature in celsius
  // Serial.print("Temperature: ");
  // Serial.println(temperature);
  // Serial.print("Humidity: ");
  // Serial.println(humidity);
  // Serial.print("Timer: ");
  // Serial.println(timer);
  // delay(5000);
  //timer--;
  // read the state of the the input pin:
  currentState = digitalRead(OBSENSOR);

  if (lastState == HIGH && currentState == LOW){
    Serial.println("The obstacle is detected");
    value = true;
  }else if (lastState == LOW && currentState == HIGH){
      Serial.println("The obstacle is cleared");
    value = false;
  }

  delay(50);
  // save the the last state
  lastState = currentState;

  //Check running time and reset if expired
  if (timer == 0 ) {
    delay(3000);
    Serial.println("Resetting..");
    ESP.restart();
  }
  
  // Handle REST calls
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  while(!client.available()){
    delay(1);
  }
  rest.handle(client);
  //Serial.println("called");
}

// Custom function accessible by the API
int ledControl(String command) {

  // Get state from command
  int state = command.toInt();

  digitalWrite(6,state);
  return 1;
}