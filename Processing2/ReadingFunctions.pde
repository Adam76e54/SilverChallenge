void read(Client sam){
  if(sam != null && sam.active()){
    String line = sam.readStringUntil('\n'); 
    
    if(line != null){
     //addLine(line, maxLines);
     print(line);

       line = trim(line);//get rid of the /n

       if(line.startsWith(comm_EVENT)){
          int firstColon = line.indexOf(':');
          if(firstColon != -1 && firstColon < line.length()-1){
            String payload = trim(line.substring(firstColon + 1));
            addLine(payload);
          }
       }
       
       if(line.startsWith(comm_LEFT_SPEED)){
         String[] splitString = split(line, ':');
         if(splitString.length == 2){
           //print(splitString[1]);
           float newRead = float(splitString[1]); 
  
           leftSlider.setValue(newRead);
           initialisedLeft = true;
         }
       }
       
       if(line.startsWith(comm_RIGHT_SPEED)){
         
         
         String[] splitString = split(line, ':');
         if(splitString.length == 2){
           //print(splitString[1]);
           float newRead = float(splitString[1]); 

           rightSlider.setValue(newRead);
           initialisedRight = true;
         }
       }
       
       if(line.startsWith(comm_CURRENT_SPEED)){
         String[] splitString = split(line, ':');
         if(splitString.length == 2){
           float newRead = float(splitString[1]);
           
           currentSpeedTextlabel.setText("Current Speed = " + newRead);
 
         }
       }
       
       if(line.startsWith(comm_CURRENT_SPEED)){
         String[] splitString = split(line, ':');
         if(splitString.length == 2){
           float newRead = float(splitString[1]);
           
           totalDistanceTextlabel.setText("Total Distance = " + newRead);
 
         }
       }

    }
  }
}
