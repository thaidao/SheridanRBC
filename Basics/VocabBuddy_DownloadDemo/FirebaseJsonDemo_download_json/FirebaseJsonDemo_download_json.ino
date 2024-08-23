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

#define BUFF_MAX_LEN_WORD					      32
#define BUFF_MAX_LEN_PARTOFSPECH			  8
#define BUFF_MAX_LEN_PRONUNCIATION			32
#define BUFF_MAX_LEN_TRANSLATION			  32
#define BUFF_MAX_LEN_DEFINITIONS			  96
#define BUFF_MAX_LEN_COMMON_PHRASES			96

struct Vocab_t{
  // const char *word;
  // const char *partOfSpech;
  // const char *pronunciation;
  // const char *translation;
  // const char *definitions;
  // const char *commonPhrases;

  char word[BUFF_MAX_LEN_WORD];
  char partOfSpech[BUFF_MAX_LEN_PARTOFSPECH];
  char pronunciation[BUFF_MAX_LEN_PRONUNCIATION];
  char translation[BUFF_MAX_LEN_TRANSLATION];
  char definitions[BUFF_MAX_LEN_DEFINITIONS];
  char commonPhrases[BUFF_MAX_LEN_COMMON_PHRASES];

  int   iPushedGotItCnt;      // Number of user push "Got it" button
  bool  bDisplay;             // Display this word or not in future

};

// struct VocabDispConfig_t{
//   int   iPushedGotItCnt;      // Number of user push "Got it" button
//   bool  bDisplay;             // Display this word or not in future
// };

#define VOCAB_DOWNLOAD_MAX_NUM        10               //Number of words, device will download from server
#define VOCAB_SET_MAX_SIZE            10
Vocab_t stCurVocabSet[VOCAB_SET_MAX_SIZE];             //First priority for diplaying
Vocab_t stOldVocabSet[VOCAB_SET_MAX_SIZE];             //Old
Vocab_t stBufferVocabSet[3*VOCAB_SET_MAX_SIZE];        //Number of vocab download from database

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

  int cnt = 0;
  for (JsonObject obj : wordArray) {
    const char* received_word = obj["word"];
    const char* received_partOfSpeech = obj["part_of_speech"];
    const char* received_pronunciation = obj["pronunciation"];
    const char* received_translation = obj["translation"];
    const char* received_definitions = obj["definitions"];
    const char* received_common_phrases = obj["common_phrases"];
    
    //Store value
    strcpy(stCurVocabSet[cnt].word,received_word);
    strcpy(stCurVocabSet[cnt].partOfSpech,received_partOfSpeech); 
    strcpy(stCurVocabSet[cnt].pronunciation,received_pronunciation); 
    strcpy(stCurVocabSet[cnt].translation,received_translation);
    strcpy(stCurVocabSet[cnt].definitions,received_definitions); 
    strcpy(stCurVocabSet[cnt].commonPhrases,received_common_phrases); 

    stCurVocabSet[cnt].iPushedGotItCnt = 0;   // Number of user push "Got it" button
    stCurVocabSet[cnt].bDisplay = true;       // Display this word or not in future

    // Print data via serial for debug/or display in other device
    Serial.print(cnt++);
    Serial.println(".");
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


    //Allow receiving only 10 latest word, Ignore a rest of data
    if (cnt > VOCAB_DOWNLOAD_MAX_NUM)
      break;
  }
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
