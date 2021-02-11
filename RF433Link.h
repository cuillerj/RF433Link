/*





*/
#include <Arduino.h>
#ifndef rF433Link_h_included
#define rF433Link_h_included

#define maxDataLen 20
#define headerLen 5
#define RFheaderLen 5
#define alertLedBit 7
#define alertBuzzBit 6
typedef struct  {byte *data; uint8_t dataLen;} receivedData;
class RF433Link
{
public:
//RF433Link(uint8_t stationAddress, uint8_t gatewayAddress, uint8_t senderPin, uint8_t reveiverPin, int speedLink, uint8_t nbRetry);
RF433Link(int speedLink);
void SetParameters(uint8_t stationAddress, uint8_t gatewayAddress, uint8_t senderPin, uint8_t reveiverPin, uint8_t nbRetry);
void Start();
void SendData(uint8_t *PData, uint8_t dataLen);   // 
void SendData(uint8_t* PData, uint8_t dataLen, uint8_t routerFlag);  // add a flag specificaly for the RF router (as alert)
void RetrySend();
receivedData Receive();
boolean HaveAMessge();

};
#endif