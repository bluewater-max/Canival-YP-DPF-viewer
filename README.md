이 프로젝트는 카니발(YP) 디젤의 DPF 진행상황을 확인가능한 기기를 제작하는것이 목표입니다.

준비물

1. 320x480해상도의 7796S 콘트롤러를 사용하는 LCD - 알리익스프레스에서 판매하는 3.5inch LCD를 사용
2. arduino nano 호환품
3. 2개의보드를 이루어진 ELM327
4. 10kohm, 5kohm 저항 각1개
5. 4wire 전선
6. USB mini 케이블


회로도구성
 - OBD DPF Viewer schematic을 참고하시면 됩니다.

아듀이노 소스

 - canival_yp_DPF_Viewr.ino 를 IDE에서 실행하고 다운로드 하시면 됩니다.
 * arduino IDE에서 Adafruit ST7735 AND st7796 Library를 설치하셔야합니다.


참고 site
 ST7796S LED구동 : https://blog.naver.com/ysahn2k/223868315651
 ELM327 datasheet : https://github.com/PowerBroker2/ELMduino/blob/master/reference/ELM327DS.pdf

 
