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
  final int HEIGHT = 75;
  panel = new ControlP5(this);
  
  forwardButton = makeButton(panel, "ForwardButton", width/2 - WIDTH/2, 50, WIDTH, HEIGHT, "Forward");  
  
  distanceTextfield = makeTextfield(panel, "DistanceTextfield", width/2 - WIDTH/4 + 25, 150, HEIGHT, HEIGHT, "Distance");

  leftButton = makeButton(panel, "LeftButton", width/2 - WIDTH/2, 250, WIDTH, HEIGHT, "Left");  
  rightButton = makeButton(panel, "RightButton", width/2 - WIDTH/2, 350, WIDTH, HEIGHT, "Right"); 
  
  matchSpeedsButton = makeButton(panel, "MatchSpeedsButton", width/2 - WIDTH/2,450, WIDTH, HEIGHT, "Match Speeds");
  calibrateLeftButton = makeButton(panel, "CalibrateLeftButton",  width/2 - WIDTH/2, 550, WIDTH, HEIGHT, "Calibrate Left Turn");
  calibrateRightButton = makeButton(panel, "CalibrateRightButton",  width/2 - WIDTH/2, 650, WIDTH, HEIGHT, "Calibrate Right Turn");
  
  leftSlider = makeSlider(panel, "LeftSlider", 800, 500, 200, 30, 0, 1, "Left Slider");
  rightSlider = makeSlider(panel, "RightSlider", 800, 550, 200, 30, 0, 1, "Right Slider");
  
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
