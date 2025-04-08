#define PIN_TX 2

#define bigEndian 1
#define Tb 1000
#define Nb 8

const char parity = 'N';
const int data = 0b10101010; //0xAA 

void setup() {
  pinMode(PIN_TX, OUTPUT); //eseguita solo all'avvio del sistema diciamo come deve lavorare il sistema
  Serial.begin(9600);
  Serial.print("\ninit\n");
  digitalWrite(PIN_TX, HIGH);
  myDelay(Tb);
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
  myDelay(Tb);
}

void  handleStart(){
  transmitBit(0);
}

void handleData() {
  #ifdef bigEndian
  for (int i = Nb-1; i>=0; i--)
  {
    int bit = extractBit(data, i);
    transmitBit(bit);
  }
  #else
  for (int i = 0; i<Nb; i++)
  {
    int bit = extractBit(data, i);
    transmitBit(bit);
  }
  #endif  
  
}
/*
estrae il bit numBit da data
input int data informazione binaria da cui estrarre il bit
input int numBit numero di bit da estrarre
output int 1 o 0
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
  if(parity == 'N')
  {
    
  }
  
  int p = 0;
  if (parity =='O')
  {
    p = 1;
  }
  for(int i = 0; i<Nb; i++)
  {
    p = p^ extractBit(data,i);
  }
  transmitBit(p);
}

void handleStop() {

  transmitBit(1);
  transmitBit(1);
}

void myDelay(int time_ms)
{
  int init = millis();
  while(init + time_ms > millis())
  {
      
  }
}