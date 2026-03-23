import processing.net.*;
import controlP5.*;

void setup(){
  size(1200, 800);
  noStroke();
  rectMode(CENTER);
  
  //connect to sam
  sam = new Client(this, IP, PORT);
  if( sam != null && sam.active()){
    //sam.write("Connected");//this is actually essential. WiFiServer::available() only pulls in client who have data waiting so we need to write something to be seen
  }
  
  final int WIDTH = 250;
  final int HEIGHT = 50;
  panel = new ControlP5(this);
  
  forwardButton = makeButton(panel, "ForwardButton", width/2 - WIDTH/2, 25, WIDTH, HEIGHT, "Forward");  
  
  distanceTextfield = makeTextfield(panel, "DistanceTextfield", width/2 - WIDTH/4 + 37, 110, HEIGHT, HEIGHT, "Distance");
  
  kp = makeTextfield(panel, "kp", 820, 200, HEIGHT, HEIGHT, "kp");
  ki = makeTextfield(panel, "ki", 880, 200, HEIGHT, HEIGHT, "ki");
  kd = makeTextfield(panel, "kd", 940, 200, HEIGHT, HEIGHT, "kd");
  

  leftButton = makeButton(panel, "LeftButton", width/2 - WIDTH/2, 200, WIDTH, HEIGHT, "Left");  
  rightButton = makeButton(panel, "RightButton", width/2 - WIDTH/2, 300, WIDTH, HEIGHT, "Right"); 
  
  matchSpeedsButton = makeButton(panel, "MatchSpeedsButton", width/2 - WIDTH/2, 400, WIDTH, HEIGHT, "Match Speeds");
  calibrateLeftButton = makeButton(panel, "CalibrateLeftButton",  width/2 - WIDTH/2, 500, WIDTH, HEIGHT, "Calibrate Left Turn");
  calibrateRightButton = makeButton(panel, "CalibrateRightButton",  width/2 - WIDTH/2, 600, WIDTH, HEIGHT, "Calibrate Right Turn");
  saveEEPROMButton = makeButton(panel, "SaveEEPROMButton", width/2 - WIDTH/2, 700, WIDTH, HEIGHT, "Save Current Speeds");
  
  leftSlider = makeSlider(panel, "LeftSlider", 800, 500, 200, 30, 0, 1, "Left Slider");
  rightSlider = makeSlider(panel, "RightSlider", 800, 550, 200, 30, 0, 1, "Right Slider");
  
  rightTimeSlider = makeSlider(panel, "RightTimeSlider", 800, 600, 200, 30, 0, 2, "Right Turn Time Slider");
  leftTimeSlider = makeSlider(panel, "LeftTimeSlider", 800, 650, 200, 30, 0, 2, "Left Turn Time Slider");
  
  totalDistanceTextlabel = makeTextlabel(panel, "TotalDistanceTextlabel", 800, 400, "Total Distance = 0.0");
  currentSpeedTextlabel = makeTextlabel(panel, "CurrentSpeedTextLabel", 800, 350, "Current Speed = 0.0");
  
}

void draw(){
  if(sam == null || !sam.active()){
    if(frameCount % 120 == 0){
      sam = new Client(this, IP, PORT);
    }
  }
  
  background(255);
  fill(0);
  textSize(32);
  textAlign(CENTER,CENTER);
  
  drawEventReporter();
  
  read(sam);
}
