#include "Arduino.h"
#include "TimerOne.h"

#define ENCODER_PIN_A 2
#define ENCODER_PIN_B 4
#define BUTTON_PIN 3
#define SHIFT_DATA_PIN 9
#define SHIFT_CLOCK_PIN 8
#define LED_PIN 5
#define FIRST_DIGIT_TRIGGER 6
#define SECOND_DIGIT_TRIGGER 7
#define BUZZER_PIN 10

volatile unsigned int displayedNumber = 0;
volatile unsigned int secondCounter = 0;
volatile bool timerStarted = false;

byte digitList[10] = {
	0b11011110, 0b00011000, 0b01110110, 0b01111010, 0b10111000,
	0b11101010, 0b11101110, 0b01011000, 0b11111110, 0b11111010
};

void encoderHandler() {
  if (digitalRead(ENCODER_PIN_A) == 1 && digitalRead(ENCODER_PIN_B) == 1) {
      if (displayedNumber < 99) {
          displayedNumber++;
      }
  }
  if (digitalRead(ENCODER_PIN_A) == 1 && digitalRead(ENCODER_PIN_B) == 0) {
      if (displayedNumber > 0) {
          displayedNumber--;
      }
  }
  _delay_ms(1);
}

void timeCount()
{
	digitalWrite(LED_PIN, digitalRead(LED_PIN) ^ 1);

	secondCounter++;

	if (secondCounter >= 60){
		displayedNumber--;
		secondCounter = 0;
	}
}

void buttonHandler()
{
	_delay_ms(5);
	if (displayedNumber > 0) {
		timerStarted = true;
		detachInterrupt(0);
		detachInterrupt(1);
		Timer1.attachInterrupt(timeCount);
		tone(BUZZER_PIN, 2000, 50);
	}
}

void showNumber(int number) {
    int secondDigit = number % 10;
	int firstDigit = (number - secondDigit) / 10;

    if (number >= 10) {
		digitalWrite(SECOND_DIGIT_TRIGGER, LOW);
        shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, LSBFIRST, digitList[firstDigit]);
		digitalWrite(FIRST_DIGIT_TRIGGER, HIGH);
    }
    _delay_ms(10);

    digitalWrite(FIRST_DIGIT_TRIGGER, LOW);
    shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, LSBFIRST, digitList[secondDigit]);
	digitalWrite(SECOND_DIGIT_TRIGGER, HIGH);

    _delay_ms(10);
}

void doSignal()
{
	tone(BUZZER_PIN, 2500, 600);
}

void stopSignalHandler()
{
	_delay_ms(5);
	Timer1.detachInterrupt();
	detachInterrupt(0);
	detachInterrupt(1);
	attachInterrupt(0, encoderHandler, RISING);
	attachInterrupt(1, buttonHandler, RISING);
}

void setup()
{
    pinMode(ENCODER_PIN_A, INPUT);
    pinMode(ENCODER_PIN_B, INPUT);
    pinMode(BUTTON_PIN, INPUT);
    pinMode(SHIFT_CLOCK_PIN, OUTPUT);
    pinMode(SHIFT_DATA_PIN, OUTPUT);
    pinMode(FIRST_DIGIT_TRIGGER, OUTPUT);
    pinMode(SECOND_DIGIT_TRIGGER, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);

	digitalWrite(ENCODER_PIN_A, HIGH);
    digitalWrite(ENCODER_PIN_B, HIGH);
    digitalWrite(BUTTON_PIN, HIGH);

    Timer1.initialize();

    attachInterrupt(0, encoderHandler, RISING);
    attachInterrupt(1, buttonHandler, RISING);
}

void loop()
{
	showNumber(displayedNumber);

	if (displayedNumber == 0 && timerStarted == true) {
		timerStarted = false;
		Timer1.detachInterrupt();
		attachInterrupt(1, stopSignalHandler, RISING);
		Timer1.attachInterrupt(doSignal);
		digitalWrite(LED_PIN, LOW);
	}
}
