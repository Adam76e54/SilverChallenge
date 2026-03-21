public void LeftSlider(float percentage){
  if(sam != null && sam.active() && initialisedLeft){
    sam.write(comm_LEFT_SPEED + comm_DELIMITER + percentage + comm_END);
  } 
}

public void RightSlider(float percentage){
  if(sam != null && sam.active() && initialisedRight){
    sam.write(comm_RIGHT_SPEED + comm_DELIMITER + percentage + comm_END);
  } 
}
