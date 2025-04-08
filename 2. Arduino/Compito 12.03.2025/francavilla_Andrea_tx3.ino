#define PIN_TX 2
#define PIN_BTN 5
#define PIN_LED 4

#define bigEndian 1
#define Tb 1000
#define Nb 8

const char* message = "Hello world!\n";
int messageIndex = 0;
bool transmitActive = false;
bool buttonPressed = false;
long press_time = 0;

const char parity = 'N';

typedef enum {
  Idle,
  Start,
  Data,
  Parity,
  Stop1,
  Stop2
} State;

void setup() {
  pinMode(PIN_TX, OUTPUT);
  Serial.begin(9600);
  Serial.print("\ninit\n");
  digitalWrite(PIN_TX, HIGH);
  
  pinMode(PIN_BTN, INPUT);
  pinMode(PIN_LED, OUTPUT);
}

void loop() {
  buttonLogic();
  
  if (transmitActive && messageIndex < strlen(message)) {
    txMachine(true, message[messageIndex]);
  } else if (messageIndex >= strlen(message)) {
    transmitActive = false;
    messageIndex = 0;
  }
}

void buttonLogic() {
  int btn_state = digitalRead(PIN_BTN);
  
  if (btn_state == HIGH && !buttonPressed) {
    // Bottone appena premuto
    buttonPressed = true;
    press_time = millis();
    digitalWrite(PIN_LED, HIGH);
    
    // Inizia la trasmissione solo se non è già attiva
    if (!transmitActive) {
      transmitActive = true;
      messageIndex = 0;
    }
  } 
  else if (btn_state == LOW && buttonPressed) {
    // Bottone rilasciato
    buttonPressed = false;
    digitalWrite(PIN_LED, LOW);
  }
  else if (buttonPressed && (millis() - press_time >= 500)) {
    // Mantieni LED acceso per 500ms
    digitalWrite(PIN_LED, LOW);
  }
}

void txMachine(bool new_data, int wanted_tx) {
  static int current_bit = 0;
  static long time = 0;
  static State current_state = Idle;
  static int data = 0x0;
  
  if (millis() - time < Tb) {
    return;
  }
  time = millis();  
  
  State future_state = current_state;
  
  switch(current_state) {
    case Idle: 
      if (new_data) {
        data = wanted_tx;
        future_state = Start;
        Serial.print("\nTx char: ");
        Serial.print((char)data);
        Serial.print(" (");
        Serial.print(data, HEX);
        Serial.println(")");
      }
      break;
    
    case Start:
      transmitBit(0);
      future_state = Data;
      current_bit = 0;
      break;
    
    case Data:
      handleDataTx(data, current_bit);
      if (current_bit == Nb - 1) {
        future_state = Parity;
      }
      current_bit++;
      break;
    
    case Parity:
      handleParity(data);
      future_state = Stop1;
      break;
    
    case Stop1:
      transmitBit(1);
      future_state = Stop2;
      break;
    
    case Stop2:
      transmitBit(1);
      future_state = Idle;
      messageIndex++; // Passa al prossimo carattere
      break;
  }
  
  current_state = future_state;
}

void transmitBit(int bit) {
  digitalWrite(PIN_TX, bit);
  Serial.print(bit);
}

void handleDataTx(int data, int current_bit) {
  #ifdef bigEndian
  int bit = extractBit(data, Nb - current_bit - 1);
  #else
  int bit = extractBit(data, current_bit);
  #endif
  transmitBit(bit);
}

int extractBit(int data, int numBit) {
  int mask = 0b1 << numBit;
  return (data & mask) ? 1 : 0;
} 

void handleParity(int data) {
  int p = 0;
  if (parity == 'O') {
    p = 1;
  }
  for (int i = 0; i < Nb; i++) {
    p = p ^ extractBit(data, i);
  }
  transmitBit(p);
}