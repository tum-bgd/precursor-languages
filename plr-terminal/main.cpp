/*
Precursor Langauges Runtime PCL/R

Implementation for VT100 Terminals.

(c) 2024 M.Werner
*/

#include<stdio.h>
#include<unistd.h>
#include<vector>
#include<string>
#include<iostream>
#include<fstream>

void cls(){
   fputs("\033[2J",stdout);
   fflush(stdout);
}

enum {
   RIGHT,UP,LEFT, DOWN,MAX_ORIENTATION
}Orientation;

int niki_x, niki_y, niki_o;

void draw_niki(){
   fputs("\033[1;37;41m",stdout); // set color
   switch(niki_o){
      case LEFT:
        fputs("\u02c2",stdout);
	break;
      case RIGHT:
        fputs("\u02c3",stdout);
	break;
      case UP:
        fputs("\u02c4",stdout);
	break;
      case DOWN:
        fputs("\u02c5",stdout);
	break;
   }
   fputs("\033[m",stdout); // reset color
}

void draw_arena(int rows, int cols){
    cls();
     for(size_t i=0; i < rows; i++)
     {
        for (size_t j=0; j< cols; j++){

	  // boundary?
	  // the robot
	  if (j==niki_x+1 && i == niki_y+1)
	  {
	      draw_niki();
	  }else // boundary
	    fputc((j==0 || j == cols-1 || i==0 || i == rows-1)?'#':' ',stdout);
	  

	  }

	  fputc('\n',stdout);
     }
     // draw the robot
     

}

/* operation implementation
what does the machine do on the instructions
*/
void turn(){
   niki_o = (niki_o +1) % MAX_ORIENTATION;
}

bool move(){
    switch(niki_o){
      case LEFT: niki_x --; break;
      case RIGHT: niki_x ++; break;
      case UP: niki_y --; break;
      case DOWN: niki_y ++; break;
    }
    // todo: collide
    return true;
}

void wait(){
 usleep(500000);
}

/*
Reading and running a program
*/
std::vector<std::string> source;
void load(std::string filename)
{
   std::ifstream ifs(filename);
   std::string str;
   while (std::getline(ifs, str))
    {
	if (str.size() > 0)
                source.push_back(str);
    }
  std::cout << "Read a file with " << source.size() << " lines" << std::endl;
}

// the fetch loop (e.g., virtual machine)
int vm(){
  int pc=0; // initialize at line 0
  int step_counter = 0; 
  while(true)
  {
     // fetch a line
     if (pc < 0 || pc >= source.size()){
	std::cout << "Program Counter (Line Pointer) outside source! Exit..." << std::endl;
	return step_counter;
     }
     std::string line = source[pc];
     pc ++;

     std::cout << "Execute " << line << std::endl;
     if (line.rfind("MOVE",0)==0)
       move(); // add boolean check
     if (line.rfind("TURN",0)==0)
       turn(); 
     if (line.rfind("HALT",0)==0)
     {
	std::cout << "Machine stopped after " << step_counter << " instructions." << std::endl;
	return step_counter;
     }
     step_counter ++;
     draw_arena(25,25);
     wait();

  }
}


int main(int argc, char **argv)
{
  load(argv[1]); // todo: check and errors
  wait();wait();
  cls();
  niki_x = niki_y = 0;
  niki_o=RIGHT;
  vm();
/*  draw_arena(25,25);
  wait();
  turn();
  draw_arena(25,25);
*/
return 0;
}
