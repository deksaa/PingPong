#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
MPU6050 accelgyro(0x68);//Creating MPU6050 object

int16_t ax, ay, az;//They stores sensor data.But we use just one axis(Y axis)

int mean_ay,ay_offset=0;

int ready=0;

int LCDcolor = 1;//Black

int boardHeight = 2;//Heihgt of the board:2 pixel
int boardWidth = 20;//Width of the board
int boardPosition;//Stores the start point of the board

int ballPositionX=28;//Initial point for the ball in X axis
int ballPositionY=24;//Initial point for the ball in Y axis

int directionX=1;//Initial direction for the direction X(1 or -1)
int directionY=1;//Initial direction for the direction Y(1 or -1)

int positionXMin=2;//Minimum limit of the game area on X axis
int positionXMax=80;//Maximum limit of the game area on X axis

int positionYMin=2;//Minimum limit of the game area on Y axis
int positionYMax=44;//Maximum limit of the game area on Y axis

int bounceCounter = 0;
int gameSpeed = 100;
int score = 0;
int bestScore = 0;

void setup() {
  
  Serial.begin(115200);
  Wire.begin();
  accelgyro.initialize();
  
  //Some required initialize for display part
  display.begin();
  display.setContrast(50);
  display.display(); // show splashscreen
  delay(1000);
  display.clearDisplay();
  //End of display Part
  
  TWBR = 24;
  
  //Informing Display   
  displayLcd(accelgyro.testConnection() ? "\n\n  Preparing.." : "Something Wrong");//Verify connection
  delay(1000);
  
  // reset offsets
  accelgyro.setYAccelOffset(ay_offset);

  delay(10);
  displayLcd("\n    Reading \n   Sensor...");
  meanSensors(); 
  
  displayLcd("\n  Calculating \n   Offsets..");
  calibration();
  
  accelgyro.setYAccelOffset(ay_offset);
  
  displayLcd("\n\n Here we go!!");
  delay(600);
    
    //End of informing display
}

void loop() {
  
  testMoveBall();
  
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
      
}


void testMoveBall(){

  //Ball goes straight right-down
  if(directionX == 1 && directionY == 1){
    //End Loop
    for(;ballPositionX<=positionXMax,ballPositionY<=positionYMax;ballPositionX+=2,ballPositionY+=2){
      
      if((ballPositionY >= positionYMax) && isBounced() == false){restartGame();break;}//If ball isn't bouncing the board,restart the game.
         
      if(ballPositionX>=positionXMax || ballPositionY >= positionYMax)break;//If ball touches to border,break the loop.
        
    refreshScreen();
    }//End loop
    
}//End Moving


  //Ball goes straight right-up
  if(directionX == 1 && directionY == -1){
    //End Loop
    for(;ballPositionX<=positionXMax,ballPositionY>=positionYMin;ballPositionX+=2,ballPositionY-=2){
      
      if(ballPositionX>=positionXMax || ballPositionY <= positionYMin)break;//If ball is coming to border,break the loop.
      
    refreshScreen();
  }//End loop
  
}//End Moving


  //Ball goes straight left-down
  if(directionX == -1 && directionY == 1){
    //End Loop
    for(;ballPositionX>=positionXMin,ballPositionY<=positionYMax;ballPositionX-=2,ballPositionY+=2){
      
      if((ballPositionY >= positionYMax) && isBounced() == false){restartGame();break;}//If ball is kissing the board,restart the game.
      
      if(ballPositionX<=positionXMin || ballPositionY >= positionYMax)break;//If ball is coming to border,break the loop.
        
    refreshScreen();
  }//End loop
  
}//End Moving

  //Ball goes straight
  if(directionX == -1 && directionY == -1){
    //End Loop
    for(;ballPositionX>=positionXMin,ballPositionY>=positionYMin;ballPositionX-=2,ballPositionY-=2){
      
      if(ballPositionX<=positionXMin || ballPositionY <= positionYMin)break;//If ball is coming to border,break the loop.
      
    refreshScreen();
  }//End loop
  
}//End Moving

}//End MoveTest Function


void refreshScreen(){
  
    display.fillCircle(ballPositionX,ballPositionY, 3, BLACK);//display.fillCircle(column,row,sizeOfCircle,color)
    refreshBoardPosition();
    display.display();//Display the changes.
    delay(gameSpeed);
    display.clearDisplay();//Clear the display.
  }
  
void restartGame(){

  String strScore = String(score);
  String strBestScore = String(bestScore);
  
  displayLcd("\n\n  Game Over!!\n");
  delay(700);
  displayLcd("\n\n   Score:"+strScore+"\n\n   Best:"+strBestScore);
  delay(3000);
  
  ballPositionX=positionXMin + random(79);
  ballPositionY=positionYMin;
  directionX=1;
  directionY=1;
  gameSpeed = 100;
  score = 0;
}

boolean isBounced(){//Control the touch between board and ball
  
    if(ballPositionX >= boardPosition && ballPositionX <= boardPosition+boardWidth){

      bounceCounter+=5;
      score+=10;

      if(bestScore <= score){
        bestScore = score;
      }
      
      if(bounceCounter >= 10 && gameSpeed >= 20 ){
        gameSpeed-=10;
        bounceCounter=0;
        }
      return true;
    }
    else
      return false;
}

void refreshBoardPosition(){
  
  //Get sensor values
  accelgyro.getAcceleration(&ax, &ay, &az);
  //Draw a board
  boardPosition = map(ay,7000,-7000,0,display.width()-boardWidth);
  display.fillRect(boardPosition, display.height()-boardHeight, boardWidth, boardHeight, LCDcolor);
                  //column, row, rectangle width(px), rectangle height(px), color(1,0);
  
}

void meanSensors(){
  long i=0,buff_ay=0,buff_gy=0;

  while (i<(buffersize+101)){
    // read raw accel/gyro measurements from device
    accelgyro.getAcceleration(&ax, &ay, &az);
    
    if (i>100 && i<=(buffersize+100)){ //First 100 measures are discarded
      buff_ay=buff_ay+ay;
    }
    if (i==(buffersize+100)){
      mean_ay=buff_ay/buffersize;
    }
    i++;
    delay(2); //Needed so we don't get repeated measures
  }
}


void calibration(){
  ay_offset=-mean_ay/8;

  while (1){
    
    accelgyro.setYAccelOffset(ay_offset);

    meanSensors();
    
    Serial.println("...");


    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (ready==1) break;
  }
}


void displayLcd(String str){

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.println(str);
  display.display();
  
}



