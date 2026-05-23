이 프로젝트는 카니발(YP) 디젤의 DPF 진행상황을 확인가능한 기기를 제작하는것이 목표입니다.

준비물

1. 320x480해상도의 7796S 콘트롤러를 사용하는 LCD - 알리익스프레스에서 판매하는 3.5inch LCD를 사용
2. arduino nano 혹은 호환품
3. 2개의보드를 이루어진 ELM327
4. 10kohm, 5kohm 저항 각1개
5. 4wire 전선
6. USB mini 케이블


ELM327개조방법
1. 그림과 같이 BT module을 인두를 사용하여 떼어낸다.
<img width="300" height="400" alt="20260519_151101" src="https://github.com/user-attachments/assets/c94c5dda-702c-477b-826f-b9fea05f1149" />

2. 그림과 같이 wiring을 한다
<img width="300" height="400" alt="20260519_151055" src="https://github.com/user-attachments/assets/9c4e1aee-843a-4b5d-aba8-6b7dda41ddec" />


회로도구성
<img width="1638" height="1085" alt="image" src="https://github.com/user-attachments/assets/ec6b8ad2-520f-481e-8476-be52d4926a70" />


ELM327 배선
 1. 검정색선 - 아듀이노 GND
 2. 초록색선 - 아듀이노 D9, 저항으로 분압하여 연결  ( 아듀이노D9 --- 5k ----GND---- ELM327왼쪽윗핀)
 3. 흰색선 - 아듀이노 D8
 4. 빨강선 - 12V이나 상시전원이어서 따로 사용하지 않음

3.5inch LED 배선
 1. GND - 아듀이노 GND
 2. VCC - 아듀이노 VCC(5V)
 3. SCL - 아듀이노 D13
 4. SDA - 아듀이노 D11
 5. RST - 아듀이노 D7
 6. DC - 아듀이노 D6
 7. CS - 아듀이노 D10
 8. BL - 아듀이노 VCC(5V) -- 연결할수있는 전원핀이 없어서 6핀컨넥터 VCC에 연결


아듀이노 소스

 - canival_yp_DPF_Viewr.ino 를 IDE에서 실행하고 다운로드 하시면 됩니다.
 
 * arduino IDE에서 Adafruit ST7735 AND st7796 Library를 설치하셔야합니다.


동작사진

1. DPF포집중 (regen req가 점점 숫자가 올라감, regen cnt는 0을 유지)

<img width="380" height="285" alt="0db1b2d8cf3cc5cf921ebdac1db5200ace8d9a34f007385eee31d698861c147c" src="https://github.com/user-attachments/assets/0c39386e-0eb6-42ec-a78e-5f432e8e17bb" />

2. DPF 동작중 ( regen req,cnt 가 5가 되면서 시작되고 동작하면서 숫자가 감소함)
   
<img width="380" height="285" alt="7100be9047b3ae61cb78282d4522b0d2d5dd5fc9f935b93a513db2454ede9c8e" src="https://github.com/user-attachments/assets/014805b9-ef08-478b-abf4-e504143d49f3" />

3. DPF 동작완료 (regen req가 0으로 바뀜)
<img width="380" height="285" alt="a1c70d902e4abfb335e2b5aae886bd7d6042d4cef578f4e931362f408537e527" src="https://github.com/user-attachments/assets/9ae47b45-f58b-4976-bb4b-bfb0e007ad80" />



참고 site

 ST7796S LED구동 : https://blog.naver.com/ysahn2k/223868315651
 
 ELM327 datasheet : https://github.com/PowerBroker2/ELMduino/blob/master/reference/ELM327DS.pdf

 
