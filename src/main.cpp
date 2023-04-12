#include <Arduino.h>

const uint8_t ledPins[] = {8, 9, 10, 11, 12, 13, A0, A1, A2};
const uint8_t buttonPins[] = {0, 1, 2, 3, 4, 5, 6, 7, A5};
const uint8_t numPins = 9;
const uint16_t debounceDelay = 50;

uint8_t sequence[10];
uint8_t sequenceLength = 1;
uint32_t startTime;

int8_t getButtonPress(uint8_t currentIndex);
void playSequence();
void startNewGame();
void signalFailure();

void setup() {
  for (uint8_t i = 0; i < numPins; i++) {
    pinMode(ledPins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  randomSeed(analogRead(A4));
  startNewGame();
}

void loop() {
  playSequence();
  uint8_t correctIndex = 0;
  while (correctIndex < sequenceLength) {
    int8_t pressedButton = getButtonPress(correctIndex);
    if (pressedButton == -1) { // Timeout
      signalFailure();
      startNewGame();
      return;
    } else if (pressedButton == -2) { // Incorrect button
      signalFailure();
      startNewGame();
      return;
    } else if (pressedButton == sequence[correctIndex]) {
      correctIndex++;
    }
  }
  sequenceLength++;
  if (sequenceLength > 10) {
    // end-of-game logic
    startNewGame();
    return;
  }
  delay(1000);
}

void startNewGame() {
  sequenceLength = 1;
  for (uint8_t i = 0; i < 10; i++) {
    sequence[i] = random(numPins);
  }
}

void playSequence() {
  for (uint8_t i = 0; i < sequenceLength; i++) {
    digitalWrite(ledPins[sequence[i]], HIGH);
    delay(500);
    digitalWrite(ledPins[sequence[i]], LOW);
    delay(500);
  }
}

int8_t getButtonPress(uint8_t currentIndex) {
  startTime = millis();
  while (true) {
    for (uint8_t i = 0; i < numPins; i++) {
      if (digitalRead(buttonPins[i]) == LOW) {
        delay(debounceDelay); // Debounce delay
        if (digitalRead(buttonPins[i]) == LOW) {
          while (digitalRead(buttonPins[i]) == LOW); // Wait for button release
          if (i == sequence[currentIndex]) {
            return i;
          } else {
            return -2; // Incorrect button
          }
        }
      }
    }
    if (millis() - startTime > 5000) { // 5 seconds timeout
      return -1;
    }
  }
}

void signalFailure() {
  for (uint8_t j = 0; j <= 4; j++) {
    for (uint8_t i = 0; i < numPins; i++) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(150);
    for (uint8_t i = 0; i < numPins; i++) {
      digitalWrite(ledPins[i], LOW);
    }
    delay(150);
  }
  delay(500);
}
