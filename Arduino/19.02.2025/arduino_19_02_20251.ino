#define PIN_TX 2
#define Tb 1000
#define Nb 8
const int data = 0b101010; //0xAA

void setup() {
    pinMode(PIN_TX, OUTPUT);
    Serial.begin(9600);
    Serial.print("init\n");
    digitalWrite(PIN_TX, HIGH);
}


void loop() {
    transmitChar();
    Serial.print("\n");
    delay(1000);
}
void transmitChar()
{
    handleStart();
    handleData();
    handleParity(); //Controllo dell'errore
    handleStop();
}
void transmitBit(int bit)
{
    if(bit == 1)
    {
        digitalWrite(PIN_TX, HIGH);

    }
    else
    {
        digitalWrite(PIN_TX, LOW);
    }
    Serial.print(bit);
    delay(Tb);
}

void handleStart()
{
    transmitBit(0);
}

void handleData()
{
    for(int i  = 0; i < Nb; i++)
    {
        int bit = extractBit(data, i);
        transmitBit(bit);
    }
}

void handleParity()
{
    int cnt = 0;
    for (int i = 0; i < Nb; ++i)
    {
        cnt+= extractBit(data,i);
    } if(cnt % 2 == 0)
    {
        transmitBit(0);
    }
    else
    {
        transmitBit(1);
    }
    
}

void handleStop()
{
    transmitBit(1);
    transmitBit(1);
}
//Estrae il bit "num bit" da "data"
//@input int data informazione da cui estarre il bit
//@input int numBit numero di bit das estrarre
//@output int 1 o 0
int extractBit(int data, int i)
{
    int mask = 0b1 << i;
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