#define PIN_TX 2

//#define bigEndian 1
#define Tb 50
#define Nb 8

const char parity = 'E'; // N, O, E

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
  Serial.print("init\n");
  digitalWrite(PIN_TX, HIGH);
  
  delay(Tb);
  txMachine(true, 0b10101010);
}

void loop() {
  bool canTransmitNewData = txMachine(false, 0x0);
  if (canTransmitNewData) {
    txMachine(true, 0b11001100);
  }
}

/** -- FUNZIONI DELLA LOGICA DEL TRASMETTITORE -- */

/**
@output true se posso ricevere nuovi dati
*/
bool txMachine(bool new_data, int wanted_tx) {
  static State current_state = Idle;
  static int current_bit = 0;
  static long last_tx_time = 0;
  
  static int data = 0x0;
  
  if (millis() - last_tx_time < Tb && current_state != Idle) {
    // se lo stato è idle non devo aspettare che passi
    // il tempo di bit precedente, per cui posso procedere
    // direttamente con la macchina di Moore (che si occuperà
    // di iniziare a trasmettere il dato quando richiesto
    // dal chiamante)
    //
    // In tutti gli altri casi, sto trasmettendo, quindi non
    // posso accettare nuovi dati in ingresso -> ritorno false
    return false;
  }
  last_tx_time = millis();
  State future_state = current_state;
  switch (current_state) {
    case Idle:
    	//future_state = Start;
    	//Serial.print("\n");
        if (new_data) {
          data = wanted_tx;
          future_state = Start;
    	  Serial.print("\n");
        }
    	break;
    
    case Start:
    	transmitBit(0);
    	future_state = Data;
    	current_bit = 0;
    	break;
    
    case Data:
    	handleDataTx(data, current_bit);
      	current_bit++;
    	if (current_bit == Nb) {
          if (parity == 'N') {
            future_state = Stop1;
          } else {
            future_state = Parity;
          }
    	}
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
    	break;
  }
  current_state = future_state;
  
  return (current_state == Idle);
}

void transmitBit(int bit) {
  if (bit == 1) {
    digitalWrite(PIN_TX, HIGH);
  } else {
    digitalWrite(PIN_TX, LOW);
  }
  Serial.print(bit);
}

/**
* estrae il bit `numBit` da `data`
* @input int data informazione (binaria) da cui estrarre il bit
* @input int numBit numero di bit da estrarre
* @output int 1 o 0
*/
int extractBit(int data, int numBit) {
  int mask = 0b1 << numBit;
  int res = data & mask;
  if (res > 0) {
    return 1;
  } else {
    return 0;
  }
}

void handleDataTx(int data, int current_bit) {
  #ifdef bigEndian
  int bit = extractBit(data, Nb - current_bit - 1);
  #else
  int bit = extractBit(data, current_bit);
  #endif
  transmitBit(bit);
}

int calculateParity(int data) {
  int p = 0;
  if (parity == 'O') {
    p = 1;
  }
  for (int i = 0; i < Nb; i++) {
    p = p ^ extractBit(data, i);
  }
  return p;
}

void handleParity(int data) {
  if (parity == 'N')
    return;
  
  transmitBit(calculateParity(data));
}

/** -- FINE METODI PER LA LOGICA DEL TRASMETTITORE -- */
