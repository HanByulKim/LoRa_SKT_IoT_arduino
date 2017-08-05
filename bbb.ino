#include <SimpleTimerL.h>
#include <LoRaShield.h>

LoRaShield LoRa(10, 11);
SimpleTimer timer;

SoftwareSerial mySerial(13, 12);                        // RX 13, TX 12
unsigned char Send_data[5] = {0x11,0x02,0x0b,0x01, 0xE1}; // 농도읽는명령
unsigned char Receive_Buff[20];                           // data buffer
unsigned long PM25, PM10;                                          // 농도저장변수 : 각 32bit(8bit*4 = 32)
//unsigned long COM_SUCCESS, COM_COUNT;                              // 통신성공/통신시도횟수
unsigned char recv_cnt = 0;

void Send_CMD(void)                                        // COMMAND
{
  unsigned char i;
  for(i=0; i<5; i++)
  {
    mySerial.write(Send_data[i]);
    delay(1);      // Don't delete this line !!
  }
}
unsigned char Checksum_cal(void)                          // CHECKSUM 
{
  unsigned char count, SUM=0;
  for(count=0; count<19; count++)
  {
     SUM += Receive_Buff[count];
  }
  return 256-SUM;
}

void repeatMe(){
  //COM_COUNT++;  
  Send_CMD();  // Send Read Command
  while(1)
  {
    if(mySerial.available())
    { 
       Receive_Buff[recv_cnt++] = mySerial.read();
      if(recv_cnt ==20){recv_cnt = 0; break;}
    }
  } 
  if(Checksum_cal() == Receive_Buff[19])  // CS 확인을 통해 통신 에러 없으면
  {
        //COM_SUCCESS++;
        PM25 = (unsigned long)Receive_Buff[3]<<24 | (unsigned long)Receive_Buff[4]<<16 | (unsigned long)Receive_Buff[5]<<8| (unsigned long)Receive_Buff[6];  // 농도계산(시프트)
        PM10 = (unsigned long)Receive_Buff[7]<<24 | (unsigned long)Receive_Buff[8]<<16 | (unsigned long)Receive_Buff[9]<<8| (unsigned long)Receive_Buff[10];  // 농도계산(시프트)
        //Serial.write("COM count : ");
        //Serial.print(COM_SUCCESS);
        //Serial.write(" / ");
        //Serial.print(COM_COUNT);
        Serial.write("    PM2.5 : ");
        Serial.print(PM25);
        Serial.write("    PM10 : ");
        Serial.println(PM10);
   }
   else
   {
     Serial.write("CHECKSUM Error");
   }
   delay(1000);       //1000ms

  if (LoRa.available()) {
    Serial.write(LoRa.read());
  }
  Serial.println("timer running");
  PM25 = 34;
  PM10 = 39;
  LoRa.PrintTTV("11",PM25);
  LoRa.PrintTTV("11",PM10);
  LoRa.SendTTV();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
  pinMode(13,INPUT);
  pinMode(12,OUTPUT);
  while (!Serial) {}
  LoRa.begin(38400);
  while (!LoRa) {}
  Serial.println("lora av");
  mySerial.begin(9600);
  while (!mySerial);
  delay(1000);
  timer.setInterval(300000,repeatMe);
  Serial.println("start");
}

void loop() {
    timer.run();
}

