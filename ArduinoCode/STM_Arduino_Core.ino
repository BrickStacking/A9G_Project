#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string.h>
//SoftwareSerial sim(PB8, PB9);
#define sim Serial2
#define led PC13
#define coi PA1
#define pwr_key PB11 //Power key for A9G
#define BT1 PC15 //TC
#define BT2 PA0 //Button 2
#define BT3 PC14 //Rung

char data[1000];
char data_right[1000];
int dot, valid_number;
unsigned long int timeout = 0, beep = 0, beep2 = 0; //Beep 1 de tao ham chinh xac cho cb rung, beep 2 de doi thoi gian cho gui tin nhan
unsigned long index_data = 0;
int enable_out = 0;
int dem_rung = 0;
int process_state = 0;
void clear_buffer();
void GPS_process();
void read_sim_data(void);
void send_AT_Read(String a);
void SendMessage(String sms, String number);
HardwareSerial Serial2(USART2);   // or HardWareSerial Serial2 (PA3, PA2);
HardwareSerial Serial3(USART3);
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(115200);
  //#define led PC13
  //#define coi PA1
  //#define pwr_key PB11 //Power key for A9G
  //#define BT1 PC15
  //#define BT2 PA0
  //#define BT3 PC14
  pinMode(led, OUTPUT);
  pinMode(pwr_key, OUTPUT); //LED
  pinMode(coi, OUTPUT); //Buzzer
  pinMode(PB6, OUTPUT); //PWR mode AG9
  pinMode(BT1, INPUT_PULLUP);
  pinMode(BT2, INPUT_PULLUP);
  pinMode(BT3, INPUT_PULLUP);
  Serial.println("Let's do it!");
  // sim.println("Let's do it2!");
  timeout = millis();
  digitalWrite(pwr_key, 0);
  delay(4000);
  digitalWrite(pwr_key, 1);

  digitalWrite(coi, 1);
  delay(300);
  digitalWrite(coi, 0);
  delay(300);
  clear_buffer();
  digitalWrite(led, 1);
  beep = millis();

}

void loop()
{

  if (0 == digitalRead(BT2)) {
    enable_out = 0;
    digitalWrite(coi, 0);
  }

  if (millis() - beep2 > 4000) {
    if (0 == digitalRead(BT2)) {
      enable_out = 0;
      digitalWrite(coi, 0);
    }
    if (11 == process_state && 1 == enable_out) {
      //      SendMessage("DONE THAT SHIT", "0389150066");
      SendMessage(data_right, "0398014545");
      //      SendMessage(data_right, "0389150066");
      //    Serial.println("Send mess ok");
      for (int i = 0; i < 20; i++) {
        digitalWrite(coi, 1);
        delay(50);
        digitalWrite(coi, 0);
        delay(50);
      }
      delay(1000);
      enable_out = 0;

    }
  }


  if (millis() - beep > 500) {
    if (dem_rung > 10) {
      if (0 == digitalRead(BT1)) {
        digitalWrite(led, 0);
        delay(200);
        digitalWrite(led, 1);
        delay(200);
        digitalWrite(coi, 1);
        enable_out = 1; //Cho phep gui tin nhan den nguoi bi tai nan
        beep2 = millis();
      }
    }
    beep = millis();
    dem_rung = 0;
  }

  if ( 0 == digitalRead(BT3)) {
    dem_rung += 1;
    //    digitalWrite(led, 0);
    //    delay(500);
    //    digitalWrite(led, 1);
  }

  read_sim_data();
  if (millis() - timeout > 3000)
  {
    timeout = millis();
    Serial.print("Data:");
    Serial.println(data);
    clear_buffer();
  }
  GPS_process();


}

void read_sim_data()
{
  while (sim.available())
  {
    int data_in = sim.read();
    data[index_data++] = data_in;
  }
}

void clear_buffer()
{
  memset(data, '\0', 1000);
  index_data = 0;
}

void send_AT_Read(String a)
{
  clear_buffer();
  sim.println(a);
}

void SendMessage(String sms, String number)
{
  sim.println("AT+CMGF=1");
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r");
  delay(1000);
  sim.println(sms);
  delay(100);
  sim.println((char)26);
  delay(5000);
}

void GPS_process() {
  if (0 == process_state)
  {

    if (strstr(data, "READY") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Get Ready :v");
      delay(1000);
      process_state = 1;
    }
  }

  if (1 == process_state)
  {
    send_AT_Read("AT");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT OK");
      process_state = 2;
    }
  }

  if (2 == process_state)
  {
    send_AT_Read("AT+CGATT = 1");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+CGATT = 1 OK");
      process_state = 3;
    }
  }

  if (3 == process_state)
  {
    send_AT_Read("AT+CGDCONT= 1,IP, cmnet");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+CGDCONT= 1,IP, cmnet OK");
      process_state = 4;
    }
  }

  if (4 == process_state)
  {
    send_AT_Read("AT+CGACT=1,1");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+CGACT=1,1 OK");
      process_state = 5;
    }
  }

  if (5 == process_state)
  {
    send_AT_Read("AT+CMGF = 1");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+CMGF = 1 OK");
      process_state = 6;
    }
  }

  if (6 == process_state)
  {
    send_AT_Read("AT+CREG=1");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+CREG=1 OK");
      process_state = 7;
    }
  }

  if (7 == process_state)
  {
    send_AT_Read("AT+GPSRD=2");
    delay(1000);
    read_sim_data();
    if (strstr(data, "OK") != NULL) //Wait for one
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+GPSRD=2 ngon :D ");
      process_state = 8;
    }
  }

  if (8 == process_state)
  {
    send_AT_Read("AT+GPS=1");
    delay(5000);
    read_sim_data();
    if (strstr(data, "GPS") != NULL) //Wait for one NOTICE
    { //Da nhan duoc chuoi Ready
      Serial.print("Check AT+AGPS=1 OK :D ");
      process_state = 9;
    }
  }

  if (9 == process_state)
  {
    delay(400);
    clear_buffer();
    send_AT_Read("AT+LOCATION=2");
    delay(100);
    read_sim_data();
    Serial.print("Data Location:");
    Serial.println(data);
    if (strstr(data, "G") == NULL) //Neu khong co ki tu G trong GPS
    {
      if (strstr(data, "OK") != NULL) //Wait for one
      { //Da nhan duoc chuoi Ready
        // Serial.print(" OFF GPSRD, Receive long lat ");
        Serial.print("Rightpacket");
        //Dot la vi tri dau cham phay, valid number la so khac 0 neu tap tin gps location la dung
        for (int i = 0; i < 100; i++)
        {
          if ((char)44 == data[i])
          {
            dot = i + 1;
            Serial.println("Detect dot");
            Serial.print("Value:");
            Serial.println(data[i + 1]);

            valid_number = data[dot];
            if ((int)valid_number > 48 && (int)valid_number < 58)
            {
              Serial.println("Yey, we got GPS");
              strcpy(data_right, data);
              send_AT_Read("AT+GPSRD=0");
              process_state = 10;
              for (int i = 0; i < 10; i++) {
                digitalWrite(PC13, 1);
                delay(30);
                digitalWrite(PC13, 0);
                delay(30);
              }
              beep = millis();
            }
          }
        }
      }
    }
  }

  if (10 == process_state)
  {
    delay(500);
    // Serial.print("Data:");
    // Serial.println(data);
    Serial.print("Right Data:");
    Serial.println(data_right);
    //    SendMessage(data_right, "0389150066");
    //    Serial.println("Send mess ok");
    process_state = 11;
    for (int i = 0; i < 10; i++) {
      digitalWrite(coi, 1);
      delay(200);
      digitalWrite(coi, 0);
      delay(200);
    }
  }
  if (11 == process_state)
  {
    //    Serial.println("Done tut");
    //    //digitalWrite(led, 0);
  }
}
