/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define timeSeconds 2

// Set GPIOs for LED and PIR Motion Sensor
const int led = D6;
const int motionSensor = D8;
bool flagStatus = 0;

// Timer: Auxiliary variables
unsigned long now = millis();
unsigned long lastTrigger = 0;
boolean startTimer = false;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Checks if motion was detected, sets LED HIGH and starts a timer
void detectsMovement() {
  flagStatus = 1;
  Serial.println("MOTION DETECTED!!!");
  digitalWrite(led, HIGH);
  startTimer = true;
  lastTrigger = millis();
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  lcdOpening("PIR AM312");

  // PIR Motion Sensor mode INPUT_PULLUP
  pinMode(motionSensor, INPUT_PULLUP);

  // Set LED to LOW
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
}

void loop() {
  // Current time
  now = millis();

  // Polling to check motion sensor status
  if (digitalRead(motionSensor) == HIGH) {
    detectsMovement();
  }

  // Turn off the LED after the number of seconds defined in the timeSeconds variable
  if (startTimer && (now - lastTrigger > (timeSeconds * 1000))) {
    flagStatus = 0;
    Serial.println("Motion stopped...");
    digitalWrite(led, LOW);
    startTimer = false;
  }

  //digitalWrite(led, digitalRead(motionSensor));
  
  display.clearDisplay();

  if (flagStatus == 1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print("MOTION DETECTED!!!");
    display.display();
  } else {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print("Motion stopped...");
  }

  display.display();
  delay(500);
}

void lcdOpening(char *teks) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 10);
  display.print("Test Sensor");
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print(teks);

  display.display();
  delay(2000);
}
