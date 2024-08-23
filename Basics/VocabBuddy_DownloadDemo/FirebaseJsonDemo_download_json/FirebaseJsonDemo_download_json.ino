/*
  Make sure your Firebase project's '.read' and '.write' rules are set to 'true'. 
  Ignoring this will prevent the MCU from communicating with the database. 
  For more details- https://github.com/Rupakpoddar/ESP8266Firebase 
*/

#include <ArduinoJson.h>            // https://github.com/bblanchon/ArduinoJson 
#include <ESP8266Firebase.h>
#include <ESP8266WiFi.h>

#define _SSID "Dell_Thomas"          // Your WiFi SSID
#define _PASSWORD "123@123a"      // Your WiFi Password
#define REFERENCE_URL "https://vocabbuddy2024-default-rtdb.firebaseio.com"  // Your Firebase project reference url

Firebase firebase(REFERENCE_URL);

#if 0 //Single word
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

//================================================================//
//================================================================//

  // Write some data to the realtime database.
  // firebase.setString("Example/setString", "It's Working");
  // firebase.setInt("Example/setInt", 123);
  // firebase.setFloat("Example/setFloat", 45.32);

  //firebase.setString("dict/common_phrases/test", "Thomas Dao");

  firebase.json(true);              // Make sure to add this line.
  
  String data = firebase.getString("dict");  // Get data from the database.
  Serial.print("dict:\t");
  Serial.println(data);

  // Deserialize the data.
  // Consider using Arduino Json Assistant- https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(3) + 250;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, data);

  // Store the deserialized data.
  // const char* received_String = doc["setString"]; // "It's Working"
  // int received_int = doc["setInt"];               // 123
  // float received_float = doc["setFloat"];         // 45.32

  const char* received_word = doc["word"];
  const char* received_translation = doc["translation"];
  const char* received_pronunciation = doc["pronunciation"];
  const char* received_partOfSpeech = doc["part_of_speech"];
  const char* received_usage = doc["definitions"];
  //const char* received_usage = doc["0"];

  // Print data
  Serial.print("received_word:\t");
  Serial.println(received_word);

  Serial.print("received_translation:\t\t");
  Serial.println(received_translation);

  Serial.print("received_pronunciation:\t\t");
  Serial.println(received_pronunciation);

  Serial.print("received_partOfSpeech:\t\t");
  Serial.println(received_partOfSpeech);

  Serial.print("received_usage:\t\t");
  Serial.println(received_usage);

  // String data1 = firebase.getString("dict/common_phrases");  // Get data from the database.
  // Serial.println(data1);
  // DynamicJsonDocument doc1(capacity);

  // deserializeJson(doc1, data1);

  // JsonObject obj0 = doc1["0"];
  // const char* obj0_phrase = obj0["phrase"];
  // const char* obj0_pronunciation = obj0["pronunciation"];
  // const char* obj0_translation = obj0["translation"];
  // const char* obj0_usage = obj0["usage"];

  // Serial.print("obj0_usage:\t\t");
  // Serial.println(obj0_usage);
}

#else
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

//================================================================//
//================================================================//

  // Write some data to the realtime database.
  // firebase.setString("Example/setString", "It's Working");
  // firebase.setInt("Example/setInt", 123);
  // firebase.setFloat("Example/setFloat", 45.32);

  //firebase.setString("dict/common_phrases/test", "Thomas Dao");

  firebase.json(true);              // Make sure to add this line.
  
  String data = firebase.getString("set_1");  // Get data from the database.
  Serial.print("set_1:\t");
  Serial.println(data);

  // Deserialize the data.
  // Consider using Arduino Json Assistant- https://arduinojson.org/v6/assistant/
  const size_t capacity = JSON_OBJECT_SIZE(3) + 250;
  DynamicJsonDocument doc(capacity);

  deserializeJson(doc, data);

  JsonArray wordArray = doc.as<JsonArray>();
  Serial.print("wordArray size:\t");
  Serial.println(wordArray.size());

  int cnt = 0
  for (JsonObject obj : wordArray) {
    const char* received_word = obj["word"];
    const char* received_partOfSpeech = obj["part_of_speech"];
    const char* received_pronunciation = obj["pronunciation"];
    const char* received_translation = obj["translation"];
    const char* received_definitions = obj["definitions"];
    const char* received_common_phrases = obj["common_phrases"];

    // Print data
    Serial.print(cnt++);
    Serial.print(".");
    Serial.print("received_word:\t");
    Serial.println(received_word);

    Serial.print("received_partOfSpeech:\t\t");
    Serial.println(received_partOfSpeech);

    Serial.print("received_pronunciation:\t\t");
    Serial.println(received_pronunciation);

    Serial.print("received_translation:\t\t");
    Serial.println(received_translation);

    Serial.print("received_definitions:\t\t");
    Serial.println(received_definitions);

    Serial.print("received_common_phrases:\t\t");
    Serial.println(received_common_phrases);
  }
#if 1


#else
  for (JsonObject word : jsnobject) {
    const char* received_word = doc["word"];
    const char* received_partOfSpeech = doc["part_of_speech"];
    const char* received_pronunciation = doc["pronunciation"];
    const char* received_translation = doc["translation"];
    const char* received_definitions = doc["definitions"];
    const char* received_common_phrases = doc["common_phrases"];

    // Print data
    Serial.print("received_word:\t");
    Serial.println(received_word);

    Serial.print("received_partOfSpeech:\t\t");
    Serial.println(received_partOfSpeech);

    Serial.print("received_pronunciation:\t\t");
    Serial.println(received_pronunciation);

    Serial.print("received_translation:\t\t");
    Serial.println(received_translation);

    Serial.print("received_definitions:\t\t");
    Serial.println(received_definitions);

    Serial.print("received_common_phrases:\t\t");
    Serial.println(received_common_phrases);
  }
#endif

#if 0
  const char* received_word = doc["word"];
  const char* received_translation = doc["translation"];
  const char* received_pronunciation = doc["pronunciation"];
  const char* received_partOfSpeech = doc["part_of_speech"];
  const char* received_usage = doc["definitions"];
  //const char* received_usage = doc["0"];

  // Print data
  Serial.print("received_word:\t");
  Serial.println(received_word);

  Serial.print("received_translation:\t\t");
  Serial.println(received_translation);

  Serial.print("received_pronunciation:\t\t");
  Serial.println(received_pronunciation);

  Serial.print("received_partOfSpeech:\t\t");
  Serial.println(received_partOfSpeech);

  Serial.print("received_usage:\t\t");
  Serial.println(received_usage);
#endif

// void setup() {
//   // Initialize serial communication for debugging purposes
//   Serial.begin(115200);
//   pinMode(LED_BUILTIN, OUTPUT);
//   digitalWrite(LED_BUILTIN, LOW);
//   WiFi.mode(WIFI_STA);
//   WiFi.disconnect();
//   delay(1000);

//   // Connect to WiFi
//   Serial.println();
//   Serial.println();
//   Serial.print("Connecting to: ");
//   Serial.println(_SSID);
//   WiFi.begin(_SSID, _PASSWORD);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print("-");
//   }

// //   String data = firebase.getString("dict");  // Get data from the database.

// //   // Deserialize the data.
// //   // Consider using Arduino Json Assistant- https://arduinojson.org/v6/assistant/
// //   const size_t capacity = JSON_OBJECT_SIZE(3) + 250;
// //   DynamicJsonDocument doc(capacity);

// //   deserializeJson(doc, data);
  
//   // Allocate a memory pool to store the JSON document
//   // Use a StaticJsonDocument if the JSON structure is fixed and known at compile time
//   //StaticJsonDocument<1024> doc;

//   const size_t capacity = JSON_OBJECT_SIZE(3) + 250;
//   DynamicJsonDocument doc(capacity);

//   String json = firebase.getString("dict");  // Get data from the database.

//   // Deserialize the JSON document
//   Serial.println(json);
//   DeserializationError error = deserializeJson(doc, json);
  

//   // Test if parsing succeeds
//   if (error) {
//     Serial.print(F("deserializeJson() failed: "));
//     Serial.println(error.f_str());
//     return;
//   }

//   // Extract values from the JSON document
//   const char* word = doc["word"];
//   const char* part_of_speech = doc["part_of_speech"];
//   const char* pronunciation = doc["pronunciation"];
//   const char* translation = doc["translation"];
  
//   // Print extracted values
//   Serial.println(F("Word: "));
//   Serial.println(word);
//   Serial.println(F("Part of Speech: "));
//   Serial.println(part_of_speech);
//   Serial.println(F("Pronunciation: "));
//   Serial.println(pronunciation);
//   Serial.println(F("Translation: "));
//   Serial.println(translation);

//   // Extract and print definitions
//   JsonArray definitions = doc["definitions"];
//   for (JsonObject definition : definitions) {
//     const char* def = definition["definition"];
//     const char* def_translation = definition["translation"];
//     Serial.println(F("Definition: "));
//     Serial.println(def);
//     Serial.println(F("Translation: "));
//     Serial.println(def_translation);
//   }

//   // Extract and print common phrases
//   JsonArray common_phrases = doc["common_phrases"];
//   for (JsonObject phrase : common_phrases) {
//     const char* ph = phrase["phrase"];
//     const char* ph_translation = phrase["translation"];
//     const char* ph_pronunciation = phrase["pronunciation"];
//     const char* ph_usage = phrase["usage"];
//     Serial.println(F("Phrase: "));
//     Serial.println(ph);
//     Serial.println(F("Translation: "));
//     Serial.println(ph_translation);
//     Serial.println(F("Pronunciation: "));
//     Serial.println(ph_pronunciation);
//     Serial.println(F("Usage: "));
//     Serial.println(ph_usage);
//   }

//   // Extract and print the audio pronunciation URL
//   const char* audio_pronunciation = doc["audio_pronunciation"];
//   Serial.println(F("Audio Pronunciation URL: "));
//   Serial.println(audio_pronunciation);

//   // Extract and print any notes
//   const char* notes = doc["notes"];
//   Serial.println(F("Notes: "));
//   Serial.println(notes);

#endif
}


void loop() {
  // // Nothing
  //   // Write some data to the realtime database.
  //   int cnt = 10;
  //   int num = 100;
  // while(cnt--)
  // {
  //   Serial.print("Sending cnt:");
  //   Serial.println(cnt);
  //   firebase.setString("Example/setString", "It's Working");
  //   firebase.setInt("Example/setInt", num++);
  //   firebase.setFloat("Example/setFloat", 45.32 + cnt);
  //   delay(1000);
  // }

  //while(1);
}
