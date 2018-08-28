#define SENSOR_PIN 1
#define LCD_PIN 3
#define LED_PIN 4

#define JITTER_THRESHOLD 10
#define ABS_RISING_THRESHOLD 25
#define BLACKOUT_MS 500
#define HZ_MIN 3
#define DELAY 10

int curDiff;
int sensorValue;
int prevValue = 0;
bool isRising = false;
size_t timer;
size_t nextRead = 0;
size_t lastRisingTime = 0;
size_t blockingTime = 0;
size_t absRising = 0;
uint16_t risingTimeoutMin = 2000 / HZ_MIN;

void testBlackout(size_t timer) {
  if (blockingTime > timer) {
    digitalWrite(LCD_PIN, HIGH);
  } else {
    digitalWrite(LCD_PIN, LOW);
  }
}

void setState(int value, size_t timer) {
  if (value > 0) {
    absRising += abs(value);
    if (absRising > ABS_RISING_THRESHOLD && !isRising) {
      isRising = true;2
      //Serial.println("RISE");
      digitalWrite(LED_PIN, HIGH);
      if (risingTimeoutMin > timer - lastRisingTime) {
        blockingTime = timer + BLACKOUT_MS;
      }
      lastRisingTime = timer;
    }
  } else {
    absRising = 0;
    isRising = false;
    digitalWrite(LED_PIN, LOW);
  }
}

void bootNotify(){
  for (short i = 0; i < 5; i++) {
    digitalWrite(LCD_PIN, i%2);
    digitalWrite(LED_PIN, !i%2);
    delay(20);
  }
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LCD_PIN, LOW);
}

void setup() {
  pinMode(LCD_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  bootNotify();
}

void loop() {
  timer = millis();
  if (nextRead <= timer) {
    sensorValue = analogRead(SENSOR_PIN);
    curDiff = prevValue - sensorValue;
    if (abs(curDiff) > JITTER_THRESHOLD) {
      setState(curDiff, timer);
    }
    prevValue = sensorValue;
    nextRead = timer + DELAY;
  }
  testBlackout(timer); 
}
