/*
   SIX DIGIT NIXIE CLOCK WITH RTC by DOMINIC BUMBACA
   Dominic@Bumbaca.com
   cut and paste this into your Arduino IDE anf you should be good to go.
	
	Use an ATMEGA328 and make sure the below tow libraries are available..good luck.
        the jukebox section was for the bulbs to flicker rapidly and randomly for a few seconds every hour, the intent is to prevent cathode poisoning.
        since this is multiplexed I dont think this is necessary.I have included the loop anyway in case you need an example.
*/

/*
  Switch to DS3232 library, general clean-up, additional features.
  todo: 12/24hr, ext brightness, blink toggle, are there any avail gpio? better debounce
*/

// Enable for DEBUG info on serial
bool DEBUG = 0;

// Global delay (brightness/flicker)
unsigned int dispDelay = 3;

// Set to 0 for 24hr (not implemented)
bool AMPM = 1;

#include <DS3232RTC.h>
DS3232RTC NixRTC;
tmElements_t tm;
time_t t;

unsigned int T_H1;    // Hour, left digit
unsigned int T_H2;    // Hour, right digit
unsigned int T_M1;
unsigned int T_M2;
unsigned int T_S1;
unsigned int T_S2;

unsigned int Hour;
unsigned int Minute;
unsigned int Second;

const unsigned int BTN_HRS = 9;    // the number of the pushbutton pin
const unsigned int BTN_MNS = 10;   // the number of the pushbutton pin
//const unsigned int buzz = 2;
//const unsigned int blink = 13;

const unsigned int Digit1 = 3;     // Pin for each tube
const unsigned int Digit2 = 4;
const unsigned int Digit3 = 5;
const unsigned int Digit4 = 6;
const unsigned int Digit5 = 7;
const unsigned int Digit6 = 8;

unsigned int starttime;   // Useful for timers
unsigned int endtime;

void setup() {
  Serial.begin(115200);
  if (DEBUG) {
    Serial.println ("Nixus DEBUG Output");
    delay(1000);
  }

  // Start RTC sync
  NixRTC.begin();  
  setSyncProvider(NixRTC.get);
  if (timeStatus() != timeSet) {
    if (DEBUG) { Serial.println("RTC Sync failure"); }
  } else {
    if (DEBUG) { Serial.println("RTC Sync success"); }
  }

  pinMode(2,  OUTPUT);
  pinMode(3,  OUTPUT);
  pinMode(4,  OUTPUT);
  pinMode(5,  OUTPUT);
  pinMode(6,  OUTPUT);
  pinMode(7,  OUTPUT);
  pinMode(8,  OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(9,  INPUT_PULLUP); //
  pinMode(10, INPUT_PULLUP); // (now the external pull up resistors are redundant)
  pinMode(A0, OUTPUT);       // dont know why but better to address these pins this way...
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  selectDigit(0); // Turn off all 6 tubes (0 = default = all off)

  if (DEBUG) { Serial.print("Boot Animations: "); }
  countdownAnim(4);
  jukeboxAnim(4);
  binaryAnim(4);
  if (DEBUG) { Serial.println("Complete"); }

}

// Count down from 9 to 0 y times, from left-to-right
void countdownAnim(unsigned int y) {
  if (DEBUG) { Serial.print("Countdown "); }
  for (unsigned int animCount = 1; animCount <= y; animCount++) {
    for (unsigned int digSel = 1; digSel <= 6; digSel++) {
      selectDigit(digSel);
      for (unsigned int cDown = 9; cDown >= 0; cDown--) {
        printNix(cDown);
        delay(6);
      }
    }
  }
}

// Jukebox animation, repeat y times
void jukeboxAnim(unsigned int y) {
  if (DEBUG) {Serial.print("Jukebox "); }
  for (unsigned int jukeCount = 1; jukeCount <= y; jukeCount++) {
    T_M2 = random(9); T_M1 = random(9);
    T_H2 = random(9); T_H1 = random(9);
    T_S2 = random(9); T_S1 = random(9);

    selectDigit(1); printNix(T_M1); delay(dispDelay); selectDigit(0); delay(dispDelay);
    selectDigit(2); printNix(T_M2); delay(dispDelay); selectDigit(0); delay(dispDelay);

    selectDigit(3); printNix(T_H1); delay(dispDelay); selectDigit(0); delay(dispDelay);
    selectDigit(4); printNix(T_H2); delay(dispDelay); selectDigit(0); delay(dispDelay);

    selectDigit(5); printNix(T_H2); delay(dispDelay); selectDigit(0); delay(dispDelay);
    selectDigit(6); printNix(T_H2); delay(dispDelay); selectDigit(0); delay(dispDelay);
  }
}

// Toggling binary, repeat y times
void binaryAnim(unsigned int y) {
  if (DEBUG) { Serial.print("Binary "); }
  for (unsigned int binCount = 1; binCount <= y; binCount++) {
    starttime = millis();
    endtime = starttime;
    while ((endtime - starttime) <=250) {
      selectDigit(1); printNix(1); delay(dispDelay);
      selectDigit(2); printNix(0); delay(dispDelay);
      selectDigit(3); printNix(1); delay(dispDelay);
      selectDigit(4); printNix(0); delay(dispDelay);
      selectDigit(5); printNix(1); delay(dispDelay);
      selectDigit(6); printNix(0); delay(dispDelay);
      endtime = millis();
    }
    starttime = millis();
    endtime = starttime;
    while ((endtime - starttime) <=250) {
      selectDigit(1); printNix(0); delay(dispDelay);
      selectDigit(2); printNix(1); delay(dispDelay);
      selectDigit(3); printNix(0); delay(dispDelay);
      selectDigit(4); printNix(1); delay(dispDelay);
      selectDigit(5); printNix(0); delay(dispDelay);
      selectDigit(6); printNix(1); delay(dispDelay);
      endtime = millis();
    }
  }
}

// Select (enable) a digit 1-6, 0 = all off
void selectDigit(unsigned int d) {
  if (d == 1) {
    digitalWrite(Digit1, HIGH);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, LOW);
  } else if (d == 2) {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, HIGH);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, LOW);
  } else if (d == 3) {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, HIGH);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, LOW);
  } else if (d == 4) {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, HIGH);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, LOW);
  } else if (d == 5) {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, HIGH);
    digitalWrite(Digit6, LOW);
  } else if (d == 6) {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, HIGH);
  } else {
    digitalWrite(Digit1, LOW);
    digitalWrite(Digit2, LOW);
    digitalWrite(Digit3, LOW);
    digitalWrite(Digit4, LOW);
    digitalWrite(Digit5, LOW);
    digitalWrite(Digit6, LOW);
  }
}

// Display number x to the currently enabled digit
void printNix(unsigned int x) {
  if (x == 1) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
  } else if (x == 2) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
  } else if (x == 3) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
  } else if (x == 4) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
  } else if (x == 5) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
  } else if (x == 6) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
  } else if (x == 7) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, LOW);
  } else if (x == 8) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
  } else if (x == 9) {
    digitalWrite(A0, HIGH);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, HIGH);
  } else if (x == 0) {
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);
  }
}

// Serial.printf()
void printSerial(const char *format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.print(buffer);
    va_end(args);
}

void loop() {

  if (digitalRead(BTN_HRS) == LOW) {
    delay(100);   //prevent over sensitive buttons
  }

  if (digitalRead(BTN_HRS) == LOW) {
    tm.Hour = hour() + 1;           // set the tm structure to 23h31m30s on 13Feb2009
    tm.Minute = minute();   // increase minute by 1
    tm.Second = second();
    tm.Day = day();
    tm.Month = month();
    tm.Year = year();
    setTime(tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tm.Year);
    NixRTC.set(now());
    delay(200);
  }

  if (digitalRead(BTN_MNS) == LOW) {
    delay(100);//prevent over sensitive buttons
  }

  if (digitalRead(BTN_MNS) == LOW) {
    tm.Hour = hour();           // set the tm structure to 23h31m30s on 13Feb2009
    tm.Minute = minute() + 1;   // increase minute by 1
    tm.Second = second();
    tm.Day = day();
    tm.Month = month();
    tm.Year = year();
    setTime(tm.Hour, tm.Minute, tm.Second, tm.Day, tm.Month, tm.Year);
    NixRTC.set(now());
    delay(200);
  }

  Hour = hour();
  if (Hour > 12) {    // Hour is 24hr
    Hour = Hour - 12; // now Hour is 12hr
  }
  if (Hour > 9) {
    T_H1 = 1;
    T_H2 = Hour - 10;
  } else if (Hour == 0) {
    T_H1 = 1;
    T_H2 = 2;
  } else {
    T_H1 = 0;
    T_H2 = Hour;
  }
  
  Minute = minute();
  T_M2 = Minute % 10;
  T_M1 = Minute / 10 % 10;
  
  Second = second();
  T_S2 = Second % 10;
  T_S1 = Second / 10 % 10;

  if (DEBUG) { printSerial("%d%d:%d%d:%d%d\n", T_H1, T_H2, T_M1, T_M2, T_S1, T_S2); }

  selectDigit(1); printNix(T_H1); delay(dispDelay);
  selectDigit(2); printNix(T_H2); delay(dispDelay);
  selectDigit(3); printNix(T_M1); delay(dispDelay);
  selectDigit(4); printNix(T_M2); delay(dispDelay);
  selectDigit(5); printNix(T_S1); delay(dispDelay);
  selectDigit(6); printNix(T_S2); delay(dispDelay);

  if ( (second() % 2) == 0) {
    digitalWrite(13, LOW);
  }

  if ( (second() % 2) != 0) {
    digitalWrite(13, HIGH);
  }

}
