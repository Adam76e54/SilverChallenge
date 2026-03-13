void setMenu(){
  state = MENU;
  // Debug: print which controllers are null
  for(int i = 0; i < menuButtons.size(); i++){
    if(menuButtons.get(i) == null){
      println("menuButtons[" + i + "] is NULL!");
    }
  }
  setVisible(menuButtons, true);
}

void setStart(){
  state = START;
  setVisible(menuButtons, false);
}

void setVisible(ArrayList<Controller<?>> buttons, boolean visible){
 for(Controller<?> b : buttons){
   if(b != null){
     if(visible) b.show();
     else b.hide();
   }
 }
}


public void Speed(float v){
  
  if(sam != null && sam.active()){
    sam.write(comm_SPEED + comm_DELIMITER + v + comm_END);
    addSample(speed, "Changed speed");

  }

}

public void LeftThresholdSlider(int t){
  if(sam != null && sam.active()){
    sam.write(comm_LEFT + comm_DELIMITER + comm_IR + comm_DELIMITER + t + comm_END);
    addSample(speed, "Changed left IR sensor threshold");

  }

}

public void RightThresholdSlider(int t){
  if(sam != null && sam.active()){
    sam.write(comm_RIGHT + comm_DELIMITER + comm_IR + comm_DELIMITER + t + comm_END);
    addSample(speed, "Changed right IR sensor threshold");
  }

}

public void MaxDistance(int d){
  if(sam != null && sam.active()){
    sam.write(comm_ULTRA_SONIC + comm_DELIMITER + d + comm_END);
    //print(comm_ULTRA_SONIC + comm_DELIMITER + d + comm_END);
    addSample(speed, "Changed distance threshold");

  }
}

//defines a function for the Button to react with when pressed
public void downloadCSV(int v){
  addSample(speed, "CSV downloaded");
  saveToCSV(); 
}

public void manualMode(int v){
  if(sam != null && sam.active()){
    sam.write(comm_MANUAL + comm_END);
    modelabel.setText("Mode: MANUAL");
    addSample(speed, "Switched to follow mode");

  }
}

public void trackMode(int v){
  if(sam != null && sam.active()){
    sam.write(comm_TRACK + comm_END);
    modelabel.setText("Mode: TRACK");
    if(stopToggle != null){
      stopToggle.setValue(true);
    }
    addSample(speed, "Switched to track mode");
  }
}

public void followMode(int v){
  if(sam != null && sam.active()){
    sam.write(comm_FOLLOW + comm_END);
    modelabel.setText("Mode: FOLLOW");

    if(stopToggle != null){
      stopToggle.setValue(true);
    }
    addSample(speed, "Switched to follow mode");
  }
}

public void StopToggle(boolean v) {
  if(sam != null && sam.active()){
    if (!v) {
       sam.write(comm_START + comm_END);
       //print(comm_START + comm_END);
      stopToggle.setCaptionLabel("Stop");
      stopToggle.setColorBackground(color(150, 0, 0));
      stopToggle.setColorForeground(color(200, 0, 0));
      stopToggle.setColorActive(color(150, 0, 0));
      
      addSample(speed, "Pressed Start");

    } else {
       sam.write(comm_STOP + comm_END);
       //print(comm_STOP + comm_END);
      stopToggle.setCaptionLabel("Start");
      stopToggle.setColorBackground(color(0, 150, 0));
      stopToggle.setColorForeground(color(0, 200, 0));
      stopToggle.setColorActive(color(0, 150, 0));
      addSample(speed, "Pressed Stop");
    }
  }
}

//assumes there's a ControlP5 called "panel" in the global space
Button makeButton(String name, int x, int y, int w, int h, String label, PFont font){
  Button button = panel.addButton(name);//attach function name to button
  button.setPosition(x, y).setSize(w,h).setLabel(label).hide();
  button.getCaptionLabel().setFont(font);
  return button;
}
