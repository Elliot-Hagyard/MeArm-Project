#include <Servo.h> 
 
Servo middle, left, right, claw ;  // creates 4 "servo objects" 
 int middlePin = 11;
 int leftPin = 10;
 int rightPin = 9;
 int clawPin = 6;

 double armOneLength = 82.6;
 double armTwoLength = 82.6;

 double xLoc = 0;
 double yLoc = 0;
 
 double armHeight;
 double clawLeng = 50;
 
 double middleRot = 0;
 double rightMotorRot = 90;
 double leftMotorRot = 0;

void setup() 
{ 
  Serial.begin(9600);
  middle.attach(middlePin);  // attaches the servo on pin 11 to the middle object
  delay(500);
  left.attach(leftPin);  // attaches the servo on pin 10 to the left object
  delay(500);
  right.attach(rightPin);  // attaches the servo on pin 9 to the right object
  delay(500);
  claw.attach(clawPin);  // attaches the servo on pin 6 to the claw object
  delay(500);
  
  middle.write(0);
  
  Serial.println("Start up at 0");
  polar(100,0,true);
  delay(100);
}

void loop() 
{ 

  //middle.write(10); // sets the servo position according to the value(degrees)
  //left.write(70); // does the same
  //right.write(90); // and again
    claw.write(140);
  //rightMoterRot = 120;
  //right.write(rightMoterRot);
    Serial.println("Starting");
    for(int i = 0; i < 100; i += 1)
    {
        goToLocation(100,i,true);
        delay(100);
    }
    delay(1000);
    for(int i = 100; i > 0; i -= 1)
    {
        goToLocation(100,i,true);
        delay(100);
    }
 delay(1000);
    //polar(100,100);
 
  
  delay(1000); // doesn't constantly update the servos which can fry them
  
}

//This will travel to zero and travel back
void goToZero()
{
  double startXLoc = xLoc;
  double startYLoc = yLoc;
  polar(xLoc,yLoc,true);
  
  stepTravel(100,0,true);
  
  stepTravel(startXLoc,startYLoc,true);
  
//  bool done = false;
//  
//  while(!done)
//  {
//    if(xLoc < 100)
//    {
//      xLoc += .5;
//    }
//    else
//    {
//      xLoc -= .5; 
//    }
//
//    if(yLoc < 0)
//    {
//      yLoc +=.5;
//    }
//    else
//    {
//       yLoc -= .5;
//    }
//     polar(xLoc,yLoc,true);
//     delay(10);
//     
//        if(((xLoc -.6) <= 100 && xLoc>100)|| ((xLoc +.6) >= 100 && xLoc<100))
//     {
//     
//       if((yLoc -.6 <= 0 && yLoc>0) || ((yLoc +.6) >= 0 && yLoc<0))
//      {
//    
//        polar(100,0,true);
//        delay(100);
//        polar(startXLoc,startYLoc,true);
//        delay(100);
//        done = true;
//      }
//     }
//  }
}

void goToLocation(float newX, float newY, bool up)
{
  Serial.println("Go To Zero");
  goToZero();
  Serial.println("Back From Zero");
  bool done = false;
  delay(2000);

  stepTravel(newX,newY,up);
  
//  while(!done)
//  {
//
//    if(xLoc < newX)
//    {
//      xLoc += .5;
//    }
//    else
//    {
//      xLoc -= .5;
//    }
//
//    if(yLoc < newY)
//    {
//      yLoc += .5;
//    }
//    else
//    {
//      yLoc -= .5; 
//    }
//     Serial.print("Going To New Location: ");
//     Serial.print(xLoc);
//     Serial.print(", ");
//     Serial.println(yLoc);
//     polar(xLoc,yLoc,true);
//     delay(100);
//     
//     if(((xLoc -.5) <= newX && xLoc>newX)|| ((xLoc +.5) >= newX && xLoc<newX))
//     {
//       if((yLoc -.5 <= newY && yLoc>newY) || ((yLoc +.5) >= newY && yLoc<newY))
//      {
//        polar(newX,newY,false);
//        done = true;
//      }
//     }
//  }
}

//This will slowly travel to the location passed to it
void stepTravel(double newX, double newY, bool up)
{
  bool done = false;
  delay(1000);
  
  while(!done)
  {
    if(xLoc < newX)
    {
      xLoc += .5;
    }
    else
    {
      xLoc -= .5;
    }

    if(yLoc < newY)
    {
      yLoc += .5;
    }
    else
    {
      yLoc -= .5; 
    }
     Serial.print("Going To New Location: ");
     Serial.print(xLoc);
     Serial.print(", ");
     Serial.println(yLoc);
     
     polar(xLoc,yLoc,up);
     delay(100);
     //abs(xLoc-newX) <= .5
     if(((xLoc -.5) <= newX && xLoc>newX)|| ((xLoc +.5) >= newX && xLoc<newX))
     {
        //abs(yLoc-newY) <= .5
        if((yLoc -.5 <= newY && yLoc>newY) || ((yLoc +.5) >= newY && yLoc<newY))
        {
          polar(newX,newY,false);
          done = true;
        }
     }
  }
}

//The will go straight to the location except for
//The hight of the arm and the middle rotating motors.
//They will move to their postion slowly.
void polar(double x, double y, bool up)
{
  //Updates xLoc and yLoc
  xLoc = x;
  yLoc = y;

  //Calculate needed length of r
  float r = sqrt(pow(x,2)+pow(y,2));
  
  double rotation;
  
  //Calculate needed rotation of the middle
  //If x is 0 then the rotation is equal to 0
  if(x != 0)
  {
    rotation = asin(x/r) * 180 /PI;
  }
  else
  {
    rotation = 0;
  }

  //This is location of the rotational value
  double newMiddleRot = rotation + 90;

  //r is not to go futher than 130
  if(r> 130)
  {
    r = 130;
  }

  //This will slowly move to the rotation
  bool done = false;
  while(!done)
  {
    //This will increment to the location
    if(middleRot<newMiddleRot)
    {
      middleRot +=.5;
    }
    else
    {
      middleRot -=.5;
    }
    
    middle.write(middleRot);
    delay(100);
    if(abs(newMiddleRot - middleRot) <= .5)
    {
      middle.write(newMiddleRot);
      delay(100);
      middleRot = newMiddleRot;
      done = true;
    }
  }

  //Test if the arm is to go up or down
  if(up == true)
  {
    //Go from current armHeight to top
    for(int i = armHeight; i < 100; i++)
    {
      goLoc(r,i);
      delay(100);
    }
  }
  else
  {
    //Go from current armHeight to bottom
    for(int i = armHeight; i > 0; i -= 2)
    {
      goLoc(r,i);
      delay(100);
    }

  }
  delay(100);
}

void goLoc(double x, double y)
{
  armHeight = y;
  if(x > 25)
  {
    y = y-20;
  }
  
  if(x> 150)
  {
     x = 150;
  }
  if(x<30)
  {
    x = 30;
  }
  double b = armOneLength;
  double c = armTwoLength;
  
  double aLength = sqrt((x*x)+(y*y));
   
  double topSec = (pow(b,2))+(pow(c,2))-pow(aLength,2);
  double total = topSec/(2*b*c);
  double aAngle = (acos(total)*(180/PI));

  double bTotal = (pow(c,2) + pow(aLength,2) - pow(b,2)) / (2*c*aLength);
  double bAngle = 180 - (acos(bTotal)*(180/PI)) - (asin(y/aLength)*(180/PI));


  rightMotorRot = bAngle;
  Serial.println(rightMotorRot);
  right.write(rightMotorRot);
  
  topRot(aAngle);
  delay(100);
}

//This will set the angle of the top triangle
void topRot(double angle)
{
    leftMotorRot = 90 - (rightMotorRot-angle);
    left.write(leftMotorRot);
}

void rightMotor(double angle)
{
  rightMotorRot = angle;
  if(angle < 45)
  {
    right.write(45);
  }
  else
  {
    right.write(angle);
  }
}

void leftMotor(double angle)
{
  leftMotorRot = angle;
  if(angle > 100)
  {
    left.write(100);
  }
  else
  {
    left.write(angle);
  }
} //Line 364
