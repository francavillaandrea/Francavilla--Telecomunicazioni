#define PIN_RX 3

//#define bigEndian 1
#define Tb 50
#define Nb 8

#define DEBUG 1

const char parity = 'E'; // N, O, E

typedef enum {
  Idle,
  Start,
  Data,
  Parity,
  Stop1,
  Stop2
} State;

int last_rx_data = 0;

void setup() {
  pinMode(PIN_RX, INPUT);
  Serial.begin(9600);
  Serial.print("init\n");
  
  delay(Tb);
}

void loop() {
  bool new_data = rxMachine();
  if (new_data) {
    Serial.print("\n");
    Serial.println(last_rx_data);
  }
}

bool rxMachine() {
  static State current_state = Idle;
  static int current_bit = 0;
  static long last_rx_time = 0;
  
  static int data = 0x0;
  
  if (millis() - last_rx_time < Tb && current_state != Idle) {
    /* se lo stato è idle devo monitorare il canale per identificare
       il momento preciso in cui inizia il bit di Start in modo
       da sincronizzare il canale, per cui posso procedere
       direttamente con la macchina di Moore (che si occuperà
       di passare allo stato di Start quando necessario)
      
       In tutti gli altri casi, sto ricevendo dati, quindi devo
       aspettare la temporizzazione corretta. In ogni caso, non
       ho ancora un dato definitivo a disposizione, per cui
    */ ritorno false
    return false;
  }
  last_rx_time = millis();
  
  State future_state = current_state;
  int channel_bit, wanted_bit;
  bool data_ok = false;
  switch (current_state) {
    case Idle:
      channel_bit = readBit(false);
      if (channel_bit == 0) {
        /* se leggo dal canale valore 0 vuol dire che è appena
           iniziato il bit di Start, quindi mi devo sincronizzare.
           Devo leggere sempre il bit a metà della trasmissione,
           per cui manipolo il tempo in modo che la prossima lettura
           avvenga tra Tb/2 anziché tra Tb. Visto che normalmente
           aspettiamo Tb dall'ultima transizione, abbasso il valore
           attualmente memorizzato per "ingannare" il controllore
        */ inserito prima dello switch.
        future_state = Start;
        last_rx_time -= Tb/2;
      }
      break;
    case Start:
      /* non devo fare nulla di particolare. Sono sicuro che lo stato
         sia valido perché l'ho già verificato per uscire da Idle.
         Inizializzo le variabili che mi servono nei passaggi successivi:
         - current_bit indica quale bit sto attualmente leggendo,
           funziona come una sorta di indice di un for, quindi lo
           faccio partire da 0 e avanzare fino a Nb (vedi step Data)
         - data è il contenitore in cui vado a memorizzare effettivamente
           l'informazione che sto ricevendo. Lo inizializzo a 0 in modo
           che tutti i bit siano a 0, dopodiché andrò ad inserire i
      */   valori corretti nello step Data
      future_state = Data;
      current_bit = 0;
      data = 0x0;
      break;
    case Data:
      /* salvo in channel_bit quello che leggo effettivamente sul
         canale di trasmissione. Userò lo stesso approccio anche
      */ negli step successivi.
      channel_bit = readBit();
      #ifdef bigEndian
      /* (guarda prima descrizione in little endian)
         l'unica differenza è che devo iniziare a riempire il byte
         da sinistra anziché da destra, per cui il primo bit va
      */ inserito alla posizione 7 se Nb è 8, e così via
      data |= channel_bit << (Nb - current_bit - 1); 
      #else
      /*in channel_bit ho il valore che ho letto sul canale (1 o 0),
         per inserirlo nel dato uso uno shift verso sinistra.
        
         esempio: assumiamo di voler ricevere il dato 0b01010101
         e di aver già ricevuto i primi 2 bit da destra (01), quindi
         sto ricevendo il terzo bit (1). Di conseguenza, attualmente ho:
         - current_bit = 2 (terzo bit, indice 2 perché parte da 0)
         - data = 0b00000001
                         ^
         - channel_bit = 1
         devo inserire il channel_bit nella posizione indicata da (^),
         quindi "sposto" il bit del numero di posizioni contenuto in
         current_bit, ottenendo 0b00000100
         A questo punto, per ottenere il dato completo, posso eseguire
         un OR bit a bit:
         0b00000001 |
         0b00000100 =
         ------------
         0b00000101
      */ora data = 0b00000101
      data |= channel_bit << current_bit;
      #endif
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
      /* leggo il bit attualmente sul canale (channel_bit)
         e calcolo quello che mi aspetto (wanted_bit), usando
         le stesse funzioni che uso nell'automa di trasmissione,
         e confronto i risultati.
      */ In caso di errore, segnalo il fallimento.
      channel_bit = readBit();
      wanted_bit = calculateParity(data);
      if (channel_bit == wanted_bit) {
        future_state = Stop1;
      } else {
        printError("controllo parita' fallito");
        future_state = Idle;
      }
      break;
    case Stop1:
      /* so che lo Stop deve arrivare con bit a 1, quindi lo
         controllo. Se dovessi identificare un bit a 0, vuol
      */ dire che qualcosa è andato storto.
      channel_bit = readBit();
      if (channel_bit == 1) {
        data_ok = true;
      } else {
        printError("controllo stop fallito");
      }
      future_state = Idle;
      break;
  }
  current_state = future_state;
  if (data_ok) {
    last_rx_data = data;
  }
  return data_ok;
}

int readBit() {
  readBit(true);
}

int readBit(bool print) {
  int bit = digitalRead(PIN_RX);
  #ifdef DEBUG
  if (print) {
  	Serial.print(bit);
  }
  #endif
  return bit;
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

void printError(char* error) {
  Serial.println("ERRORE TRASMISSIONE");
  Serial.println(error);
}
