void drawStart(){
  background(0);
  fill(255);
  textAlign(CENTER, CENTER);
  textSize(32);
  text("Sam.", width/2, height/2);
  
  textSize(16);
  if(sam == null || !sam.active()){
    text("Connecting...", width/2, height/2 + 40);
  } else{
     text("Connected", width/2, height/2 + 40);
  }
}

void drawMenu(){ 
  background(255);
  fill(0);
  textSize(32);
  textAlign(CENTER,CENTER);
  text("Menu", width/2, height/2);
  
  printSensors();
  
  textSize(12);
  int lineHeight = int(textAscent() + textDescent()) + 20;
  printLines(lineHeight);
}

void printSensors(){
  stroke(255);
  fill(180);
  rect(220, 500, 350, 150, 10);
  
}


void printLines(int lineHeight){
  
  stroke(255);
  fill(0);
  rect(width/2, height/5 + 10, 720, 320, 10);
  
  fill(255);                  // white text
  textAlign(LEFT, TOP);       // easier to reason about
  textSize(20);
  
  for(int i = 0; i < lines.size(); i++){
    text(lines.get(i), 360, 20 + i * lineHeight);
  }
}
void addLine(String s, int maxLines){
  lines.add(s);
  if(lines.size() > maxLines){
    lines.remove(0); 
  }
}
