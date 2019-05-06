// Sample code to send information from Arduino to Wifi Chip to Google Sheets

// sample variable to input into your Google Sheets
int temp = 0;
String GScriptID = "Enter GScriptID here";

void setup() {
  Serial.begin(9600);
}

void loop() {
  // posts data with a delay of 3 seconds
  delay(3000);

  // Code format to follow for posting to GoogleSheets
  String message = GScriptID + "," + String(temp);
  Serial.println(message);
  
  temp++;
}
