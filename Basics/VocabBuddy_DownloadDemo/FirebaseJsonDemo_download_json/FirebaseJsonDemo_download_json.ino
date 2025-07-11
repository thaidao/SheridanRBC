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

// #define _SSID "Thuytien"          // Your WiFi SSID
// #define _PASSWORD "4372283361"      // Your WiFi Password

#define REFERENCE_URL "https://vocabbuddy2024-default-rtdb.firebaseio.com"  // Your Firebase project reference url

Firebase firebase(REFERENCE_URL);

#include "convertUnaccent.h"

#include <LiquidCrystal.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 0, en = 2, d4 = 4, d5 = 14, d6 = 12, d7 = 13; // for wemos d1 esp8266 + lcd robot shield d1
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int buttonDispModePin = 3;  // the number of the Display mode pin

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


#define MODE_NORMAL         0x00  //Display all
#define MODE_FAST_FORWARD   0x01  //Display word and translation only

int gDispMode = MODE_NORMAL;

//For debug purpose
#define DBG_EN  1

void test_unaccented_conversion(char *unicode_str) {
    char unaccented_str[100];
    convert_to_unaccented(unicode_str, unaccented_str);
    Serial.println(unaccented_str);
}


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

void setup_displayMode()
{
  // initialize the buttonDispModePin pin as an input:
  pinMode(buttonDispModePin, INPUT);

  //#define MODE_NORMAL         0x00  //Display all
  //#define MODE_FAST_FORWARD   0x01  //Display word and translation only
  gDispMode |= MODE_NORMAL;
}

void setDisplayMode(int mode)
{
  gDispMode |= mode;
}

void checkPinSwitchMode()
{
  // read the state of the pushbutton value:
  int buttonState = digitalRead(buttonDispModePin);
  if(buttonState == LOW)
  {
    setDisplayMode(MODE_FAST_FORWARD);
  }
  else
  {
    setDisplayMode(MODE_NORMAL); 
  }
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
  int retryCnt = 0;
  //lcd_clear_line(0);
  lcd.clear();
  lcd_print_line(1,"Waiting for wifi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("-");
    //lcd.scrollDisplayLeft();
    if (timeoutCnt++ > 120)
    {
        WiFi.begin("Thuytien", "4372283361");
        timeoutCnt = 0;
        if(retryCnt ++ > 3)
          return 1;
    }
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
  ret = 0;//setup_wifi();
  if(ret)
  {
    lcd_print_line(1,"Resetting wifi");
    delay(1000);
    printDbg("Resetting wifi");
    resetFunc();
  }

  //Set up lcd 16x2
  setup_lcd();

  //Set up display mode
  //setup_displayMode();

  //synchronize with database
  //sync_database();

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


// The message to scroll
//const char *longText = "This is a very long text that needs to scroll on a 16x2 LCD screen!";
const char *longText;
int gPosition = 0; // Tracks the current position of the text

void lcd_display_word()
{
  int cnt = 0;
  const Vocab_t* psCurVocabSet = stCurVocabSet;

  printDbg("lcd_display_start");

  //Display all words in one set
  for(cnt = 0;cnt<VOCAB_DOWNLOAD_MAX_NUM;cnt++)
  {
    if(psCurVocabSet[cnt].bDisplay)
    {

      //Serial.print("received_word:\t");
      Serial.println(psCurVocabSet[cnt].word);

      //Serial.print("received_partOfSpeech:\t\t");
      Serial.println(psCurVocabSet[cnt].pronunciation);
      //=================================================
      //LCD - scene 1
      //line 0
      int len = strlen(psCurVocabSet[cnt].word);
      char tmpBuf[16];

      if(len <=12)
      {
        strncpy(tmpBuf,psCurVocabSet[cnt].word,len);
        strncpy(&tmpBuf[len],"-",1);
        strncpy(&tmpBuf[len+1],psCurVocabSet[cnt].partOfSpech,3);

        lcd_print_line(0, tmpBuf);
      }
      else
      {
        lcd_print_line(0, psCurVocabSet[cnt].word);
      }

      //line 1
      char unAccentTranslationBuff[BUFF_MAX_LEN_TRANSLATION]; 
      memset(unAccentTranslationBuff, 0x00 , sizeof(unAccentTranslationBuff) - 1);

      printDbg("Before:");
      printDbg(psCurVocabSet[cnt].translation);
      convert_to_unaccented((char*)psCurVocabSet[cnt].translation, unAccentTranslationBuff);

      printDbg("After:");
      printDbg(unAccentTranslationBuff);

      lcd_print_line(1, unAccentTranslationBuff);   

      delay(3000);
      lcd.clear();

      //=================================================
      //LCD - scene 2
      //line 0
      // lcd_print_line(0, psCurVocabSet[cnt].definitions);
      // delay(1000);
      len = strlen(psCurVocabSet[cnt].definitions);

      printDbg("definitions len:");
      Serial.print(len);

      if( len < 16)
      {
        lcd_print_line(0, psCurVocabSet[cnt].definitions);
        delay(1000);
    
      }else if ( len >= 16 && len <= 40)
      {
        lcd_print_line(0, psCurVocabSet[cnt].definitions);
        delay(1000); 

        for (int positionCounter = 0; positionCounter < len-16; positionCounter++) 
        {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(400);
        }
      }
      else
      {
        longText = psCurVocabSet[cnt].definitions;
        gPosition = 0; // Tracks the current position of the text
        lcd_displayLongText(); 
      }

      delay(1000);
      lcd.clear();
      delay(2000);

      //=================================================
      //LCD - scene 3
      //line 0
      lcd_print_line(0, psCurVocabSet[cnt].commonPhrases);
      delay(1000);
      len = strlen(psCurVocabSet[cnt].commonPhrases);

      printDbg("commonPhrases len:");
      Serial.print(len);

      if( len > 16)
      {
        for (int positionCounter = 0; positionCounter < len - 16; positionCounter++) {
          // scroll one position left:
          lcd.scrollDisplayLeft();
          // wait a bit:
          delay(300);
        }
      }

      //line 1 
      delay(1000);
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

bool isDatabaseUpdate()
{
  bool bRet = false;
  return bRet;
}

void lcd_displayLongText() {
  int len = strlen(longText);
  do{
    lcd.clear(); // Clear the display for the new text

    // Print the next 16 characters starting at 'position' in the text
    for (int i = 0; i < 16; i++) {
      if ((gPosition + i) < strlen(longText)) { // Ensure we do not read past the string's end
        lcd.setCursor(i, 0); // Set the cursor to column i, line 0
        lcd.print(longText[gPosition + i]); // Print the character at 'gPosition + i'
      }
    }

    gPosition++; // Move the starting position right for the next frame of text

    // If the end of the message is reached, reset to start
    if (gPosition + 16 > strlen(longText)) {
      gPosition = 0; // Reset position to the start
      delay(1500); // Optional: wait a bit before restarting the scroll
      break;
    }

    delay(500); // Delay for text scrolling speed
  }while(gPosition < len);
}


// Optimized function
void lcd_display_word_optimized() {
    const Vocab_t* psCurVocabSet = stCurVocabSet;
    char tmpBuf[16];
    char unAccentTranslationBuff[BUFF_MAX_LEN_TRANSLATION];

    printDbg("lcd_display_start");

    for (int cnt = 0; cnt < VOCAB_DOWNLOAD_MAX_NUM; cnt++) {
        if (!psCurVocabSet[cnt].bDisplay) continue;

        // Display word and pronunciation
        Serial.println(psCurVocabSet[cnt].word);
        Serial.println(psCurVocabSet[cnt].pronunciation);

        // Prepare the word for LCD display
        int len = strlen(psCurVocabSet[cnt].word);
        if (len <= 12) {
            snprintf(tmpBuf, sizeof(tmpBuf), "%s-%s", psCurVocabSet[cnt].word, psCurVocabSet[cnt].partOfSpech);
            lcd_print_line(0, tmpBuf);
        } else {
            lcd_print_line(0, psCurVocabSet[cnt].word);
        }

        // Convert translation to unaccented version
        printDbg("Before:");
        printDbg(psCurVocabSet[cnt].translation);
        convert_to_unaccented((char *)psCurVocabSet[cnt].translation, unAccentTranslationBuff);
        printDbg("After:");
        printDbg(unAccentTranslationBuff);
        lcd_print_line(1, unAccentTranslationBuff);

        delay(3000);
        lcd.clear();

        // Handle definitions
        len = strlen(psCurVocabSet[cnt].definitions);
        printDbg("definitions len:");
        Serial.print(len);

        lcd_print_line(0, psCurVocabSet[cnt].definitions);

        if (len > 16 && len <= 40) {
            delay(1000);
            for (int positionCounter = 0; positionCounter < len - 16; positionCounter++) {
                lcd.scrollDisplayLeft();
                delay(400);
            }
        } else if (len > 40) {
            lcd_displayLongText();
        }

        delay(1000);
        lcd.clear();
        delay(2000);

        // Handle common phrases
        lcd_print_line(0, psCurVocabSet[cnt].commonPhrases);
        len = strlen(psCurVocabSet[cnt].commonPhrases);
        printDbg("commonPhrases len:");
        Serial.print(len);

        if (len > 16) {
            for (int positionCounter = 0; positionCounter < len - 16; positionCounter++) {
                lcd.scrollDisplayLeft();
                delay(300);
            }
        }

        delay(1000);
        lcd.clear();
    }

    printDbg("lcd_display_end");
}

//Main looo
void loop() {
  while(1)
  {
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Have a nice day");
    lcd.setCursor(1, 1);
    lcd.print("How are you?");
    delay(2000);

    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.print("Welcome to");
    lcd.setCursor(1, 1);
    lcd.print("Robotic Club ^^");
    delay(2000);    
  }
  //Display by context
  lcd_display_word();

  //lcd_display_word_optimized();
}
