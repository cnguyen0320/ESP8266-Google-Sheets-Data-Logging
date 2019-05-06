/*  Uploading data from ESP8266 to Google Sheets
    Uses HTTPSRedirect library by Github user: @electronicsguy

    Sketch to be uploaded to ESP8266 based development board (NodeMCU, Sparkfun Thing, etc.)
    User should change the variables in the SET PARAMETERS SECTION to match their parameters
    The ESP8266 listens to the Serial communication port for incoming data and uploads it to Google Sheets
    Serial communcation from Master Arduino should be in String format with values comma delimited:
      the first value is the GoogleScriptID
      subsequent values will be uploaded to the Google Sheet

    Originally written by Chris Nguyen
    clnguyen2@wisc.edu
    Last updated: December 2018
*/

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"

/* Debug enable section
   Debug mode enabled will print out items to Serial.
   ***Only run Debug Mode when not passing data through Serial Communication
   CAUTION: in Debug mode, Serial communication or use of TX/RX pins may perform unexpectedly

   Comment out the line #define DEBUG to disable debug mode
   Disable if using the board without a Serial Monitor
*/
//#define DEBUG

#ifdef DEBUG
#define DBEGIN(...)    Serial.begin(__VA_ARGS__)
#define DPRINT(...)    Serial.print(__VA_ARGS__)
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#define DRINTF(...)    Serial.print(F(__VA_ARGS__))
#define DPRINTLNF(...) Serial.println(F(__VA_ARGS__))
#define DFLUSH(...)    Serial.flush(__VA_ARGS__)
#else
#define DBEGIN(...)     //blank line
#define DPRINT(...)     //blank line
#define DPRINTLN(...)   //blank line
#define DPRINTF(...)    //blank line
#define DPRINTLNF(...)  //blank line
#define DFLUSH(...)     //blank line
#endif

/*
    SET PARAMETERS SECTION
    User required to edit the following variables for the code to work:
*/
//The following are for WiFi connection
const char* ssid = "";
const char* password = "";//The password for the above SSID

/*
     HTTPS request setup
     Creates port and host URL
     Fill in the Google Script ID for your spreadsheet
*/
const char* host = "script.google.com";
const int httpsPort = 443;
HTTPSRedirect* client;

/* Initialize the arguments for the http requests */
String GoogleScriptID = ""; //unique Google Sheets identifier found in URL of Google Sheets, initialize as empty
String url = ""; //url for the POST request. Start as empty string
String fetch = ""; //url for the GET request. Start as empty string

//The payload is formatted as a JSON and requires the following format:
const String payload_base =  "{\"command\": \"appendRow\", \"values\": \"";

/*Set program specific variables*/
static bool connectedFlag = false; //! boolean that contains a flag indicating whether the microcontroller is connected to Google


/* Function: setup
    This code runs at the beginning to set everything up:
      initializes Serial Communication
      connects to Wifi
*/
void setup() {
  //Initializes the Serial Monitor
  Serial.begin(9600); //Use the same baud rate as the Arduino
  Serial.flush();


  pinMode(LED_BUILTIN, OUTPUT);
  //connects to the wifi
  connectToWifi();
}


/* Function: loop
   This loop runs for infinite time
   Make connection with Google and read in data from Serial Communication
   If data is available from Serial, it will upload data to Google Sheets
*/
void loop() {

  makeConnection(); //connects Google

  //Will only send data if connected to Wifi and Google
  if (client->connected() && WiFi.status() == WL_CONNECTED) {

    //Only read data if data is available from Sender
    if (Serial.available()) {
      //read the full message (until carriage return and newline character)
      String fullMessage = Serial.readStringUntil('\r');
      Serial.readStringUntil('\n');

      //find the comma in the message
      int comma_index = fullMessage.indexOf(',');

      //If a comma doesn't exist (index of -1), print error and then go to next loop
      if (comma_index == -1) {
        Serial.println("WARNING: invalid format");
      }

      //If a comma does exist, compute
      else {
        //update the GoogleScriptID if necessary
        if (GoogleScriptID.length() < 5) {
          //get the script ID from the message (from beginning to the comma)
          GoogleScriptID = fullMessage.substring(0, comma_index);
          url = String("/macros/s/") + GoogleScriptID + "/exec?"; //update the url
          fetch = String("/macros/s/") + GoogleScriptID + "/exec?read"; //update the fetch
        }

        //Gets the rest of the Serial input as the data message
        String message = fullMessage.substring(comma_index + 1);
        upload(message); //uses the message and passes it to the upload function
      }
    }
  }
  //If not connected, print error message to Serial Monitor under Debug Mode
  else {
    DPRINTLN("Could not post");
  }
}












/*
    Function: upload
    data: String that contains the data that will be appended to the spreadsheet

    Takes argument of type String and appends it to the payload_base
    Uses the payload to send data to Google Sheets
*/
void upload(String data) {
  String payload = payload_base + data + "\"}"; //appends data to the payload

  DPRINTLN("POST: append memory data to spreadsheet: " +  payload);
  //Attempts to make a data POST to the Google server
  if (client->POST(url, host, payload)) {

    //If debug is on, it will make a GET request to the spreadsheet and pull the last line of data.
#ifdef DEBUG
    DPRSINTLN("Trying to GET last data file");
    client->GET(fetch, host); //read last written data to confirm
#endif
  }
}

/* Function: connectToWifi
   Connects to the local network. This should only be called once unless it fails
*/
void connectToWifi() {
  Serial.println("\nConnecting to wifi: " + String(ssid));
  DFLUSH(); // flush() is needed to print the above reliably, in case the wireless connection doesn't go through

  WiFi.begin(ssid, password);
  delay(200);
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress()); //Print out the MAC address
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, HIGH); //turns the LED off (active low) while connecting...
    delay(500);
    Serial.println(".");
  }
  digitalWrite(LED_BUILTIN, LOW); //turns the LED on (active low) when WiFi is connected
  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + String(WiFi.localIP()));
  Serial.readString(); //clears out the Serial input buffer
}

/* Function: initConnection
   Initializes the https connection to the host server
*/
void makeConnection() {
  //Checks to see if a connection has been initialized and connect if not
  if (!connectedFlag) {
    DPRINT("Connecting to "); DPRINTLN(host);
    client = new HTTPSRedirect(httpsPort);
    connectedFlag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  //Connects the client to the host
  if (client != nullptr && !client->connected()) {
    client->connect(host, httpsPort);
    DPRINTLN("Trying to connect to host");
  }

  //Notify the user that a connection could not be connected
  else if (!client->connected()) {
    DPRINTLN("Error creating client object");
  }
}
