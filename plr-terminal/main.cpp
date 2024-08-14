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
#include<algorithm>

/* Linux Interactive*/


#include <termios.h>
struct termios orig_termios;
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw;
  tcgetattr(STDIN_FILENO, &raw);
  raw.c_lflag &= ~(ECHO|ICANON);
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void cls(){
   fputs("\033[2J",stdout);
   fflush(stdout);
}

enum {
   RIGHT,UP,LEFT, DOWN,MAX_ORIENTATION
}Orientation;

int niki_x, niki_y, niki_o;
std::vector<std::string> world;
std::vector<std::string> source;
std::ofstream *pTrace=NULL;



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

void draw_arena(){
    cls();
     for(size_t i=0; i < world.size(); i++)
     {
        for (size_t j=0; j< world[0].size(); j++){
	  // the robot
	  if (j==niki_x && i == niki_y)
	  {
	      draw_niki();
	  }else{
	      	fputc(world[i][j], stdout);
	  }
     }
	  fputc('\n',stdout);
     }
     // draw the robot
     

}

/* operation implementation
what does the machine do on the instructions
*/
std::vector<std::string> trace;

void turn(){
   trace.push_back("TURN");
   if (pTrace != NULL)
      (*pTrace) << "TURN" << std::endl;
   niki_o = (niki_o +1) % MAX_ORIENTATION;
}

bool move(){
    trace.push_back("MOVE");
   if (pTrace != NULL)
      (*pTrace) << "MOVE" << std::endl;
    switch(niki_o){
      case LEFT: niki_x --; break;
      case RIGHT: niki_x ++; break;
      case UP: niki_y --; break;
      case DOWN: niki_y ++; break;
    }
    
    if (world.size() != 0){
      if (world[niki_y][niki_x] == 'X')
      {
	  std::cout << "Maze solved by finding an X in " << trace.size() << "steps" << std::endl;
	  exit (0);
      }
      if (world[niki_y][niki_x] != ' ') // bounds check!
        return false;
    }
    return true;
}

void wait(){
 usleep(100000);
}

/*
Reading and running a program
*/
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

/*load a world*/
void load_world(std::string filename)
{
   std::ifstream ifs(filename);
   std::string str;
   int size=-1;
   while (std::getline(ifs, str))
    {
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.cend());
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.cend());
        if (size == -1) {
           size = str.size();
	}
	if (str.size() != size)
	{
	    std::cerr << "Syntax error: file rows not all of the same length" << std::endl;
	}
	for (int i=0; i<  str.size(); i++)
	{
	    if (str[i] == 'N') {niki_x = i; niki_y = world.size(); niki_o=UP; str[i] = ' ';  };
	    if (str[i] == 'S') {niki_x = i; niki_y = world.size(); niki_o=DOWN; str[i] = ' ';  };
	    if (str[i] == 'E') {niki_x = i; niki_y = world.size(); niki_o=RIGHT; str[i] = ' '; };
	    if (str[i] == 'W') {niki_x = i; niki_y = world.size(); niki_o=LEFT; str[i] = ' '; };
	}
	world.push_back(str);
    }
    std::cout << "Read a world of " << size << "x" << world.size() << std::endl;
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

     if (line.rfind("MOVE",0)==0)
       if (!move())
       {
	  std::cout << "Collision" << std::endl;
	  exit (-1);
       }; // add boolean check
     if (line.rfind("TURN",0)==0)
       turn(); 
     if (line.rfind("HALT",0)==0)
     {
	std::cout << "Machine stopped after " << step_counter << " instructions." << std::endl;
	return step_counter;
     }
     step_counter ++;
     draw_arena();
     wait();

  }
}

void die(std::string reason="unknown reason."){
   std::cout << "Niki diead with reason: " << reason << std::endl;
   disableRawMode();
   exit(-1);
}


int niki_interactive(){
   char key=' ';
    std::cout << "Manual: \n we could add it. Keys: t,m,q\nPress any key...\n";
    enableRawMode();
    key = (char) fgetc(stdin);
    while (key != 'q')
   {
     draw_arena();
     key = (char) fgetc(stdin);
      switch(key)
      {
	    case 'm': if (!move()) die("Niki move collided");
	              break;
	    case 't': turn();
	              break;

      }
 
   }
   return 0;

}

int main(int argc, char **argv)
{
  // flags
  bool interactive = false;
 std::string filename(""),worldname("");
  char c;
  while ((c = getopt (argc, argv, "if:w:t:")) != -1)
    switch (c)
      {
      case 'i':
        interactive=true;
        break;
      case 'f':
        filename = std::string(optarg);
        break;
      case 'w':
        worldname = std::string(optarg);
        break;
      case 't':
        pTrace= new std::ofstream(optarg);
	break;
       
      case '?':
        if (optopt == 'f')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        return -1;;
      }
      
  if (worldname != "")
    load_world(worldname); 

   if (interactive)
      return niki_interactive();
   if (filename == "")
   {
       std::cerr << "use -i for interactive or -f for running a file!" << std::endl;
       return -1;
   }

  load(filename); // todo: check and errors
  vm();
  return 0;
}
