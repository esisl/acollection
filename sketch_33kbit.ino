#include <EEPROM.h>
#include <can.h>
#include <mcp2515.h>

//получить данные из pin 6
//отдать в CAN pin 10

MCP2515 mcp2515(9);
MCP2515 mcp2515c(10);

struct can_frame canMsg;
struct can_frame canMsgACC;
struct can_frame canMsggb;
struct can_frame canMsgfuel;
struct can_frame canMsgsuv;
struct can_frame canMsgtaho;
struct can_frame canMsgtaho1;
struct can_frame canMsgspeed;
struct can_frame canMsgairbag;
struct can_frame canMsgpov;
struct can_frame canMsgdal;
struct can_frame canMsgkorobka;
struct can_frame canMsgchek;
struct can_frame canMsgruch_tmz;
struct can_frame canMsgowibka;
struct can_frame canMsgowib;
struct can_frame canMsgow;
struct can_frame canMsgewu;

//пройденный путь
uint32_t distanceTraveled = 0;//непосредственно пройденный путь
uint32_t distanceTraveledOld = 0;//непосредственно пройденный путь старое значение, это для того, чтобы пересохранять новое значение, только после пройденного километра.
                                  //Нужно экономить ресурс ячеек!
uint16_t eepromKey = 0;
uint32_t distanceTraveledTimer = 0;
uint16_t oldSpeed = 0;

void setup() {
  //проверка инициализации EEPROM
  EEPROM.get(0, eepromKey);
  //если ключ прописан, значит мыдавно едем и надо прочитать distanceTraveled из памяти
  //если ключа нет, значит обнулить
  if(eepromKey == 0xf0f0){
    EEPROM.get(2, distanceTraveled);
  }else{
    distanceTraveled = 0;
    EEPROM.put(0, distanceTraveled);
  }
  distanceTraveledOld = distanceTraveled;
  
  //на прием
  mcp2515c.reset();
  mcp2515c.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515c.setNormalMode();
  //на передачу
  mcp2515.reset();
  mcp2515.setBitrate(CAN_33KBPS,MCP_8MHZ);//CAN_33K3BPS
  mcp2515.setNormalMode();
//Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);



      canMsgow.can_id = 0x1022A040 | CAN_EFF_FLAG;
      canMsgow.can_dlc=8;
      canMsgow.data[0] = 0x00;
      canMsgow.data[1] = 0xF0;
      canMsgow.data[2] = 0x00;
      canMsgow.data[3] = 0x12;
      canMsgow.data[4] = 0x02;
      canMsgow.data[5] = 0x00;
      canMsgow.data[6] = 0x00;
      canMsgow.data[7] = 0x22;




      canMsgairbag.can_id = 0x10330058 | CAN_EFF_FLAG;
      canMsgairbag.can_dlc=6;
      canMsgairbag.data[0] = 0x73;
      canMsgairbag.data[1] = 0x73;
      canMsgairbag.data[2] = 0x73;
      canMsgairbag.data[3] = 0x00;
      canMsgairbag.data[4] = 0x00;
      canMsgairbag.data[5] = 0x00;


      canMsgchek.can_id = 0x10780040 | CAN_EFF_FLAG;
      canMsgchek.can_dlc=8;
      canMsgchek.data[0] = 0x00;
      canMsgchek.data[1] = 0x00;
      canMsgchek.data[2] = 0x01;
    //  canMsgchek.data[3] = 0x00;
      canMsgchek.data[4] = 0x08;
      canMsgchek.data[5] = 0x00;
      canMsgchek.data[6] = 0x00;
      canMsgchek.data[7] = 0x22;


      canMsgowibka.can_id = 0x10240040 | CAN_EFF_FLAG;
      canMsgowibka.can_dlc=8;
      canMsgowibka.data[0] = 0x80;
      canMsgowibka.data[1] = 0x12;
      canMsgowibka.data[2] = 0x88;
      canMsgowibka.data[3] = 0x01;
      canMsgowibka.data[4] = 0x00;
      canMsgowibka.data[5] = 0x00;
      canMsgowibka.data[6] = 0x00;
      canMsgowibka.data[7] = 0x00;


      canMsgowib.can_id = 0x103D4040 | CAN_EFF_FLAG;
      canMsgowib.can_dlc=6;
      canMsgowib.data[0] = 0x24;
      canMsgowib.data[1] = 0x24;
      canMsgowib.data[2] = 0x34;
      canMsgowib.data[3] = 0x31;
      canMsgowib.data[4] = 0x30;
      canMsgowib.data[5] = 0x31;

      canMsgACC.can_id = 0x102CC040 | CAN_EFF_FLAG;
      canMsgACC.can_dlc=8;
      canMsgACC.data[0] = 0x01;
      canMsgACC.data[1] = 0x00;
      canMsgACC.data[2] = 0xCF;
      canMsgACC.data[3] = 0x00;
      canMsgACC.data[4] = 0x00;
      canMsgACC.data[5] = 0x86;
      canMsgACC.data[6] = 0x06;
      canMsgACC.data[7] = 0x3C;


      canMsgfuel.can_id = 0x0C800040 | CAN_EFF_FLAG;
      canMsgfuel.can_dlc=6;
  canMsgfuel.data[0] = 0x08;
  canMsgfuel.data[1] = 0xEC;
  canMsgfuel.data[2] = 0x00;
  canMsgfuel.data[3] = 0x00;
  canMsgfuel.data[4] = 0x03;
  canMsgfuel.data[5] = 0x1F;


/*
      canMsgsuv.can_dlc=8;
      canMsgsuv.data[0] = 0x10;
      canMsgsuv.data[1] = 0x00;
     // canMsgsuv.data[2] = 0xCF;
      canMsgsuv.data[3] = 0x37;
      canMsgsuv.data[4] = 0x5E;
      canMsgsuv.data[5] = 0x00;
      canMsgsuv.data[6] = 0x3A;
     // canMsgsuv.data[7] = 0x3C;
  */
}

uint32_t tmr = millis();//micros();
uint32_t tmr_oldOverlap = tmr;
uint32_t tmr_old100 = tmr;
uint32_t tmr_old150 = tmr;
uint32_t tmr_old250 = tmr;
uint32_t tmr_old550 = tmr;
uint32_t tmr_old1300 = tmr;
bool flag_pin2 = false;
bool flag_pin3 = false;
bool flag_pin4 = false;
bool flag_pin5 = false;
bool flag_pin6 = false;
bool flag_pin7 = false;
bool flag_pin8 = false;

void loop() {
  while(1){

    if(mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK)
    {
    }
    
    //прием на 500
 
    if(mcp2515c.readMessage(&canMsg) == MCP2515::ERROR_OK){
      //отправка на 33.3
      if(canMsg.can_id==0x0C9){
         canMsgtaho = canMsg;
       // canMsg.can_id = 0x102CA040 | CAN_EFF_FLAG;
         canMsgtaho.can_id = 0x102CA040 | CAN_EFF_FLAG;

  canMsgtaho.data[0] = 0x00;

 canMsgtaho.data[6] = canMsgkorobka.data[3];


   ///////////--------  TAHO ----------//////////

         
        canMsgtaho.data[3] = canMsg.data[2];
        canMsgtaho.data[2] = canMsg.data[1];
        canMsgtaho.data[1] = 0x40;
        // Serial.println("aaa");
      }
      
   //   mcp2515.sendMessage(&canMsg); 
  //     mcp2515.sendMessage(&canMsgtaho);  


////////////------- SPEED -----------//////////

      if(canMsg.can_id==0x3E9){
         canMsgspeed = canMsg;
        canMsgspeed.can_id = 0x10210040 | CAN_EFF_FLAG;



//0bb8 60 км/ч
//1770 100 км/ч


        //смотрим время с предыдущего получения информации
        //умножаем на среднюю скорость между прошлым разом и этим
        //приплюсовываем к пройденному пути
        //если пройденный путь изменился более чем на 1 км, то записываем в eeprom
        uint16_t Speed = canMsgspeed.data[0] << 8 | canMsgspeed.data[1]; //скорость в км/ч * 60
        distanceTraveled += (tmr - distanceTraveledTimer) * (oldSpeed + Speed) / 2 / 100000;//получаем пройденный путь в 1 / 2160 км
        oldSpeed = Speed;
        if(distanceTraveled - distanceTraveledOld > 0x870){//здесь надо проверить "пройден километр", т.е. 2160 == 0x870
          distanceTraveledOld = distanceTraveled;
          EEPROM.put(2, distanceTraveled);

          Serial.print("distanceTraveled = ");
          Serial.println(distanceTraveled);

          uint32_t distanceTraveledKilometers = distanceTraveled * 64 / 2160;
          Serial.print("distanceTraveledKilometers = ");
          Serial.println(distanceTraveledKilometers);
          canMsg.can_id = 0x106C0040 | CAN_EFF_FLAG;
          canMsg.can_dlc=8;
          canMsg.data[0] = (byte)(distanceTraveledKilometers & 0xFF);
          canMsg.data[1] = (byte)((distanceTraveledKilometers >> 8) & 0xFF);
          canMsg.data[2] = (byte)((distanceTraveledKilometers >> 16) & 0xFF);
          canMsg.data[3] = (byte)((distanceTraveledKilometers >> 24) & 0xFF);
          canMsg.data[4] = 0x00;
          canMsg.data[5] = 0x00;
          canMsg.data[6] = 0x00;
          canMsg.data[7] = 0x00;
          mcp2515.sendMessage(&canMsg);

//           ID 106C0040
          
        }
        distanceTraveledTimer = tmr;
       
       canMsgspeed.data[3] = canMsgspeed.data[0];
       canMsgspeed.data[2] = canMsgspeed.data[1];
        // Serial.println("bb");
      }
      
   
   //   mcp2515.sendMessage(&canMsgspeed);

      ///////////------------ SUV -----------///

            if(canMsg.can_id==0x4C1){
         canMsgsuv = canMsg;
        canMsgsuv.can_id = 0x102E0040 | CAN_EFF_FLAG;
        
             canMsgsuv.can_dlc=8;
      canMsgsuv.data[0] = 0x10;
      canMsgsuv.data[1] = 0x50;
     // canMsgsuv.data[2] = 0xCF;
      canMsgsuv.data[3] = 0x37;
      canMsgsuv.data[4] = 0x5E;
      canMsgsuv.data[5] = 0x00;
      canMsgsuv.data[6] = 0x36;
     // canMsgsuv.data[7] = 0x3C;
       canMsgsuv.data[7] = canMsgsuv.data[2];
        canMsgsuv.data[2] = 0xC0;
    
      }
      
   // mcp2515.sendMessage(&canMsgsuv);

 /////////////-------------------------------////////
           if(canMsg.can_id==0x1F5){
         canMsgkorobka = canMsg;
       // canMsgkorobka.can_id = 0x102CA040 | CAN_EFF_FLAG;
       
      // canMsgtaho.data[6] = canMsgkorobka.data[3];
     
      }   
      // mcp2515.sendMessage(&canMsgkorobka);
      
    }

    //проверка pin8

    if(digitalRead(8)==0){
      flag_pin8 = true;
    }else{
      flag_pin8 = false;
    }
        if(digitalRead(7)==0){
      flag_pin7 = true;
    }else{
      flag_pin7 = false;
    }
    if(digitalRead(6)==0){
      flag_pin6 = true;
    }else{
      flag_pin6 = false;
    }

        if(digitalRead(5)==0){
      flag_pin5 = true;
    }else{
      flag_pin5 = false;
    }

        if(digitalRead(4)==0){
      flag_pin4 = true;
    }else{
      flag_pin4 = false;
    }


            if(digitalRead(3)==0){
      flag_pin3 = true;
    }else{
      flag_pin3 = false;
    }

            if(digitalRead(2)==0){
      flag_pin2 = true;
    }else{
      flag_pin2 = false;
    }
    
          //отправка раз в 1300
 
    if(tmr - 350 > tmr_old550){
      tmr_old550= tmr;


mcp2515.sendMessage(&canMsgowib);
delay(2);
      /////----- airbag ----------////////

 /*     
      canMsgairbag.can_id = 0x10330058 | CAN_EFF_FLAG;
      canMsgairbag.can_dlc=6;
      canMsgairbag.data[0] = 0x73;
      canMsgairbag.data[1] = 0x73;
      canMsgairbag.data[2] = 0x73;
      canMsgairbag.data[3] = 0x00;
      canMsgairbag.data[4] = 0x00;
      canMsgairbag.data[5] = 0x00;
*/
     mcp2515.sendMessage(&canMsgairbag);

     ///////-------- chek -------------///////////
/*
      canMsgchek.can_id = 0x10780040 | CAN_EFF_FLAG;
      canMsgchek.can_dlc=8;
      canMsgchek.data[0] = 0x00;
      canMsgchek.data[1] = 0x00;
      canMsgchek.data[2] = 0x01;
    //  canMsgchek.data[3] = 0x00;
      canMsgchek.data[4] = 0x08;
      canMsgchek.data[5] = 0x00;
      canMsgchek.data[6] = 0x00;
      canMsgchek.data[7] = 0x22;
      */
      delay(2);
      mcp2515.sendMessage(&canMsgchek); 

     /////////------ owibka -----------////////
/*
      canMsgowibka.can_id = 0x10240040 | CAN_EFF_FLAG;
      canMsgowibka.can_dlc=8;
      canMsgowibka.data[0] = 0x80;
      canMsgowibka.data[1] = 0x12;
      canMsgowibka.data[2] = 0x88;
      canMsgowibka.data[3] = 0x01;
      canMsgowibka.data[4] = 0x00;
      canMsgowibka.data[5] = 0x00;
      canMsgowibka.data[6] = 0x00;
      canMsgowibka.data[7] = 0x00;
      */
   //  mcp2515.sendMessage(&canMsgowibka);

      //////////----kl_owibka----////

/*
      canMsgow.can_id = 0x1022A040 | CAN_EFF_FLAG;
      canMsgow.can_dlc=8;
      canMsgow.data[0] = 0x00;
      canMsgow.data[1] = 0xF0;
      canMsgow.data[2] = 0x00;
      canMsgow.data[3] = 0x12;
      canMsgow.data[4] = 0x02;
      canMsgow.data[5] = 0x00;
      canMsgow.data[6] = 0x00;
      canMsgow.data[7] = 0x22;
      */
   //   mcp2515.sendMessage(&canMsgow);

//////////----------/////////////////
/*
            canMsgowibka2.can_id = 0x103D4040 | CAN_EFF_FLAG;
      canMsgowibka2.can_dlc=6;
      canMsgowibka2.data[0] = 0x24;
      canMsgowibka2.data[1] = 0x24;
      canMsgowibka2.data[2] = 0x34;
      canMsgowibka2.data[3] = 0x31;
      canMsgowibka2.data[4] = 0x30;
      canMsgowibka2.data[5] = 0x31;
    */
    //  mcp2515.sendMessage(&canMsgowibka2);

      
     
    }

      //отправка раз в 250
    if(tmr - 250 > tmr_old250){
      tmr_old250 = tmr;
/*
      canMsgACC.can_id = 0x102CC040 | CAN_EFF_FLAG;
      canMsgACC.can_dlc=8;
      canMsgACC.data[0] = 0x01;
      canMsgACC.data[1] = 0x00;
      canMsgACC.data[2] = 0xCF;
      canMsgACC.data[3] = 0x00;
      canMsgACC.data[4] = 0x00;
      canMsgACC.data[5] = 0x86;
      canMsgACC.data[6] = 0x06;
      canMsgACC.data[7] = 0x3C;
      */
      mcp2515.sendMessage(&canMsgACC); 
      delay(2); 
       mcp2515.sendMessage(&canMsgow);
       delay(2); 
       mcp2515.sendMessage(&canMsggb);
       delay(2); 
       mcp2515.sendMessage(&canMsgowibka);
       delay(2); 
/*
      canMsgsuv.can_id = 0x102E0040 | CAN_EFF_FLAG;
      canMsgsuv.can_dlc=8;
      canMsgsuv.data[0] = 0x10;
      canMsgsuv.data[1] = 0x55;
      canMsgsuv.data[2] = 0xC0;
      canMsgsuv.data[3] = 0x3B;
      canMsgsuv.data[4] = 0x5E;
      canMsgsuv.data[5] = 0x00;
      canMsgsuv.data[6] = 0x36;
      canMsgsuv.data[7] = 0x6D;
      */

   //   mcp2515.sendMessage(&canMsgewu);

   //  mcp2515.sendMessage(&canMsggb); 
mcp2515.sendMessage(&canMsgsuv);
delay(2);
//mcp2515.sendMessage(&canMsgowib);
   //   mcp2515.sendMessage(&canMsgsuv);
   //    Serial.println("mur");
    }
    //отправка раз в 100
    if(tmr - 100 > tmr_old100){
      tmr_old100 = tmr;


    canMsgpov.can_id = 0x1020C040 | CAN_EFF_FLAG;
    canMsgpov.can_dlc=8;
    canMsgpov.data[0] = 0x00;
        // gabarit
       
         if(flag_pin5&&flag_pin4 ){
        canMsgpov.data[1] = 0x00;
    
      }else{
        if(!flag_pin5)
        canMsgpov.data[1] = 0x40;
      if(!flag_pin4)
        canMsgpov.data[1] = 0x20;
      }
          // dalniy
          if(!flag_pin5&& !flag_pin4 ){
        canMsgpov.data[1] = 0x60;
    
      }
      
 // canMsgpov.data[1] = 0x00;
  canMsgpov.data[2] = 0x00;
        if(flag_pin6 &&flag_pin7){
        canMsgpov.data[3] = 0x04;
    
      }else{
        if(!flag_pin6)
        canMsgpov.data[3] = 0x24;
     if(!flag_pin7)canMsgpov.data[3] = 0x06;
     if(!flag_pin6 &&!flag_pin7)canMsgpov.data[3] = 0x66;
      }
 
//  canMsgpov.data[3] = 0x7C;
  canMsgpov.data[4] = 0x01;
 mcp2515.sendMessage(&canMsgpov);
 delay(1); 
   mcp2515.sendMessage(&canMsgtaho);  
   delay(1); 
    mcp2515.sendMessage(&canMsgspeed);

    }
    
    //отправка раз в 150
    if(tmr - 150 > tmr_old150){
      tmr_old150 = tmr;

          mcp2515.sendMessage(&canMsgewu);
//mcp2515.sendMessage(&canMsgowib);

      canMsgewu.can_id = 0x0C630040 | CAN_EFF_FLAG;
   canMsgewu.can_dlc=1;
       if(flag_pin2){
     
        canMsgewu.data[0] = 0x03;
     
      }else{
        canMsgewu.data[0] = 0x00;
     
      }

       if(flag_pin3&& !flag_pin8){
       // canMsgruch_tmz.data[3] = 0x04;
        canMsgchek.data[3] = 0x04;
     
      }else{
        canMsgchek.data[3] = 0x00;
       // canMsgruch_tmz.data[3] = 0x00;
      //  canMsggb.data[2] = 0xFF;
      }
 // canMsgruch_tmz.data[4] = 0x08;
 // canMsgruch_tmz.data[5] = 0x00;
 // canMsgruch_tmz.data[6] = 0x00;
 // canMsgruch_tmz.data[7] = 0x22;
      // ACC can_id = 0x10242040
   //    mcp2515.sendMessage(&canMsgruch_tmz); 
      canMsg.can_id = 0x10242040 | CAN_EFF_FLAG;

//    canMsggb.can_dlc=3;
      canMsg.can_dlc=1;
      if(flag_pin8){
        canMsg.data[0] = 0x08;
     // canMsggb.data[2] = 0x00;
      }else{
        canMsg.data[0] = 0x02;
      //  canMsggb.data[2] = 0xFF;
      }

        canMsggb.can_id = 0x10644040 | CAN_EFF_FLAG;
      canMsggb.can_dlc=3;
  canMsggb.data[0] = 0x02;
  canMsggb.data[1] = 0xB3;
       if(flag_pin5){
        canMsggb.data[2] = 0xE6;
    
      }else{
        canMsggb.data[2] = 0x7D;
     
      }
    
   // mcp2515.sendMessage(&canMsgACC);   
   delay(2); 
     mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
// delay(60);
delay(2); 
// Serial.println("rr");
// mcp2515.sendMessage(&canMsggb);  
 mcp2515.sendMessage(&canMsgfuel); 
    }
  
  
    tmr = millis();// micros();
    if(tmr < tmr_oldOverlap){
      //произошло переполнение таймера
      //сбросим все остальные таймеры
      tmr_old100 = 0;
      tmr_old150 = 0;
      tmr_old250 = 0;
       tmr_old550 = 0;
     tmr_old1300 = 0;
    }
    tmr_oldOverlap = tmr;
  }
}
