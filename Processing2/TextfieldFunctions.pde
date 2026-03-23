public void DistanceTextfield(String text){
  if (text == null || text.length() == 0) return;

  try {
    int value = Integer.parseInt(text.trim());
    
    if(sam != null && sam.active()){

      // Send as text + newline for your Arduino-side line parser
      sam.write(comm_CHANGE_TARGET + comm_DELIMITER + value + comm_END);
      print(comm_CHANGE_TARGET + comm_DELIMITER + value + comm_END);
    }
  } catch (NumberFormatException e) {
    println("Invalid int: " + text);
  }

}

public void kp(String text){
  if (text == null || text.length() == 0) return;

  try {
    float value = Float.parseFloat(text.trim());
    
    if(sam != null && sam.active()){

      // Send as text + newline for your Arduino-side line parser
      sam.write(comm_KP + comm_DELIMITER + value + comm_END);
    }
  } catch (NumberFormatException e) {
    println("Invalid int: " + text);
  }

}

public void ki(String text){
  if (text == null || text.length() == 0) return;

  try {
    float value = Float.parseFloat(text.trim());
    
    if(sam != null && sam.active()){

      // Send as text + newline for your Arduino-side line parser
      sam.write(comm_KI + comm_DELIMITER + value + comm_END);
    }
  } catch (NumberFormatException e) {
    println("Invalid int: " + text);
  }

}

public void kd(String text){
  if (text == null || text.length() == 0) return;

  try {
    float value = Float.parseFloat(text.trim());
    
    if(sam != null && sam.active()){

      // Send as text + newline for your Arduino-side line parser
      sam.write(comm_KD + comm_DELIMITER + value + comm_END);
    }
  } catch (NumberFormatException e) {
    println("Invalid int: " + text);
  }

}
