#include <can.h>
#include <mcp2515.h>

//получить данные из pin
//отдать в CAN

MCP2515 mcp2515(10);
MCP2515 mcp2515b(9);
MCP2515 mcp2515c(6);

struct can_frame canMsg;
struct can_frame canMsg308;
struct can_frame canMsg208;
struct can_frame canMsg00A;
struct can_frame canMsg222;
struct can_frame canMsg005;
struct can_frame canMsg608;
struct can_frame canMsg418;

void setup() {
  //на прием
  mcp2515c.reset();
  mcp2515c.setBitrate(CAN_80KBPS,MCP_8MHZ);
//  mcp2515c.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515c.setNormalMode();
  mcp2515b.reset();
  mcp2515b.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515b.setNormalMode();
  //на передачу
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS,MCP_8MHZ);
  mcp2515.setNormalMode();

  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
  pinMode(7, INPUT);
  pinMode(A0, INPUT);
  
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(A0, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(7, LOW);

  canMsg308.can_id = 0x0B1;
  canMsg308.can_dlc = 8;
  canMsg308.data[0] = 0;
  canMsg308.data[1] = 0;
  canMsg308.data[2] = 0;
  canMsg308.data[3] = 0;
  canMsg308.data[4] = 0;
  canMsg308.data[5] = 0;
  canMsg308.data[6] = 0;
  canMsg308.data[7] = 0;
  canMsg208 = canMsg308;
  canMsg208.can_id = 0x0A1;
  canMsg00A = canMsg308;
  canMsg00A.can_id = 0x339;
  canMsg222 = canMsg308;
  canMsg222.can_id = 0x21D;
  canMsg005 = canMsg308;
  canMsg005.can_id = 0x11F;
  canMsg608 = canMsg308;
  canMsg608.can_id = 0x2B9;
  canMsg418 = canMsg308;
  canMsg418.can_id = 0x143;
  Serial.begin(9600);
}

uint32_t tmr = micros();
uint32_t tmr_oldOverlap = tmr;
uint32_t tmr_old = tmr;
uint32_t tmr_old_r = tmr;
uint32_t tmr_old4 = tmr;
uint32_t tmr_oldA = tmr;
uint32_t tmr_old100 = tmr;
uint32_t tmr_old1000 = tmr;
uint32_t tmr_old20 = tmr;
uint32_t tmr_old30 = tmr;

int flagKey = 0;
int flagKeyZapret = 0;
int key[] = {0,0,0,0,0,0,0,0};
int z=1024;
int flagIsPressed = 0;

uint32_t vh;
uint32_t vyh;
byte low;
byte high;

int flagCurrentCommand = 0;

bool flag33d=false;
bool flag33d_1=false;
bool flag33d_2=false;
int cnt33d=0;
int cnt33d_1=0;
int cnt33d_2=0;

void loop() {
  //прием на 83.333
  if(mcp2515c.readMessage(&canMsg) == MCP2515::ERROR_OK){
    switch(canMsg.can_id){
      case 0x00A:
        canMsg00A = canMsg;
        canMsg00A.can_id = 0x339;
        canMsg00A.data[2] = canMsg.data[0];
        canMsg00A.data[0] = 0x00;
        mcp2515.sendMessage(&canMsg00A);
      break;
      case 0x222:
        canMsg222 = canMsg;
        canMsg222.can_id = 0x21D;
        canMsg222.data[3] = canMsg.data[2] >> 1;
        canMsg222.data[2] = 0;
        mcp2515.sendMessage(&canMsg222);
      break;
      case 0x005:
        canMsg005 = canMsg;
        canMsg005.can_id = 0x11F;
        mcp2515.sendMessage(&canMsg005);
      break;
      case 0x028:
        if(canMsg.data[0] == 0x40){
          flag33d = true;
          cnt33d= 100;
        }
      break;
    }    
  }

  //прием 500
  if (mcp2515b.readMessage(&canMsg) == MCP2515::ERROR_OK) // To receive data (Poll Read)
  {    
    switch(canMsg.can_id){
      case 0x608:
        canMsg608 = canMsg;
        canMsg608.can_id = 0x2B9;
        canMsg608.data[4] = canMsg.data[0];
        canMsg608.data[0] = 0x00;
        mcp2515.sendMessage(&canMsg608);
      break;
      case 0x418:
        canMsg418 = canMsg;
        canMsg418.can_id = 0x143;
        canMsg418.data[2] = canMsg.data[0];
        canMsg418.data[0] = 0x00;
        mcp2515.sendMessage(&canMsg418);

        if(canMsg.data[0]==0x52){
          digitalWrite(5, HIGH);
        }else{
          digitalWrite(5, LOW);
        }
      break;
      case 0x308:
        canMsg308.can_id = 0x0B1;
        canMsg308.data[4] = canMsg.data[1];
        canMsg308.data[1] = 0x00;
        canMsg308.data[3] = canMsg.data[2];
        canMsg308.data[2] = 0x00;
        mcp2515.sendMessage(&canMsg308);
      break;
      case 0x208:
        canMsg208.can_id = 0x0A1;
  
        vh = canMsg.data[6] * 0x100 + canMsg.data[7];
        vyh = vh + vh;
        vyh = vyh + vyh;
        vyh = vyh + vyh;
        vyh = vyh + vh + vh + vh + vh + vh + vh + vh;
        vyh = vyh >> 4;
        low = vyh;
        high = vyh >> 8;
  
        canMsg208.data[5] = low;
        canMsg208.data[6] = high;
        canMsg208.data[7] = 0x00;
        mcp2515.sendMessage(&canMsg208);
      break;
      /*
      case 0x7bb:
        if(canMsg.data[0]==0x01){
          flag33d = true;
          cnt33d  = 100;
        }else if(canMsg.data[0]==0x00){
          flag33d = false;
          cnt33d  = 0; 
        }
      break;
      */

      ///////////////////////////////////
      /*
      case 0x70b:
        if(canMsg.data[2]==0x01){
          flag33d_1 = true;
          cnt33d_1 = 100;
        }else{
          flag33d_1 = false;
          cnt33d_1 = 0;
        }
      */
        //////////////////////////////
        /*
        if(canMsg.data[1]==0x01){
          flag33d_2 = true;
          cnt33d_2 = 100;
        }else{
          flag33d_2 = false;
          cnt33d_2 = 0;
        }
        */
      break;
    }   
  }


//отправка 1 раз в 1000мс
  if(tmr - 1000 > tmr_old1000){
    tmr_old1000 = tmr;

    canMsg.can_id = 0x33D;
    canMsg.can_dlc=8;
    canMsg.data[0] = 0x00;
    if(flag33d_1){
      canMsg.data[1] = 0x33;
      cnt33d_1--;
      if(cnt33d_1==0){
        flag33d_1 = false;
      }
    }else if(flag33d_2){
      cnt33d_2--;
      if(cnt33d_2==0){
        flag33d_2 = false;
      }
    }else{
      canMsg.data[1] = 0x00;
    }    
    canMsg.data[2] = 0x00;
    canMsg.data[3] = 0x00;
    if(flag33d){
      canMsg.data[4] = 0x86;
      cnt33d--;
      if(cnt33d==0){
        flag33d = false;
      }
    }else{
      canMsg.data[4] = 0x00;
    }
    canMsg.data[5] = 0x00;
    canMsg.data[6] = 0x00;
    canMsg.data[7] = 0x00;
    mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение

    if(flag33d_1){
      canMsg.can_id = 0x30F;
      canMsg.can_dlc=8;
      canMsg.data[0] = 0x0A;
      canMsg.data[1] = 0x00;
      canMsg.data[2] = 0x00;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x00;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
    }
    if(flag33d_2){
      canMsg.can_id = 0x3dd;
      canMsg.can_dlc=8;
      canMsg.data[0] = 0x00;
      canMsg.data[1] = 0x00;
      canMsg.data[2] = 0x00;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x86;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
    }
  }
  

  //отправув 1 раз в 20 мс
  if(tmr - 20 > tmr_old20){
    tmr_old20 = tmr;
    canMsg.can_id = 0x147;
    canMsg.can_dlc=8;
    canMsg.data[0] = 0x00;
    canMsg.data[1] = 0x00;
    canMsg.data[2] = 0x00;
    canMsg.data[3] = 0x00;
    canMsg.data[4] = 0x00;
    canMsg.data[5] = 0x00;
    canMsg.data[6] = 0x00;
    canMsg.data[7] = 0x00;
    mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
  }
  //отправка раз в 30 мс
  if(tmr - 30 > tmr_old30){
    tmr_old30 = tmr;
    canMsg.can_id = 0x22b;
    canMsg.can_dlc=8;
    canMsg.data[0] = 0x0D;
    canMsg.data[1] = 0x09;
    canMsg.data[2] = 0x00;
    canMsg.data[3] = 0x00;
    canMsg.data[4] = 0x00;
    canMsg.data[5] = 0x00;
    canMsg.data[6] = 0x00;
    canMsg.data[7] = 0x00;
    mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
    canMsg.data[0] = 0x10;
    canMsg.data[1] = 0x00;
    mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
  }

  //проверка кнопки раз в 300 мс
  if(tmr - 200 > tmr_old){
    tmr_old = tmr;
    if(digitalRead(3) == 1){
          flag33d_1 = true;
          cnt33d_1 = 100;
        }else{
          flag33d_1 = false;
          cnt33d_1 = 0;
        
      /*
      canMsg.can_id = 0x401;
      canMsg.can_dlc=8;
      canMsg.data[0] = 0x00;
      canMsg.data[1] = 0x00;
      canMsg.data[2] = 0x04;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x00;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
   */
    }
  }

  //проверка кнопки раз в 100 мс
  if(tmr - 100 > tmr_old4){
    tmr_old4 = tmr;
    if(digitalRead(4) == 1
    ){
      canMsg.can_id = 0x020;
      canMsg.can_dlc=8;
      canMsg.data[0] = 0x00;
      if(digitalRead(7) == 1)
     { canMsg.data[1] = 0x40;}
     else
     { canMsg.data[1] = 0x20;}
      
      canMsg.data[2] = 0x00;
      canMsg.data[3] = 0x00;
      canMsg.data[4] = 0x00;
      canMsg.data[5] = 0x00;
      canMsg.data[6] = 0x00;
      canMsg.data[7] = 0x00;
      mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
    }
  }


  //И!!
  //проверять аналоговую кнопку тоже будем только раз в 30 мс
  //т.е. чтение аналогового пина может занимать время
  if(tmr - 30 > tmr_oldA){
    tmr_oldA = tmr;

    z = analogRead(A0);

//    Serial.print("z = ");
//    Serial.println(z);
  
    if(z>900){
      flagKey=0;
      key[0] = 0;
      key[1] = 0;
      key[2] = 0;
      key[3] = 0;
      key[4] = 0;
      key[4] = 0;
      key[5] = 0;
      key[7] = 0;
      flagKeyZapret = 0;
    }else{
      if(flagKeyZapret==0){
      
        if(flagKey==0){//
          if(z<100){
            key[7] = 1;
          }else if(z>150 && z<310){
            key[7] = 2;
          }else if(z>350 && z<450){
            key[7] = 3;
          }else if(z>470 && z<600){
            key[7] = 4;
          }else if(z>650 && z<750){
            key[7] = 5;
          }else if(z>800 && z<950){
            key[7] = 6;
          }
    
          //проверим, что все key одинаковы
          flagIsPressed=1;
          for(int i=0;i<7;i++){
            if(key[i]!=key[i+1]){
              flagIsPressed = 0;
            }
            //Serial.print(key[i]);
            key[i] = key[i+1];
          }
//          Serial.println();
    
          if(flagIsPressed==1){//
            flagKeyZapret = 1;
            flagKey = 1;
//          Serial.println(key[7]);
            switch(key[7]){//switch
              case 1:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x04;
                canMsg.data[3] = 0x00;
                canMsg.data[4] = 0x00;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//              Serial.println("a0 100");
              break;
              case 2:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x01;
                canMsg.data[3] = 0x00;
                canMsg.data[4] = 0x00;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//                Serial.println("a0 310");
              break;
              case 3:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x10;
                canMsg.data[3] = 0x00;
                canMsg.data[4] = 0x00;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//                Serial.println("a0 450");
              break;
              case 4:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x00;
                canMsg.data[3] = 0x00;
                canMsg.data[4] = 0x40;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//                Serial.println("a0 600");
              break;
              case 5:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x00;
                canMsg.data[3] = 0x00;
                canMsg.data[4] = 0x04;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//                Serial.println("a0 750");
              break;
              case 6:
                canMsg.can_id = 0x401;
                canMsg.can_dlc=8;
                canMsg.data[0] = 0x00;
                canMsg.data[1] = 0x00;
                canMsg.data[2] = 0x00;
                canMsg.data[3] = 0x50;
                canMsg.data[4] = 0x00;
                canMsg.data[5] = 0x00;
                canMsg.data[6] = 0x00;
                canMsg.data[7] = 0x00;
                mcp2515.sendMessage(&canMsg);     //передаем CAN сообщение
//                Serial.println("a0 950");
              break;
            }//switch
          }//if(flagIsPressed==1)
        }//if(flagKey==0)
      }//if(flagKeyZapret==0)
    }//else
  }//if(tmr - 30 > tmr_oldA)


  tmr = micros();
  if(tmr < tmr_oldOverlap){
    //произошло переполнение таймера
    //сбросим все остальные таймеры
    tmr_old = 0;
    tmr_old_r = 0;
    tmr_old4 = 0;
    tmr_old100 = 0;
    tmr_oldA = 0;
    tmr_old1000 = 0;
    tmr_old20 = 0;
    tmr_old30 = 0;
  }
  tmr_oldOverlap = tmr;
}
