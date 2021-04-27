#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include<SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

//인스턴스를 선언한다
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


#define PIN 8
#define NUMPIXELS      5
SoftwareSerial mySerial(2,3);
int date;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
   
  
  Serial.begin(115200);
  mySerial.begin(9600);
 
  // 초기화 : 아래 사항은 필수로 기입한다.
  // I2C addr 0x3D (for the 128x64)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D); 
  delay(500);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
  // 초기화 완료
 
  
  // 스플레시 화면 출력
  display.display(); 
  delay(500);

  // 화면과 버퍼 클리어(초기화시 화면 클리어)
  display.clearDisplay();
  display.setTextColor(WHITE);  // text : 글자색 (Black 또는 White)
  display.setTextSize(2);
  display.setCursor(30, 30);
  display.println(" Hello!");
 // display.drawBitmap(5,25, footprint, 32, 32, 1);
   pixels.begin();
  display.display();
 //display.clearDisplay();
 
 // 출력할 이미지나 문자열을 버퍼에 전송후 화면에 뿌려주기 위해서는 반드시 함수가 사용되어야한다
}

void loop() {
   display.clearDisplay();
// put your main code here, to run repeatedly:
//digitalWrite(led, HIGH);   
if (mySerial.available())
  {
    //display.setCursor(0,0);
     date = (int)mySerial.read();
//   Serial.write(date);
     //display.print(date);
 // display.display();
 if(date==51)
   {
    for(int i=0;i<NUMPIXELS;i++)
     {      pixels.setPixelColor(i, pixels.Color(0,10,0));
   pixels.show();
 }
     
      display.setCursor(35,25);
      display.print("GOOD");      
      
   }
   
  else if(date==49)
  {
     display.setCursor(25,25);
      display.print("outside");      
    //  display.display();
      
     for(int i=0;i<NUMPIXELS;i++)
     {      pixels.setPixelColor(i, pixels.Color(150,0,0));
   pixels.show();
 }
     // Moderately bright green color.
     // pixels.show();
    
  }
  else if(date==50)
  { 
    display.setCursor(25,25);
      display.print("inside");      
 //     display.display(); 
      
      for(int i=0;i<NUMPIXELS;i++)
      {
      pixels.setPixelColor(i, pixels.Color(0,0,150)); 
      // Moderately bright green color.
      // This sends the updated pixel color to the hardware.
       pixels.show();
     
      }
    //  pixels.show();
     
   }
   
   
  //  pixels.show();
        display.display();

   
  }

}

