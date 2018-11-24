 // Knock탭은 main탭이다.
#include <Keypad.h>            // 키패드 라이브러리, https://steemit.com/kr-newbie/@codingman/keypad
#include "LiquidCrystal_I2C.h"  // LCD 라이브러리
#include <Wire.h>               // LCD 라이브러리
#include <LedControl.h>        // 도트매트릭스 라이브러리, http://www.eduino.kr/product/detail.html?product_no=340&cate_no=57&display_group=1
#include <SoftwareSerial.h>   // 초음파, 시리얼통신 라이브러리


bool isKeyPad = false;          // 키패드 입력없을 경우??? 입력받은 전화번호 저장하여 LCD,도트매트릭스 띄울용도

LedControl lc = LedControl(12,11,10,4);    // 도트매트릭스, DIN 핀을 12번, CS 핀을 10번, CLK핀을 11번에 연결(DIN, CLK, CS, 연결할 도트 매트릭스의 개수) 
byte digits[15][8]={                         // 도트(도트겟수, 행&열??)
{ 
   B01111110, 
   B00010010, 
   B00111010,
   B00101110,
   B00101010,
   B00000010,
   B01000000,
   B01111110 
},                                           // 출력: 전
{  
   B00010010, 
   B01111110, 
   B00111010, 
   B00101011, 
   B00111010, 
   B00010010, 
   B01111110, 
   B00000010  
},                                           // 출력: 화
{  
   B01001010, 
   B01001010, 
   B01111010, 
   B01001110, 
   B01111010, 
   B00000010, 
   B01000000, 
   B01111110  
},                                           // 출력: 번
{  
   B00010000, 
   B01111101, 
   B00111001, 
   B00101000, 
   B00111000, 
   B00000001, 
   B00010001, 
   B01111100  
},                                           // 출력: 호:
{
   B00010000,
   B00110000,
   B00010000,
   B00010000,
   B00010000,
   B00010000,
   B00010000,
   B00111000
},                                           // 출력: 1
{
   B00111000,
   B01000100,
   B00000100,
   B00000100,
   B00001000,
   B00010000,
   B00100000,
   B01111100
},                                           // 출력: 2
{
   B00111000,
   B01000100,
   B00000100,
   B00011000,
   B00000100,
   B00000100,
   B01000100,
   B00111000
},                                           // 출력: 3
{
   B00000100,
   B00001100,
   B00010100,
   B00100100,
   B01000100,
   B01111110,
   B00000100,
   B00000100
},                                           // 출력: 4
{
   B01111100,
   B01000000,
   B01000000,
   B01111000,
   B00000100,
   B00000100,
   B01000100,
   B00111000
},                                           // 출력: 5
{
   B00111000,
   B01000100,
   B01000000,
   B01111000,
   B01000100,
   B01000100,
   B01000100,
   B00111000
},                                           // 출력: 6
{
   B01111100,
   B00000100,
   B00000100,
   B00001000,
   B00010000,
   B00100000,
   B00100000,
   B00100000
},                                           // 출력: 7
{
   B00111000,
   B01000100,
   B01000100,
   B00111000,
   B01000100,
   B01000100,
   B01000100,
   B00111000
},                                           // 출력: 8
{
   B00111000,
   B01000100,
   B01000100,
   B01000100,
   B00111100,
   B00000100,
   B01000100,
   B00111000
},                                           // 출력: 9

{  
   B00111000, 
   B01000100, 
   B01001100, 
   B01010100, 
   B01100100, 
   B01000100, 
   B01000100, 
   B00111000  
},                                           // 출력: 0
{ 
   B00000000, 
   B00000000, 
   B00000000, 
   B00000000, 
   B00000000, 
   B00000000, 
   B00000000, 
   B00000000                                
}};                                          // 출력: null  
/*--------------------위: 도트메트릭스, 아래: 키패드----------------------*/
const byte rows = 4;                 // 키패드 가로줄
const byte cols = 4;                 // 키패드 세로줄
byte rowPins[rows] = {9,8,7,6};      // 키패드 가로줄
byte colPins[cols] = {5,4,3,2};      // 키패드 세로줄
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);     // 키패드(키값배열, 열(세로줄)핀, 행(가로줄)핀, 열수, 행수)
/*--------------------위: 키패드, 아래: 블루투스--------------------------*/
unsigned int Tx=2;                                     // 블루투스 Tx (보내는핀 설정), http://deneb21.tistory.com/267
unsigned int Rx=3;                                     // 블루투스 Rx (받는핀 설정)
char inputChar[12] = {};                               // 문자형으로 입력받은12글자 0으로 전부 초기화(블루투스에 받은값 저장용)
SoftwareSerial mySerial(Tx, Rx);                     // 시리얼 통신을 위한 객체선언
String myString="";                                    // 받는(시리얼->블루투스) 문자열
/*--------------------위: 블루투스, 아래: LCD 디스플레이------------------*/
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2);     // 16 문자 및 2 라인 디스플레이의 경우 LCD 주소를 0x27로 설정하십시오.()????
//LiquidCrystal_I2C lcd(0x27,16,2);    // 객체선언, 0x27(주소)의 가로16 세로2의 LCD를 제어선의 연결핀에 설정함(이거도됨)(아날로그 쓰므로 핀모듈설정 불필요)
/*--------------------위: LCD 디스플레이, 아래: 초음파------------------*/ 
//int SDA = A4;                        // 초음파 센서 제조단계에서 이미 예약되어 있는거라 선언 불필요
//int SCL = A5;                        // 초음파 센서 제조단계에서 이미 예약되어 있는거라 선언 불필요 
const unsigned int TRIGGER = 6;      // 초음파 발신(상수에 락걸어 2바이트씩 양수로만 저장) 
const unsigned int ECHO    = A0;     // 초음파 수신(상수에 락걸어 2바이트씩 양수로만 저장)
const float SENSOR_PARAMETER = 32;   // 출력이 너무작게 나오면 이값을 키우고, 출력이 크면 이값을 줄여라(부동수소점 수에 락걸어 4바이트씩 소수점수 저장)


void setup() {
  Serial.begin(9600);               // 컴퓨터에 데이터 전송(속도9600)
  while(!Serial);                   // 0이외의 값이 넘어올때까지(시리얼 통신받을때 까지)기다리겠다  
  mySerial.begin(9600);              // 블루투스에 데이터 전송(속도96500)
  pinMode(TRIGGER,OUTPUT);           // 초음파의 TRIGGER는 데이터 송신용임을 선언
  pinMode(ECHO,INPUT);               // 초음파의 ECHO는 데이터 발신용임을 선언
  Wire.begin();                 // 화면에 나오게 하기
  lcd.begin();                  // 가로16개, 세로2개를 초기화 
  lcd.home();                   // 커서를 원점으로 보냄
  lcd.clear();                  // 액정 모두삭제, 원점이동 
  lcd.print("Welcome to LUX");  // 화면에 "방문 환영"출력
  lcd.setCursor(0,1);           // 커서설정(다음줄로 커서를 내려라)
  lcd.print("Corp.");           // 화면에 "럭스사(社)"출력 
  delay(3000);                  // 딜레이 3초 
  for(int i=0; i<4; i++){            // 도트 매트릭스 4번반복
    lc.shutdown(i,false);            // 디스플레이 초기화 및 절전취소
    lc.setIntensity(i,1);            // 도트 매트릭스 밝기 (매트릭스 번호, 밝기) 1~15
    lc.clearDisplay(i);              // 도트 매트락스의 led 를 전체끔(초기화) 함수
  }
}


void loop() {
  BLUETOOTH();             // 블루투스  
  ULTRASONIC();            // 초음파 & 함수 내부에 DOTMATRIX(),LCD()가 연동됨
//  DOTMATRIX();             // 도트매트릭스  
//  KEYPAD();                // 키패드 
}

/*-----------------------------(아래)블루투스 탭--------------------------------*/
void BLUETOOTH(){              // 블루투스
//  Serial.println("Bluetooth() 진입");   
//  unsigned int sizeChar = 0;                              // 양의정수값으로 입력받은 글자의 길이
  int numberLength = 0;                                   // 정수형으로 번호길이 초기화
  char inputChar[12] = {'\0',};                           // 문자형으로 입력받은12글자 0으로 전부 초기화

   
  if(mySerial.available()){                               // 만일 블루투스에서 입력이 있다면(블루투스->시리얼 제어용) 
    numberLength = mySerial.readBytes(inputChar, 11);     // 블루투스에서 11자리만 입력받아 inputChar입력후 numberLength에 저장  
//    numberLength = mySerial.read();        //블루투스측 내용을 시리얼모니터에 출력                                
//    if(numberLength < 12 && numberLength > 10){           // numberLength가 11자리면 출력(12작다 그리고 10보다 크다) 
    if(numberLength > 10){    
      Serial.print("lcd.print(intputChar): "); 
      Serial.println(inputChar); 
//      lcd.setCursor(0,0);                                 // 커서설정
      lcd.clear();                                        // 액정 모두삭제, 원점이동
      lcd.print("Call me phone #");
      lcd.setCursor(0,1);
      lcd.print(inputChar); 
//      lcd.flush();  
//      lcd.home(); 
    }              
  } 

  else{                                                   // 만일 블루투스에서 입력없을시
//    Serial.flush();                                      // 시리얼통신 데이터 초기화(쓰레기값 제거용)
    return;                                               // 리턴
  }
  if (isKeyPad){                                          // 만일 isKeyPad값이 존재한다면    
    if(mySerial.available()){                             // 만일 블루투스에서 입력이 있다면(블루투스->시리얼 제어용)
      mySerial.print("010-0000-0000");                    // 블루투스에 "전화번호" 출력전송
    }    
  }
// Serial.println("Bluetooth() 탈출");      
}

// https://m.blog.naver.com/PostView.nhn?blogId=vkdlsvnt&logNo=220237820354&proxyReferer=https%3A%2F%2Fwww.google.co.kr%2F
// 시리얼,블루트스 연결 https://m.blog.naver.com/PostView.nhn?blogId=makist2015&logNo=220408135622&proxyReferer=https%3A%2F%2Fwww.google.co.kr%2F

/*-----------------------------(아래)도트매트릭스 탭--------------------------------*/
void DOTMATRIX(){  // 도트매트릭스 http://turtleshell.kr/75 , http://www.makeshare.org/bbs/board.php?bo_table=arduinodisp&wr_id=1
//  Serial.print("DOTMATRIX() 진입");
  
  int row, col;                                     // 행(가로 줄), 열(세로 줄)

  for(int i = 0; i < 8; i++){              // "전화번호" 라는글자출력
    for(int j = 0; j < 8; j++){
      lc.setRow(0, j, digits[3][j]);       // (연결된 매트릭스의 순서,행,[배너?][열])
      lc.setRow(1, j, digits[2][j]);       // (연결된 매트릭스의 순서,행,[배너?][열])
      lc.setRow(2, j, digits[1][j]);       // (연결된 매트릭스의 순서,행,[배너?][열])
      lc.setRow(3, j, digits[0][j]);       // (연결된 매트릭스의 순서,행,[배너?][열])      
    }delay(500);                           // 딜레이 0.5초
  }
  for(int i = 0; i < 4; i++){              // 3번 반복, LED 전체 꺼주기 
    lc.clearDisplay(i);                    // 화면 지우기
  } 

  for(int i = 0; i < 8; i++){              // "전화번호"&"번호" 라는글자출력 및 배너
    for(int j = 0; j < 8; j++){
      lc.setRow(0, j, digits[i+3][j]);     // (연결된 매트릭스의 순서,행,[Knock탭에서 정의한 도트 모양 불러옴][열])
      lc.setRow(1, j, digits[i+2][j]);     // (연결된 매트릭스의 순서,행,[Knock탭에서 정의한 도트 모양 불러옴][열])
      lc.setRow(2, j, digits[i+1][j]);     // (연결된 매트릭스의 순서,행,[Knock탭에서 정의한 도트 모양 불러옴][열])
      lc.setRow(3, j, digits[i][j]);       // (연결된 매트릭스의 순서,행,[Knock탭에서 정의한 도트 모양 불러옴][열])      
    }delay(500);                           // 딜레이 0.5초
  }
  for(int k = 0; k < 4; k++){              // 3번 반복, LED 전체 꺼주기 
    lc.clearDisplay(k);                    // 화면 지우기
  } 
//  Serial.println("DOTMATRIX() 탈출");      
}

/*-----------------------------(아래)키패드 탭--------------------------------*/
void KEYPAD(){          // 키패드
//    Serial.print("KEYPAD() 진입");
  
  char key = keypad.getKey();
  if (key != NO_KEY){           // 키값이 눌렀는지 확인문
    Serial.println(key);
  }
  if (key == 0){                // 입력이 없다면
    isKeyPad = true;            // isKeyPad에 입력된건 입력한다???
  }  
//    Serial.println("KEYPAD() 탈출");
}

/*-----------------------------(아래)LCD 탭--------------------------------*/
void LCD(){       
//  Serial.print("LCD() 진입");
/*  delay(500);                     // 딜레이 0.5초 
  lcd.clear();
  lcd.print("Call me phone #");
  lcd.setCursor(0,1);             // 커서설정(다음줄로 커서를 내려라)
  lcd.print(inputChar);           // 시리얼 화면에 입력값(전화번호) 출력   
  */
//  Serial.println("LCD() 탈출");  
}
/*-----------------------------(아래)블루투스 탭--------------------------------*/
void ULTRASONIC(){               // 초음파
//  Serial.println("ULTRASONIC 진입");
  unsigned int duration = 0;                     // 시간
  unsigned int distance = 0;                     // 거리
//  digitalWrite(TRIGGER, LOW);                    // 초음파 발사중지
//  delayMicroseconds(1000);                       // 용이한 수신을 위해 딜레이 0.001초 
  digitalWrite(TRIGGER, HIGH);                   // 초음파 발사
  delayMicroseconds(1000);                       // 용이한 수신을 위해 딜레이 0.001초 줌
  digitalWrite(TRIGGER, LOW);                    // 초음파 발사중지
  duration = pulseIn(ECHO, HIGH);                // 펄스가 high에서 low로 바뀔때 딜레이를 리턴해주는 함수
  distance = (duration / 2)/(SENSOR_PARAMETER);  // 거리=(속력/2)/시간 , 속력/2 하는이유: 초음파발신후 송신하기에 2배의 값을얻음 
//  Serial.print(distance);                       // 거리
//  Serial.println("cm");                         // 값조절 단위[cm]
  delay(200);                                    // 딜레이 0.2초
  
  if (distance < 50){                     // distance(거리) 가 50보다 작으면(인간 감지 된다면)
//    LCD();                                // LCD디스플레이어 
    DOTMATRIX();                          // 도트매트릭스     
  } 
  else{                                   // 감지 않되었다면
//    LCD();                                // LCD디스플레이어                                  
  }
//  Serial.println("ULTRASONIC 탈출");
}
