public void ForwardButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_FORWARD + comm_END);
    print(comm_FORWARD + comm_END);
  } 
}

public void LeftButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_TURN_LEFT + comm_END);
  } 
}

public void RightButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_TURN_RIGHT + comm_END);
  }  
}

public void MatchSpeedsButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_MATCH_WHEELS + comm_END);
  } 
}

public void CalibrateLeftButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_CALIBRATE_LEFT + comm_END);
  }  
}

public void CalibrateRightButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_CALIBRATE_RIGHT + comm_END);
  }  
}

public void SaveEEPROMButton(int nan){
  if(sam != null && sam.active()){
    sam.write(comm_SAVE_EEPROM + comm_END);
  }  
  
}
