/*
   Test pin soldering (PCB socket pins) for Arduino Nano
   USB connection only required for sketch upload, but can also be used while running test (14.)
   Initial testing using external power source (1. to 13.)
*/

/*
   Test setup:
   1. connect batterys (4*1.2V) to + and - on breadboard and check for 5V voltage
   2. connect + to 5V and - to -GND(1) and check if arduino blinks/startup -> GND(1) ok
   3. re-connect GND from (1) to (2 on ICSP) and check if arduino blinks/startup -> GND(2) ok
   4. re-connect GND from (2) to (3) and check if arduino blinks/startup -> GND(3) ok
   5. measure 5V on ICSP 5V (and on RST 1,2,3)
   6. measure 3V3 on 3V3
   7. press RST button and check if arduino blinks/startup
   8. connect RST(1) to GND and check if arduino blinks/startup, disconnect
   9. connect RST(2 on ICSP) to GND and check if arduino blinks/startup, disconnect
   10. connect RST(3) to GND and check if arduino blinks/startup, disconnect
   11. disconnect batterys
   12. connect USB and upload script, disconnect USB
   13. connect batterys
   14. script will set all analog and digital pins to INPUT (default),
       then one by one (D0..D13, A0..A7):
        blink three times (ready),
        wait 2sec
        set pin to INPUT_PULLUP
        read, blink once on internal diode if ok (HIGH) / twice if nok,
        set pin to INPUT
       blink four times to signal check for LOW
       then one by one (D0..D13, A0..A7):
        blink three times (ready),
        wait 2sec -> connect pin to GND using a wire
        set pin to INPUT_PULLUP
        read, blink once on internal diode if ok (LOW) / twice if nok,
        set pin to INPUT
        disconnect wire

   VIN?
   REF?
   TODO: D13 not detected correctly (LOW instead HIGH)
   TODO: A6,A7 do not have INPUT_PULLUP (-> floating default values) and are only analogRead()

*/

#define LED_PIN 13 // internal LED pin
#define WAIT_TIME 2000 // in msec

int digitalPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
int analogPins[] = {A0, A1, A2, A3, A4, A5, A6, A7}; // A0=14...

void initPinsAsInput(int pinarr[], size_t arrsize) {
  for (int i = 0; i < arrsize; i++) {
    pinMode(pinarr[i], INPUT);
  }
}

void blink(int blinkPerSecond, int blinkPin) {
  int onTime = 1000 / (blinkPerSecond * 2);

  // init again for sure after digitalWrite on pin 13
  pinMode(blinkPin, OUTPUT);
  digitalWrite(blinkPin, LOW);

  for (int i = 0; i < blinkPerSecond; i++ ) {

    // turn LED on:
    digitalWrite(blinkPin, HIGH);
    delay(onTime);

    // turn LED off:
    digitalWrite(blinkPin, LOW);
    delay(onTime);
  }
}

void checkPins(int pinarr[], size_t arrsize, char pinType, int checkState) {
  // print what will be tested
  Serial.print(F("Testing "));
  if (pinType == 'D') {
    Serial.print(F("digital "));
  } else if (pinType == 'A') {
    Serial.print(F("analog "));
  }
  if (checkState == HIGH) {
    Serial.print(F("HIGH: "));
  } else if (checkState == LOW) {
    Serial.print(F("LOW: "));
  }
  Serial.println(arrsize);


  for (int i = 0; i < arrsize; i++) {
    int pinState = 0;

    Serial.print(F("Next pin is "));
    Serial.print(pinType);
    Serial.print(pinarr[i]);
    Serial.print(": ");

    blink(3, LED_PIN);
    delay(WAIT_TIME);

    pinMode(pinarr[i], INPUT_PULLUP);

    pinState = (pinType == 'D') ? digitalRead(pinarr[i]) : analogRead(pinarr[i]);
    Serial.print(pinState);

    // 'convert' analog to digital
    if (pinType == 'A' && pinState > 950) {
      pinState = 1;
    } else if (pinType == 'A' && pinState < 100) {
      pinState = 0;
    }

    if (pinState == checkState) {
      blink(1, LED_PIN); // 1x = ok
      Serial.println(F("ok "));
    } else {
      blink(2, LED_PIN); // 2x = nok
      Serial.println(F("nok "));
    }

    pinMode(pinarr[i], INPUT);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // initialize the internal LED pin as an output:
  pinMode(LED_PIN, OUTPUT);

  // initialize the digital & analog pins on pin as an input (default)
  initPinsAsInput(digitalPins, sizeof digitalPins / sizeof * digitalPins);
  initPinsAsInput(analogPins, sizeof analogPins / sizeof * analogPins);

}

void loop() {
  // put your main code here, to run rexpeatedly:

  // checking pin for HIGH (do not connect respective pin to ground with wire)
  checkPins(digitalPins, sizeof digitalPins / sizeof * digitalPins, 'D', HIGH);
  checkPins(analogPins, sizeof analogPins / sizeof * analogPins, 'A', HIGH);

  blink(4, LED_PIN);

  // checking pin for LOW (DO connect respective pin to ground with wire)
  checkPins(digitalPins, sizeof digitalPins / sizeof * digitalPins, 'D', LOW);
  checkPins(analogPins, sizeof analogPins / sizeof * analogPins, 'A', LOW);

  blink(4, LED_PIN);
  blink(4, LED_PIN);
}


