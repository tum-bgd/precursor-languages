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
#ifndef DEBUG
   fputs("\033[2J",stdout);
   fflush(stdout);
#endif
}

enum {
   RIGHT,UP,LEFT, DOWN,MAX_ORIENTATION
}Orientation;

int niki_x, niki_y, niki_o;
int step_limit=0;
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
bool walkable(int y,int x){
  char ch = world[y][x];
  if (ch == ' ' || ch == '*' || ch == 'X')
    return true;
  return false;
}

bool occupied(int orientation){
    int x = niki_x, y = niki_y;
    switch(orientation){
      case LEFT: x --; break;
      case RIGHT: x ++; break;
      case UP: y --; break;
      case DOWN: y ++; break;
    }
    if (world.size() != 0)
      if (!walkable(y,x))
        return true; // occupied
    return false; // default non-occup
    
}
bool pick(){
  if (world[niki_y][niki_x] == '*'){
    world[niki_y][niki_x] = ' ';
    return true;
  }else
    return false;
}
bool deposit(){
  if (world[niki_y][niki_x] == ' '){
    world[niki_y][niki_x] = '*';
    return true;
  }else
    return false;
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
      if (!walkable(niki_y,niki_x)) // bounds check!
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
  bool cpu_flag=false; // jumps depend on this
  while(true)
  {
     // fetch a line
     if (pc < 0 || pc >= source.size()){
	std::cout << "Program Counter (Line Pointer) outside source! Exit... (pc=" << pc <<")" << std::endl;
	return step_counter;
     }
     std::string line = source[pc];
     #ifdef DEBUG
     std::cout << "Executing <" << line << ">" << std::endl;
     #endif
     pc ++;
     if (line.rfind("JMP",0)==0){
        bool found=false;
        auto label=line.substr(4);
	label += ":";
	#ifdef DEBUG
	std::cout << "JUMPING TO: <" << label<<">" << std::endl;
	#endif
	for (size_t i=0; i < source.size(); i++)
	  if (label == source[i])
	  {
	      pc=i+1; // skip label
	      found = true;
	  }
	  if (!found){
	    std::cout << "Jump to invalid label: " << label <<std::endl;
	    exit(-1);
	  }
     }
     if (line.rfind("JNZ",0)==0){
        if (cpu_flag != 0) //JNZ
	{
	  bool found=false;
        auto label=line.substr(4);
	label += ":";
	for (size_t i=0; i < source.size(); i++)
	  if (label == source[i])
	  {
	      pc=i+1;found=true; // skip label
	  }
       	  if (!found){
	    std::cout << "Jump to invalid label: " << label <<std::endl;
	    exit(-1);
	  }

	  }
     }
     if (line.rfind("JZ",0)==0){
        if (cpu_flag == 0) //JZ
	{
	  bool found=false;
	auto label=line.substr(3);
	label += ":";
        //std::cout << "JZ TO: <" << label<<">" << std::endl;
	for (size_t i=0; i < source.size(); i++)
	  if (label == source[i])
	  {
	      pc=i+1; found=true;// skip label
	  }
       	  if (!found){
	    std::cout << "Jump to invalid label: " << label <<std::endl;
	    exit(-1);
	  }
	  }
     }
     if (line.rfind("MOVE",0)==0)
       if (!move())
       {
	  std::cout << "Collision" << std::endl;
	  exit (-1);
       }; // add boolean check
     if (line.rfind("PICK",0)==0)
       if (!pick())
       {
	  std::cout << "Pick failed" << std::endl;
	  exit (-1);
       }; // add boolean check
     if (line.rfind("DEPO",0)==0)
       if (!deposit())
       {
	  std::cout << "Deposit failed" << std::endl;
	  exit (-1);
       }; 
     if (line.rfind("TURN",0)==0)
       turn(); 
     if (line.rfind("HALT",0)==0)
     {
	std::cout << "Machine stopped after " << step_counter << " instructions." << std::endl;
	return step_counter;
     }
     if (line.rfind("LOADFB",0)==0)
     {
        cpu_flag=occupied(niki_o); // in niki_o
	//std::cout << "Loading Sensor front_blocked: " << cpu_flag << std::endl; 
     }
     if (line.rfind("LOADHI",0)==0)
     {
        cpu_flag=(world[niki_y][niki_x] == '*'); // in niki_o
	//std::cout << "Loading Sensor front_blocked: " << cpu_flag << std::endl; 
     }
     step_counter ++;
     if (step_limit != 0 && step_counter >= step_limit){
	  std::cout << "Maximal steps reached. Exiting" << std::endl;
	  exit (4);
     }
     draw_arena();
     fflush(stdout);
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
	    case 'p': if (!pick()) die("Niki tried to pick, but there was nothing.");;
	              break;
	    case 'd': if (!deposit()) die("Niki tried to deposit, but the cell was nonempty");;
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
  while ((c = getopt (argc, argv, "if:w:t:l:")) != -1)
    switch (c)
      {
      case 'l':
        step_limit = std::atoi(optarg);
	break;
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
       std::cerr << "use -i for interactive or -f for running a file, -t for writing a trace and -l for imposing a step limit" << std::endl;
       return -1;
   }

  load(filename); // todo: check and errors
  vm();
  return 0;
}
