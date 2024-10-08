/*
Globals
*/

var pllib={
  "programs": {
    "00-sequential.pl1": "void main(){\n  move();\n  turn();\n  eat();\n}\n",
    "01-whiletrue-stmt.pl1": "void main(){\n  while(true)\n    move();\n}\n",
    "02-whiletrue-stmts.pl1": "void main(){\n    while(true){\n    move();\n    turn();\n}\n  \n}\n",
    "book10a.pl1": "/*\nTesting and Introducing IF ELSE\nTwo Blocks\n\n*/\n\nvoid main(){\n   if (front_blocked){\n     turn();\n   }else{\n     move();\n   }\n\n\n}",
    "book10b.pl1": "/*\nTesting and Introducing IF ELSE\nTwo Blocks\n\n*/\n\nvoid main(){\n   if (front_blocked){\n     turn();\n   }else\n     move();\n   \n\n\n}",
    "book10c.pl1": "/*\nTesting and Introducing IF ELSE\nTwo Blocks\n\n*/\n\nvoid main(){\n   if (front_blocked)\n     turn();\n   else\n     move();\n   \n\n\n}",
    "book10d.pl1": "/*\nTesting and Introducing IF ELSE\nTwo Blocks\n\n*/\n\nvoid main(){\n   if (front_blocked)\n     turn();\n   else{\n     move();\n   }\n\n\n}",
    "book11.pl1": "/*\nFunction call\n*/\n\nvoid turnleft(){\n  turn();\n  turn();\n  turn();\n}\n\nvoid main(){\n  turnleft();\n}\n",
    "book12.pl1": "/*\n  Recursion\n*/\n\nvoid walk(){\n   if (front_blocked)\n   {\n\t// prepare for going back\n\tturn();\n\tturn();\n   }else{\n     move();\n     walk(); // function calls itself\n     move();\n   }\n}\n\n\nvoid main(){\n   // recursive loop\n   walk();\n}\n",
    "book13.pl1": "/*\n Test return\n This now walks to the first wall \n*/\n\n\n\nvoid main(){\n   // recursive loop\n   move();\n   return;\n   move();\n}\n",
    "book1.pl1": "/*\nFirst program. This is a multiline comment and can explain the code.\n\nIf you want to compile this program, you should invoke the\nPrecursor Language Compiler (plc) for example as follows:\n\nplc book1.pl1\n\n(c) 2024 M. Werner\n*/\n\nvoid main(){\n   move(); // this will move our robot.\n}\n",
    "book2.pl1": "/*\nProgram to solve world book02.wrl\n\n(c) 2024 M. Werner\n*/\n\nvoid main(){\n   // look right\n   turn(); \n   // move to the wall\n   move();move();move();\n   // look down\n   turn();turn();turn();\n   // move to the wall\n   move();move();move();move();\n   // look left\n   turn();turn();turn();\n   // three steps left\n   move();move();move();\n}\n",
    "book3.pl1": "/*\nConditional Execution - Breaking out of the loop\n\n(c) 2024 M. Werner\n*/\n\nvoid main(){\n  while (true){\n    if (front_blocked) {\n      break;\n      }\n    move();\n  }\n}\n",
    "book4.pl1": "/*\nConditional Execution - Breaking out of the loop\n\n(c) 2024 M. Werner\n*/\n\nvoid main(){\n  while (true){\n    if (front_blocked) \n      break;\n    move();\n  }\n}\n",
    "book5.pl1": "/*\n    Walks infinitely independent start\n    location.\n*/\nvoid main(){  \n  while (true){\n    if (front_blocked)\n    {\n       turn();\n       continue;  \n    }\n    move();\n  }\n}\n",
    "book6.pl1": "/*\n  Walks until obstacle and picks all items\n  along the way\n*/\nvoid main(){\n   while(true)\n   {\n      if(has_item)\n        pick();\n      if (front_blocked)\n        break;\n      move();\n   }\n\n}\n",
    "book7.pl1": "/*\n  Walks until obstacle and picks all items\n  along the way\n*/\nvoid main()\n{\n   // we assume to be bottom-left looking right!\n   while(true)  // go upwards\n   {\n      while(true) // this loop goes and eats to the right\n      {\n         if(has_item)\n           pick();\n         if (front_blocked)\n           break;\n         move();\n      }\n      // now, we should be right, looking right, all food eaten\n      turn();\n      // we are looking up, let us see if there is more work to do and a new\n      // scanline!\n      if (front_blocked)\n         break;\n      move();\n      \n      turn();\n      while(true){ // go back without eating\n        if (front_blocked)\n\t  break;\n\t  move();\n     }\n     // we should be now one line up, but entirely to the left. Let us look right\n     turn(); \n     turn();\n     }\n}\n",
    "book8.pl1": "/*\nA conditional loop\n\n*/\nvoid main()\n{\n   // we assume to be bottom-left looking right!\n   while(front_blocked)  // go upwards\n   {\n     move();\n   }\n}\n",
    "book9.pl1": "/*\nThe right-hand rule maze solver\n\nThis program is surprisingly powerful. When you are in a maze and you always take the\nrightmost possible way, you solve the maze. This is because the sequence of walls you walk along\nis a closed polygon and \n\n\n*/\nvoid main()\n{\n   // we assume to be bottom-left looking right!\n   while(true)  // go upwards\n   {\n      turn();turn();turn(); // look right\n      while (front_blocked)\n        turn();\n      move();\n   }\n}\n",
    "test.pl1": "void main(){\n  move();\n  turn();\n  eat();\n}\n"
  },
  "worlds": {
    "00-box14.wrl": "################\n#              #\n#              #\n#              #\n#       S      #\n#              #\n#              #\n#              #\n################\n",
    "00-roman.wrl": "#################\r\n#      #        #\r\n# #### # ###### #\r\n# #    # ##   # #\r\n# # #### ## # # #\r\n# #      ## #   #\r\n# ######### #####\r\n# #X      #     #\r\n# ####### ##### #\r\n# #     #     # #\r\n# # ### ##### # #\r\n# #   # #     # #\r\n# ### # # ##### #\r\n#     # #       #\r\n#######N#########",
    "book01.wrl": "##########\n#E       #\n######## #\n#        #\n# ########\n#        #\n######## #\n#        #\n# ########\n#       X#\n##########\n",
    "book02.wrl": "##########\n#    S   #\n# ###### #\n# ###### #\n# ###### #\n#    X   #\n##########\n",
    "book03.wrl": "##########\n#    S   #\n##########\n#    X   #\n##########\n",
    "book04.wrl": "##############################\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#E  *  *       *          *  #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n##############################\n",
    "book06.wrl": "##############################\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#E  *  *       *          *  #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n#                            #\n##############################\n",
    "book07.wrl": "##############################\n#                            #\n#               *            #\n#    *                       #\n#       *   *          *     #\n#                            #\n#                            #\n#                            #\n#  ******  *   *   **   **   #\n#    *     *   *   * * * *   #\n#    *     *   *   *  *  *   #\n#    *     *   *   *     *   #\n#    *     ****    *     *   #\n#                            #\n#E                           #\n##############################\n",
    "box2.wrl": "################\n#              #\n#              #\n#              #\n#              #\n#              #\n#              #\n#       W      #\n################\n",
    "roman-closed.wrl": "#################\r\n#      #        #\r\n# #### # ###### #\r\n# #    # ##   # #\r\n# # #### ## # # #\r\n# #      ## #   #\r\n# ######### #####\r\n# #X      #     #\r\n# ####### ##### #\r\n# #     #     # #\r\n# # ### ##### # #\r\n# #   # #     # #\r\n# ### # # ##### #\r\n#     #N#       #\r\n#################\r\n",
    "test.wrl": "#########################\n#                       #\n# ##################### #\n# #                   # #\n# #                   # #\n# #                   # #\n# #         *         # #\n# #                   # #\n# #                   # #\n# #                   # #\n# #########   ######### #\n#                       #\n#                       #\n#########################\n"
  }
};


var world;
var niki_x,niki_y,niki_o;
var trace=[];
var gTimeout=250; 
/*Create an empty grid with a prefix and return it as a string*/
function createGridHTML( r,  c,prefix="img")
  {
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
	  case '*':unrotate(image);   image.attr('src',"gfx/item.png"); break;
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
      if (niki_o == 'S') niki_o='E'
      else if (niki_o == 'E') niki_o='N'
      else if (niki_o == 'N') niki_o='W'
      else if (niki_o == 'W') niki_o='S';
      trace.push("TURN");
      updateGridFromGlobals("worldpreview-int");
  }
function pick(){
    trace.push("PICK");
    if (world[niki_y][niki_x] == '*')
    {
	world[niki_y][niki_x]=' ';
    }else{
	vm_message = "Trying to pick an item on an empty place";
	return false;
    }
    
    updateGridFromGlobals("worldpreview-int");
    return true;
}

function deposit(){
    trace.push("DEPOSIT");
    if (world[niki_y][niki_x] == ' ')
    {
	world[niki_y][niki_x]='*';
    }else{
	vm_message = "Trying to deposit an item on an non-empty place";
	return false;
    }
    
    updateGridFromGlobals("worldpreview-int");
    return true;
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
    ll = $('#selectLL').val()
    console.log("Language Level: ",ll);
    compiled = compile(src,ll); // language level. todo set to 1
    if (compiled.startsWith("SYNTAX ERROR")){
	alert(compiled);
    }else{
	$('#assembly').val(compiled);
	document.getElementById('btnTabMachineCode').click();
	
    }
}



function interactive_move(){
    if(!move()) {
	modalTextfieldMessage('You crashed<BR/><BR/>Your trace so far:<BR/>'+trace.join('<BR/>'));
	
    }
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
	ui_stopped();
	return; // any pending timeout after stop is ignored
    }
    
     // fetch a line
     if (pc < 0 || pc >= source.length){
	 modalTextfieldMessage("Program Counter (Line Pointer) outside source! Exit... (pc=" + pc.toString() +  ")");
	return step_counter;
     }
    line = source[pc];
    console.log("@"+pc.toString() + ":" + line );
    pc ++;
    $("#instruction").html(`${pc}: ${line}`);
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
	    vm_message="Machine stopped from instruction HALT on line " + pc.toString();
	    return false; // stops machine?! make it more positive
	},
	"JMP": function(t) {
	    //console.log(`In JMP with ${t}`);
	    row = getLabelRow(t[1]+":")
	    if (row < 0) {
		vm_state="ABORTED";
		vm_message="Machine tried to jump to non-existing label "+t[1];
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
	    //console.log(`JZ with flag ${cpu_flag}`);
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
	    //console.log(`RET TO ${lno}`)
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
	},
	"PICK": function(t){
	    return pick();
	},
	"DEPOSIT": function(t){
	    return deposit();
	}
	
    }

    // labels are ignored.
    if (tokens[0].endsWith(":")){
	//console.log("Label detected: "+line);
	setTimeout(step,0); // no delay
	
	return;
    }
    // all other must be valid instructions
    if (tokens[0] in statemachine){
	//console.log("Detected " + tokens[0]);
	state = statemachine[tokens[0]](tokens);
        step_counter ++;
	 
	if (state){
	    // continue
	    setTimeout(step,gTimeout);
	}else{
	    modalTextfieldMessage(`VM State: ${vm_state}<BR/>VM Message: ${vm_message}`);	   
	}
    }else{
	modalTextfieldMessage(`Encountered an unknown assembly instruction: <BR/>Instruction: ${tokens[0]}`);
	vm_state="stopped";

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

function ui_running(){
    $("#btnRun").prop("disabled",true);
    $("#btnShowTrace").prop("disabled",true);
    $("#btnReset").prop("disabled",true);
    $("#btnStop").prop("disabled",false);
}
function ui_stopped(){
    $("#btnRun").prop("disabled",false);
    $("#btnShowTrace").prop("disabled",false);
    $("#btnReset").prop("disabled",false);
    $("#btnStop").prop("disabled",true);

}

function vm_run()
{
    // invalidate all traces
    resetEnvironment();
    ui_running();
    vm_state="RUNNING";
    source=$("#assembly").val().split("\n");
    stack=[];
    pc=0;
    step_counter=0;
    cpu_flag=false;
    //console.log(source);
    step();
}

function resetEnvironment(){
	  resetTrace();
	  onBtnShowWorld(); // load all grids and place niki
  
}
function loadLibrary(){
    $("#dropdown_worlds").empty();
    $("#dropdown_program").empty();
    Object.keys(pllib.programs).forEach(function (e){
	$("#dropdown_program").append(`<option value="${e}">${e}</option>`);
    });
    Object.keys(pllib.worlds).forEach(function (e){
	$("#dropdown_worlds").append(`<option value="${e}">${e}</option>`);
    });
    
}


/*
MAIN
*/
  $(document).ready(function (e) {
      loadLibrary();
      
      $("#btnShowWorld").click(onBtnShowWorld);
    $("#btnCompile").click(do_compile); 
      $("#btnMove").click(interactive_move);
      $("#btnTurn").click(turn);
      $("#btnDismissMessage").click(hideMessage);
      $("#btnRun").click(vm_run);
      $("#btnStop").click(function(){vm_state="STOPPED";ui_stopped();});
      $('#btnChangeViewAndRun').click(function(){
	  document.getElementById('btnTabInteractive').click();
	  vm_run();
      });
      $("#btnShowTrace").click(showTrace);
      $("#btnReset").click(function(){
	  resetEnvironment();
      });
      $("#btnPresetWorld").click(function(){
	  world = $("#dropdown_worlds").val();
	  world_src = pllib.worlds[world];
	  
	  $("#world").val(world_src);	  
	  resetEnvironment();
      });
      $("#btnPresetProgram").click(function(){
	  prg = $("#dropdown_program").val();
	  prg_src = pllib.programs[prg];
	  $("#source").val(prg_src);	  
	  resetEnvironment();
      });

      $("#selectSpeed").on('change', function(e){
	  gTimeout=$("#selectSpeed").val();
      });
      $("#cfgFontSize").on('change', function(e){
	  fs = $("#cfgFontSize").val();
	  fs = Number(fs);
	  if (fs == 0){
	      $("textarea").css({"font-size":"initial"});
	  }else{
	      $("textarea").css({"font-size":fs});
	  }
	  
      });
      $("#cfgWorldSize").on('change', function(e){
	  ws = $("#cfgWorldSize").val();
	  console.log("World Size: ",ws);
	  $(".worldcontainer").css({"width":ws});
      });

      
      onBtnShowWorld(); // load default world
      hideMessage(); // enable user interface

  });
