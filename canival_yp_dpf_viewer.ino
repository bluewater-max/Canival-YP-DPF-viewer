#include <Adafruit_GFX.h>
#include <Adafruit_ST7796S.h>
#include <Fonts/FreeMonoBold12pt7b.h> // A custom font
#include <SoftwareSerial.h>
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long uint32_t;

// Define display pin connections
#define TFT_SCLK    13
#define TFT_MOSI    11
#define TFT_CS      10
#define TFT_RST     7 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC      6
Adafruit_ST7796S display(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define OBD_RX      9
#define OBD_TX      8
SoftwareSerial OBD_Serial(OBD_RX, OBD_TX);

uint8_t isConnected = 0;
const uint8_t maxRxSize = 200;
uint8_t rxStatus;
byte rxCnt;
char rxData[maxRxSize];
#define RxSuccess   0x80
#define RxTimeout   0x40
#define RxOK        0x08
#define RxDataResp  0x08
#define RxDelimeter 0x04
#define RxQuestion  0x02
#define RxELM327    0x01

uint32_t lastEventTime = 0;
const uint32_t timeoutInterval =  5000; // 5 seconds

//default font pixel size
const uint16_t xx = 14;
const uint16_t yy = 24;

// memory for display data
// to minimize update
uint8_t rcp = '_';
uint8_t tcp = '_';
uint8_t dbuf[6];  // display number buffer
uint8_t et1[5]; // temp sensor
uint8_t et2[5]; // temp sensor
uint8_t sm[5]; // soot mass
uint8_t ts[5];  //total sulphur mass
uint8_t dist[5]; // total distance
uint8_t regen[5]; // regen progress percent

uint8_t flagRegen = 0;
uint16_t sm_regenStart = 0;
uint16_t sm_current = 0;


void lcd_print(uint16_t x, uint16_t y, uint8_t len, char* dbuf, char* dbufp, uint16_t tSize) {
  uint8_t i = 0;
  display.setTextSize(tSize);
  while (i < len) {
    if (dbuf[i] != dbufp[i]) {
      display.fillRect((x + i * tSize)*xx, y * yy - 18 * tSize , xx * tSize, yy * tSize, 0x001f);
      display.setCursor((x + i * tSize)*xx, y * yy);
      display.print(dbuf[i]);
    }
    i++;
  }
}

void intX100ToStrFloat(uint16_t val, char* ch) {
  uint8_t i;

  i = (val / 10000) % 10;
  if (i == 0) ch[0] = ' ';
  else ch[0] = i + '0';

  i = (val / 1000) % 10;
  if (i == 0 && ch[0] == ' ') ch[1] = ' ';
  else ch[1] = i + '0';

  i = (val / 100) % 10;
  ch[2] = i + '0';

  ch[3] = '.';

  ch[4] = (val / 10) % 10 + '0';
}

void saveDisplayMemory(uint8_t* now, uint8_t* previous) {
  uint8_t i;
  for (i = 0; i < 5; i++) {
    previous[i] = now[i];
  }
}




uint8_t receiveOBD() {
  uint32_t currentMillis;
  uint8_t rxd, rxdp, rxdpp, i;

  rxCnt = 0;
  rxStatus = 0;
  lastEventTime = millis();

  while (true) {
    currentMillis = millis();
    if (currentMillis - lastEventTime >= timeoutInterval) {
      rxStatus |= RxTimeout;
      Serial.println("#rx timeout");
      return rxStatus;
    }

    if (OBD_Serial.available()) {
      rxdpp = rxdp;
      rxdp = rxd;
      rxd = OBD_Serial.read();
      if ((rxd >= 'A' && rxd <= 'F') || (rxd >= '0' && rxd <= '9')) {
        rxData[rxCnt++] = rxd;
      }

      if (rxd == '>') {
        rxStatus |= RxDelimeter | RxSuccess;
        if (rxData[0] == '7' && rxData[1] == 'E' && rxData[2] == '8') {
          rxStatus |= RxDataResp;
          Serial.println("rxed data response");
        }
        else if (rxData[1] == '7' && rxData[2] == 'E' && rxData[3] == '8') {
          for (i = 0; i < maxRxSize - 1 ; i++) {
            rxData[i] = rxData[i + 1];
          }
          rxStatus |= RxDataResp;
          Serial.println("rxed data response.+1");
        }
        else if (rxData[0] == 'E' && rxData[1] == '8' && rxData[2] == '1') {
          for (i = maxRxSize; i != 0 ; i--) {
            rxData[i] = rxData[i - 1];
          }
          rxStatus |= RxDataResp;
          Serial.println("rxed data response.-1");
        }
        else {
          Serial.println("rxed no data response.");
        }
        return rxStatus;
      }
      else if (rxd == '⸮' || rxd == '?' ) {
        rxStatus |= RxQuestion;
      }
      else if (rxdp == 'O' && rxd == 'K') {
        rxStatus |= RxOK;
      }
      else if (rxdpp == 'E' && rxdp == 'L' && rxd == 'M') {
        rxStatus |= RxELM327;
      }
    }
  }
}


uint8_t initOBD(void) {
  Serial.println("#start ELM327 INIT");

  delay(200);
  OBD_Serial.println("ATZ");
  if (!(receiveOBD() & RxELM327)) {
    return 0;
  }
  Serial.println("#ATZ - OK");


  delay(200);
  OBD_Serial.println("ATD");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATD - OK");

  delay(200);
  OBD_Serial.println("ATD0");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATD0 - OK");

  delay(200);
  OBD_Serial.println("ATH1");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATH1 - OK");

  delay(200);
  OBD_Serial.println("ATM0");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATM0 - OK");

  delay(200);
  OBD_Serial.println("ATS0");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATS0 - OK");

  delay(200);
  OBD_Serial.println("ATAL");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATAL - OK");

  delay(200);
  OBD_Serial.println("ATST64");
  if (!(receiveOBD() & RxOK)) {
    return 0;
  }
  Serial.println("#ATST64 - OK");

  return 1;
}

uint8_t charToHex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return 0;
}

uint8_t asciiToByte(char* d) {
  return (charToHex(d[0]) << 4 | charToHex(d[1]));
}

uint16_t asciiToInt(char* d) {
  return (charToHex(d[0]) << 12 | charToHex(d[1]) << 8 | charToHex(d[2]) << 4 | charToHex(d[3]));
}

uint32_t asciiToLong(char* d) {
  uint32_t ret1, ret2;
  ret1 = (charToHex(d[0]) << 12 | charToHex(d[1]) << 8 | charToHex(d[2]) << 4 | charToHex(d[3]));
  ret1 = 0 + ret1 << 16;
  ret2 = 0xffff & (charToHex(d[4]) << 12 | charToHex(d[5]) << 8 | charToHex(d[6]) << 4 | charToHex(d[7]));
  ret1 = ret1 + ret2;
  return ret1;
}

void setup() {
  Serial.begin(38400);
  Serial.println("#debug serial ready");

  OBD_Serial.begin(38400);

  display.init(320, 480, 0, 0, ST7796S_RGB);

  display.setRotation(1);
  display.invertDisplay(true);
  display.setFont(&FreeMonoBold12pt7b);
  display.setTextWrap(false);
  display.setTextSize(1);

  display.setCursor(0, 10 * yy);
  display.print(F("DPF viewer for canival(YP)\n"));
  display.print(F("Test version 2026-05-17\n"));
  display.print(F("by yj04.choi@gmail.com"));

  display.fillScreen(0);

  display.setCursor(0, 0);
  //  display.print(F("\nRegen Req     Tot.Cnt\n\n\n"));
  display.print(F("\nRegen    Regen\n"));
  display.print(F(  "Req.     Cnt.\n\n"));
  display.print(F(  "Exhaust\n"));
  display.print(F(  "temp             Deg           Deg\n\n"));
  display.print(F(  "Soot           Total\n"));
  display.print(F(  "mass         g sulphur          mg\n\n"));
  display.print(F(  "Distance since last Regen       km"));
}

void loop() {
  uint8_t ret, i;
  uint16_t result;
  uint32_t resultL;
  //  long resultSL;

  if (!isConnected) {
    isConnected = initOBD();
    if (isConnected == 1) {
      Serial.println("#ELM327 initialized");
    }
    else {
      Serial.println("#ELM327 init-fail");
    }
  }
  else {
    //    delay(1000);
    //    OBD_Serial.println("017C");
    //    if (receiveOBD() & RxSuccess) {
    //      resultL = asciiToInt(&rxData[11]);
    //      resultL = resultL * 10 - 4000;
    //      Serial.print("#DPF temp*100:");
    //      Serial.println(resultL);
    //    }


    delay(500);
    OBD_Serial.println("22ED94");
    ret = receiveOBD();
    if (ret & RxSuccess) {

      resultL = asciiToByte(&rxData[32]);
      if (flagRegen == 0) {
        sm_regenStart = (uint16_t)resultL;
      }
      else {
        sm_current = (uint16_t)resultL;
      }
      resultL = resultL * 9900 / 255;

      intX100ToStrFloat((uint16_t)resultL, dbuf);
      lcd_print(5, 8, 4, &dbuf[1], &sm[1], 2);
      saveDisplayMemory(dbuf, sm);



      Serial.print("#DPF soot mass*100:");
      Serial.println(resultL);
    }

    delay(500);
    OBD_Serial.println("22ED03");
    ret = receiveOBD();
    if (ret & RxSuccess) {

      //      resultL = asciiToLong(&rxData[125]) / 10;
      //      Serial.print("#Odometer at last DPF regen*100:");
      //      Serial.println(resultL);

      resultL = asciiToLong(&rxData[138]) / 10;
      intX100ToStrFloat((uint16_t)resultL, dbuf);
      lcd_print(26, 10, 5, dbuf, dist, 1);
      saveDisplayMemory(dbuf, dist);
      Serial.print("#Distance since Last DPF regen*100:");
      Serial.println(resultL);

      // Regen demand counter
      dbuf[0] = asciiToByte(&rxData[123]) + '0';
      lcd_print(6, 2, 1, dbuf, &rcp, 2);
      rcp = dbuf[0];
      //      saveDisplayMemory(dbuf, &rcp);
      Serial.print("#Regen demand counter by soot load:");
      Serial.println(dbuf[0]);


      // Regen Total counter
      dbuf[0] = asciiToByte(&rxData[163]) + '0';
      lcd_print(15, 2, 1, dbuf, &tcp, 2);

      display.setTextSize(1);
      display.fillRect(25 * 14, 1 * 24 - 18, 24 * 14, 24, 0x0000);
      display.setCursor(25 * 14, 1 * 24);
      display.print(sm_regenStart);
      display.fillRect(25 * 14, 2 * 24 - 18, 24 * 14, 24, 0x0000);
      display.setCursor(25 * 14, 2 * 24);
      display.print(sm_current);

      if (flagRegen == 0) {
        if ((tcp == '0' || tcp == '_') && dbuf[0] != '0' && dbuf[0] == rcp) { // totcnt is changed 0 -> 4?5?
          flagRegen = 1;
          display.setCursor(0, 24 * 12);
          display.setTextSize(1);
          display.print(F("Regen  0% |____________________|"));
        }
      }
      else {
        if ( dbuf[0] != '0'&& dbuf[0] == rcp) {

          result = sm_current * 100;
          result = result / sm_regenStart;
          result = result * 100;
          result = 10000 - result;

          intX100ToStrFloat(result, &dbuf[1]);
          lcd_print(5, 12, 3, &dbuf[1], regen, 1);
          saveDisplayMemory(&dbuf[1], regen);

          display.setTextSize(1);
          display.fillRect(5 * 14, 12 * 24 - 18, 9 * 14, 24, 0x0000);
          display.setCursor(5 * 14, 12 * 24);
          display.print(result);

          display.setCursor(11 * 14, 12 * 24);
          for (i = 0; i < result / 500; i++) {
            display.print('#');
          }
        }
        else {
          display.fillRect(0, 12 * 24 - 18, 480, 24, 0x0000);
          flagRegen = 0;
        }
      }
      tcp = dbuf[0];
      Serial.print("#Total Regen conter:");
      Serial.println(dbuf[0]);




      resultL = asciiToByte(&rxData[112]);
      resultL = resultL * 120100 / 255 + 9900;
      intX100ToStrFloat((uint16_t)resultL, dbuf);
      lcd_print(7, 5, 5, dbuf, et1, 2);
      saveDisplayMemory(dbuf, et1);

      Serial.print("#temp1*100:");
      Serial.println(resultL);


      resultL = asciiToByte(&rxData[119]);
      resultL = resultL * 120100 / 255 + 9900;
      intX100ToStrFloat((uint16_t)resultL, dbuf);
      lcd_print(21, 5, 5, dbuf, et2, 2);
      saveDisplayMemory(dbuf, et2);

      Serial.print("#temp2*100:");
      Serial.println(resultL);
    }

    //    delay(500);
    //    OBD_Serial.println("22E0F1");
    //    if (receiveOBD() & RxSuccess) {
    //      resultSL = asciiToByte(&rxData[100]);
    //      resultSL = resultSL * 110000 / 255 - 10000;
    //      Serial.print("#DPF diff pressure*100:");
    //      Serial.println(resultSL);
    //    }

    delay(500);
    OBD_Serial.println("22ED29");
    ret = receiveOBD();
    if (ret & RxSuccess) {
      resultL = (uint32_t)asciiToInt(&rxData[13]);
      resultL = resultL * 15259; //*0.15259 *100
      resultL = resultL /  1000;
      intX100ToStrFloat((uint16_t)resultL, dbuf);
      lcd_print(22, 8, 5, dbuf, ts, 2);
      saveDisplayMemory(dbuf, ts);

      Serial.print("#Total sulphur mass*100:");
      Serial.println(resultL);
    }
  }
}
