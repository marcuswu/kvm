const int FADER_PINS[4] = {A0, A1, A2, A3};
const int NUM_FADERS = 4;
int faderValues[4] = {-1, -1, -1, -1};
const int KVM_PIN = 8;
bool kvmState = false;
const int debounceDelay = 50;
unsigned long lastKvmChange = 0;
bool stateChanged = false;

void setup() {
  // put your setup code here, to run once:
  initFaders(NUM_FADERS);
  pinMode(KVM_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  // read the state of the faders
  stateChanged |= readFaders(faderValues, NUM_FADERS);
  // check kvm button
  stateChanged |= checkKvm();
  // send fader state to terminal
  if (stateChanged) {
    outputSerial(faderValues, NUM_FADERS, kvmState);
    stateChanged = false;
  }
}

void initFaders(int numFaders) {
  for (int i = 0; i < numFaders; i++) {
    pinMode(FADER_PINS[i], INPUT);
  }
}

bool readFaders(int *faderValues, int numFaders) {
  int newValue;
  bool stateChange = false;
  for (int i = 0; i < numFaders; i++) {
    newValue = analogRead(FADER_PINS[i]);
    stateChange |= newValue != faderValues[i];
    faderValues[i] = newValue;
  }
  return stateChange;
}

void outputSerial(int *faderValues, int numFaders, bool kvmState) {
  for (int i = 0; i < numFaders; i++) {
    if (i != 0) {
      Serial.print("|");
    }
    Serial.print(faderValues[i]);
  }

  Serial.print("\r\n");

  if (kvmState) {
    Serial.print("event:0\r\n");
  }
}

bool checkKvm() {
  bool pressed = kvmState;
  if (millis() - lastKvmChange < debounceDelay) {
    return false;
  }

  pressed = digitalRead(KVM_PIN) == LOW;
  if (pressed == kvmState) {
    return false;
  }
  kvmState = pressed;
  lastKvmChange = millis();
  return true;
}
