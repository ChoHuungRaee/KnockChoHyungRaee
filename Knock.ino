// Knock탭은 main탭이다.
#include <Keypad.h>            // 키패드라이브러리
#include "LiquidCrystal_I2C.h"  // LCD라이브러리
#include <Wire.h>               // LCD라이브러리
#include <LedControl.h>        // 도트매트릭스라이브러리

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2);  // 16 문자 및 2 라인 디스플레이의 경우 LCD 주소를 0x27로 설정하십시오.()????
//LiquidCrystal_I2C lcd(0x27,16,2);  // 객체선언, 0x27(주소)의 가로16 세로2의 LCD를 제어선의 연결핀에 설정함(이거도됨)(아날로그 쓰므로 핀모듈설정 불필요)
//int SDA = A4;
//int SCL = A5;
const unsigned int TRIGGER = 6;      // 초음파 발신(상수에 락걸어 2바이트씩 양수로만 저장) 
const unsigned int ECHO    = 7;      // 초음파 수신(상수에 락걸어 2바이트씩 양수로만 저장)
const float SENSOR_PARAMETER = 32;   // 출력이 너무작게 나오면 이값을 키우고, 출력이 크면 이값을 줄여라(부동수소점 수에 락걸어 4바이트씩 소수점수 저장)

void setup() {
  Serial.begin(115200);             // 컴퓨터에 데이터 전송(속도112500)
  while(!Serial);                   // 0이외의 값이 넘어올때까지(시리얼 통신받을때 까지)기다리겠다
  pinMode(TRIGGER,OUTPUT);           // TRIGGER는 데이터 송신용임을 선언
  pinMode(ECHO,INPUT);               // ECHO는 데이터 발신용임을 선언
  //  Wire.begin();                 // 화면에 나오게 하기
  lcd.begin();                  // 가로16개, 세로2개를 초기화 
  lcd.home();                   // 커서를 원점으로 보냄
  lcd.clear();                  // 액정 모두삭제, 원점이동 
  lcd.print("Welcome to ");     // 화면에 "방문 환영"출력
  lcd.setCursor(0,1);           // 커서설정(다음줄로 커서를 내려라)
  lcd.print("LUX Corp.");       // 화면에 "럭스사(社)"출력 
  delay(3000);                  // 딜레이 3초 
}

void loop() {
  LCD();
  UITRASONIC();
}

void LCD(){     
  delay(500);                   // 딜레이 0.5초  
  lcd.clear();                  // 액정 모두삭제, 원점이동  
  lcd.print("Waiting for");
  lcd.setCursor(0,1);           // 커서설정(다음줄로 커서를 내려라)
  lcd.print("input");
  for(int i=0; i<5; i++){       // 5번 반복
    delay(500);                 // 딜레이 0.5초
    lcd.print(".");
  }
}
void KEYPAD(){ 
}
void WIFI(){
}
void DOTMATRIX(){                // 도트매트릭스
}
void UITRASONIC(){               // 초음파
  unsigned int duration = 0;                     // 시간
  unsigned int distance = 0;                     // 거리
  digitalWrite(TRIGGER, HIGH);                   // 초음파 발사
  delayMicroseconds(1000);                       // 용이한 수신을 위해 딜레이 0.001초 줌
  digitalWrite(TRIGGER, LOW);                    // 초음파 발사중지
  duration = pulseIn(ECHO, HIGH);                // 펄스가 high에서 low로 바뀔때 딜레이를 리턴해주는 함수
  distance = (duration / 2)/(SENSOR_PARAMETER);  // 초음파가 보냈다가 돌어온 시간가지고 거리계산, 값조절 단위[cm] 센서파라미터
  Serial.println(distance);
  delay(200);
}


/**
조대리, 4칙 연산을 할 수 있는 함수를 4개를 만들어서 함수명을(addition, subtraction, multiplication, division)이라고 하고, 
시리얼 포트 화면에서 2개의 정수를 입력 받아서, 그 결과값을 시리얼 화면에서 보여주는 코드를 만들어봐.
숙제임.
*/
