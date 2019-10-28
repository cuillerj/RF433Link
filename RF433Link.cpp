/*
*/
#include "RF433Link.h"
#include <VirtualWire.h>       // needed for RF434 network
//#define debugOn
uint8_t _stationAddress;
uint8_t _gatewayAddress;
uint8_t _senderPin;
uint8_t _reveiverPin;
int _speedLink;
uint8_t _outMsg[VW_MAX_MESSAGE_LEN];
uint8_t _outFrameNumber=0x00;
uint8_t _nbRetry;
uint8_t _retry;
uint8_t _outLen;
uint8_t _inBbuf[VW_MAX_MESSAGE_LEN];
uint8_t _inBuflen = VW_MAX_MESSAGE_LEN;
uint8_t *_PinBuflen =&_inBuflen;
uint8_t _inRFaddrE = 0x00;
uint8_t _inRFaddrR = 0x00;
uint8_t _inRFcmde = 0x00;
uint8_t _inRFdlen = 0x00;
uint8_t _inRFcount = 0x00;
 char _inC = 0;
unsigned long _lastSentTime;

RF433Link::RF433Link(int speedLink)
{
	_speedLink=speedLink;
}
void RF433Link::SetParameters(uint8_t stationAddress, uint8_t gatewayAddress, uint8_t senderPin, uint8_t reveiverPin, uint8_t nbRetry)
{
	_stationAddress=stationAddress;
	_gatewayAddress=gatewayAddress;
	_senderPin=senderPin;
	_reveiverPin=reveiverPin;;
	_nbRetry=nbRetry;
}

void RF433Link::Start()
{
	_outMsg[0]=_stationAddress;
	_outMsg[1]=_gatewayAddress;
	_outMsg[2]=0x00;
	_outMsg[3]=0x00;
	vw_set_tx_pin(_senderPin);   // choisir la broche 3 pour émettre
	vw_set_rx_pin(_reveiverPin);     // choisir la broche 5 pour recevoir
	//vw_set_ptt_pin(6);     // choisir la broche 6 pour ptt - ajoute suite bug ecriture carte SD
	vw_setup(_speedLink);       // vitesse de reception  - tunned pour etre en synch avec le master ??
	vw_rx_start();        // démarrer la réception	
}
void RF433Link::SendData(uint8_t *PData, uint8_t dataLen){
	dataLen=min(dataLen,maxDataLen);
	_outFrameNumber++;
	_outMsg[3]=_outFrameNumber;
	for (int i=0;i<dataLen;i++){
		PData++;
        _outMsg[i+headerLen]=*PData;
     }
	_outMsg[4]=dataLen;
	_outLen=dataLen+headerLen;
	 vw_send((uint8_t *)_outMsg,dataLen+headerLen);
     vw_wait_tx(); 
     _lastSentTime=millis();
	_retry=0;
}
void RF433Link::RetrySend(){
	if(millis()>_lastSentTime+random(1500,2500) && _retry < _nbRetry ){
#if defined(debugOn)
		Serial.println(_retry);
#endif
		_retry++;
		 vw_send((uint8_t *)_outMsg,_outLen);
		vw_wait_tx(); 
		_lastSentTime=millis();	
	}
}
boolean RF433Link::HaveAMessge(){
	return vw_have_message();
}
receivedData RF433Link::Receive(){
	_inBuflen = VW_MAX_MESSAGE_LEN;
	receivedData rc;
	if (vw_get_message(_inBbuf, &_inBuflen)) {
#if defined(debugOn)
		Serial.print("RFf In Len:");
		Serial.print(_inBuflen);
		Serial.print(" - ");
#endif
		_inRFaddrE = _inBbuf[0];
		_inRFaddrR = _inBbuf[1];
		if (_inBbuf[3]==_inRFcount){
#if defined(debugOn)
			Serial.print("already:");
			Serial.println(_inRFcount);
#endif
			_inRFdlen=0x00; // nothing new
		}
		else{			
			_inRFcount = _inBbuf[3];
			_inRFdlen = _inBbuf[4];
		}
#if defined(debugOn)
		for (int i = 0; i < _inBuflen; i++)
		  {
			Serial.print(_inBbuf[i], HEX);
			Serial.print("-");
		  }
		  Serial.println("");		
#endif
		if (_inRFaddrR == _stationAddress && _inRFaddrE == _gatewayAddress){ // is this gateway the destination ?
#if defined(debugOn)
		Serial.println("for me");
#endif
		}
		else {
#if defined(debugOn)
		  Serial.print("ignored from:");
		  Serial.print(_inRFaddrE);
		  Serial.print(" to:");
		  Serial.println(_inRFaddrR);
 #endif
		  _inRFdlen=0x00;
		}
	}
#if defined(debugOn)
	Serial.print("data Len:");
	Serial.println(_inRFdlen);
#endif
	rc.dataLen=_inRFdlen;
	uint8_t *Pdata;
	Pdata=&(_inBbuf[5]);
	rc.data=Pdata;
	return (rc);
}	