#include <Servo.h>

Servo middle, left, right, claw; // creates 4 "servo objects"
int middlePin = 11;
int leftPin = 10;
int rightPin = 9;
int clawPin = 5;

double armOneLength = 80;
double armTwoLength = 80;

double xLoc = 0;
double yLoc = 0;

double armHeight = 100;
// double clawLeng = 50;

double middleRot = 90;
double rightMotorRot = 90;
double leftMotorRot = 90;

void setup()
{
    Serial.begin(9600);
    middle.attach(middlePin); // attaches the servo on pin 11 to the middle object
    delay(500);
    left.attach(leftPin); // attaches the servo on pin 10 to the left object
    delay(500);
    right.attach(rightPin); // attaches the servo on pin 9 to the right object
    delay(500);
    claw.attach(clawPin); // attaches the servo on pin 6 to the claw object
    delay(500);
    claw.write(160);

    middle.write(middleRot);
    right.write(rightMotorRot);
    left.write(leftMotorRot);
    delay(1000);
}

void loop()
{
    claw.write(130);
    int test[][3] = {
        {106, 44, 1},
        {106, 44, 0},
        {173, 44, 0},

        {85, 34, 1},
        {85, 34, 0},
        {85, 66, 0},

        {190, 34, 1},
        {190, 34, 0},
        {217, 64, 0},
    };
    Serial.println("Starting");
    int rows = (sizeof(test) / sizeof(int)) / 3;
    //
    //     Serial.println("Running set 1");
    polar(test[0][0], test[0][1], true);

    Serial.print("Actual: x ");
    Serial.println(xLoc);
    Serial.print("Actual: y ");
    Serial.println(yLoc);
    //    for(int x = -200; x < 200; x += 10)
    //    {
    //      for(int y = 0; y < 200; y += 10)
    //      {
    ////
    //////        Serial.print(is);
    //////         Serial.print(", ");
    //////         Serial.println(j);
    ////       if(!polar(x,y,true))
    ////        delay(100);
    ////       Serial.print(x);
    ////       Serial.print('\t');
    ////       Serial.println(y);
    ////       //polar(i,j,false);
    ////       //polar(i,j,true);
    //////       polar(100,-100,true);
    ////
    //
    //      }
    //    }
    //    delay(200000);
    for (int i = 0; i < rows - 1; i++)
    {
        Serial.println(i);
        Serial.print(test[i][0]);
        Serial.print('\t');
        Serial.println(test[i][1]);
        double newXLoc = xLoc;
        double newYLoc = yLoc;

        Serial.print("Actual: x ");
        Serial.println(newXLoc);
        Serial.print("Actual: y ");
        Serial.println(newYLoc);

        double xDif = abs(test[i][0] - newXLoc);
        double yDif = abs(test[i][1] - newYLoc);

        int xNegative = (test[i][0] - newXLoc) < 0 ? -1 : 1;
        int yNegative = (test[i][1] - newYLoc) < 0 ? -1 : 1;

        double maxDif = max(xDif, yDif);
        while (abs(test[i][0] - newXLoc) >= 1 || abs(test[i][1] - newYLoc) >= 1)
        {

            newXLoc += xDif / maxDif * xNegative;
            newYLoc += yDif / maxDif * yNegative;
            Serial.print("Actual: x ");
            Serial.println(newXLoc);
            Serial.print("Actual: y ");
            Serial.println(newYLoc);
            polar(newXLoc, newYLoc, true);
            polar(newXLoc, newYLoc, false);
            polar(newXLoc, newYLoc, true);
            polar(100, 100, true);
            polar(newXLoc, newYLoc, true);

            // polar(xLoc,yLoc,true);
        }

        // polar(test[i][0],test[i][1],true);
        // polar(0,50,true);
    }

    //
    //       polar(test[i][0],test[i][1],true);
    //       delay(1000);
    //       polar(test[i][0],test[i][1],true);
    //            delay(1000);
    //       polar(test[i][0],test[i][1],true);
    //            delay(1000);
    //       polar(test[i][0],test[i][1],test[i][2]);
    //       polar(test[i][0],test[i][1],true);
    //       polar(1,-100,true);
    //        polar(-100,-100,true);

    //
    //       polar(test[i][0],newYLoc,true);
    //       polar(newXLoc,newYLoc,test[0][2]);
    //       polar(newXLoc,newYLoc,true);
    //       polar(-100,100,true);
    //       polar(newXLoc,newYLoc,true);

    // polar(xLoc,yLoc,true);

    // polar(test[i][0],test[i][1],true);
    // polar(0,50,true);
    delay(10);
    // polar(test[rows-1][0],test[rows-1][1],true);
}

// The will go straight to the location except for
// The hight of the arm and the middle rotating motors.
// They will move to their postion slowly.b
bool polar(double x, double y, bool up)
{
    //  Serial.print("Planned: x ");
    //  Serial.println(x);
    //  Serial.print("Planned: y ");
    //  Serial.println(y);

    // Updates xLoc and yLoc
    xLoc = x;
    yLoc = y;

    x = 145 - x;
    y = 145 - y;
    //  x*=-1;
    //  y*=-1;
    //  Serial.print("Actual: x ");
    //  Serial.println(x);
    //  Serial.print("Actual: y ");
    //  Serial.println(y);

    // Calculate needed length of r
    float r = sqrt(pow(x, 2) + pow(y, 2));

    // r is not to go futher than 130
    if (r > 145)
    {
        Serial.println("MAX");
        return true;

        r = 145;
    }
    if (r < 60)
    {
        Serial.println("MIN");
        return true;
    }

    double rotation = 0;

    // Ca  lculate needed rotation of the middle
    // If x is 0 then the rotation is equal to 0
    if (y != 0)
    {
        rotation = atan(x / y) * 180 / PI;
    }
    else
    {
        rotation = 90 * (x / abs(x));
    }
    //  if(y > 0 && x != 0)
    //  {
    //    rotation = atan(x/y) * 180 /PI;
    //  }

    // This is location of the rotational value
    double newMiddleRot = rotation + 90;
    if (newMiddleRot < 0 || newMiddleRot > 180)
    {
        return true;
    }

    int goLocY = -13;

    if (r < 45)
    {
        r = 45;
    }

    if (r <= 40)
    {
        // Serial.println("Lv 1");
        goLocY = -8;
    }
    else if (r <= 50)
    {
        // Serial.println("Lv 2");
        goLocY = -9;
    }
    else if (r <= 60)
    {
        // Serial.println("Lv 3");
        goLocY = -10;
    }
    else if (r <= 80)
    {
        // Serial.println("Lv 4");
        goLocY = -20;
    }
    else
    {
        // Serial.println("Lv 5");
        goLocY = -20;
    }

    if (up)
    {
        goLocY = 30;
    }

    if (armHeight != 30 && goLocY == 30)
    {
        right.write(90);
        rightMotorRot = 90;
        leftMotorRot = 90;
        left.write(90);
        delay(300);
    }

    // GOLOC FUNCTION
    armHeight = goLocY;
    double b = armOneLength;
    double c = armTwoLength;

    double aLength = sqrt((r * r) + (goLocY * goLocY));

    double topSec = (pow(b, 2)) + (pow(c, 2)) - pow(aLength, 2);
    double total = topSec / (2 * b * c);
    double aAngle = (acos(total) * (180 / PI));

    double bTotal = (pow(c, 2) + pow(aLength, 2) - pow(b, 2)) / (2 * c * aLength);
    double bAngle = 180 - (acos(bTotal) * (180 / PI)) - (asin(goLocY / aLength) * (180 / PI));

    double newRightRot = bAngle;
    double newLeftRot = 90 - (newRightRot - aAngle);

    double MiddleDif = abs(middleRot - newMiddleRot);
    double RightDif = abs(rightMotorRot - bAngle);
    double LeftDif = abs(leftMotorRot - newLeftRot);

    int MiddleNegative = (newMiddleRot - middleRot) < 0 ? -1 : 1;
    int RightNegative = (bAngle - rightMotorRot) < 0 ? -1 : 1;
    int LeftNegative = (newLeftRot - leftMotorRot) < 0 ? -1 : 1;

    double maxDif = max(MiddleDif, RightDif);
    maxDif = max(LeftDif, maxDif);

    while (maxDif > 2)
    {
        claw.write(160);
        // newLeftRot = 90 - (rightMotorRot-aAngle);

        MiddleDif = abs(middleRot - newMiddleRot);
        RightDif = abs(rightMotorRot - bAngle);
        LeftDif = abs(leftMotorRot - newLeftRot);

        MiddleNegative = (newMiddleRot - middleRot) < 0 ? -1 : 1;
        RightNegative = (bAngle - rightMotorRot) < 0 ? -1 : 1;
        LeftNegative = (newLeftRot - leftMotorRot) < 0 ? -1 : 1;

        maxDif = max(MiddleDif, RightDif);
        maxDif = max(LeftDif, maxDif);

        middleRot += (MiddleDif / maxDif) * MiddleNegative;
        middle.write(middleRot);

        rightMotorRot += (RightDif / maxDif) * RightNegative;
        right.write(rightMotorRot);

        leftMotorRot += (LeftDif / maxDif) * LeftNegative;
        left.write(leftMotorRot);

        delay(10);
        // delay(20);
    }

    middle.write(newMiddleRot);
    left.write(newLeftRot);
    right.write(bAngle);

    middleRot = newMiddleRot;
    leftMotorRot = newLeftRot;
    rightMotorRot = bAngle;
    return false;

    //      Serial.print("Middle");
    //    Serial.println(middleRot);
    //   Serial.print("Right");
    //    Serial.println(rightMotorRot);
    //    Serial.print("Left");
    //    Serial.println(leftMotorRot);
    // delay(100);

    // Test if the arm is to go up or down
    //  if(up == true)
    //  {
    //    //Go from current armHeight to top
    //    for(int i = armHeight; i < 100; i+= 50)
    //    {
    //      if(i > 100)
    //      {
    //        i == 100;
    //      }
    //      //goLoc(r,i);
    //      //delay(100);
    //    }
    //  }
    //  else
    //  {
    //    //Go from current armHeight to bottom
    //    for(int i = armHeight; i > 0; i -= 50)
    //    {
    //
    //      if(i < 0)
    //      {
    //        i == 0;
    //      }
    //      //goLoc(r,i);
    //      //delay(100);
    //    }
    //
    //  }
    //  if(up == true)
    //  {
    //    //goLoc(r,100);
    //  }
    //  else
    //  {
    //    //goLoc(r,0);
    //  }
    // delay(100);
}
