#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
int ballPositionX=28;
int ballPositionY=45;
int directionX=-1;
int directionY=-1;
int positionXMin=0;
int positionXMax=86;
int positionYMin=0;
int positionYMax=48;
boolean start = 1;
void setup() {
  Serial.begin(9600);

  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);

  
  display.display(); // show splashscreen
  delay(1000);
  display.clearDisplay();

}

void testMoveBall(int posX,int posY,int dX,int dY){
  if(dX == 1 && dY == 1){
    for(;ballPositionX<positionXMax,ballPositionY<positionYMax;ballPositionX+=2,ballPositionY+=2){
      if(ballPositionX>86)break;
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);
    display.display(); 
    delay(20);
    display.clearDisplay();
    }
}
  if(dX == 1 && dY == -1){
    for(;ballPositionX<positionXMax,ballPositionY>positionYMin;ballPositionX+=2,ballPositionY-=2){
      if(ballPositionX>86)break;
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);
    display.display(); 
    delay(20);
    display.clearDisplay();
  }
}

  if(dX == -1 && dY == 1){
    for(;ballPositionX>positionXMin,ballPositionY<positionYMax;ballPositionX-=2,ballPositionY+=2){
      if(ballPositionX<0)break;
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);
    display.display(); 
    delay(20);
    display.clearDisplay();
  }
}

  if(dX == -1 && dY == -1){
    for(;ballPositionX>positionXMin,ballPositionY>positionYMin;ballPositionX-=2,ballPositionY-=2){
      if(ballPositionX<0)break;
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);
    display.display(); 
    delay(20);
    display.clearDisplay();
  }
}
}


void loop() {

  
  if(start == true){
    //display.fillRect(count, 45, 30, 2, 1);//sütun,satır,dikdortgen genişlik(px),diktortgen uzunluk(px),renk(1,0);
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);
    display.display(); 
    delay(10);
    display.clearDisplay();
    start = false;
  }
  if(ballPositionY >= positionYMax){
    directionY = -1;
  }

  if(ballPositionY <= positionYMin){
    directionY = 1;
  }
  if(ballPositionX >= positionXMax){
    directionX = -1;
  }
  if(ballPositionX <= positionXMin){
    directionX = 1;
  }
    testMoveBall(ballPositionX,ballPositionY,directionX,directionY);
    Serial.print(ballPositionX);
    Serial.print(" - ");
    Serial.println(ballPositionY);

}

  


