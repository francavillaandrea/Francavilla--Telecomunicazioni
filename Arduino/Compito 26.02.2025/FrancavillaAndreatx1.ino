#define PIN_TX 2

#define Tb 1000
#define Nb 8

const int data = 0b10101010; //0xAA

void setup() {
  pinMode(PIN_TX, OUTPUT);
  Serial.print("\ninit\n");
  digitalWrite(PIN_TX, HIGH);
  
}

void loop() {
  transmitChar();
  Serial.print("\n");
  delay(1000);
}

void transmitChar() {
  handleStart();
  handleData();
  handleParity(); // controllo errori
  handleStop();
}

void transmitBit(int bit){
  if(bit == 1)
  {
    digitalWrite(PIN_TX,HIGH);
  }
  else
  {
    digitalWrite(PIN_TX,LOW);
  }
  Serial.print(bit);
  delay(Tb);
}

void  handleStart(){
  transmitBit(0);
}

void handleData() {
  for (int i = Nb-1; i>=0; i--)
  {
    int bit = extractBit(data, i);
    transmitBit(bit);
  }
}
/*
* estrae il bit numBit da data
* input int data informazione binaria da cui estrarre il bit
* input int numBit numero di bit da estrarre
* output int 1 o 0
*/
int extractBit(int data, int numBit){
  
  int mask = 0b1 << numBit;
  int res = data & mask;
  if(res > 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
  
}


void handleParity() {
  int cnt = 0;
  for(int i = 0; i<Nb; i++)
  {
    cnt += extractBit(data, i);
  }
  
  Serial.print("N");
  
  if(cnt % 2 == 0)
  {
    transmitBit(1);
    Serial.print("E");
  }
  else
  {
    transmitBit(0);
    Serial.print("0");
  }
  
  
  
}

void handleStop() {

  transmitBit(1);
  transmitBit(1);
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

/*ESERCIZI

1) effettuare l'esercizio in BIG-ENDIAN

2) creare il controllo di parità DISPARI
   e relativo parametro di configurazione:
     "N" -> null, non trasmette parità
     "E" -> even, parità pari
     "O" -> odd, parità dispari
*/
