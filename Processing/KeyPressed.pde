boolean w = false, a = false, s = false, d = false;

void keyPressed(){
  if(state == START && keyCode == ENTER){
    if(keyCode == ENTER){
      text("Pressed ENTER", width/2, height/2 + 80);
      setMenu();
    }

    
  }
  if(state == MENU){
    if(keyCode == ESC){
      key = 0; //otherwise the window closes
      state = START;
      setStart();
    }
    
    if(sam != null){
      if(mode == mode_MANUAL){
        if((key == 'w' || key == 'W') && !w){
          w = true;
          sam.write(comm_DIRECTION + comm_DELIMITER + comm_W + comm_END); 
          //print(comm_DIRECTION + comm_DELIMITER + comm_W + comm_END);
  
        }
        if((key == 'a' || key == 'A') && !a){
          a = true;
          sam.write(comm_DIRECTION + comm_DELIMITER + comm_A + comm_END); 
        }
        if((key == 's' || key == 'S') && !s){
          s = true;
          sam.write(comm_DIRECTION + comm_DELIMITER + comm_S + comm_END); 
        }
        if((key == 'd' || key == 'D') && !d){
          d = true;
          sam.write(comm_DIRECTION + comm_DELIMITER + comm_D + comm_END); 
        }
  
      }
    }

    
  }
  
}

void keyReleased(){
  if(state == MENU){
    if(keyCode == ESC){
      key = 0;
      state = START;
      setStart();
    }
    
    if(sam != null && sam.active()){
        if(mode == mode_MANUAL){
          if(key == 'w' || key == 'W'){
            w = false;
            sam.write(comm_DIRECTION + comm_DELIMITER + comm_STOP + comm_END); 
            //print(comm_DIRECTION + comm_DELIMITER + comm_STOP + comm_END);
          }
          if(key == 'a' || key == 'A'){
            a = false;
            sam.write(comm_DIRECTION + comm_DELIMITER + comm_STOP + comm_END); 
          }
          if(key == 's' || key == 'S'){
            s = false;
            sam.write(comm_DIRECTION + comm_DELIMITER + comm_STOP + comm_END); 
          }
          if(key == 'd' || key == 'D'){
            d = false;
            sam.write(comm_DIRECTION + comm_DELIMITER + comm_STOP + comm_END); 
          }
    
        } 
    }
    
  }
}
