
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
    console.log("setImage prefix="+prefix);
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
		
		  theworld[i][j]=' '
		  theniki=[i,j,theworld[i][j]];
		
	      }
	  }
      }
    return {
	"world": theworld,
	"niki": theniki
    }
  }



/* Button Handler */


function onBtnShowWorld(){
    console.log("default clicker");
    w = loadGridFromEditor("#world","#worldpreview-world","worldeditorpreview");
    loadGridFromEditor("#world","#worldpreview-source","worldpreview-source");
    loadGridFromEditor("#world","#worldpreview-assembly","worldpreview-asm");
    loadGridFromEditor("#world","#world-interactive","worldpreview-int");
    
    console.log(w);
}
