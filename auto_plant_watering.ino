//this is a script for my auto plant waterererer haha
//
//
//pin defining
const int LED_PIN = 3;
const int BUTTON_PIN = 4;
const int RELAY_PIN = 7;

//integers defined
unsigned long heldDownTime = 0;
unsigned long heldDownLimit = 0;
unsigned long waterTime = 0;
unsigned long days = 86400000;          //86400000 day, 3600000 hr
unsigned long timeToWater = 20000;  //30s seems to be a good time to water the plants
unsigned long daysToWater = 0;      //days
unsigned long printDelay = 0;
unsigned long daySelectTime = 0;
unsigned long daySelectTimeLeft = 10000;//10s

//on off state defined
int previouslyPressed = 0;
int longPress = 0;
int waterOn = 0;
int daySelectMode = 0;
int buttonState;

//FUNCTIONS
//loop function for easy loop creation
int loopOUT(int INPUT_PIN, int LOOP_NUM, int DELAY_NUM) {
  for (int i = 0; i < LOOP_NUM; i++) {
    digitalWrite(INPUT_PIN, HIGH);
    delay(DELAY_NUM);
    digitalWrite(INPUT_PIN, LOW);
    delay(DELAY_NUM);
  }
}
//
//SETUP
void setup() {
  Serial.begin(9600);
  // initialize digital pin as an output.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  //run blunkLED function on startup
  loopOUT(LED_PIN, 4, 100);
}
//
//LOOP
void loop() {
  //read button press
  buttonState = !digitalRead(BUTTON_PIN);
  //day selection mode start
  while (daySelectMode == 1) {
    daySelect();
  }
  //print status
  if (printDelay < millis()) {
    printDelay = millis() + 3000;
    digitalWrite(LED_PIN, LOW);
  } else if (printDelay == millis() && !buttonState && !waterOn && daySelectMode == 0) {
    digitalWrite(LED_PIN, HIGH);
    //state and count report
    Serial.println("\n----------- DEVICE STATUS -----------\n\nBUTTON STATE : ");
    Serial.println(buttonState);
    Serial.println("PREVIOUSLY PRESSED : ");
    Serial.println(previouslyPressed);
    Serial.println("BUTTON BEING HELD : ");
    Serial.println(longPress);
    Serial.println("HOLD TIME :: HOLD LIMIT : ");
    Serial.println(heldDownTime);
    Serial.println(heldDownLimit);
    Serial.println("RUNTIME : ");
    Serial.println(millis());
    Serial.println("WATER TIME :: WATER STATE : ");
    Serial.println(waterTime);
    Serial.println(waterOn);
    Serial.println("DAY SELECT MODE : ");
    Serial.println(daySelectMode);
    Serial.println("\n----------------------------");
  }

  //pre-set water schedule start
  if (millis() == waterTime && waterOn == 0) {
    Serial.println("!!!WATER TIME!!!");
    Serial.println("WATER ON");
    loopOUT(LED_PIN, 18, 50);
    digitalWrite(RELAY_PIN, HIGH);
    delay(500);
    waterOn = 1;
  }
  //pre-set water schedule stop
  if (millis() > waterTime + timeToWater && waterOn == 1 && waterTime > 0) {
    Serial.println("\n!!!WATER TIME COMPLETE!!!");
    loopOUT(LED_PIN, 2, 100);
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    delay(500);
    waterOn = 0;
    waterTime = millis() + (days * daysToWater);
  }
  //button release
  if (!buttonState) {
    //single press action water on
    if (previouslyPressed == 1 && waterOn == 0) {
      Serial.println("Single press");
      Serial.println("WATER ON");
      loopOUT(LED_PIN, 2, 100);
      digitalWrite(RELAY_PIN, HIGH);
      delay(500);
      waterOn = 1;
      //single press action water off
    } else if (previouslyPressed == 1 && waterOn == 1) {
      Serial.println("Single press");
      Serial.println("WATER OFF");
      loopOUT(LED_PIN, 2, 100);
      digitalWrite(RELAY_PIN, LOW);
      digitalWrite(LED_PIN, LOW);
      delay(500);
      waterOn = 0;
      //cancel water time if doing watering scheduled
      if (millis() > waterTime && waterTime != 0) {
        Serial.print("WATER TIME CANCELLED");
        waterTime = 0;
      }
    }
    //reset counts
    previouslyPressed = 0;
    heldDownTime = 0;
    heldDownLimit = 0;
    longPress = 0;
  }
  //button pressed set previous count and time limit
  if (buttonState && previouslyPressed == 0 && longPress == 0) {
    Serial.println("Button pressed");
    delay(200);
    heldDownTime = millis();
    heldDownLimit = heldDownTime + 3000;  //held for 3s
    previouslyPressed = 1;
  }
  //if held down and past limit do an action
  if (buttonState && previouslyPressed == 1 && millis() > heldDownLimit) {
    Serial.print("Button held");
    //reset time and count
    heldDownTime = 0;
    heldDownLimit = 0;
    previouslyPressed = 0;
    longPress = 1;
    loopOUT(LED_PIN, 10, 50);
    printDelay = 0;
    daysToWater = 0;
    daySelectMode = 1;
    daySelectTime = millis() + daySelectTimeLeft;
    Serial.println("\n\n----------- DAY SELECTION MODE -----------");
    Serial.println("DAYS SELECTED : ");
    Serial.println(daysToWater);
  }
}

//Day select mode loop
void daySelect() {
  //read button press
  buttonState = !digitalRead(BUTTON_PIN);
  //print status
  if (printDelay < millis()) {
    printDelay = millis() + 1000;//set 1s delay
    digitalWrite(LED_PIN, LOW);
  } else if (printDelay == millis() && !buttonState) {
    digitalWrite(LED_PIN, HIGH);
    //state and count report
    Serial.println("\n\n----------- DAY SELECTION MODE -----------\n\n");
    Serial.println("DAYS SELECTED : ");
    Serial.println(daysToWater);
    Serial.println("\n----------------------------");
  }
  //button pressed during selection
  if (buttonState) {
    daysToWater = daysToWater + 1;
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    Serial.print("\nDAYS SET TO: ");
    Serial.print(daysToWater);
  }
  //day select mode stop
  if (millis() >= daySelectTime || daysToWater >= 7) {
    Serial.print("\nDAY SELECTION COMPLETE");
    //set time to water, every * days
    waterTime = millis() + (days * daysToWater);
    if (daysToWater == 0){
      waterTime = 0;
    }
    printDelay = 0;
    Serial.println("\nDAYS SELECTED : ");
    Serial.println(daysToWater);
    Serial.print("\nWATERING TIME SET: \n");
    Serial.print(waterTime);
    Serial.println("\n----------------------------");
    loopOUT(LED_PIN, 10, 50);
    delay(500);
    loopOUT(LED_PIN, daysToWater, 250);
    daySelectMode = 0;
  }
}
//
//
//
//
