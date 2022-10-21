#include <SPI.h>
#include <SD.h>
#include<string.h>
//Buffer of arbitrary len. Just don't want an svg line that is too long
char buffer[300];
//path needs to retain info about the path command after parsing
char path_array[8];
//Store the data from a line.
char line_array[4];
//Just a char array
char* draw_types[6];
//Need to retain this info for lower case commands in svg
float cur_pos[2]; 
char begining_of_string[10];
File svg_file;
void setup() {
  // put your setup code here, to run once:
  svg_file = SD.open("some_text_file",FILE_READ);
  //C++ hates me
  draw_types[0] = "path";
  draw_types[1] = "polyline";
  draw_types[2] = "rect";
  draw_types[3] = "line";
  draw_types[4] = "polygon";
  draw_types[5] = "circle";
}

int readLineIntoBuffer(){
  int idx = 0;
  while(svg_file.available()){
    //Return wehn you encounter a newline
    if (svg_file.peek() != '\n'){
        buffer[idx] = svg_file.read();
        idx+=1;
    }
    else{
      return idx;
    }
  }
}
void parse_line(int size_of_line){
  //I'm creating the parsing assuming that it is the svg 1.1 tiny
  //Size of line is supposed to avoid issues with needing to reset the buffer each time, might be smarter to reset each time
  //This will have some inscrutable bug I am sure
  /*This function is parsing a line of the svg, treating it as a character array. Code makes a lot of assumptions about
   * file structure so it may be a little inscrutable (on top of my really messy code)
   * The line should be stored in buffer, and idx reprents the index of the character in the array we are looking at
   * Becasue we don't know the len of the any particular part of the line, we have to use while loops to iterate till we find somehting
   * TODO: Test Line parsing, Finish Path, Add parsing for other types?
   */
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
  if(strstr(begining_of_string,draw_types[0])){
    //Should parse <path fill-rule="evenodd" fill="none" stroke="#000000" stroke-miterlimit="10" d="M73.5,31.5c0.3,1.9,0.5,6-2,9
    //c-4.4,5.1-14.3,3.3-20-1c-4.2-3.1-9.1-9.8-7-16c2.8-8.4,16.7-11.1,19-8c3.9,5.1-23.1,27.1-18,35c4,6.3,24.2-2.7,28,4
    //c3,5.3-6.8,15.4-6,16c1,0.8,17.6-13.6,14-21c-1.9-3.9-9.4-5.7-13-3c-4.1,3-0.6,10-3,15c-2.9,6.2-15.1,10-49,5"/>
     idx+=5;
     while(idx < size_of_line){
      idx+=1;
      if(buffer[idx] == 'd' and buffer[idx+1] == '='){
        //Move past the '=' and '"'
        
        idx+=2;
        while(buffer[idx != '"']){

          switch(buffer[idx]){
            case 'M':
              short i = 0;
              //Assume that the cordinates for the 'M' go till the next character, since that will store the values
              while(int(buffer[idx]) < int('a')){
                if(buffer[idx] < int('0')){
                  //Ascii values for - and , are less than ascii value for '0'
                  i+=1;
  
                  if(buffer[idx] == '-'){
                    //If there is a negative sign, read the first number in as a negative number
                    path_array[i]=-1;
                  }
                  idx+=1;
                }
                path_array[i]*=10;
                if(path_array[i] >1){
                  path_array[i]+=int(buffer[idx]);
                }
                else{
                  path_array[i]-=int(buffer[idx]);
                  }
                idx+=1;
              }
             //placeholder move_command();
             break;
           case 'c':
           case 'C':
           case 'l'
           case 'L'
           case ''
          }}
      }
     }}
  else if(strstr(begining_of_string,draw_types[1])){
     //TODO polyline parsing

  }
  else if(strstr(begining_of_string,draw_types[2])){
     //TODO rect parsing
  }
  else if(strstr(begining_of_string,draw_types[3])){
    //NOTE: line has to be after polyline in how I've configured this
    //<line fill="none" stroke="#000000" stroke-width="2.834646e-02" stroke-miterlimit="10" x1="72.4" y1="766" x2="72.4" y2="752"/>
     //Only care about d flag for n
     //move buffer idx up  by <line.
     idx+=5;
     while(idx < size_of_line){
        //Instead of indexing idx through the array, it might make sense to create the 
        idx++;
        if(buffer[idx] == 'x' or buffer[idx] == 'y'){
          //creating this value to index the lineArray to correctly place the x1, y1, x2, y2 vals
          //This is done to avoid having seprate nested if statements for both x and y and seprate things for x1 x2
          const short x_or_y = int(buffer[idx]) - int('y');
          idx++;
          if(buffer[idx] == '1' or buffer[idx] == '2'){
            // if the value is 2, set equal to 2, else should be 0
            const short one_or_two = (int(buffer[idx])-int('2'))*2;
            //move past the quotes
            idx+=3;
            //I'll need to come back to add a better name
            short neg = 1; 
            if(buffer[idx] == '-'){
              idx+=1;
              neg = -1;
            }
            while(buffer[idx]!='"'){
              line_array[x_or_y+one_or_two]*=10*neg;
              line_array[x_or_y+one_or_two]+=int(buffer[idx]); 
            }
            //If this is three, then it should be y2
            if(x_or_y + one_or_two == 3){
              //TODO create a 
              line_command();
            }
        }
      }
  }}
  else if(strstr(begining_of_string,draw_types[4])){
    //TODO polygon
  }
  else if (strstr(begining_of_string, draw_types[5])){
    //TODO Circle
  }
  else {
    //Then the line is somehting else
    return;
  }
}

void line_command(){
  //TODO: Read from the global line_array and then make the mearm actually draw it
}
void move_command(){
  //Need to check svg docs for what 'M' means
}
void loop() {
  // put your main code here, to run repeatedly:
  
} 
