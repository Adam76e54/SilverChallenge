ArrayList<String> lines = new ArrayList<String>();
final int MAX_LINES = 20;

void printLines(){
  fill(255);                  // white text
  textAlign(LEFT, TOP);       // easier to reason about
  textSize(20);
  
  int lineHeight = int(textAscent() + textDescent()) + 20;

  for(int i = 0; i < lines.size(); i++){
    text(lines.get(i), 50, 30 + i * lineHeight);
  }
}
void addLine(String s){
  lines.add(s);
  if(lines.size() > MAX_LINES){
    lines.remove(0); 
  }
}

void drawEventReporter(){
  fill(0);
  rect(width / 5, height / 2, 400, 750, 10);

  
  printLines();  
}
