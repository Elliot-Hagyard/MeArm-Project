#include <stdio.h>
#include <fstream>
#include <string.h>
#include <iostream>
std::fstream file;
const int buffer_len = 300;
char buffer[buffer_len];
//path needs to retain info about the path command after parsing
float path_array[8] = {0,0,0,0,0,0,0,0};
//Store the data from a line.
int line_array[4] = {0,0,0,0};
int circle_array[3] = {0,0,0};
int rect_array[4] = {0,0,0,0};
//Just a char array
char* draw_types[6];
//Need to retain this info for lower case commands in svg
float cur_pos[2]; 
float initial_points[2];
char begining_of_string[10];

char path[5]= "path";
char polyline[9] = "polyline";
char rect[5] = "rect";
char line[5] = "line";
char polygon[8] = "polygon";
char circle[7] = "circle";

void line_command(){
  //TODO: Read from the global line_array and then make the mearm actually draw it
  std::cout<<"x1: "<<line_array[0]<<std::endl;
  std::cout<<"y1: "<<line_array[1]<<std::endl;
  std::cout<<"x2: " << line_array[2]<<std::endl;
  std::cout<<"y2: "<<line_array[3]<<std::endl;

}
void circle_command(){
    std::cout<<"cx: "<< circle_array[0]<<std::endl;
    std::cout<<"cy: "<< circle_array[1]<<std::endl;
    std::cout<<"r: "<< circle_array[2]<<std::endl;

}
void rect_command(){
    std::cout<<"x: "<<rect_array[0]<<std::endl;
    std::cout<<"y: "<<rect_array[1]<<std::endl;
    std::cout<<"w: "<<rect_array[2]<<std::endl;
    std::cout<<"h: "<<rect_array[3]<<std::endl;

}
void path_command(char command){
  //TODO Implement specfic behavior for each path type
  if(int(command) < int('A')){
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
      std::cout<<"Move"<<std::endl;
      std::cout<<"x"<<path_array[0]<<'\t'<<"y"<<path_array[1]<<std::endl;
      //go_to(x,y);
      //initial_point[0]=path_array[0]
      //initial_point[1]=path_array[1]
      break;
    }
    case 's':{}
    case 'S':{
      std::cout<<"reflected cubic Bezier curve"<<std::endl;
      break; // FUCK STOP WITH THE TYPES
      //Cubic Bezier curve shortend to be reflection of previous.
      }
    case 'c':{}
    case 'C':{
        std::cout<<"cubic curve"<<std::endl;
      //Is anyhting else needed here?
      //cubic_bezier_curve();
      break;
      }
    case 'q':
    case 'Q':
        std::cout<<"Quadratic Curve"<<std::endl;
      //quadratic bezier curve
      break;
    case 'z':{}
    case 'Z':{
      //Close path command. I think it just draws a straight line to the iniital point
      //goto(inital_points[0],initial_points[1])
      std::cout<<"Go to inital x and y"<<std::endl;
      break;
    }
    case 'h':{}
    case 'H':{
      //Just a horiziontal line
        std::cout<<"Move across to:"<<path_array[0]<<std::endl;
      //move_to(path_ar[0], cur_y)
      break;
    }
    case 'l':{}
    case 'L':{
        std::cout<<"Move x:"<<path_array[0]<<std::endl;
        std::cout<<"Move y:"<<path_array[1]<<std::endl;
      //Line
      break;
      }
    case 'v':{}
    case 'V':{
        std::cout<<"Move up to:"<<path_array[0]<<std::endl;
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
  std::cout<<"Staring index "<<idx<<std::endl;
  idx+=1;
  int starting_index = idx;
  bool neg = false;
              //Assume that the cordinates for the 'M' go till the next character, since that will store the values
      while(int(buffer[idx]) < int('A') and buffer[idx] != '"') {
          if (buffer[idx] == '.') {
              idx += 1;
              while (int(buffer[idx]) < int('A') and int(buffer[idx]) > '0') {
                  idx += 1;
              }

          }
          else {
              if (buffer[idx] < '0') {
                  //Ascii values for - and , are less than ascii value for '0'
                  if (buffer[idx] == '-') {
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
                  path_array[i] -= (int(buffer[idx]) - int('0'));
              } else {
                  path_array[i] += (int(buffer[idx]) - int('0'));
              }
              idx += 1;
          }
      }
      for(int a = 0; a< 8; a++){
          std::cout<<a<<'\t';
          std::cout<<path_array[a]<<std::endl;
      }
      std::cout<<std::endl;

  return idx;
  }
int get_num_from_quote(int num_array[], int index_in_array, int index_in_buffer){

    index_in_buffer+=3;
    short sign = 1;
    if(buffer[index_in_buffer] == '-'){
        index_in_buffer+=1;
        sign = -1;
    }
    //THis logic should probably be in a function, but will need to revisit that
    while(buffer[index_in_buffer]!='"'){
        if(buffer[index_in_buffer]=='.'){
            while(buffer[index_in_buffer]!='"'){
                //Choping off the decimal for now
                index_in_buffer+=1;
            }
        }
        else {
            num_array[index_in_array] *= 10;
            num_array[index_in_array] += (int(buffer[index_in_buffer]) - int('0')) * sign;
            index_in_buffer += 1;
        }
    }
    return index_in_buffer;
}
void parse_line(int size_of_line){
  //I'm creating the parsing assuming that it is the svg 1.1 tiny
  //Size of line is supposed to avoid issues with needing to reset the buffer each time, might be smarter to reset each time
  //This will have some inscrutable bug I am sure
  /*This function is parsing a line of the svg, treating it as a character array. Code makes a lot of assumptions about
   * file structure so it may be a little inscrutable (on top of my really messy code)
   * The line should be stored in buffer, and idx reprents the index of the character in the array we are looking at
   * Becasue we don't know the len of the any particular part of the line, we have to use while loops to iterate till we find somehting\
   * Then, assign the values into the path_array
   * Then call a function which gets values from path array and 
   * TODO: Test Line parsing, Finish Path, Add parsing for other types?
   */
  std::cout<<"Lenght of line: "<< size_of_line<<std::endl;
  int idx = 0;

  while(buffer[idx] != '<'){
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
  //Copying the begining of the buffer string over to hopefully speed up the strstr calls
  strncpy(begining_of_string,buffer,10);
  for (int i = 0; i<10; i++){
    std::cout<<begining_of_string[i];
  }
  std::cout<<std::endl;
  if(strstr(begining_of_string,path)){
    //Should parse <path fill-rule="evenodd" fill="none" stroke="#000000" stroke-miterlimit="10" d="M73.5,31.5c0.3,1.9,0.5,6-2,9
    //c-4.4,5.1-14.3,3.3-20-1c-4.2-3.1-9.1-9.8-7-16c2.8-8.4,16.7-11.1,19-8c3.9,5.1-23.1,27.1-18,35c4,6.3,24.2-2.7,28,4
    //c3,5.3-6.8,15.4-6,16c1,0.8,17.6-13.6,14-21c-1.9-3.9-9.4-5.7-13-3c-4.1,3-0.6,10-3,15c-2.9,6.2-15.1,10-49,5"/>
     while(idx < size_of_line){
      std::cout<<buffer[idx];
      
      if(buffer[idx] == 'd' and buffer[idx+1] == '='){
        //Move past the '=' and '"'
        idx+=3;
        while(buffer[idx] != '"'){
          char command = buffer[idx];
          idx = parse_path_command(idx);
          path_command(command);
         }
        }
        idx+=1;
       }
     }
//   else if(strstr(begining_of_string,draw_types[1])){
//      //TODO polyline parsing

//   }
   else if(strstr(begining_of_string,rect)){
      //TODO rect parsing
      for(int i =0; i < 4; i++) {
          rect_array[i] = 0;
      }
      //<rect x="18.5" y="19.5" fill="#FFFFFF" width="272.5" height="23"/
      while(idx < size_of_line){
          if(buffer[idx] == 'x' or buffer[idx] == 'y' and buffer[idx+1] =='='){
              idx = get_num_from_quote(rect_array,int(buffer[idx] == 'y'),idx);
          }
          else if(buffer[idx] == 'w' or buffer[idx] == 'h'){
              int w_or_h = int('h' == buffer[idx]);
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
     //move buffer idx up  by <line.
     while(idx < size_of_line){
        //Instead of indexing idx through the array, it might make sense to create the 
        idx++;
        if(buffer[idx] == 'x' or buffer[idx] == 'y'){
          //creating this value to index the lineArray to correctly place the x1, y1, x2, y2 vals
          //This is done to avoid having seprate nested if statements for both x and y and seprate things for x1 x2
          const short x_or_y =  int(buffer[idx])-int('x');
          idx++;
          if(buffer[idx] == '1' or buffer[idx] == '2'){
            // if the value is 2, set equal to 2, else should be 0
            const short one_or_two = (int(buffer[idx])-int('1'))*2;
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
//   else if(strstr(begining_of_string,draw_types[4])){
//     //TODO polygon
//   }
   else if (strstr(begining_of_string, circle)){
       for(int i = 0; i < 3; i++){
           circle_array[i] = 0;
       }
     //TODO Circle
     //<circle class="st0" cx="70" cy="51.6" r="22.7"/>
        while(idx < size_of_line){
            if(buffer[idx] == 'c'){
                idx+=1;
                if(buffer[idx] == 'x' or buffer[idx] == 'y' and buffer[idx+1] == '=' ){
                    short array_loc = int(buffer[idx])-int('x');
                    idx = get_num_from_quote(circle_array, array_loc, idx);
                }

            }
            if(buffer[idx] == 'r' and  buffer[idx+1] == '=' ){
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
    for(int i =0; i < buffer_len; i++)
        buffer[i]='\0';
    for(int i=0; i <4; i++)
        line_array[i]=0;
    int idx = 0;
    while(true){
        //Return wehn you encounter a newline
        if (file.peek() != '>' and file.peek() != EOF){
            if(file.peek() == '\n' or file.peek()=='\t'){
                //Get rid of newlines between svg commands
                file.get();
            }
            else {
                file.get(buffer[idx]);
                idx += 1;
            }
        }
        else{
            //avoid an inf loop, need to read \n for next iteration
            file.get(buffer[idx]);
            idx+=1;
            return idx;
        }
  }
}

int main(void){
    file.open("/Users/elliothagyard/Documents/ISTA303/glasses.svg",std::fstream::in);
    while( not file.eof()){
    int len_of_line = readLine();
    std::cout<<len_of_line;
    for(int i = 0; i < len_of_line; i++){
        std::cout<<buffer[i];
    }
    std::cout<<std::endl;
    parse_line(len_of_line);
    }
    file.close();
}