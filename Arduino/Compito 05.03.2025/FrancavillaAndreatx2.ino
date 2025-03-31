#define PIN_TX 2

#define bigEndian 1
#define Tb 1000  //tempo di bit in ms
#define Nb 8

const char parity = 'N'; //N,O,E
const int data = 0b10101010; 

typedef enum {
  Idle,
  Start,
  Data,
  Parity,
  Stop1,
  Stop2
} State;

State current_state = Idle;
int current_bit = 0;
unsigned long previousMillis = 0;

void setup() {
  pinMode(PIN_TX, OUTPUT);
  Serial.begin(9600);
  Serial.print("init\n");
  digitalWrite(PIN_TX, HIGH);
}

void loop() {
  txMachine();
}

void txMachine() {
  if (millis() - previousMillis < Tb) {
    return; //aspetta il tempo di bit
  }
  previousMillis = millis();

  State future_state = current_state;
  switch (current_state) {
    case Idle:
      future_state = Start;
      Serial.print("\n");
      break;
    
    case Start:
      transmitBit(0);
      future_state = Data;
      current_bit = 0;
      break;
    
    case Data:
      handleDataTx();
      if (current_bit == Nb) {
        future_state = Parity;
      }
      break;
    
    case Parity:
      handleParity();
      future_state = Stop1;
      break;
    
    case Stop1:
      transmitBit(1);
      future_state = Stop2;
      break;
    
    case Stop2:
      transmitBit(1);
      future_state = Idle;
      break;
  }
  current_state = future_state;
}

void transmitBit(int bit) {
  digitalWrite(PIN_TX, bit ? HIGH : LOW);
  Serial.print(bit);
}

int extractBit(int data, int numBit) {
  #ifdef bigEndian
    int adjustedBit = (Nb - 1) - numBit;
  #else
    int adjustedBit = numBit;
  #endif
  int mask = 0b1 << adjustedBit;
  return (data & mask) ? 1 : 0;
}

void handleDataTx() {
  int bit = extractBit(data, current_bit);
  transmitBit(bit);
  current_bit++;
}

void handleParity() {
  if (parity == 'N') return;
  int p = (parity == 'O') ? 1 : 0;
  for (int i = 0; i < Nb; i++) {
    p ^= extractBit(data, i);
  }
  transmitBit(p);
}



/**
ESERCIZI

1) gestire il tempo: rispettare Tb (trasmettere un bit ogni Tb)
2) gestione Big Endian

*/
