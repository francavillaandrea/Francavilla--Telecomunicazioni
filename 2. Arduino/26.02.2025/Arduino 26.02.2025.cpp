#define PIN_TX 2

//#define bigEndian 1
#define Tb 1000
#define Nb 8


const char parity = 'N';
const int data = 0b10101010; //0xAA 

typedef enum
{
  Idle,
  Start,
  Data,
  Parity,
  Stop1,
  Stop2
} State;

State current_state = Idle;
int current_bit = 0;

void setup() {
  pinMode(PIN_TX, OUTPUT); //eseguita solo all'avvio del sistema diciamo come deve lavorare il sistema
  Serial.begin(9600);
  Serial.print("\ninit\n");
  digitalWrite(PIN_TX, HIGH);
}

void loop()
{
  txMachine();
}

void txMachine()
{
  State future_state = current_state;
  switch(current_state)
  {
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
        if(current_bit == Nb)
        {
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


void transmitBit (int bit)
{
  if(bit == 1)
  {
    digitalWrite(PIN_TX,HIGH);
  }
  else
  {
    digitalWrite(PIN_TX,LOW);
  }
  Serial.print(bit);
}

void handleDataTx()
{
  int bit = extractBit(data, current_bit);
  transmitBit(bit);
  current_bit++;
}

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

void handleParity()
{
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