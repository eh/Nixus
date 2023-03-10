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
  todo: ext 12/24hr, ext brightness, ext blink toggle, better debounce
  blink type (off, 1x1sec, 2x1sec, heartbeat, ?) 
  available gpio: 2, 3, 17, 18
*/

// Enable for DEBUG info on serial
const bool DEBUG = 0;

// Set to 0 for 24hr
const bool AMPM = 1;

// Play a random animation at the top of the hour
// 0 = off, 1 = random, 2 = jukebox, 3 = binary, 4 = countdown
const int hourlyAnim = 1;

// Animation time in ms
const int animTime = 2000;

// Global delay (brightness/flicker)
const int dispDelay = 3;

// Slow down animations
const int animDelay = 10;

#include <DS3232RTC.h>
DS3232RTC NixRTC;
tmElements_t NixTime;

int T_H1;   // Hour, left digit
int T_H2;   // Hour, right digit
int T_M1;   // Minute, left digit
int T_M2;   // Minute, right digit
int T_S1;   // Second, left digit
int T_S2;   // Second, right digit

int Hour;
int Minute;
int Second;

const int buttonHours = 9;   // the number of the Hours+ pushbutton pin
const int buttonMins = 10;   // the number of the Minutes+ pushbutton pin
const int blinkLED = 13;     // the number of the blinking LEDs pin

const int Digit1 = 3;   // Pin for each tube
const int Digit2 = 4;
const int Digit3 = 5;
const int Digit4 = 6;
const int Digit5 = 7;
const int Digit6 = 8;

unsigned long starttime;   // Useful for timers
unsigned long endtime;

void setup() {
  if (DEBUG) {
    Serial.begin(115200);
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

  randomSeed(millis());   // Initialize PRNG

  selectDigit(0);   // Turn off all 6 tubes (0 = default = all off)
  delay(1000);   // Tube warm-up (not really needed)

  if (DEBUG) { Serial.print("Boot Animations: "); }
  jukeboxAnim(animTime);
  binaryAnim(animTime);
  countdownAnim(animTime);
  if (DEBUG) { Serial.println("Complete"); }

  if (DEBUG) { Serial.println("Clock running"); }

}

// Play a random animation
void animRandom() {
  if (DEBUG) { Serial.println("Playing random animation"); }
  int y = random(1,4);
  switch(y) {
    case 1: jukeboxAnim(animTime); break;
    case 2: binaryAnim(animTime); break;
    case 3: countdownAnim(animTime); break;
    default: break;
  }
}

// Count down from 9 to 0, left-to-right, for y ms
void countdownAnim(int y) {
  if (DEBUG) { Serial.print("Countdown "); }
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= y) {  
    for (int digSel = 1; digSel <= 6; digSel++) {
      selectDigit(digSel);
      for (int cDown = 9; cDown >= 0; cDown--) {
        printNix(cDown); delay(animDelay);
        selectDigit(0); delay(dispDelay);
      }
    }
    endtime = millis();
  }
  selectDigit(0);  // Clear display
  delay(250);  
}

// Jukebox animation, cycle for y ms
void jukeboxAnim(int y) {
  if (DEBUG) {Serial.print("Jukebox "); }
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= y) {  
    T_M2 = random(0,10); T_M1 = random(0,10);
    T_H2 = random(0,10); T_H1 = random(0,10);
    T_S2 = random(0,10); T_S1 = random(0,10);

    selectDigit(1); printNix(T_M1); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(2); printNix(T_M2); delay(animDelay); selectDigit(0); delay(dispDelay);

    selectDigit(3); printNix(T_H1); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(4); printNix(T_H2); delay(animDelay); selectDigit(0); delay(dispDelay);

    selectDigit(5); printNix(T_H2); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(6); printNix(T_H2); delay(animDelay); selectDigit(0); delay(dispDelay);
    endtime = millis();
  }
  selectDigit(0); // Clear display
  delay(250);
}

// Random binary, cycle for y ms
void binaryAnim(int y) {
  if (DEBUG) { Serial.print("Binary "); }
  starttime = millis();
  endtime = starttime;
  while ((endtime - starttime) <= y) {
    selectDigit(1); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(2); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(3); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(4); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(5); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    selectDigit(6); printNix(random(0,2)); delay(animDelay); selectDigit(0); delay(dispDelay);
    endtime = millis();
  }
  selectDigit(0); // Clear display
  delay(250);
}

// Select (enable) a digit 1-6, 0 = all off
void selectDigit(int d) {
  // This brings the appropriate pin HIGH
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
void printNix(int x) {
  // Send BCD of (0..9) to the K155ID1 (74141)
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

// Behaves like Serial.printf()
void printSerial(const char *format, ...) {
    char buffer[512];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    Serial.print(buffer);
    va_end(args);
}

void loop() {
  // convert these to while loops
  if (digitalRead(buttonHours) == LOW) {
    delay(100);   // prevent over sensitive buttons
  }

  if (digitalRead(buttonHours) == LOW) {
    NixTime.Hour = hour() + 1;       // increase hour by 1
    NixTime.Minute = minute();
    NixTime.Second = second();
    NixTime.Day = day();
    NixTime.Month = month();
    NixTime.Year = year();
    setTime(NixTime.Hour, NixTime.Minute, NixTime.Second, NixTime.Day, NixTime.Month, NixTime.Year);
    NixRTC.set(now());
    delay(200);
  }

  if (digitalRead(buttonMins) == LOW) {
    delay(100);//prevent over sensitive buttons
  }

  if (digitalRead(buttonMins) == LOW) {
    NixTime.Hour = hour();
    NixTime.Minute = minute() + 1;   // increase minute by 1
    NixTime.Second = second();
    NixTime.Day = day();
    NixTime.Month = month();
    NixTime.Year = year();
    setTime(NixTime.Hour, NixTime.Minute, NixTime.Second, NixTime.Day, NixTime.Month, NixTime.Year);
    NixRTC.set(now());
    delay(200);
  }

  Hour = hour();
  if (AMPM) {
    if (Hour >= 12) { Hour -= 12; }
    if (Hour == 0) { Hour = 12; }
  }
  T_H2 = Hour % 10;
  T_H1 = Hour / 10 % 10;
  
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

  if ((T_M1 == 0) && (T_M2 == 0) && (T_S1 == 0) && (T_S2 == 0)) {
    // 0 = off, 1 = random, 2 = jukebox, 3 = binary, 4 = countdown
    switch(hourlyAnim) {
      case 0: break;
      case 1: animRandom(); break;
      case 2: jukeboxAnim(animTime); break;
      case 3: binaryAnim(animTime); break;
      case 4: countdownAnim(animTime); break;
      default: break;
    }
  }

  // blink
  if ((second() % 2) == 0) { digitalWrite(blinkLED, LOW); }
  if ((second() % 2) != 0) { digitalWrite(blinkLED, HIGH); }
}
