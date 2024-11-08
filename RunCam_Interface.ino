#define BUFF_SIZE 20

char szStr[30];

uint8_t
  txBuf[BUFF_SIZE],
  rxBuf[BUFF_SIZE];

uint8_t
  idx,
  crc,
  exp_crc,
  ch;

enum eRXStates{
  RX_HEADER = 0,
  RX_PACKET
};

uint8_t crc8_calc(uint8_t crc, unsigned char a, uint8_t poly){
  crc ^= a;
  for(int ii = 0; ii<8 ; ++ii){
    if(crc & 0x80)
      crc = (crc << 1) ^ poly;
    else
      crc = crc <<1;
  }
  return crc;
}

uint8_t calcCrc(uint8_t *buf, uint8_t numBytes){
  uint8_t crc = 0;
  for(uint8_t i=0; i<numBytes; i++)
    crc = crc8_calc(crc, buf[++i], 0xd5);
  return crc;

}



void setup(void){
  Serial.begin(115200);
  //give runcam time to transmit data when initialized
  delay(3000);
  //flush any chars out of receive buffer
  while( Serial.available() > 0)
    Serial.read();

  //get device info command

  txBuf[0] = 0xCC;
  txBuf[1] = 0x00;
  txBuf[2] = calcCrc( txBuf, 2);
  Serial.write(txBuf, 3);
}


void loop(void){
  static uint8_t state = RX_HEADER;

  //check if byte is available
  if(Serial.available() > 0){
    ch = Serial.read();
    switch(state){
      case RX_HEADER:
        //we're looking for the header (0xcc) byte
        if(ch == 0xcc){
          //we have found the header
          idx = 0;
          rxBuf[idx++] = ch;
          state = RX_PACKET;
        }
      break;

      case RX_PACKET:
        //collect bytes from packet into receive buffer
        rxBuf[idx++] = ch;

        //ensure we don't run off end of buffer
        if(idx ==BUFF_SIZE)
          idx--;
        if(idx==5){
          Serial.println();
          //compute the crc
          crc = calcCrc(rxBuf, 4);
          exp_crc = rxBuf[4];
          sprintf(szStr, "CRC 0x%02X (0x%02X)\n",crc,exp_crc);
          Serial.print(szStr);

          Serial.print(F("Message received: ") );
          //print the human readable HEX output
          for(ch = 0; ch < 5; ch++){
            sprintf(szStr, "%02X ", rxBuf[ch]);
            Serial.print(szStr);
          }//for
          Serial.println();

          while(1);

        }//if
      break;
    }//switch
  }//if
}//loop
