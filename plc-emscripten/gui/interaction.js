/*
Globals
*/
var world;
var niki_x,niki_y,niki_o;
var trace=[];
/*Create an empty grid with a prefix and return it as a string*/
function createGridHTML( r,  c,prefix="img")
  {
      console.log(r,c)
      p = (100 / c); 
      q = (100 / r); 
      if (q<p) p = q; // we always scale squares on width 
      ret="";
      for( i=0; i < r; i++){
	  ret += "<div class=\"row\">\n";
	  for ( j=0; j<c; j++){
              ret += "<img width=\""+p.toString()+"%\" id=\""+prefix+"-"+i.toString()+"-"+j.toString()+"\" src=\"gfx/empty.png\">";
	      
	  }
	  ret += "\n</div>";
	  
      }
      return ret;
  }

/* Manage Globals */
function  resetTrace(){
    trace = [];
}


/* Initialize or Update Visuals*/

  function rotate(x, degrees)
  {
      x.css({
      'transform': 'rotate(' + degrees + 'deg)',
      '-ms-transform': 'rotate(' + degrees + 'deg)',
      '-moz-transform': 'rotate(' + degrees + 'deg)',
      '-webkit-transform': 'rotate(' + degrees + 'deg)',
      '-o-transform': 'rotate(' + degrees + 'deg)'
    }); 
  }
  function unrotate(x)
  {
      x.css({
      'transform': '',
	  '-ms-transform':'',
	  '-moz-transform': '',
	  '-webkit-transform':'',
	  '-o-transform': ''
    }); 
  }


function setImageFromChar(prefix,r,c,ch){
    //console.log("setImage prefix="+prefix);
      image = $("#"+prefix+"-"+r.toString()+"-"+c.toString())
      switch(ch)
      {
	  case '#': unrotate(image);   image.attr('src',"gfx/blocked.png"); break;
	  case 'S': rotate(image,90);  image.attr('src',"gfx/robot.png"); break;
	  case 'N': rotate(image,-90); image.attr('src',"gfx/robot.png"); break;
	  case 'E': rotate(image,0);   image.attr('src',"gfx/robot.png"); break;
	  case 'W': rotate(image,180); image.attr('src',"gfx/robot.png"); break;
	  case 'X':unrotate(image);   image.attr('src',"gfx/goal.png"); break;
	  default:  unrotate(image); image.attr('src',"gfx/empty.png"); break;
      }

  }


function loadGridFromEditor(editor, container, prefix="img"){
      // load the textarea grid into a variable and initialize a fresh grid
      theworld = $(editor).val().split("\n").filter(row=>row.length > 0);
      
      $(container).html(createGridHTML(theworld.length, theworld[0].length,prefix));
      for (i=0; i < theworld.length; i++)
      {
	  theworld[i] = theworld[i].split(''); // turn into array
          for (j=0; j < theworld[0].length; j++)
	  {
	      setImageFromChar(prefix, i,j,theworld[i][j]);
	      // remove niki
	      if (theworld[i][j] == 'E' || theworld[i][j] == 'N' || theworld[i][j] == 'S' || theworld[i][j] == 'W'){
		
		  theniki=[i,j,theworld[i][j]];
		  theworld[i][j]=' '
		
	      }
	  }
      }
    return {
	"world": theworld,
	"niki": theniki
    }
  }

  function updateGridFromGlobals(prefix){
      // an update to niki can only update niki and its surroundings. Javascript will
      // silently ignore wrong values, so the following works even out of bounds
      for (i=niki_y-1; i <= niki_y+1; i++)
	  for (j=niki_x-1; j <= niki_x +1; j++){
	      if (i >= 0 && i < world.length && j >= 0 && j < world[0].length)
		  setImageFromChar(prefix,i,j,world[i][j]);
	  }
      //console.log("setting", niki_y,niki_x,niki_o, prefix);
      setImageFromChar(prefix, niki_y,niki_x,niki_o);
  }



/*NIKI Actions*/
  /*
    Visualization and Application of Operations
  */

  function walkable(y,x)
  {
      ch = world[y][x];
      if (ch == ' ' || ch == '*' || ch == 'X')
	  return true;
      return false;
  }

function front_occupied(y,x,o)
{
    switch(o){
      case 'W': x --; break;
      case 'E': x ++; break;
      case 'N': y --; break;
      case 'S': y ++; break;
    }
    return (!walkable(y,x));
}

  function turn(){
      if (niki_o == 'S') niki_o='W'
      else if (niki_o == 'W') niki_o='N'
      else if (niki_o == 'N') niki_o='E'
      else if (niki_o == 'E') niki_o='S';
      trace.push("TURN");
      updateGridFromGlobals("worldpreview-int");
  }
  function move(){
      if (niki_o == 'S') niki_y++;
      if (niki_o == 'N') niki_y--;
      if (niki_o == 'E') niki_x++;
      if (niki_o == 'W') niki_x--;
      trace.push("MOVE");
      updateGridFromGlobals("worldpreview-int");

    if (world.length != 0){
      if (world[niki_y][niki_x] == 'X')
      {
	  modalTextfieldMessage("Maze solved by finding a goal in " + trace.length.toString() + " steps. <BR/> Well done!<BR/><BR/>Trace:<BR/>"+ trace.join("<BR/>"));
	  vm_state="STOPPED"
      }
      if (!walkable(niki_y,niki_x)) // bounds check!
        return false;
    }
    return true;
  }

/* Compiler */

function do_compile(){
    src = $('#source').val()
    compiled = compile(src);
    if (compiled.startsWith("SYNTAX ERROR")){
	alert(compiled);
    }else{
	$('#assembly').val(compiled);
	document.getElementById('btnTabMachineCode').click();
	
    }
}



function interactive_move(){
      if(!move()) modalTextfieldMessage('You crashed');
  }
/* Message*/
  function hideMessage(){
	  $("#message").css('display','none'); 
      }
  
  function modalTextfieldMessage(s)
  {
      disableKeyboardMode();
      $("#message").css('display',''); 
      $("#messagecontent").html(s);
  }
  function showTrace(){
      modalTextfieldMessage(trace.join('<BR/>'));
  }


/* IO Handler */
function disableKeyboardMode()
{
}

function onBtnShowWorld(){
    
    w = loadGridFromEditor("#world","#worldpreview-world","worldeditorpreview");
    loadGridFromEditor("#world","#worldpreview-source","worldpreview-source");
    loadGridFromEditor("#world","#worldpreview-assembly","worldpreview-asm");
    loadGridFromEditor("#world","#world-interactive","worldpreview-int");
    // set globals
    world = w.world;
    niki_y = w.niki[0];
    niki_x = w.niki[1];
    niki_o = w.niki[2];
    
}

/* VM Globals */
var source;
var stack=[];
var pc, step_counter, cpu_flag;
var vm_message="";
var vm_state;

/*utility: find label*/
function getLabelRow(label){
    
    for (i=0; i< source.length; i++)
	if (label == source[i])
	    return i;
    return -1;
}

// async loop  VM
function step(){
    if (vm_state == "STOPPED")
    {
	return; // any pending timeout after stop is ignored
    }
    
     // fetch a line
     if (pc < 0 || pc >= source.length){
	 modalTextfieldMessage("Program Counter (Line Pointer) outside source! Exit... (pc=" + pc.toString() +  ")");
	return step_counter;
     }
    console.log("PC="+pc.toString());
    line = source[pc];
    console.log("Source Line: " + line);
    pc ++;
    
    tokens = line.split(" "); 

    statemachine = {
	"MOVE": function (t){
	    if (!move()){
		vm_state="COLLISION";
		vm_message="Niki collided with world!";
		return false;
	    }
	    return true;
	}, // MOVE
	"TURN": function (t){
	    turn();
	    return true;
	}, 
	"HALT": function (t){
	    vm_state="STOPPED"
	    vm_message="Machine stopped from instruction HOLD on line " + pc.toString();
	    return false; // stops machine?! make it more positive
	},
	"JMP": function(t) {
	    console.log(`In JMP with ${t}`);
	    row = getLabelRow(t[1]+":")
	    if (row < 0) {
		vm_state="ABORTED";
		vm_message="Machine tried to jump to non-existing label "+label;
		return false;
	    }else{
		pc=row+1
		return true;
	    }   
	},
	"JNZ": function(t){
	    if (cpu_flag) return statemachine["JMP"](t); 
	    return true;
	},
	"JZ": function(t){
	    console.log(`JZ with flag ${cpu_flag}`);
	    if (!cpu_flag) return statemachine["JMP"](t);
	    return true;
	},
	"CALL": function(t){
	    stack.push(pc);
	    row = getLabelRow(t[1]+":")
	    if (row < 0) {
		vm_state="ABORTED";
		vm_message="Machine tried to call non-existing function "+t[1];
		return false;
	    }else{
		pc=row+1
		return true;
	    }   
	},
	"RET": function(t){
	    lno = stack.pop();
	    console.log(`RET TO ${lno}`)
	    pc = lno;
	    return true; // TODO: detect and catch stack underflow and overflow
	},
	"LOADFB": function(t){
	    cpu_flag=front_occupied(niki_y, niki_x,niki_o);
	    return true;
	},
	"LOADHI": function(t){
            cpu_flag=(world[niki_y][niki_x] == '*'); 
	    return true;
	}
	
    }

    // labels are ignored.
    if (tokens[0].endsWith(":")){
	console.log("Label detected: "+line);
	setTimeout(step,0); // no delay
	
	return;
    }
    // all other must be valid instructions
    if (tokens[0] in statemachine){
	console.log("Detected " + tokens[0]);
	state = statemachine[tokens[0]](tokens);
        step_counter ++;
	 
	if (state){
	    // continue
	    setTimeout(step,250);
	}else{
	    modalTextfieldMessage(`VM State: ${vm_state}<BR/>VM Message: ${vm_message}`);	    
	    console.log("Check if failed or success or clean terminate");
	}
    }else{
	console.log("UNIMPLEMENTED; STILL CONTINUING FURTHER; SHOULD SET GLOBAL ERROR CONDITION OR CALL OUT");
    }


/////////////////////////////////
//     if (line.rfind("PICK",0)==0)
//       if (!pick())
//       {
//	  std::cout << "Pick failed" << std::endl;
//	  exit (-1);
//       }; // add boolean check
//     if (line.rfind("DEPO",0)==0)
//       if (!deposit())
//       {
//	  std::cout << "Deposit failed" << std::endl;
//	  exit (-1);
//       }; 
//     if (line.rfind("TURN",0)==0)
//       turn(); 
//     if (line.rfind("HALT",0)==0)
//     {
//	std::cout << "Machine stopped after " << step_counter << " instructions." << std::endl;
//	return step_counter;
//     }
//     if (line.rfind("LOADFB",0)==0)
//     {
//        cpu_flag=occupied(niki_o); // in niki_o
//	//std::cout << "Loading Sensor front_blocked: " << cpu_flag << std::endl; 
//     }
//     if (line.rfind("LOADHI",0)==0)
//     {
//        cpu_flag=(world[niki_y][niki_x] == '*'); // in niki_o
//	//std::cout << "Loading Sensor front_blocked: " << cpu_flag << std::endl; 
//     }
//     if (step_limit != 0 && step_counter >= step_limit){
//	  std::cout << "Maximal steps reached. Exiting" << std::endl;
//	  exit (4);
//     }
//     draw_arena(message);
//     fflush(stdout);
//     wait();
//
//  }
//



}

function vm_run()
{
    vm_state="RUNNING";
    source=$("#assembly").val().split("\n");
    stack=[];
    pc=0;
    step_counter=0;
    cpu_flag=false;
    console.log(source);
    step();
}
/*
MAIN
*/
  $(document).ready(function (e) {
      console.log("ready");
      $("#btnShowWorld").click(onBtnShowWorld);
    $("#btnCompile").click(do_compile); 
      $("#btnMove").click(interactive_move);
      $("#btnTurn").click(turn);
      $("#btnDismissMessage").click(hideMessage);
      $("#btnRun").click(vm_run);
      $("#btnShowTrace").click(showTrace);
      $("#btnRestart").click(function(){
	  resetTrace();
	  onBtnShowWorld(); // load all grids and place niki
      });
      hideMessage();

  });
