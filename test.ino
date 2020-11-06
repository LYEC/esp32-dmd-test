#include "BluetoothSerial.h"
#include <DMD32.h>        //
#include "fonts/SystemFont5x7.h"
#include "fonts/Arial_black_16.h"
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 2
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);
BluetoothSerial SerialBT;
hw_timer_t * timer = NULL;
void IRAM_ATTR triggerScan()
{
  dmd.scanDisplayBySPI();
}
String gelen_veri = "";
#define setNames 100
#define setSkor 200
String name1 = "M.Y";
String name2 = "E.Y";
String skorA = "10";
String subSkorA = "10";
String skorB = "10";
String subSkorB = "10";
void setup() {
  delay(3000);
  Serial.begin(115200);
  SerialBT.begin("emta"); 
  uint8_t cpuClock = ESP.getCpuFreqMHz();
  timer = timerBegin(0, cpuClock, true);
  timerAttachInterrupt(timer, &triggerScan, true);
  timerAlarmWrite(timer, 300, true);
  timerAlarmEnable(timer);
  dmd.clearScreen( true );  
  delay(1000); 
  updateScreen();
}
String splitData(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void processing(){
    /*
      header | data | #
                      v
                    last byte
  */
    int header = splitData(gelen_veri,'|',0).toInt();
    Serial.println("Packet Header : " + String(header));
    switch(header){
      case setNames:
          name1 = splitData(gelen_veri,'|',1);
          name2 = splitData(gelen_veri,'|',2);
          Serial.println("Name1 : " + name1);
          Serial.println("Name2 : " + name2);
          updateScreen();
      break;
      case setSkor:
          skorA = splitData(gelen_veri,'|',1);
          subSkorA = splitData(gelen_veri,'|',2);
          skorB =  splitData(gelen_veri,'|',3);
          subSkorB = splitData(gelen_veri,'|',4);
          updateScreen();
          Serial.println("SkorA : " + skorA + " SubSkorA : " +subSkorA );
          Serial.println("SkorB : " + skorB + " SubSkorB : " +subSkorB );
      break;
    }
}

void updateScreen(){
   dmd.clearScreen( true );
   dmd.selectFont(Arial_Black_16);
   char n1[name1.length()+1];
   name1.toCharArray(n1,name1.length()+1);
   dmd.drawString(  1,0, n1, 3, GRAPHICS_NORMAL );
   dmd.drawLine(30, 2, 30, 13, GRAPHICS_NORMAL);
   char n2[name2.length()+1];
   name2.toCharArray(n2,name2.length()+1);
   dmd.drawString(  32,0, n2, 3, GRAPHICS_NORMAL );  
   dmd.selectFont(System5x7);
   char sAs[subSkorA.length()+1];
   subSkorA.toCharArray(sAs,subSkorA.length()+1);
   dmd.drawString(  0,25, sAs, 2, GRAPHICS_NORMAL );
   dmd.selectFont(Arial_Black_16);
   char sA[skorA.length()+1];
   skorA.toCharArray(sA,skorA.length()+1);
   dmd.drawString(  skorA.length() == 1 ? subSkorA.length() == 2 ? 16 : 14 :12,16, sA, 2, GRAPHICS_NORMAL );
   dmd.drawLine(30, 18, 30, 29, GRAPHICS_NORMAL);
   dmd.selectFont(System5x7);
   char sBs[subSkorB.length()+1];
   subSkorB.toCharArray(sBs,subSkorB.length()+1);
   dmd.drawString(  32,25, sBs, 2, GRAPHICS_NORMAL );
   dmd.selectFont(Arial_Black_16);
   char sB[skorB.length()+1];
   skorB.toCharArray(sB,skorB.length()+1);
   dmd.drawString( skorB.length() == 1 ?  subSkorB.length() == 2 ? 47:46:44,16, sB, 2, GRAPHICS_NORMAL );
    
}

void loop() { 
  if (SerialBT.available()) {
    char readByte = SerialBT.read();
    gelen_veri = gelen_veri + readByte;
    if(readByte == '#'){
      Serial.println("Packet OK!");
      processing();
      gelen_veri = "";
      }      
  }
}
