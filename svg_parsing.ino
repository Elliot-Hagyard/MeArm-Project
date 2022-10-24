#include "SD.h"
#include "string.h"

File file;
const int str_buffer_len = 250;
char str_buffer[str_buffer_len];
//path needs to retain info about the path command after parsing
float path_array[8];
//Store the data from a line.
int line_array[4];
int circle_array[3];
int rect_array[4];
//Just a char array
char* draw_types[6];
//Need to retain this info for lower case commands in svg
float cur_pos[2]; 
float initial_points[2];
char begining_of_string[10];
char path[5] = "path";
char polyline[9] = "polyline";
char rect[5] = "rect";
char line[5] = "line";
char polygon[8] = "polygon";
char circle[7] = "circle";

void setup() {
  Serial.begin(9600);

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  file = SD.open("glasses.svg", FILE_READ);
  //Apparently, reading the file once before you parse is causing errors
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

void line_command(){
  //TODO: Read from the global line_array and then make the mearm actually draw it
  Serial.print("x1: ");
  Serial.println(line_array[0]);
  Serial.print("y1: ");
  Serial.println(line_array[1]);
  Serial.print("x2: ");
  Serial.println( line_array[2]);
  Serial.print("y2: ");
  Serial.println(line_array[3]);

}
void circle_command(){
    Serial.print("cx: ");
    Serial.println( circle_array[0]);
    Serial.print("cy: ");
    Serial.println( circle_array[1]);
    Serial.print("r: ");
    Serial.println( circle_array[2]);

}
void rect_command(){
    Serial.print("x: ");
    Serial.print(rect_array[0]);
    Serial.print("y: ");
    Serial.print(rect_array[1]);
    Serial.print("w: ");
    Serial.print(rect_array[2]);
    Serial.print("h: ");
    Serial.print(rect_array[3]);

}
void path_command(char command){
  if(int(command) <= int('a')){
      //translate the relative commands to be absolute
      for(int i =0; i < 8; i++)
        path_array[i]+=cur_pos[i%2];
  }
  switch(command){
    //Placing break statements after the capital letters only since 
    case 'm':
    case 'M':{
      //Move abs
      //M moves the baseline point
      //lift_pen(); //placeholder function
      //Move x and y. NOTE X AND Y in the svg are X and Z in our
      Serial.print("Move");
      Serial.print("x");
      Serial.print(path_array[0]);
      Serial.print('\t');
      Serial.print("y");
      Serial.println(path_array[1]);
      //go_to(x,y);
      //initial_point[0]=path_array[0]
      //initial_point[1]=path_array[1]
      break;
    }
    case 's':{}
    case 'S':{
      Serial.print("reflected cubic Bezier curve");
      break; // FUCK STOP WITH THE TYPES
      //Cubic Bezier curve shortend to be reflection of previous.
      }
    case 'c':{}
    case 'C':{
        Serial.print("cubic curve");
      //Is anyhting else needed here?
      //cubic_bezier_curve();
      break;
      }
    case 'q':
    case 'Q':
        Serial.print("Quadratic Curve");
      //quadratic bezier curve
      break;
    case 'z':{}
    case 'Z':{
      //Close path command. I think it just draws a straight line to the iniital point
      //goto(inital_points[0],initial_points[1])
      Serial.println("Go to inital x and y");
      break;
    }
    case 'h':{}
    case 'H':{
      //Just a horiziontal line
        Serial.print("Move across to:");
        Serial.println(path_array[0]);
      //move_to(path_ar[0], cur_y)
      break;
    }
    case 'l':{}
    case 'L':{
        Serial.print("Move x:");
        Serial.print(path_array[0]);
        Serial.print("Move y:");
        Serial.print(path_array[1]);
      //Line
      break;
      }
    case 'v':{}
    case 'V':{
        Serial.print("Move up to:");
        Serial.print(path_array[0]);
        //move_to(cur_x, path_ar[0])
      //Vertiacal line
      break;
      }
    
    
  }
  for(int i = 0; i < 8; i++){
        path_array[i] = 0;

  }
}

int parse_path_command(int idx){
  short i = 0;
  Serial.print("Staring index ");
  Serial.println(idx);
  idx+=1;
  int starting_index = idx;
  bool neg = false;
              //Assume that the cordinates for the 'M' go till the next character, since that will store the values
      while(int(str_buffer[idx]) < int('A') and str_buffer[idx] != '"') {
          if (str_buffer[idx] == '.') {
              idx += 1;
              while (int(str_buffer[idx]) < int('A') and int(str_buffer[idx]) > '0') {
                  idx += 1;
              }

          }
          else {
              if (str_buffer[idx] < '0') {
                  //Ascii values for - and , are less than ascii value for '0'
                  if (str_buffer[idx] == '-') {
                      neg = true;

                      if(idx != starting_index){
                          i++;
                      }
                      //If there is a negative sign, read the first number in as a negative number
                  }
                  else {
                      i++;
                  }
                  idx++;
              }
              path_array[i] *= 10;
              if (neg) {
                  path_array[i] -= (int(str_buffer[idx]) - int('0'));
              } else {
                  path_array[i] += (int(str_buffer[idx]) - int('0'));
              }
              idx += 1;
          }
      }
      for(int a = 0; a< 8; a++){
          Serial.print(a);
          Serial.print('\t');
          Serial.println(path_array[a]);
      }
      ;

  return idx;
  }
int get_num_from_quote(int num_array[], int index_in_array, int index_in_str_buffer){

    index_in_str_buffer+=3;
    short sign = 1;
    if(str_buffer[index_in_str_buffer] == '-'){
        index_in_str_buffer+=1;
        sign = -1;
    }
    //THis logic should probably be in a function, but will need to revisit that
    while(str_buffer[index_in_str_buffer]!='"'){
        if(str_buffer[index_in_str_buffer]=='.'){
            while(str_buffer[index_in_str_buffer]!='"'){
                //Choping off the decimal for now
                index_in_str_buffer+=1;
            }
        }
        else {
            num_array[index_in_array] *= 10;
            num_array[index_in_array] += (int(str_buffer[index_in_str_buffer]) - int('0')) * sign;
            index_in_str_buffer += 1;
        }
    }
    return index_in_str_buffer;
}
void parse_line(int size_of_line){
  //I'm creating the parsing assuming that it is the svg 1.1 tiny
  //Size of line is supposed to avoid issues with needing to reset the str_buffer each time, might be smarter to reset each time
  //This will have some inscrutable bug I am sure
  /*This function is parsing a line of the svg, treating it as a character array. Code makes a lot of assumptions about
   * file structure so it may be a little inscrutable (on top of my really messy code)
   * The line should be stored in str_buffer, and idx reprents the index of the character in the array we are looking at
   * Becasue we don't know the len of the any particular part of the line, we have to use while loops to iterate till we find somehting\
   * Then, assign the values into the path_array
   * Then call a function which gets values from path array and 
   * TODO: Test Line parsing, Finish Path, Add parsing for other types?
   */
  Serial.print("Lenght of line: ");
  Serial.println( size_of_line);
  int idx = 0;

  while(str_buffer[idx] != '<'){
    //Ignore whitespace at the begining of the line
    idx++;
  }
  //  draw_types[0] = "path";
  //  draw_types[1] = "line";
  //  draw_types[2] = "rect";
  //  draw_types[3] = "polyline";
  //  draw_types[4] = "polygon";
  //  draw_types[5] = "circle";
  //This is probably incredibly slow;
  //Copying the begining of the str_buffer string over to hopefully speed up the strstr calls
  strncpy(begining_of_string,str_buffer,10);
  for (int i = 0; i<10; i++){
    Serial.print(begining_of_string[i]);
  }
  Serial.println();
  if(strstr(begining_of_string,path)){
    //Should parse <path fill-rule="evenodd" fill="none" stroke="#000000" stroke-miterlimit="10" d="M73.5,31.5c0.3,1.9,0.5,6-2,9
    //c-4.4,5.1-14.3,3.3-20-1c-4.2-3.1-9.1-9.8-7-16c2.8-8.4,16.7-11.1,19-8c3.9,5.1-23.1,27.1-18,35c4,6.3,24.2-2.7,28,4
    //c3,5.3-6.8,15.4-6,16c1,0.8,17.6-13.6,14-21c-1.9-3.9-9.4-5.7-13-3c-4.1,3-0.6,10-3,15c-2.9,6.2-15.1,10-49,5"/>
     while(idx < size_of_line){
      Serial.print(str_buffer[idx]);
      
      if(str_buffer[idx] == 'd' and str_buffer[idx+1] == '='){
        //Move past the '=' and '"'
        idx+=3;
        while(str_buffer[idx] != '"'){
          char command = str_buffer[idx];
          idx = parse_path_command(idx);
          path_command(command);
         }
        }
        idx+=1;
       }
     }
  else if(strstr(begining_of_string,polyline) or strstr(begining_of_string,polygon)){
      for(int i = 0; i <4; i++)
          line_array[i] = 0;
      int initial_x = 0;
      int initial_y =0;
//      //TODO polyline parsing
        //Just going to treat it as a line array
        //<polyline fill="none" stroke="#000000" stroke-width="0.2835" stroke-miterlimit="10" points="224,128.2 224,135 224,128.2
      //        230.9,128.2 230.9,119 224,119 224,112.3 224,119       "/>
      while(idx < size_of_line){
          if(str_buffer[idx] == 'p' and str_buffer[idx+6] == '='){
              if(strstr(begining_of_string,polygon)){

              }
              idx+=8;
              short idx_line_arr = 0;
              while(str_buffer[idx] != '"'){

                  short sign  = 1;
                  if(str_buffer[idx] == '-'){
                      sign=-1;
                      idx++;
                  }
                  while(str_buffer[idx]!=' ' and str_buffer[idx]!= ','){
                      if(str_buffer[idx]=='.'){
                          while(str_buffer[idx]!= ',' and str_buffer[idx] != ' '){
                              //Choping off the decimal for now
                              idx+=1;
                          }
                      }
                      else {
                          line_array[idx_line_arr] *= 10;
                          line_array[idx_line_arr] += (int(str_buffer[idx]) - int('0'))*sign;
                          idx += 1;
                      }
                  }
                  idx++;
                  idx_line_arr+=1;
                  if(idx_line_arr > 3){
                      if(initial_x == 0 and initial_y == 0){
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
              if(strstr(begining_of_string,polygon)){
                  line_array[2] = initial_x;
                  line_array[3] = initial_y;
                  line_command();
              }
          }
          idx++;
      }
  }
   else if(strstr(begining_of_string,rect)){
      //TODO rect parsing
      for(int i =0; i < 4; i++) {
          rect_array[i] = 0;
      }
      //<rect x="18.5" y="19.5" fill="#FFFFFF" width="272.5" height="23"/
      while(idx < size_of_line){
          if(str_buffer[idx] == 'x' or str_buffer[idx] == 'y' and str_buffer[idx+1] =='='){
              idx = get_num_from_quote(rect_array,int(str_buffer[idx] == 'y'),idx);
          }
          else if(str_buffer[idx] == 'w' or str_buffer[idx] == 'h'){
              int w_or_h = int('h' == str_buffer[idx]);
              idx+=4+w_or_h;
              //Code is starting to get alittle silly at 2 AM
              idx = get_num_from_quote(rect_array,w_or_h+2, idx);
              if (w_or_h){
                  rect_command();
              }
          }
          idx+=1;
      }
   }
  else if(strstr(begining_of_string,line)){
    //NOTE: line has to be after polyline in how I've configured this
    //<line fill="none" stroke="#000000" stroke-width="2.834646e-02" stroke-miterlimit="10" x1="72.4" y1="766" x2="72.4" y2="752"/>
     //Only care about d flag for n
     //move str_buffer idx up  by <line.
     while(idx < size_of_line){
        //Instead of indexing idx through the array, it might make sense to create the 
        idx++;
        if(str_buffer[idx] == 'x' or str_buffer[idx] == 'y'){
          //creating this value to index the lineArray to correctly place the x1, y1, x2, y2 vals
          //This is done to avoid having seprate nested if statements for both x and y and seprate things for x1 x2
          const short x_or_y =  int(str_buffer[idx])-int('x');
          idx++;
          if(str_buffer[idx] == '1' or str_buffer[idx] == '2'){
            // if the value is 2, set equal to 2, else should be 0
            const short one_or_two = (int(str_buffer[idx])-int('1'))*2;
            //move past the quotes
            idx = get_num_from_quote(line_array,(one_or_two+x_or_y), idx);
            //If this is three, then it should be y2
            if(x_or_y + one_or_two == 3){
              //TODO create an actual line function
              line_command();
            }
        }
      }
  }}

   else if (strstr(begining_of_string, circle)){
       for(int i = 0; i < 3; i++){
           circle_array[i] = 0;
       }
     //TODO Circle
     //<circle class="st0" cx="70" cy="51.6" r="22.7"/>
        while(idx < size_of_line){
            if(str_buffer[idx] == 'c'){
                idx+=1;
                if(str_buffer[idx] == 'x' or str_buffer[idx] == 'y' and str_buffer[idx+1] == '=' ){
                    short array_loc = int(str_buffer[idx])-int('x');
                    idx = get_num_from_quote(circle_array, array_loc, idx);
                }

            }
            if(str_buffer[idx] == 'r' and  str_buffer[idx+1] == '=' ){
                idx = get_num_from_quote(circle_array, 2, idx);
                circle_command();
            }
            idx+=1;
        }
   }
  else {
    //Then the line is somehting else
    return;
  }
}

int readLine()
{
    for(int i =0; i < str_buffer_len; i++)
        str_buffer[i]='\0';
    for(int i=0; i <4; i++)
        line_array[i]=0;
    int idx = 0;
    while(true){
        Serial.flush();
        //Return wehn you encounter a newline
        if (file.peek() != '>'){
            if(file.peek() == '\n' or file.peek()=='\t'){
                //Get rid of newlines between svg commands
                Serial.print(char(file.read()));
                Serial.println("debug");
            }
            else {
                str_buffer[idx] = file.read();
                idx += 1;
            }
        }
        else{
            Serial.print(char(file.peek()));
            //avoid an inf loop, need to read \n for next iteration
            str_buffer[idx]= file.read();
            file.read();
            idx+=1;
            return idx;
        }
  
  }
}


void loop() {
    while(file){
      
    int len_of_line = readLine();
    Serial.println(len_of_line);
    for(int i = 0; i < len_of_line; i++){
        Serial.print(str_buffer[i]);
    }
    Serial.println();
    parse_line(len_of_line);
    }
    file.close();
    while(1);
}

