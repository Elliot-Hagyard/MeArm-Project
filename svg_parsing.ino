#include "SD.h"
#include "string.h"

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
const short fidelity = 5;
const short circle_fidelity = 10;
// Just a char array
// Need to retain this info for lower case commands in svg
float cur_pos[2];
float initial_points[2];
float previous_bezier[2];
char begining_of_string[10];

static const short x = 0;
const short y = 1;
const char path[5] = "path";
const char polyline[9] = "polyline";
const char rect[5] = "rect";
const char line[5] = "line";
const char polygon[8] = "polygon";
const char circle[7] = "circle";
const char ellipse[8] = "ellipse";

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

void line_command()
{
    // TODO: Read from the global line_array and then make the mearm actually draw it
    Serial.print("x1: ");
    Serial.println(line_array[0]);
    Serial.print("y1: ");
    Serial.println(line_array[1]);
    Serial.println();
    Serial.print("x2: ");
    Serial.println(line_array[2]);
    Serial.print("y2: ");
    Serial.println(line_array[3]);
    // goLoc ); "L," ); line_array[0] ); "," ); line_array[1] ); "\n");
    // goLoc ); "L," ); line_array[2] ); "," ); line_array[3] ); "\n");
    // goLoc ); "L," ); line_array[0] ); "," ); line_array[1] ); "\n");
    // goLoc ); "L," ); line_array[2] ); "," ); line_array[3] ); "\n");
    // goLoc);"e\n");
    cur_pos[1] = line_array[3];
    cur_pos[0] = line_array[2];
}

void circle_command()
{
    Serial.print("cx: ");
    Serial.print(circle_array[0]);
    Serial.println();
    Serial.print("cy: ");
    Serial.println(circle_array[1]);
    Serial.print("rx: ");
    Serial.println(circle_array[2]);
    Serial.print("ry: ");
    Serial.print(circle_array[3]);
    Serial.println();
    ;

    int r1 = circle_array[2];
    int r2 = (circle_array[3] == 0) ? r1 : circle_array[3];
    float i = 0;
    initial_points[0] = circle_array[0] + r1;
    initial_points[1] = circle_array[1];
    while (i < 3.141516 * 2)
    {
        cur_pos[x] = r1 * cos(i) + circle_array[0];
        cur_pos[y] = r2 * sin(i) + circle_array[1];
        // goLoc << "c," << cur_pos[x] << "," << cur_pos[y] << "\n");
        i += (3.1415 * 2) / circle_fidelity;
    }
    // goLoc<<"c,"<<initial_points[0]<<","<<initial_points[1]<<"\n");
    // goLoc<<"e\n");
    cur_pos[0] = initial_points[0];
    cur_pos[1] = initial_points[1];
    for (int i = 0; i < 4; i++)
        circle_array[i] = 0;
}
void rect_command()
{
    const short x = rect_array[0];
    const short y = rect_array[1];
    const short w = rect_array[2];
    const short h = rect_array[3];
    Serial.print("x:");
    Serial.println(x);
    Serial.print("y:");
    Serial.println(y);
    Serial.print("w:");
    Serial.print(w);

    Serial.print("h:");
    Serial.println(h);
    // Line one (x-w/2, y-h/2) -> (x-w/2,y+h/2)
    // Line Two (cur_pos[x], cur_pos[y], cur_pos[x]+w, cur_pos[y])
    // line three (cur_pos[x],cur_pos[y], cur_pos[x], cur_pos[y]-h)
    // line 4 (cur_pos[x], cur_pos[y], cur_pos[x]-w, cur_pos[y]);
    line_array[0] = x;
    line_array[1] = y;
    line_array[2] = x;
    line_array[3] = y + h;
    line_command();
    line_array[0] = cur_pos[0];
    line_array[1] = cur_pos[1];
    line_array[2] = cur_pos[0] + w;
    line_array[3] = cur_pos[1];
    line_command();
    line_array[0] = cur_pos[0];
    line_array[1] = cur_pos[1];
    line_array[2] = cur_pos[0];
    line_array[3] = cur_pos[1] - h;
    line_command();
    line_array[0] = cur_pos[0];
    line_array[1] = cur_pos[1];
    line_array[2] = cur_pos[0] - w;
    line_array[3] = cur_pos[1];
    line_command();
}
void cubicBezierCurve(float cX1, float cY1, float cX2, float cY2, float finalX, float finalY)
{

    Serial.print("Here");

    float i = 0;
    int index = 0;
    float starting_x = cur_pos[x];
    float starting_y = cur_pos[y];
    while (i < 1)
    {
        cur_pos[x] = (1 - i) * (1 - i) * (1 - i) * starting_x + 3 * (1 - i) * (1 - i) * i * cX1 + 3 * (1 - i) * i * i * cX2 + i * i * i * finalX;
        cur_pos[y] = (1 - i) * (1 - i) * (1 - i) * starting_y + 3 * (1 - i) * (1 - i) * i * cY1 + 3 * (1 - i) * i * i * cY2 + i * i * i * finalY;
        i += 1.0 / fidelity;
        // goLoc << "c," << cur_pos[x] << "," << cur_pos[y] << "\n");
        index += 1;
    }
    cur_pos[0] = finalX;
    cur_pos[1] = finalY;
    // goLoc<<"c,"<<cur_pos[x]<<","<<cur_pos[y]<<"\n");
    previous_bezier[0] = cX2;
    previous_bezier[1] = cY2;
    // goLoc<<"e\n");
}
void quadraticBezierCurve(float cX1, float cY1, float finalX, float finalY)
{
    float i = 0;
    float starting_x = cur_pos[x];
    float starting_y = cur_pos[y];
    while (i < 1)
    {
        cur_pos[x] = (1 - i) * (1 - i) * cur_pos[x] + 2 * (1 - i) * i * cX1 + i * i * finalX;
        cur_pos[y] = (1 - i) * (1 - i) * cur_pos[y] + 2 * (1 - i) * i * cY1 + i * i * finalY;
        // goLoc << "c," << cur_pos[x] << "," << cur_pos[y] << "\n");

        i += 1.0 / fidelity;
    }
    // goLoc << "e\n");
}
void path_command(char command)
{
    if (int(command) >= int('a'))
    {
        // translate the relative commands to be absolute
        Serial.print(cur_pos[0]);
        Serial.print(',');
        Serial.print(cur_pos[1]);
        Serial.println();
        if (command != 'v')
            for (int i = 0; i < 6; i++)
            {
                path_array[i] += cur_pos[i % 2];
                Serial.print(path_array[i]);
                Serial.println();
            }
        else
        {
            path_array[0] += cur_pos[1];
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
        Serial.print("Move");
        Serial.println();
        Serial.print("x");
        Serial.print(path_array[0]);
        Serial.print('\t');
        Serial.print("y");
        Serial.print(path_array[1]);
        Serial.println();
        // go_to(x,y);
        cur_pos[x] = path_array[x];
        cur_pos[y] = path_array[y];
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
            float cX1 = 2 * cur_pos[0] - previous_bezier[0];
            float cY1 = 2 * cur_pos[1] - previous_bezier[1];
            cubicBezierCurve(cX1, cY1, path_array[0], path_array[1], path_array[2],
                             path_array[3]);
        }
        break;
        // Cubic Bezier curve shortend to be reflection of previous.
    }
    case 'c':
    case 'C':
    {
        Serial.print("cubic curve");
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
        Serial.print("Quadratic Curve");
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
        line_array[x] = cur_pos[x];
        line_array[y] = cur_pos[y];
        line_array[x + 2] = initial_points[x];
        line_array[y + 2] = initial_points[y];
        line_command();
        Serial.print("Go to inital x and y");
        Serial.println();
        ;
        break;
    }
    case 'h':
    case 'H':
    {
        // Just a horiziontal line should contain one point in path array
        line_array[0] = cur_pos[x]; //
        line_array[1] = cur_pos[y];
        line_array[2] = path_array[0];
        line_array[3] = cur_pos[y];
        line_command();
        Serial.print("Move across to:");
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
        line_array[x] = cur_pos[x];        // x
        line_array[y] = cur_pos[y];        // y
        line_array[x + 2] = path_array[x]; // x_final
        line_array[y + 2] = path_array[y]; // y final
        line_command();
        Serial.print("Move x:");
        Serial.print(path_array[0]);
        Serial.println();
        Serial.print("Move y:");
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
        line_array[0] = cur_pos[x];    // x
        line_array[1] = cur_pos[y];    // y
        line_array[2] = cur_pos[x];    // x_final
        line_array[3] = path_array[0]; // y final
        line_command();
        Serial.print("Move up to:");
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

char get_num_from_quote(int num_array[], int index_in_array)
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
    while ((file.peek() != ' ' and file.peek() != '\n') and idx <= 10 and file)
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
        for (int i = 0; i < 3; i++)
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
        Serial.print("<g> fuck");
        return;
    }
    else
    {
        while (file.read() != '>' and file)
        {
        }
    }
    Serial.println();
    return;
}

void loop()
{
    while (file)
    {
        readLine();
    }
    file.close();
    while (1)
        ;
}
