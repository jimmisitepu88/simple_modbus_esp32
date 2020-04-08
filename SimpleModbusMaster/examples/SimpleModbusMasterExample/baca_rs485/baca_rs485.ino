#include <SimpleModbusMaster.h>

#define baud 9600
#define timeout 1000
#define polling 200 // the scan rate
#define retry_count 10 
#define TxEnablePin 14

const int _id = 80;
unsigned long cur_get_rs485, old_get_rs485;

enum
{
  PACKET1,
  PACKET2,
  PACKET3,
  PACKET4,
  PACKET5,
  PACKET6,
  
  // leave this last entry
  TOTAL_NO_OF_PACKETS
};
Packet packets[TOTAL_NO_OF_PACKETS];
//for voltage
packetPointer packet1 = &packets[PACKET1];
packetPointer packet2 = &packets[PACKET2];
packetPointer packet3 = &packets[PACKET3];
packetPointer packet4 = &packets[PACKET4];
packetPointer packet5 = &packets[PACKET5];
packetPointer packet6 = &packets[PACKET6];

unsigned int regs_van[2];
unsigned int regs_vbn[2];
unsigned int regs_vcn[2];
unsigned int regs_freq[2];
unsigned int regs_kwh[4];
unsigned int regs_tap[4];

float kwh; float tap;
float van, vbn, vcn, freq;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  packet1->id = _id;
  packet1->function = READ_HOLDING_REGISTERS;
  packet1->address = 0;
  packet1->no_of_registers = 1;
  packet1->register_array = regs_van;

  packet2->id = _id;
  packet2->function = READ_HOLDING_REGISTERS;
  packet2->address = 1;
  packet2->no_of_registers = 1;
  packet2->register_array = regs_vbn;

  packet3->id = _id;
  packet3->function = READ_HOLDING_REGISTERS;
  packet3->address = 2;
  packet3->no_of_registers = 1;
  packet3->register_array = regs_vcn;

  packet4->id = _id;
  packet4->function = READ_HOLDING_REGISTERS;
  packet4->address = 24;
  packet4->no_of_registers = 1;
  packet4->register_array = regs_freq;

  packet5->id = _id;
  packet5->function = READ_HOLDING_REGISTERS;
  packet5->address = 25;
  packet5->no_of_registers = 2;
  packet5->register_array = regs_kwh;
  
  modbus_configure(baud, timeout, polling, retry_count, TxEnablePin, packets, TOTAL_NO_OF_PACKETS);
  
}
void loop() {
  get_rs485(2000);
}

void baca_rs485(){
  modbus_update(packets);
  van = (regs_van[0])*0.01;
  vbn = (regs_vbn[0])*0.01;
  vcn = (regs_vcn[0])*0.01;
  freq = (regs_freq[0])*0.01;
  kwh = (unsigned long)regs_kwh[1] << 16 | regs_kwh[0];
  kwh = kwh*0.1;
}

void get_rs485(unsigned long _waktu){
  cur_get_rs485 = millis();
  old_get_rs485 = cur_get_rs485;
  clear_rs485();
  Serial.println("baca_rs485");
  while(cur_get_rs485 - old_get_rs485 < _waktu){
    cur_get_rs485 = millis();
    baca_rs485();
  }
  Serial.print("van: "); Serial.print(van);Serial.print(", ");
  Serial.print("vbn: "); Serial.print(vbn);Serial.print(", ");
  Serial.print("vcn: "); Serial.print(vcn);Serial.print(", "); 
  Serial.print("kwh: "); Serial.print(kwh);Serial.print(", "); 
  Serial.print("freq: ");Serial.print(freq);Serial.println(); delay(500);

}
void clear_rs485(){
  Serial.println("clear rs485");
  regs_van[0] = 0;
  regs_vbn[0] = 0;
  regs_vcn[0] = 0;
  regs_freq[0] = 0;
  regs_kwh[0] = 0; regs_kwh[1] = 0;
}
