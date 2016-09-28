#include "BasicSerial3.h"

// timeout in centi seconds after which servo signal will no longer evaluated and a restart is required
// set to 0 if disabled
#define SWITCH_TIMEOUT  300

#define MIN_CHANNEL_CMD 1000
#define MAX_CHANNEL_CMD 2000
#define NUM_CHANNELS  8

#define SERVO_PORT  PINB
#define SERVO_PIN   3
#define SERVO_HIGH()  (PINB & (1 << SERVO_PIN))

#define MAX_VALID_CHANNELCOUNT  15

byte gSetChannel;
byte gLastChannel;
byte gValidChannelCount;
unsigned int gTimeout= 0;

void setup() {
  // calibrate internal oszillator for 5V VCC
  OSCCAL += 7; 
  gLastChannel= 0xFF;
  gValidChannelCount= 0;
  gSetChannel= 0xFF;
  // wait to boot the rest of the system
  delay(2000);
}

void loop() {
#if SWITCH_TIMEOUT != 0
  if (gTimeout > SWITCH_TIMEOUT)
    return;
  ++gTimeout;
#endif

  evalChannel();
  delay(100);
}

void evalChannel()
{
  byte channel= readChannel();
  if (channel >= NUM_CHANNELS) {
    gValidChannelCount= 0;
    return;
  }

  if (channel != gLastChannel) {
    gLastChannel= channel;
    gValidChannelCount= 0;
    return;
  }
  ++gValidChannelCount;
  if (gValidChannelCount < MAX_VALID_CHANNELCOUNT)
    return;
  gValidChannelCount= 0;

  if (channel != gSetChannel) {
    transmitChannel(channel);
    gSetChannel= channel;
  }
}

byte readChannel()
{
  unsigned int pulse= readServoPulse();
  // put into range
  if (pulse < MIN_CHANNEL_CMD)
    return 0xFF;
  if (pulse > MAX_CHANNEL_CMD)
    return 0xFF;

  unsigned int pos= pulse - MIN_CHANNEL_CMD;
  return pos / ((MAX_CHANNEL_CMD - MIN_CHANNEL_CMD) / NUM_CHANNELS);
}

// external overflow variable from attiny13 arduino core
extern unsigned long ovrf;
unsigned int readServoPulse()
{
  // turn off timer0 and reset all state variables
  TCCR0B= 0;  
  TCNT0= 0;
  ovrf= 0;
  
  // wait until low
  while(SERVO_HIGH()) {}

  // wait until high (signal start)
  while(!SERVO_HIGH()) {}

  // enable timer0 with system clock
  TCCR0B= (1 << CS00);  
  while(SERVO_HIGH()) {}
  TCCR0B= 0;

  if (ovrf > 0xFF)
    return 0xFFFF;
    
  // make one big timer value and convert tick to uS (divide by 1.2)
  return ((((unsigned int)ovrf << 8) | TCNT0) * 5) / 6;
}

void transmitChannel(byte channel)
{
  if (channel > 8)
    return;
    
  // precomputed crc8
  // 0: 0x1C
  // 1: 0xC9
  // 2: 0x63
  // 3: 0xB6
  // 4: 0xE2
  // 5: 0x37
  // 6: 0x9D
  // 7: 0x48
  const byte preCompCrc8[8]= { 0x1C, 0xC9, 0x63, 0xB6, 0xE2, 0x37, 0x9D, 0x48 };
  
  byte cmd[6];
  cmd[0]= 0xAA;
  cmd[1]= 0x55;
  cmd[2]= 0x07;
  cmd[3]= 0x01;
  cmd[4]= 32 + channel;  // Race Band offset 32
  cmd[5]= preCompCrc8[channel];

  TxByte((byte)0);

  for(int i= 0; i < 6; ++i) {
    TxByte(cmd[i]);
  }

  TxByte((byte)0);
  // set tx pin to input
  UART_DDR &= ~(1<<UART_Tx);
}

