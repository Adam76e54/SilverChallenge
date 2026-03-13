final int MAX_SAMPLES = 1000;  


float[] speedHistory   = new float[MAX_SAMPLES];
String[] eventHistory  = new String[MAX_SAMPLES];
String[] times = new String[MAX_SAMPLES];

int writeIndex = 0;   // next slot to write
int sampleCount = 0;  // how many valid samples (<= MAX_SAMPLES)
float speed = 0;

void addSample(float speed, String event) {
  speedHistory[writeIndex]  = speed;
  eventHistory[writeIndex]  = event;
  
  String now = hour() + ":" + minute() + ":" + second();
  times[writeIndex] = now;

  writeIndex = (writeIndex + 1) % MAX_SAMPLES;  // wrap around
  if (sampleCount < MAX_SAMPLES) {
    sampleCount++;
  }
}

void saveToCSV() {
  String[] lines = new String[sampleCount + 1];
  lines[0] = "index;time;speed;event";

  // oldest index is (writeIndex - sampleCount + MAX_SAMPLES) % MAX_SAMPLES
  int start = (writeIndex - sampleCount + MAX_SAMPLES) % MAX_SAMPLES;

  for (int i = 0; i < sampleCount; i++) {
    int idx = (start + i) % MAX_SAMPLES;
    lines[i+1] = i + ";" + times[idx] + ";" + speedHistory[idx] + ";" + eventHistory[idx];
  }

  saveStrings("speed_event_history.csv", lines);
}

void read(Client sam){
  if(sam != null && sam.active()){
    String line = sam.readStringUntil('\n'); 
    
    if(line != null){
     //addLine(line, maxLines);
     //print(line);

       line = trim(line);//get rid of the /n

       if(line.startsWith(comm_EVENT)){
          int firstColon = line.indexOf(':');
          if(firstColon != -1 && firstColon < line.length()-1){
            String payload = trim(line.substring(firstColon + 1));
            addSample(speed, payload);
            addLine(payload, maxLines);
            
            if(payload.equals(comm_OBSTACLE_MESSAGE) && stopToggle != null){
              addSample(speed, "Detected obstacle");
              stopToggle.setValue(true);
            }
            
            if(payload.equals(comm_REMOVED_MESSAGE) && stopToggle != null){
              addSample(speed, "Removed obstacle");

              stopToggle.setValue(false);
            }
          }
       }
        
       if(line.startsWith(comm_LEFT)){
         //print(line);
         String[] splitString = split(line, ':');
         if(splitString.length == 3 && splitString[1].equals(comm_IR)){
           //print(splitString[1]);
           int newRead = int(splitString[2]); 

           if(newRead < leftThreshold.getValue() && stopToggle.getValue() == 0){
             addSample(speed, "Turned left");
             addLine("Turned left", maxLines); 
           }
           
           if(leftIRlabel != null){
            leftIRlabel.setText("Left IR Sensor Reading: " + nf(newRead, 0)); 
           }
         }
       }
       
       if(line.startsWith(comm_RIGHT)){
         String[] splitString = split(line, ':');
         if(splitString.length == 3 && splitString[1].equals(comm_IR)){
           int newRead = int(splitString[2]); 
           
           if(newRead < rightThreshold.getValue() && stopToggle.getValue() == 0){
             addSample(speed, "Turned right");
             addLine("Turned right", maxLines); 
           }
           
           if(rightIRlabel != null){
            rightIRlabel.setText("Right IR Sensor Reading: " + nf(newRead, 0)); 
           }
         }
       }
       
       if(line.startsWith(comm_ULTRA_SONIC)){
         String[] pair = split(line, ':');
         if(pair.length == 2){
            float distance = float(pair[1]);
            
            if(uslabel != null && distance != 999){
              uslabel.setText("Ultra Sonic Sensor Reading: " + nf(distance, 1, 2) + " cm");
            } else{
              uslabel.setText("Ultra Sonic Sensor Reading: NaN" );
            }
         }
       }

    }
  }
}
