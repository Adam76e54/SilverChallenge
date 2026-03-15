import processing.net.*;
import controlP5.*;

void setup(){
  size(1400, 800);
  noStroke();
  rectMode(CENTER);
  

  //connect to sam
  //sam = new Client(this, SAM, PORT);
  //if( sam != null && sam.active()){
  //  sam.write("Connected");//this is actually essential. WiFiServer::available() only pulls in client who have data waiting so we need to write something to be seen
  //}
  
  //make buttons
  panel = new ControlP5(this);
  
  font = createFont("Arial", 16);
  
  downloadButton = makeButton("downloadCSV", width/2 - 70, height/2 + 60,140,60,"Download CSV", font);
  manualModeButton = makeButton("manualMode", width/2 - 50, 700, 100, 60, "Manual", font);
  trackModeButton = makeButton("trackMode", width/2 + 75, 700, 100, 60, "Track", font);
  followModeButton = makeButton("followMode", width/2 - 175, 700, 100, 60, "Follow", font);

  stopToggle = panel.addToggle("StopToggle").setPosition(width/2 - 70, height/2 + 150).setSize(140, 60);
  stopToggle.setCaptionLabel("Stop");
  stopToggle.getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER).setPaddingX(0).setPaddingY(0).setFont(font);
  stopToggle.hide();
  stopToggle.setColorBackground(color(150, 0, 0));
  stopToggle.setColorForeground(color(175, 0, 0));
  stopToggle.setColorActive(color(150, 0, 0));
  

  
  speedlabel = panel.addTextlabel("speedLabel").setPosition(30,25).setText("Speed: 0.0");
  speedlabel.setColor(color(0));
  speedlabel.setFont(font);
  
  modelabel = panel.addTextlabel("modeLabel").setPosition(width/2 - 63, 670).setText("Mode: MANUAL");
  modelabel.setColor(color(0));
  modelabel.setFont(font);
  
  uslabel = panel.addTextlabel("uslabel").setPosition(65, 480).setText("Ultra Sonic Sensor Reading: 0.0 cm");
  uslabel.setColor(color(0));
  uslabel.setFont(font);
  
  slider = panel.addSlider("Speed").setPosition(50,50).setSize(40, 200).setRange(0,1).setValue(0);
  slider.getValueLabel().setColor(color(0)).setFont(createFont("Arial", 12));
  slider.getCaptionLabel().setVisible(false);
  slider.hide().setTriggerEvent(Slider.RELEASE);
  menuButtons.add(slider);
  
  
  float distanceDefault = 15;
  maxDistance = panel.addSlider("MaxDistance").setPosition(1200,500).setSize(40,200).setRange(0,50).setValue(distanceDefault);
  maxDistance.getValueLabel().setColor(color(0)).setFont(createFont("Arial", 12));
  maxDistance.getCaptionLabel().setVisible(true).setColor(color(0)).setFont(createFont("Arial", 12)).setText("Distance");
  maxDistance.hide().setTriggerEvent(Slider.RELEASE);
  menuButtons.add(maxDistance);

  distanceInputLabel = panel.addTextlabel("distanceInputLabel")
    .setPosition(1100, 80)
    .setText("Distance (cm)");
  distanceInputLabel.setColor(color(50));
  distanceInputLabel.setFont(font);
  distanceInputLabel.hide();

  distanceTextBox = panel.addTextfield("distanceTextBox")
    .setPosition(1100, 105)
    .setSize(120, 40)
    .setAutoClear(false)
    .setText("25");
  distanceTextBox.setFont(font);
  distanceTextBox.hide();

  moveButton = makeButton("moveDistance", 1250, 105, 120, 40, "Move", font);

  angleInputLabel = panel.addTextlabel("angleInputLabel")
    .setPosition(1100, 170)
    .setText("Angle (deg)");
  angleInputLabel.setColor(color(0));
  angleInputLabel.setFont(font);
  angleInputLabel.hide();

  angleTextBox = panel.addTextfield("angleTextBox")
    .setPosition(1100, 195)
    .setSize(120, 40)
    .setAutoClear(false)
    .setText("90");
  angleTextBox.setFont(font);
  angleTextBox.hide();

  turnLeftButton = makeButton("turnLeftAngle", 1250, 185, 120, 40, "Turn Left", font);
  turnRightButton = makeButton("turnRightAngle", 1250, 235, 120, 40, "Turn Right", font);
  
  // Set the value LAST, after everything is initialised
  stopToggle.setValue(true);
  menuButtons.add(downloadButton);
  menuButtons.add(manualModeButton);
  menuButtons.add(trackModeButton);
  menuButtons.add(followModeButton);
  menuButtons.add(stopToggle);
  menuButtons.add(distanceTextBox);
  menuButtons.add(angleTextBox);
  menuButtons.add(moveButton);
  menuButtons.add(turnLeftButton);
  menuButtons.add(turnRightButton);
  menuButtons.add(distanceInputLabel);
  menuButtons.add(angleInputLabel);
  //menuButtons.add(startButton);
}


String inBuffer = "";

void draw(){
  //if(sam == null || !sam.active()){
  //  if(frameCount % 120 == 0){
  //    sam = new Client(this, SAM, PORT);
  //  }
  //}
  
  ////DEBUG
  //if(frameCount % 120 == 0){
  //  println("sam:", sam, "active:", sam != null && sam.active());
  //}
  
  
  if(state == START){
    drawStart();
  }
  if(state == MENU){
    drawMenu();
  }

  read(sam);

}
