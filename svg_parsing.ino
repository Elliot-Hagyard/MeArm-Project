#include "SD.h"
#include "string.h"
#include "Servo.h"
File file;
const int buffer_len = 11;
char buffer[buffer_len];
// path needs to retain info about the path command after parsing
float path_array[6] = {0, 0, 0, 0, 0, 0};
// Store the data from a line.
int line_array[4] = {0, 0, 0, 0};
int circle_array[4] = {0, 0, 0, 0};
int rect_array[4] = {0, 0, 0, 0};
char previous_command;
const uint8_t fidelity = 5;
const uint8_t circle_fidelity = 10;
// Just a char array
// Need to retain this info for lower case commands in svg

Servo middle, left, right, claw; // creates 4 "servo objects"
int middlePin = 11;
int leftPin = 10;
int rightPin = 9;
int clawPin = 6;

double armOneLength = 80;
double armTwoLength = 80;

double xLoc = 0;
double yLoc = 0;

double armHeight = 100;
// double clawLeng = 50;

double middleRot = 90;
double rightMotorRot = 90;
double leftMotorRot = 90;

float initial_points[2];
float previous_bezier[2];

static const short x = 0;
const short y = 1;
const char path[5] = "path";
const char polyline[9] = "polyline";
const char rect[5] = "rect";
const char line[5] = "line";
const char polygon[8] = "polygon";
const char circle[7] = "circle";
const char ellipse[8] = "ellipse";
void polar(double x, double y, bool up);

void setup()
{
    Serial.begin(9600);

    Serial.print("Initializing SD card...");

    if (!SD.begin(10))
    {
        Serial.println("initialization failed!");
        while (1)
            ;
    }
    file = SD.open("glasses.svg", FILE_READ);
    // Apparently, reading the file once before you parse is causing errors
    //  Serial.println("initialization done.");
    //
    //  // open the file. note that only one file can be open at a time,
    //  // so you have to close this one before opening another.
    //  file = SD.open("glasses.svg", FILE_READ);
    //  if (file) {
    //    Serial.println("test.txt:");
    //
    //    // read from the file until there's nothing else in it:
    //    while (file.available()) {
    //      Serial.write(file.read());
    //    }
    //    // close the file:
    //    file.close();
    //  } else {
    //    // if the file didn't open, print an error:
    //    Serial.println("error opening test.txt");
    //  }
}
void draw_line(float newX, float newY)
{
    double newXLoc = xLoc;
    double newYLoc = yLoc;
    while (abs(newX - newXLoc) >= 1 || abs(newY - newYLoc) >= 1)
    {
        double xDif = abs(newX - newXLoc);
        double yDif = abs(newY - newYLoc);

        int xNegative = (newX - newXLoc) < 0 ? -1 : 1;
        int yNegative = (newY - newYLoc) < 0 ? -1 : 1;

        double maxDif = max(xDif, yDif);
        newXLoc += xDif / maxDif * xNegative;
        newYLoc += yDif / maxDif * yNegative;

        polar(newXLoc, newYLoc, true);
        polar(newXLoc, newYLoc, false);
        polar(newXLoc, newYLoc, true);
        polar(newX, newY, true);
        polar(newXLoc, newYLoc, true);

        // polar(xLoc,yLoc,true);
    }

    // polar(test[i][0],test[i][1],true);
    // polar(0,50,true);
}
void line_command()
{
    // Serial.print(F("x1: "));
    // Serial.println(line_array[0]);
    // Serial.print(F("y1: "));
    // Serial.println(line_array[1]);
    // Serial.println();
    // Serial.print(F("x2: "));
    // Serial.println(line_array[2]);
    // Serial.print(F("y2: "));
    // Serial.println(line_array[3]);
    Serial.println("Calling polar with params:");
    Serial.println(line_array[0]);
    Serial.println(line_array[0]);
    Serial.println(false);
    polar(line_array[0], line_array[1], false);
    polar(line_array[2], line_array[3], false);
    polar(xLoc, yLoc, true);
    // polar ); "L," ); line_array[2] ); "," ); line_array[3] ); "\n");
    // goLoc);"e\n");
}
void polar(double x, double y, bool up)
{
    // Updates xLoc and yLoc
    xLoc = x;
    yLoc = y;
    Serial.println(x);
    // Calculate needed length of r
    double r = sqrt(pow(x, 2) + pow(y, 2));
    double rotation = asin(x / r) * (180 / M_PI) + 90;
    r -= 8;
    // r is not to go futher than 130
    Serial.println(r);

    // This is location of the rotational value
    double newMiddleRot = rotation;

    int goLocZ = 0;

    if (up)
    {
        goLocZ = 30;
    }
    if (armHeight != 30 && goLocZ == 30)
    {
        rightMotorRot = 90;
        leftMotorRot = 90;
    }

    // GOLOC FUNCTION
    armHeight = goLocZ;
    double b = armOneLength;
    double c = armTwoLength;

    double aLength = sqrt((r * r) + (goLocZ * goLocZ));

    double topSec = 1 - (aLength * aLength / (2 * b * b));
    double aAngle = (acos(topSec) * (180 / M_PI));

    double bAngle = 180 - aAngle - (asin(goLocZ / aLength) * (180 / M_PI));

    double newRightRot = bAngle;
    double newLeftRot = aAngle;

    double MiddleDif = abs(middleRot - newMiddleRot);
    double RightDif = abs(rightMotorRot - newRightRot);
    double LeftDif = abs(leftMotorRot - newLeftRot);

    int MiddleNegative = (newMiddleRot - middleRot) < 0 ? -1 : 1;
    int RightNegative = (bAngle - rightMotorRot) < 0 ? -1 : 1;
    int LeftNegative = (newLeftRot - leftMotorRot) < 0 ? -1 : 1;

    double maxDif = max(MiddleDif, RightDif);
    maxDif = max(LeftDif, maxDif);

    while (maxDif > 1)
    {

        MiddleDif = abs(middleRot - newMiddleRot);
        RightDif = abs(rightMotorRot - newRightRot);
        LeftDif = abs(leftMotorRot - newLeftRot);

        MiddleNegative = (newMiddleRot - middleRot) < 0 ? -1 : 1;
        RightNegative = (newRightRot - rightMotorRot) < 0 ? -1 : 1;
        LeftNegative = (newLeftRot - leftMotorRot) < 0 ? -1 : 1;

        maxDif = max(MiddleDif, RightDif);
        maxDif = max(LeftDif, maxDif);

        middleRot += (MiddleDif / maxDif) * MiddleNegative;
        middle.write(leftMotorRot);
        rightMotorRot += (RightDif / maxDif) * RightNegative;
        right.write(leftMotorRot);
        leftMotorRot += (LeftDif / maxDif) * LeftNegative;
        left.write(leftMotorRot);
    }

    // delay(20);
    middleRot = newMiddleRot;
    middle.write(middleRot);
    leftMotorRot = newLeftRot;
    left.write(leftMotorRot);
    rightMotorRot = bAngle;
    right.write(rightMotorRot);

    //      std::cout<<"Middle");
    //    std::cout<<middleRot);
    //   std::cout<<"Right");
    //    std::cout<<rightMotorRot);
    //    std::cout<<"Left");
    //    std::cout<<leftMotorRot);
    // delay(100);
}

void circle_command()
{
    Serial.print(F("cx: "));
    Serial.print(circle_array[0]);
    Serial.println();
    Serial.print(F("cy: "));
    Serial.println(circle_array[1]);
    Serial.print(F("rx: "));
    Serial.println(circle_array[2]);
    Serial.print(F("ry: "));
    Serial.print(circle_array[3]);
    Serial.println();
    ;

    const unsigned short r1 = circle_array[2];
    const unsigned short r2 = (circle_array[3] == 0) ? r1 : circle_array[3];
    float i = 0;
    initial_points[0] = circle_array[0] + r1;
    initial_points[1] = circle_array[1];
    while (i < 3.141516 * 2)
    {
        polar(r1 * cos(i) + circle_array[0], r2 * sin(i) + circle_array[1], false);
        i += (3.1415 * 2) / circle_fidelity;
    }
    polar(initial_points[0], initial_points[1], false);
    for (int i = 0; i < 4; i++)
        circle_array[i] = 0;
}
void rect_command()
{
    const short new_x = rect_array[0];
    const short new_y = rect_array[1];
    const short w = rect_array[2];
    const short h = rect_array[3];
    Serial.print(F("x:"));
    Serial.println(x);
    Serial.print(F("y:"));
    Serial.println(y);
    Serial.print(F("w:"));
    Serial.print(w);

    Serial.print(F("h:"));
    Serial.println(h);
    // I think it should be:
    //   Line one (x-w/2, y-h/2) -> (x-w/2,y+h/2)
    //   Line Two (cur_pos[x], cur_pos[y], cur_pos[x]+w, cur_pos[y])
    //   line three (cur_pos[x],cur_pos[y], cur_pos[x], cur_pos[y]-h)
    //   line 4 (cur_pos[x], cur_pos[y], cur_pos[x]-w, cur_pos[y])
    //
    // But the following seems to place the rectangle in the right spot
    line_array[0] = new_x;
    line_array[1] = new_y;
    line_array[2] = new_x;
    line_array[3] = new_y + h;
    line_command();
    line_array[0] = xLoc;
    line_array[1] = yLoc;
    line_array[2] = xLoc + w;
    line_array[3] = yLoc;
    line_command();
    line_array[0] = xLoc;
    line_array[1] = yLoc;
    line_array[2] = xLoc;
    line_array[3] = yLoc - h;
    line_command();
    line_array[0] = xLoc;
    line_array[1] = yLoc;
    line_array[2] = xLoc - w;
    line_array[3] = yLoc;
    line_command();
}
void cubicBezierCurve(float cX1, float cY1, float cX2, float cY2, float finalX, float finalY)
{

    Serial.print(F("Cubic_func"));

    float i = 0;
    float starting_x = xLoc;
    float starting_y = yLoc;
    float new_x;
    float new_y;
    while (i < 1)
    {
        new_x = (1 - i) * (1 - i) * (1 - i) * starting_x + 3 * (1 - i) * (1 - i) * i * cX1 + 3 * (1 - i) * i * i * cX2 + i * i * i * finalX;
        new_y = (1 - i) * (1 - i) * (1 - i) * starting_y + 3 * (1 - i) * (1 - i) * i * cY1 + 3 * (1 - i) * i * i * cY2 + i * i * i * finalY;
        i += 1.0 / fidelity;
        polar(new_x, new_y, false);
    }
    polar(finalX, finalY, false);
    previous_bezier[0] = cX2;
    previous_bezier[1] = cY2;
}
void quadraticBezierCurve(float cX1, float cY1, float finalX, float finalY)
{
    float i = 0;
    float new_x;
    float new_y;
    float starting_x = xLoc;
    float starting_y = yLoc;
    while (i < 1)
    {
        new_x = (1 - i) * (1 - i) * xLoc + 2 * (1 - i) * i * cX1 + i * i * finalX;
        new_y = (1 - i) * (1 - i) * yLoc + 2 * (1 - i) * i * cY1 + i * i * finalY;
        // goLoc << "c," << cur_pos[x] << "," << cur_pos[y] << "\n");
        polar(new_x, new_y, false);
        i += 1.0 / fidelity;
    }
    // Lift the pen u
}
void path_command(char command)
{
    if (int(command) >= int('a'))
    {
        // translate the relative commands to be absolute
        Serial.print(xLoc);
        Serial.print(',');
        Serial.print(yLoc);
        Serial.println();
        if (command != 'v')
            for (short i = 0; i < 3; i++)
            {
                path_array[2 * i] += xLoc;
                path_array[2 * i + 1] = yLoc;
                Serial.print(path_array[2 * i]);
                Serial.println();
                Serial.println(path_array[2 * i + 1]);
            }
        else
        {
            path_array[0] += yLoc;
        }
    }
    switch (command)
    {
    // Placing break statements after the capital letters only since
    case 'm':
    case 'M':
    {
        // Move abs
        // M moves the baseline point
        // lift_pen(); //placeholder function
        // Path array contains 2 points
        // Move x and y. NOTE X AND Y in the svg are X and Z in our
        Serial.print(F("Move"));
        Serial.println();
        Serial.print(F("x"));
        Serial.print(path_array[0]);
        Serial.print(F("'\t'y"));
        Serial.println(path_array[1]);

        // go_to(x,y);
        polar(path_array[x], path_array[y], true);
        initial_points[x] = path_array[x];
        initial_points[y] = path_array[y];
        break;
    }
    case 's':
    case 'S':
    {
        Serial.print("reflected cubic Bezier curve");
        Serial.println();

        {
            float cX1 = 2 * xLoc - previous_bezier[0];
            float cY1 = 2 * yLoc - previous_bezier[1];
            cubicBezierCurve(cX1, cY1, path_array[0], path_array[1], path_array[2],
                             path_array[3]);
        }
        break;
        // Cubic Bezier curve shortend to be reflection of previous.
    }
    case 'c':
    case 'C':
    {
        Serial.print(F("cubic curve"));
        Serial.println();
        ;
        // Path array should contain 6 points
        // Also uses the gloabal var of the cur_pos
        cubicBezierCurve(
            path_array[0],
            path_array[1],
            path_array[2],
            path_array[3],
            path_array[4],
            path_array[5]);

        break;
    }
    case 'q':
    case 'Q':
        // Path array should contain 4 points
        // Also uses the gloabal var of the cur_pos
        Serial.print(F("Quadratic Curve"));
        Serial.println();
        quadraticBezierCurve(
            path_array[0],
            path_array[1],
            path_array[2],
            path_array[3]);
        // quadratic bezier curve
        break;
    case 'z':
    case 'Z':
    {
        // Close path command. I think it just draws a straight line to the iniital point
        // Path array should contain 2 points
        line_array[x] = xLoc;
        line_array[y] = yLoc;
        line_array[x + 2] = initial_points[x];
        line_array[y + 2] = initial_points[y];
        line_command();
        Serial.print(F("Go to inital x and y"));
        Serial.println();
        break;
    }
    case 'h':
    case 'H':
    {
        // Just a horiziontal line should contain one point in path array
        line_array[0] = xLoc; //
        line_array[1] = yLoc;
        line_array[2] = path_array[0];
        line_array[3] = yLoc;
        line_command();
        Serial.print(F("Move across to:"));
        Serial.print(path_array[0]);
        Serial.println();
        ;
        // move_to(path_ar[0], cur_y)
        break;
    }
    case 'l':
    case 'L':
    {
        // contains 2 points in path array
        line_array[x] = xLoc;              // x
        line_array[y] = yLoc;              // y
        line_array[x + 2] = path_array[x]; // x_final
        line_array[y + 2] = path_array[y]; // y final
        line_command();
        Serial.print(F("Move x:"));
        Serial.print(path_array[0]);
        Serial.println();
        Serial.print(F("Move y:"));
        Serial.print(path_array[1]);
        Serial.println();
        ;
        // Line
        break;
    }
    case 'v':
    case 'V':
    {
        // path array should have one point
        line_array[0] = xLoc;          // x
        line_array[1] = yLoc;          // y
        line_array[2] = xLoc;          // x_final
        line_array[3] = path_array[0]; // y final
        line_command();
        Serial.print(F("Move up to:"));
        Serial.print(path_array[0]);
        Serial.println();
        // move_to(cur_x, path_ar[0])
        // Vertiacal line
        break;
    }
    }
    for (int i = 0; i < 6; i++)
    {
        path_array[i] = 0;
    }
    previous_command = command;
}

char get_num_from_quote(int num_array[], const uint8_t index_in_array)
{
    char cur_char = file.read();
    Serial.print(cur_char);
    cur_char = file.read();
    Serial.print(cur_char);
    cur_char = file.read();
    while (cur_char == '\t' or cur_char == '\n')
        cur_char = file.read();
    Serial.print(cur_char);
    Serial.println();
    ;
    short sign = 1;
    if (cur_char == '-')
    {
        cur_char = file.read();

        sign = -1;
    }
    // THis logic should probably be in a function, but will need to revisit that
    while (cur_char != '"')
    {
        if (cur_char == '.')
        {
            cur_char = file.read();
            Serial.print(cur_char);
            if (cur_char > '5')
            {
                num_array[index_in_array] += 1;
            }
            while (cur_char != '"')
            {
                // Choping off the decimal for now
                cur_char = file.read();
                Serial.print(cur_char);
            }
        }
        else
        {
            num_array[index_in_array] *= 10;
            num_array[index_in_array] += (int(cur_char) - int('0')) * sign;
            cur_char = file.read();
            Serial.print(cur_char);
        }
    }
    Serial.println();
    return cur_char;
}

void readLine()
{
    for (int i = 0; i < buffer_len; i++)
        buffer[i] = '\0';
    for (int i = 0; i < 4; i++)
        line_array[i] = 0;
    int idx = 0;

    while (file.peek() == '\t' or file.peek() == '\n' or file.peek() == ' ')
    {
        file.read();
    }
    idx = 0;
    Serial.print(file.peek());
    while ((file.peek() != ' ' and file.peek() != '\n') and idx <= 10 and file.available())
    {
        buffer[idx] = file.read();
        Serial.print(buffer[idx]);
        idx += 1;
    }
    Serial.println();

    if (strstr(buffer, path))
    {
        char car = file.read();
        while (car != '>')
        {
            if (car == 'd')
            {
                car = file.read();
                if (car == '=')
                {
                    for (int i = 0; i < 2; i++)
                    {
                        car = file.read();
                        Serial.print(car);
                    }
                    while (car != '"')
                    {
                        Serial.print(car);
                        Serial.println(" here");
                        char command = car;
                        car = file.read();

                        short i = 0;

                        bool first = true;
                        bool neg = false;
                        // Assume that the cordinates for the 'M' go till the next character, since that will store the values
                        while ((int(car) < int('A')) and car != '"')
                        {
                            Serial.print(car);
                            if (car == '.')
                            {
                                car = file.read();
                                while (car == '\n' or car == '\t')
                                    car = file.read();
                                Serial.print(car);
                                neg = false;
                                while (int(car) <= int('9') and int(car) >= '0')
                                {
                                    car = file.read();
                                    while (car == '\n' or car == '\t')
                                        car = file.read();
                                    Serial.print(car);
                                }
                            }
                            else
                            {
                                if (car < '0')
                                {
                                    neg = false;
                                    // Ascii values for - and , are less than ascii value for '0'
                                    if (car == '-')
                                    {
                                        neg = true;

                                        if (not first)
                                        {
                                            i++;
                                        }
                                        first = false;
                                        // If there is a negative sign, read the first number in as a negative number
                                    }
                                    else
                                    {
                                        i++;
                                    }
                                    car = file.read();
                                    while (car == '\n' or car == '\t')
                                        car = file.read();
                                    Serial.print(car);
                                }
                                path_array[i] *= 10;
                                if (neg)
                                {
                                    path_array[i] -= (int(car) - int('0'));
                                }
                                else
                                {
                                    path_array[i] += (int(car) - int('0'));
                                }
                                car = file.read();
                                while (car == '\n' or car == '\t')
                                    car = file.read();
                                Serial.print(car);
                            }
                            first = false;
                        }
                        Serial.println();
                        for (int a = 0; a < 6; a++)
                        {
                            Serial.print(a);
                            Serial.print('\t');
                            Serial.print(path_array[a]);
                            Serial.println();
                            ;
                        }
                        path_command(command);
                    }
                }
            }
            car = file.read();
            Serial.print(car);
        }
        car = file.read();
        Serial.print(car);
    }
    else if (strstr(buffer, polyline) or strstr(buffer, polygon))
    {
        for (int i = 0; i < 4; i++)
        {
            line_array[i] = 0;
        }
        int initial_x = 0;
        int initial_y = 0;
        char cur_char;
        for (int i = 0; i < 6; i++)
            cur_char = file.read();

        //      //TODO polyline parsing
        // Just going to treat it as a line array
        //<polyline fill="none" stroke="#000000" stroke-width="0.2835" stroke-miterlimit="10" points="224,128.2 224,135 224,128.2
        //				230.9,128.2 230.9,119 224,119 224,112.3 224,119 			"/>
        while (cur_char != '>')
        {
            if (cur_char == 'p')
            {
                Serial.println();
                for (int i = 0; i < 6; i++)
                {
                    cur_char = file.read();
                    Serial.print(cur_char);
                }
                Serial.println();
                if (cur_char == '=')
                {
                    cur_char = file.read();
                    Serial.print(cur_char);
                    while (cur_char == '\t' or cur_char == '\n')
                        cur_char = file.read();
                    Serial.print(cur_char);
                    cur_char = file.read();
                    Serial.print(cur_char);
                    Serial.println();
                    short idx_line_arr = 0;
                    while (cur_char != '"')
                    {
                        short sign = 1;
                        if (cur_char == '-')
                        {
                            sign = -1;

                            cur_char = file.read();
                            Serial.print(cur_char);
                        }
                        while (cur_char != ' ' and cur_char != ',' and cur_char != '\t')
                        {
                            if (cur_char == '.')
                            {
                                while (cur_char != ',' and cur_char != ' ')
                                {
                                    // Choping off the decimal for now
                                    cur_char = file.read();
                                    while (cur_char == '\t' or cur_char == '\n')
                                        cur_char = file.read();
                                    Serial.print(cur_char);
                                }
                            }
                            else
                            {
                                line_array[idx_line_arr] *= 10;
                                line_array[idx_line_arr] += (int(cur_char) - int('0')) * sign;
                                cur_char = file.read();
                                while (cur_char == '\t' or cur_char == '\n')
                                    cur_char = file.read();
                                Serial.print(cur_char);
                            }
                        }
                        cur_char = file.read();

                        while (cur_char == '\t' or cur_char == '\n')
                            cur_char = file.read();
                        idx_line_arr += 1;
                        if (idx_line_arr > 3)
                        {
                            if (initial_x == 0 and initial_y == 0)
                            {
                                initial_x = line_array[0];
                                initial_y = line_array[1];
                            }
                            line_command();
                            line_array[0] = line_array[2];
                            line_array[1] = line_array[3];
                            line_array[2] = 0;
                            line_array[3] = 0;
                            idx_line_arr = 2;
                        }
                    }
                    if (strstr(buffer, polygon))
                    {
                        line_array[2] = initial_x;
                        line_array[3] = initial_y;
                        line_command();
                    }
                }
            }
            cur_char = file.read();
            Serial.print(cur_char);
        }
        cur_char = file.read();
    }
    else if (strstr(buffer, line))
    {
        // NOTE: line has to be after polyline in how I've configured this
        //<line fill="none" stroke="#000000" stroke-width="2.834646e-02" stroke-miterlimit="10" x1="72.4" y1="766" x2="72.4" y2="752"/>
        // Only care about d flag for n
        // move buffer idx up  by <line.
        char cur_char = file.read();
        while (cur_char != '>')
        {
            // Instead of indexing idx through the array, it might make sense to create the
            if (cur_char == 'x' or cur_char == 'y')
            {
                // creating this value to index the lineArray to correctly place the x1, y1, x2, y2 vals
                // This is done to avoid having seprate nested if statements for both x and y and seprate things for x1 x2
                const short x_or_y = int(cur_char) - int('x');
                cur_char = file.read();
                Serial.print(cur_char);
                while (cur_char == '\t' or cur_char == '\n')
                    cur_char = file.read();
                if (cur_char == '1' or cur_char == '2')
                {
                    // if the value is 2, set equal to 2, else should be 0
                    const short one_or_two = (int(cur_char) - int('1')) * 2;
                    // move past the quotes
                    cur_char = get_num_from_quote(line_array, (one_or_two + x_or_y));
                    // If this is three, then it should be y2
                    Serial.print(cur_char);
                    if (x_or_y + one_or_two == 3)
                    {
                        // TODO create an actual line function
                        line_command();
                    }
                }
            }
            cur_char = file.read();
        }
    }
    else if (strstr(buffer, circle) or strstr(buffer, ellipse))
    {
        for (unsigned short i = 0; i < 3; i++)
        {
            circle_array[i] = 0;
        }
        // TODO Circle
        //<circle class="st0" cx="70" cy="51.6" r="22.7"/>
        char cur_char = file.read();
        Serial.print(cur_char);
        while (cur_char != '>')
        {
            if (cur_char == 'c')
            {
                cur_char = file.read();
                Serial.print(cur_char);
                if (cur_char == 'x' or cur_char == 'y')
                {
                    if (file.peek() == '=')
                    {
                        short array_loc = int(cur_char) - int('x');
                        cur_char = get_num_from_quote(circle_array, array_loc);
                    }
                }
            }
            if (cur_char == 'r')
            {
                int x_or_y = 0;
                bool is_ellipse = 0;
                if (file.peek() == 'x' or file.peek() == 'y')
                {
                    cur_char = file.read();
                    x_or_y = int(cur_char == 'y');
                    is_ellipse = true;
                }
                Serial.print(cur_char);
                if (file.peek() == '=')
                {
                    cur_char = get_num_from_quote(circle_array, 2 + x_or_y);
                    Serial.print(cur_char);
                    if (not(is_ellipse) or x_or_y)
                    {
                        circle_command();
                    }
                }
            }
            cur_char = file.read();
            Serial.print(cur_char);
        }
    }
    else if (strstr(buffer, rect))
    {
        // TODO rect parsing
        for (int i = 0; i < 4; i++)
        {
            rect_array[i] = 0;
        }
        char cur_char = file.read();
        Serial.print(cur_char);
        //<rect x="18.5" y="19.5" fill="#FFFFFF" width="272.5" height="23"/
        while (cur_char != '>')
        {
            if (cur_char == 's')
            {
                for (int i = 0; i < 8; i++)
                {
                    cur_char = file.read();
                    Serial.print(cur_char);
                }
                Serial.println();
            }
            if (cur_char == 'x' or cur_char == 'y')
            {
                Serial.print(cur_char);
                Serial.println();
                ;

                cur_char = get_num_from_quote(rect_array, int(cur_char == 'y'));
                Serial.print(cur_char);
            }
            else if (cur_char == 'w' or cur_char == 'h')
            {
                // height
                // width
                int w_or_h = int('h' == cur_char);
                for (int i = 0; i < 4 + w_or_h; i++)
                {
                    cur_char = file.read();
                    Serial.print(cur_char);
                    while (cur_char == '\t' or cur_char == '\n')
                        cur_char = file.read();
                }
                Serial.println();
                if (file.peek() == '=')
                {

                    // Code is starting to get alittle silly at 2 AM
                    cur_char = get_num_from_quote(rect_array, w_or_h + 2);
                    Serial.print(cur_char);
                    if (w_or_h)
                    {
                        rect_command();
                    }
                }
            }
            cur_char = file.read();
            while (cur_char == '\t' or cur_char == '\n')
                cur_char = file.read();
            Serial.print(cur_char);
        }
        file.read();
        Serial.print(cur_char);
    }
    else if (strstr(buffer, "<g>") or strstr(buffer, "</g>"))
    {
        for (uint8_t i = 0; i < i++; i++)
        {
            buffer[i];
        }
        return;
    }
    else
    {
        while (file.read() != '>' and file.available())
        {
        }
    }
    Serial.println();
    return;
}

void polar(double x, double y, bool up)
{
    // Updates xLoc and yLoc
    xLoc = x;
    yLoc = y;

    // Calculate needed length of r
    float r = sqrt(pow(x, 2) + pow(y, 2));
    r -= 8;
    // r is not to go futher than 130
    if (r > 150)
    {
        r = 150;
    }

    double rotation;

    // Calculate needed rotation of the middle
    // If x is 0 then the rotation is equal to 0
    if (x != 0)
    {
        // 90 <-> -90
        rotation = asin(x / r) * 180 / PI;
    }
    else
    {
        rotation = 0;
    }

    // This is location of the rotational value
    double newMiddleRot = rotation + 90;

    int goLocZ = -13;

    if (r < 45)
    {
        r = 45;
    }

    if (r <= 40)
    {
        Serial.println("Lv 1");
        goLocZ = -8;
    }
    else if (r <= 50)
    {
        Serial.println("Lv 2");
        goLocZ = -9;
    }
    else if (r <= 60)
    {
        Serial.println("Lv 3");
        goLocZ = -10;
    }
    else if (r <= 80)
    {
        Serial.println("Lv 4");
        goLocZ = -20;
    }
    else
    {
        Serial.println("Lv 5");
        goLocZ = -20;
    }

    if (up)
    {
        goLocZ = 30;
    }

    if (armHeight != 30 && goLocZ == 30)
    {
        right.write(90);
        rightMotorRot = 90;
        leftMotorRot = 90;
        left.write(90);
        delay(300);
    }

    // GOLOC FUNCTION
    armHeight = goLocZ;
    double b = armOneLength;
    double c = armTwoLength;

    double aLength = sqrt((r * r) + (goLocZ * goLocZ));

    double topSec = (pow(b, 2)) + (pow(c, 2)) - pow(aLength, 2);
    double total = topSec / (2 * b * c);
    double aAngle = (acos(total) * (180 / PI));

    double bTotal = (pow(c, 2) + pow(aLength, 2) - pow(b, 2)) / (2 * c * aLength);
    double bAngle = 180 - (acos(bTotal) * (180 / PI)) - (asin(goLocZ / aLength) * (180 / PI));

    double newRightRot = bAngle;
    double newLeftRot = 90 - (rightMotorRot - aAngle);

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
        newLeftRot = 90 - (rightMotorRot - aAngle);

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

        delay(20);
        // delay(20);
    }

    middle.write(newMiddleRot);
    left.write(newLeftRot);
    right.write(bAngle);

    middleRot = newMiddleRot;
    leftMotorRot = newLeftRot;
    rightMotorRot = bAngle;

    //      Serial.print("Middle");
    //    Serial.println(middleRot);
    //   Serial.print("Right");
    //    Serial.println(rightMotorRot);
    //    Serial.print("Left");
    //    Serial.println(leftMotorRot);
    // delay(100);

    // Test if the arm is to go up or down
    if (up == true)
    {
        // Go from current armHeight to top
        for (int i = armHeight; i < 100; i += 50)
        {
            if (i > 100)
            {
                i == 100;
            }
            // goLoc(r,i);
            // delay(100);
        }
    }
    else
    {
        // Go from current armHeight to bottom
        for (int i = armHeight; i > 0; i -= 50)
        {

            if (i < 0)
            {
                i == 0;
            }
            // goLoc(r,i);
            // delay(100);
        }
    }
    if (up == true)
    {
        // goLoc(r,100);
    }
    else
    {
        // goLoc(r,0);
    }
    // delay(100);
}

void loop()
{
    while (file.available())
    {
        readLine();
        file.close();
    }
}
