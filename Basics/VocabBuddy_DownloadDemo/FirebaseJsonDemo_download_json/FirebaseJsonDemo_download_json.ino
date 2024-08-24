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

#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 0, en = 2, d4 = 4, d5 = 14, d6 = 12, d7 = 13; // for wemos d1 esp8266 + lcd robot shield d1
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

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

#define VOCAB_DOWNLOAD_MAX_NUM        5               //Number of words, device will download from server
#define VOCAB_SET_MAX_SIZE            5
Vocab_t stCurVocabSet[VOCAB_SET_MAX_SIZE];             //First priority for diplaying
Vocab_t stOldVocabSet[VOCAB_SET_MAX_SIZE];             //Old
//Vocab_t stBufferVocabSet[3*VOCAB_SET_MAX_SIZE];        //Number of vocab download from database


//For debug purpose
#define DBG_EN  1

void printDbg(const char* str)
{
  #ifdef DBG_EN
    Serial.println(str);
  #endif
}


//Set up LCD
void setup_lcd()
{
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, new land!");
  //lcd.autoscroll();
  lcd.clear();
}

void lcd_print_line(int row, const char* str)
{
  lcd.setCursor(0,row);
  lcd.print(str);
}

void lcd_print_xy(int col, int row, const char* str)
{
  lcd.setCursor(col,row);
  lcd.print(str);
}

void lcd_clear_line(int row)
{
  lcd.setCursor(0,row);
  lcd.print("                ");
}

//Set up wifi
int setup_wifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(_SSID);
  WiFi.begin(_SSID, _PASSWORD);

  int timeoutCnt = 0;
  //lcd_clear_line(0);
  lcd.clear();
  lcd_print_line(1,"Waiting for wifi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
    //lcd.scrollDisplayLeft();
    
    if (timeoutCnt++ > 360) //3 min
      return 1;
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  digitalWrite(LED_BUILTIN, HIGH);

  return 0;
}
void (*resetFunc)(void) = 0; //declare reset function att address 0

void setup_serial()
{
  //Set up serial
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}
//General setup
void setup() {

  int ret = 0;

  //Set up serial
  setup_serial();

  //Set up wifi
  ret = setup_wifi();
  if(ret)
  {
    lcd_print_line(1,"Resetting wifi");
    delay(1000);
    printDbg("Resetting wifi");
    resetFunc();
  }

  //Set up lcd 16x2
  setup_lcd();

  //synchronize with database
  sync_database();

}

void sync_database()
{
  #if 1
//================================================================//
//================================================================//
  firebase.json(true);              // Make sure to add this line.
  
  //Get data from Firebase server
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
#endif

}

void lcd_display()
{

  int cnt = 0;
  const Vocab_t* psCurVocabSet = stCurVocabSet;

  printDbg("lcd_display_start");

  //stCurVocabSet[cnt].bDisplay

  //lcd.autoscroll();

  for(cnt = 0;cnt<VOCAB_DOWNLOAD_MAX_NUM;cnt++)
  {
    if(psCurVocabSet[cnt].bDisplay)
    {

      //Serial.print("received_word:\t");
      Serial.println(psCurVocabSet[cnt].word);

      //Serial.print("received_partOfSpeech:\t\t");
      Serial.println(psCurVocabSet[cnt].pronunciation);

      // Serial.print("received_pronunciation:\t\t");
      // Serial.println(received_pronunciation);

      // Serial.print("received_translation:\t\t");
      // Serial.println(received_translation);

      // Serial.print("received_definitions:\t\t");
      // Serial.println(received_definitions);

      // Serial.print("received_common_phrases:\t\t");
      // Serial.println(received_common_phrases);


      lcd_print_line(0, psCurVocabSet[cnt].word);
      //lcd_print_line(1, psCurVocabSet[cnt].pronunciation);      //Challenge: special characters
      //partOfSpech

      lcd_print_line(1, psCurVocabSet[cnt].definitions);
      delay(1000);

      int len = strlen(psCurVocabSet[cnt].definitions);

      if( len > 16)
      {
        for (int positionCounter = 0; positionCounter < BUFF_MAX_LEN_DEFINITIONS - len; positionCounter++) {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(500);
        }
      }
      delay(3000);
      lcd.clear();

      lcd_print_line(0, psCurVocabSet[cnt].translation);
      lcd_print_line(1, psCurVocabSet[cnt].commonPhrases);
      delay(1000);

      len = strlen(psCurVocabSet[cnt].commonPhrases);

      if( len > 16)
      {
        for (int positionCounter = 0; positionCounter < BUFF_MAX_LEN_DEFINITIONS - len; positionCounter++) {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(500);
        }
      }

      delay(3000);
      lcd.clear();
    }
  }

  printDbg("lcd_display_end");
}

void lcd_test()
{
  lcd.setCursor(0, 0);
  lcd.print("Test line 1st");
  lcd.setCursor(0, 1);
  lcd.print("Test line 2nd");

  delay(2000);
  lcd.clear();
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

  lcd_display();

  // lcd.setCursor(0, 1);
  // // print the number of seconds since reset:
  // lcd.print(millis() / 1000);

  // while(1){
  //   lcd_test();
  // }
  //delay(2000);
}
