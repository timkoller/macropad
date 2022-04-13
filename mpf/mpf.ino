#include <LiquidCrystal.h>
#include <LiquidCrystalFast.h>
#include <time.h>
#include <Keypad.h>

LiquidCrystalFast lcd(0, 1, 2, 3, 4, 5);
// Dimension of the LCD display: width, height.
static const int LCD_WIDTH = 16;
static const int LCD_HEIGHT = 2;

static const int MAX_SLIDER_VAL = 1023;
static const int SLIDER_VAL_UPDATE_THRESHOLD = 5;
static const int NUMBER_OF_SLIDERS = 4; // 5;

// slider pins
const int SLIDER_PINS[NUMBER_OF_SLIDERS] = {A0, A1, A2, A3}; // , A4};
// slider names
const char SLIDER_NAMES[NUMBER_OF_SLIDERS][LCD_WIDTH] = {"mic", "gme", "ent", "oth"}; // , "slider4"};
const char SLIDER_NAMES_SHORT[NUMBER_OF_SLIDERS][LCD_WIDTH] = {"mic", "gme", "ent", "oth"}; // , "slider4"};
// current slider values in percent: [0..100].
int vals[NUMBER_OF_SLIDERS];

// button pins
const int OUTPUT_BUTTON_PIN = 12;
const int INPUT_BUTTON_PIN = 11;
int BUTTON_STATE = 0;

static const long SCREENMODE_CHANGE_TIME = 2000;
// 0: show time and date
// 1: show slider values
static const int SCREENMODE_TIME_AND_DATE = 0;
static const int SCREENMODE_SLIDER_VALS = 1;
// store current screen mode
int screenMode = SCREENMODE_TIME_AND_DATE;
long screenModeTime = 0;

// get unix timestamp of startup
long startTimestamp = 0;

/*// Dimension of the KeyPad
const byte ROWS = 1; //four rows
const byte COLS = 1; //three columns
char keys[ROWS][COLS] = {
  {"h 1"},
};

byte rowPins[ROWS] = {12}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
*/
void setup() {
  // init slider values
  for (int valIdx = 0; valIdx < NUMBER_OF_SLIDERS; valIdx++) {
    vals[valIdx] = -1;
  }
  
  // put your setup code here, to run once:
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  Serial.begin(9600);
  Serial.setTimeout(10);
  pinMode(OUTPUT_BUTTON_PIN, OUTPUT);
  // print statup message
  lcd.setCursor(2, 0);
  lcd.print("TMK mpf v0.3");
  
  int waittime = 100; // 3000;
  /* while (startTimestamp <= 0) {
    if (Serial.available() > 0) {
      String str = Serial.readString();
      char timestamp[str.length()];
      str.toCharArray(timestamp, str.length());
      startTimestamp = atol(timestamp) - millis();
    }
    delay(50);
    waittime -= 50;
  } */
  
  delay(abs(waittime));
}

void loop() {
  // check for slider value changes
  // boolean valChanged = false;
  boolean valChanged[NUMBER_OF_SLIDERS];
  int numberOfChangedVals = 0;
  for (int valIdx = 0; valIdx < NUMBER_OF_SLIDERS; valIdx++) {
    /* valChanged = valChanged 
        || checkForValUpdate(SLIDER_PINS[valIdx], valIdx, SLIDER_NAMES[valIdx]); */
    valChanged[valIdx] = checkForValUpdate(SLIDER_PINS[valIdx], valIdx, SLIDER_NAMES[valIdx]);
    numberOfChangedVals += valChanged[valIdx];
  }

  // adapt screen mode according whether a slider has changed
  boolean reprintScreen = numberOfChangedVals > 0; // valChanged;
  if (numberOfChangedVals > 0) {
    if (screenMode == SCREENMODE_TIME_AND_DATE) {
      lcd.clear();  
    }
    // at least one value was changed. Show the changes on the lcd screen.
    screenMode = SCREENMODE_SLIDER_VALS;
    screenModeTime = millis();
  } else if (millis() - screenModeTime > SCREENMODE_CHANGE_TIME) {
    // no value was changed for SCREENMODE_CHANGE_TIME, therefore change screen to SCREENMODE_TIME_AND_DATE
    reprintScreen = screenMode != SCREENMODE_TIME_AND_DATE;
    screenMode = SCREENMODE_TIME_AND_DATE;
  } else if (millis() % 1000 == 0) {
    // reprint screen at least every second to update the date and time
    reprintScreen = true;
  }

  // print on lcd
  if (reprintScreen) {
    switch (screenMode) {
    case SCREENMODE_TIME_AND_DATE: {
        // TODO: show time and date
        /* // compute current unix time stamp
        time_t currentTimestamp = startTimestamp + millis();
        struct tm* tm_info = localtime(&currentTimestamp);
        // format serial message
        char timeFormatted[27] = "";
        strftime(timeFormatted, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        // print formatted time to screen
        lcd.clear();
        lcd.setCursor(0, 0);
        dtostrf(startTimestamp, strlen(timeFormatted) + 1, 0, timeFormatted);
        lcd.print(timeFormatted); */

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("TODO: show date");
      }
      break;
    case SCREENMODE_SLIDER_VALS: {
        // show the changed values
        // compute the print width for each slider value
        int printWidth = LCD_WIDTH / NUMBER_OF_SLIDERS; // numberOfChangedVals;
        // compute the number of remaining pixels
        int remWidth = LCD_WIDTH % NUMBER_OF_SLIDERS; // numberOfChangedVals;
        // print slider values on lcd
        int horizontalLcdOffset = 0;
        for (int valIdx = 0; valIdx < NUMBER_OF_SLIDERS; valIdx++) {
          // printValOnLcd(SLIDER_PINS[valIdx], valIdx, SLIDER_NAMES[valIdx]);
          // print each changed slider value
          // if (valChanged[valIdx]) {
              // distribute the remaining pixel across the first sliders
              printValOnLcd(horizontalLcdOffset, printWidth + min(remWidth, 1), SLIDER_PINS[valIdx], valIdx, SLIDER_NAMES[valIdx]);
              // increase horizontal offset on screen for the next value
              horizontalLcdOffset += printWidth + min(remWidth, 1);
              // reduce the number of remaining pixels until 0 is reached
              remWidth = max(remWidth -1, 0);
          // }
        }
      }
      break;
    default:
      lcd.clear();
      break;
    }
  }
}

/*
  Read slider value and check whether the value has changed.
  return true when the value has changed, false otherwise.
*/
boolean checkForValUpdate(int pin, int valIdx, const char* sliderName) {
  // read analog pin value
  int newVal = analogRead(pin);
  if (abs(vals[valIdx] - newVal) > SLIDER_VAL_UPDATE_THRESHOLD 
        || /* or inital value */ vals[valIdx] < 0) {
    // update value variable
    vals[valIdx] = newVal;
    // create serial message
    char msg[] = "0 00";
    int valPercent = getValPercentage(vals[valIdx]);
    // format serial message
    sprintf (msg, "%i %i", valIdx + 1, valPercent);
    // send new value via serial
    Serial.println(msg);
    // return true, because the slider value has changed
    return true;
  }
  return false;
}

/*
  Display the current slider value and the slider name on the lcd screen.
  @param printWidth determins the number of characters to print on the screen.
*/
void printValOnLcd(int horizontalLcdOffset, int printWidth, int pin, int valIdx, const char* sliderName) {
  char lcdString[printWidth + 1];
  // compute percentage value from slider value and format slider percentage
  dtostrf(getValPercentage(vals[valIdx]), printWidth, 0, lcdString);
  // print slider percentage
  lcd.setCursor(horizontalLcdOffset, 0);
  lcd.print(lcdString);
  // print the slider name in the second line
  lcd.setCursor(horizontalLcdOffset, 1);
  lcd.print(sliderName);
}

/*
  Convert the analog slider value [0...MAX_SLIDER_VAL] to a percentage integer [0...100].
*/
int getValPercentage(int value) {
  return (int) ((float) value / (float) MAX_SLIDER_VAL * 100.0);
}
