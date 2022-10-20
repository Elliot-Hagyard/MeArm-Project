#include <Servo.h> 
 
Servo middle, left, right, claw ;  // creates 4 "servo objects"
 
void setup() 
{ 
  Serial.begin(9600);
  middle.attach(11);  // attaches the servo on pin 11 to the middle object
  left.attach(10);  // attaches the servo on pin 10 to the left object
  right.attach(9);  // attaches the servo on pin 9 to the right object
  claw.attach(6);  // attaches the servo on pin 6 to the claw object
} 

 double rightMoterRot = 90;
 double leftMoterRot = 0;
void loop() 
{ 

  middle.write(180); // sets the servo position according to the value(degrees)
  //left.write(70); // does the same
  //right.write(90); // and again
  claw.write(100); // yes you've guessed it
  //rightMoterRot = 120;
  //right.write(rightMoterRot);


 
  for(double i = 0; i < 90; i = i +.1)
  {
     goLoc(i,82.6);
     delay(3);
  }
   delay(100);
    for(double i = 90; i > 0; i = i -.1)
  {
     goLoc(i,82.6);
     delay(3);
  }
  
  delay(1000); // doesn't constantly update the servos which can fry them
  
}
double armOneLength = 82.6;
double armTwoLength = 82.6;
void goLoc(double x, double y)
{
  if(x> 165)
  {
     x = 165;
  }
  if(x<20)
  {
    x = 20;
  }
  double b = armOneLength;
  double c = armTwoLength;
  
  double aLength = sqrt((x*x)+(y*y));
   
  double topSec = (pow(b,2))+(pow(c,2))-pow(aLength,2);
  double total = topSec/(2*b*c);
  double aAngle = (acos(total)*(180/PI));

  double bTotal = (pow(c,2) + pow(aLength,2) - pow(b,2)) / (2*c*aLength);
  double bAngle = 180 - (acos(bTotal)*(180/PI)) - (asin(y/aLength)*(180/PI));

  Serial.print("aAngle: ");
  Serial.print(aAngle);
  
  Serial.print(" bAngle: ");
  Serial.println(bAngle);

  rightMoterRot = bAngle;
  right.write(rightMoterRot);
  
  topRot(aAngle);
}
void topRot(double i)
{
  double extra = 0;
  if(rightMoterRot>90)
  {
    extra = 2;
  }
    if(rightMoterRot<70)
    {
      //extra = 5;
    }
    leftMoterRot = 90 - (rightMoterRot-i)+extra;
    left.write(leftMoterRot);
}

void rightMoter(double i)
{
  Serial.print("Right: ");
  Serial.println(i);
  if(i < 45)
  {
    left.write(45);
  }
  else
  {
    left.write(i);
  }
}

void leftMoter(double i)
{
   Serial.print("Left: ");
   Serial.println(i);
  if(i > 100)
  {
    left.write(100);
  }
  else
  {
    left.write(i);
  }
}
