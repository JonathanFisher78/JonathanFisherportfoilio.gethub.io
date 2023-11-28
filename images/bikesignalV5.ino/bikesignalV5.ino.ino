const int right_handle = 13; // replace with your first button pin
const int leftArrow = 10;    // replace with your first LED pin

const int left_handle = 12;   // replace with your second button pin
const int rightArrow = 9;     // replace with your second LED pin
const int BrakeLights = 11;   // Pin LED
int debounceDelay = 100;

// variables for debounce
int buttonState1 = LOW;              // current state of the RIGHT handle
int lastButtonState1 = LOW;          // previous state of the first button
unsigned long lastDebounceTime1 = 0; // last time the first button state was toggled

int buttonState2 = LOW;              // current state of the LEFT handle
int lastButtonState2 = LOW;
unsigned long lastDebounceTime2 = 0; // last time the second button state was toggled

bool brakesOn = false;
bool brakeLightsOn = false; // Track if brake lights are on
unsigned long lastReleaseTime = 0;   // last time both handles were released

void setup() {
  Serial.begin(9600); // Initialize serial communication
  pinMode(right_handle, INPUT);
  pinMode(leftArrow, OUTPUT);

  pinMode(left_handle, INPUT);
  pinMode(rightArrow, OUTPUT);
  pinMode(BrakeLights, OUTPUT);

  // set initial LED states
  digitalWrite(leftArrow, LOW);
  digitalWrite(rightArrow, LOW);
  digitalWrite(BrakeLights, LOW);
}

void loop() {
  checkButton(right_handle, leftArrow, debounceDelay, lastButtonState1, lastDebounceTime1, buttonState1, "left");
  checkButton(left_handle, rightArrow, debounceDelay, lastButtonState2, lastDebounceTime2, buttonState2, "right");

  // Check for at least one handle pulled
  if (buttonState1 == LOW || buttonState2 == LOW) {
    // At least one handle is pulled, update the lastReleaseTime
    lastReleaseTime = millis();

    // Turn on both arrows
    digitalWrite(leftArrow, HIGH);
    digitalWrite(rightArrow, HIGH);

    // Turn on brake lights if they were previously off
    if (!brakeLightsOn) {
      brakesOn = true;
      digitalWrite(BrakeLights, HIGH);
      brakeLightsOn = true;
    }
  } else {
    // Both handles at rest
    // Check if both handles were released within 2 seconds of each other
    if (millis() - lastReleaseTime > 2000) {
      // Both handles were released more than 2 seconds ago, turn off both arrows and brake lights
      digitalWrite(leftArrow, LOW);
      digitalWrite(rightArrow, LOW);
      digitalWrite(BrakeLights, LOW);

      // Update the flags
      brakesOn = false;
      brakeLightsOn = false;
    } else if (brakesOn && brakeLightsOn) {
      // Handles were released within 2 seconds, keep the brake lights on
      digitalWrite(BrakeLights, HIGH);
    }
  }
}

void checkButton(int buttonPin, int ledPin, int debounceDelay, int &lastButtonState, unsigned long &lastDebounceTime, int &buttonState, const char* direction) {
  int reading = digitalRead(buttonPin);

  // Check if the reading is different from the last button state
  if (reading != lastButtonState) {
    // Update last debouncing time
    lastDebounceTime = millis();
  }

  // Check if the difference between the current time and last debouncing time exceeds the debounce delay
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the reading is different from the current button state, update the button state
    if (reading != buttonState) {
      buttonState = reading;

      // If the button is pressed (HIGH), print the direction and blink the arrow
      if (buttonState == HIGH) {
        Serial.println(direction);
        blinkArrow(direction, buttonState1, buttonState2);
      }
    }
  }

  // Update the last button state
  lastButtonState = reading;
}

void blinkArrow(const char* direction, int buttonState1, int buttonState2) {
  const unsigned long blinkInterval = 300;  // Interval for each blink
  const int blinkCount = 3;                 // Number of blinks

  // Check if at least one handle is pulled
  if (buttonState1 == LOW || buttonState2 == LOW) {
    for (int i = 0; i < blinkCount; ++i) {
      // Check handles state before each blink
      if (buttonState1 == LOW || buttonState2 == LOW) {
        // Toggle LED on
        digitalWrite((strcmp(direction, "left") == 0) ? leftArrow : rightArrow, HIGH);
        Serial.println("LED ON");

        // Wait for blinkInterval
        unsigned long startTime = millis();
        while (millis() - startTime < blinkInterval) {
          // Check handles state during the blink
          if (buttonState1 == HIGH && buttonState2 == HIGH) {
            // If both handles are released, stop blinking
            digitalWrite(leftArrow, LOW);
            digitalWrite(rightArrow, LOW);
            return;
          }
        }

        // Toggle LED off
        digitalWrite((strcmp(direction, "left") == 0) ? leftArrow : rightArrow, LOW);
        Serial.println("LED OFF");

        // Wait for blinkInterval
        startTime = millis();
        while (millis() - startTime < blinkInterval) {
          // Check handles state during the blink
          if (buttonState1 == HIGH && buttonState2 == HIGH) {
            // If both handles are released, stop blinking
            digitalWrite(leftArrow, LOW);
            digitalWrite(rightArrow, LOW);
            return;
          }
        }
      } else {
        // Stop blinking if handles are released during the sequence
        digitalWrite(leftArrow, LOW);
        digitalWrite(rightArrow, LOW);
        break;
      }
    }

    // Ensure both arrows are off after the blink sequence
    digitalWrite(leftArrow, LOW);
    digitalWrite(rightArrow, LOW);
  }
}
