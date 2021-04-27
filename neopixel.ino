/*지자기 간섭 방지 설정
 * <sftm>
 * <sftmc>
 * 센서회전(8자)
 * >
 * 
 * 지자기 캘리브레이션
 * <cmf>
 * 센서회전(도리도리)
 * >
 * 
 * -소프트웨어적 오차줄임: 소수점변화가 생기면 그만큼 빼주기 -> 그래도 오차는 계속 커진다.
 * 2) 현재 속도:15hz, 보레이트 : 115200을 높여 준다.
 */

#include<SoftwareSerial.h>
#define SBUF_SIZE 64

//SoftwareSerial btSEOSerial(2,3); // 서진 블루투스tx-아두이노 4번(rx)
SoftwareSerial btJUSerial(4,5); // 주형

char sbuf[SBUF_SIZE];
signed int sbuf_cnt=0;
float euler[3];

//NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      1

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


int EBimuAsciiParser(float *item, int number_of_item)
{
  int n,i;
  int rbytes;
  char *addr; 
  int result = 0;
  
  rbytes = Serial1.available();
  for(n=0;n<rbytes;n++)
  {
    sbuf[sbuf_cnt] = Serial1.read();
  
    if(sbuf[sbuf_cnt]==0x0a)
       {
           addr = strtok(sbuf,",");
           for(i=0;i<number_of_item;i++)
           {
              item[i] = atof(addr);
              addr = strtok(NULL,",");
           }

           result = 1;

         // Serial.print("\n\r");
         // for(i=0;i<number_of_item;i++)  {  Serial.print(item[i]);  Serial.print(" "); }
       }
     else if(sbuf[sbuf_cnt]=='*')
       {   sbuf_cnt=-1;
       }

     sbuf_cnt++;
     if(sbuf_cnt>=SBUF_SIZE) sbuf_cnt=0;
  }
  
  return result;
}
//EBIMU FUNCTION 
//
int WcntF, Wcnt; //WcntF는 걸음걸이 수를 측정하기 위한 flag, Wcnt는 걸음걸이 수
float zeronum; // 걸음걸이 수 받는 초기값

void calWcnt() //걸음 걸이 측정 함수
{
  if(WcntF==1){
    if(euler[0]<zeronum+5&&euler[0]>zeronum-5){
      Wcnt++;
      WcntF=0;
    }
  }
   // btSEOSerial.print(",");
   // btSEOSerial.print("W");  // 걸음수 서진이에게 전송
   // btSEOSerial.print(",");
   // btSEOSerial.print(Wcnt); //걸음 수 (오른 발, 왼 발 합쳤으므로 2배정도 걸음걸이수를 측정할 수 있다.)
 
    // Serial.print("                            w  :  ");
   //  Serial.println(Wcnt);
    if(euler[0]>zeronum+5||euler[0]<zeronum-5)
    {WcntF=1;}
}

float zero, out, in; // zero는 팔자 걸음, 안짱 걸음 구분을 위한 초기 기준값, out은 팔자, in은 안짱
int zF/*, Ocnt, Icnt*/; // zF 초기 기준값을 받아오기 위한 flag, cnt 안정적인 데이터 전송을 위한 카운팅

void calWang() //팔자 걸음, 안짱 걸음 구분 함수
{ 
  out= zero + 15;
  in = zero - 15;

  
  if(euler[2]>out){
     Serial.println("out");
     
  //  btSEOSerial.print(",");
   // btSEOSerial.print("o");
    
    btJUSerial.print(1);

    for(int i=0;i<NUMPIXELS;i++){
     pixels.setPixelColor(i, pixels.Color(150,0,0)); //red
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(50); // Delay for a period of time (in milliseconds).
    }
    
  }

  else if(euler[2]<in){
    Serial.println("in");
    
  //  btSEOSerial.print(",");
 //   btSEOSerial.print("i");
    
    btJUSerial.print(2); 

    for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,0,150)); //blue
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(50); // Delay for a period of time (in milliseconds).
    }
  }

  else if((euler[2]>in)&&(euler[2]<out)){
    for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.  
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(50); // Delay for a period of time (in milliseconds).
  }
  }
}

float preWcnt/*, prePITCH*/;

void yawR() // 가만히 서있으면 Yaw값 기준점 다시 잡기
{
  static int T_C;
  T_C++;
  Serial.print("T_C :  ");
  Serial.println(T_C);
  Serial.print("ZERO :  ");
  Serial.println(zero);
  
  if(T_C==120) //5초일 때 걸음 걸이 수를 저장한다.
  { preWcnt = Wcnt;}
  
  if(T_C==235){ //10초가 됬을 때 걸음 걸이 수를 검사한다.
    if(preWcnt == Wcnt) //걷지 않을 때
    {
      zero = euler[2]; // yaw 기준값 변경     
    }
    T_C=0; // 다시 0초부터 시작한다
  }
}

float U; //U턴 기준값

void U_R()
{
   static int U_T; //U턴하기 위한 시간
   U_T++;  
  //Serial.print("U_T:  ");
  //Serial.println(U_T);
  //Serial.print("ZERO :  ");
  //Serial.println(zero);
  if(U_T==60) //2초일 때 U턴 기준 값을 잡는다
  { U = zero;} 
  
  if(U_T==120){ //5초일 때 U턴 기준값과 현재 yaw값을 비교한다
    if(U+60<euler[2]||U-60>euler[2]) // U턴 기준 값보다 60도 이상 돌았을 때
    {
       zero = euler[2]; // yaw 기준값 변경         
    }
      U_T=0;
   }  
}


void setup() {
  Serial.begin(115200);
  Serial1.begin(115200); // AHRS 센서
  
  //btSEOSerial.begin(115200);  //블투
  btJUSerial.begin(9600);

  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {
  
  if(EBimuAsciiParser(euler, 3))
  {
    // Serial.print("\n\r");
    // Serial.print(euler[0]);   Serial.print(" ");
    // Serial.print(euler[1]);   Serial.print(" ");
    // Serial.print(euler[2]);   Serial.print(" ");

     if(zF==0) // yaw(zero), roll(zero) 초기 기준값 잡는 if문
  { zero = euler[2];  zF = 1; zeronum = euler[0];}
  
   calWcnt(); //걸음 걸이 수 측정 함수
   calWang(); // 걸음 걸이 각도 측정 함수
   yawR(); // 가만히 서있으면 YAW값 기준 다시 잡는 함수
   U_R(); // U턴했을 때 YAW값 기준 다시 잡는 함수
  }
}
