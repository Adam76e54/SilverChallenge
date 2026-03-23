ControlP5 panel;

Button forwardButton, 
  leftButton, rightButton, 
  matchSpeedsButton, calibrateLeftButton, calibrateRightButton, 
  saveEEPROMButton;
  
Textlabel totalDistanceTextlabel, currentSpeedTextlabel;
  
Textfield distanceTextfield, kp, ki, kd;
  
boolean initialisedLeft = false, initialisedRight = false;
Slider leftSlider, rightSlider, rightTimeSlider, leftTimeSlider;

Client sam;

final int PORT = 80;
final String IP = "192.168.4.1";


Button makeButton(ControlP5 controller, String name, int x, int y, int w, int h, String label){
  Button button = controller.addButton(name);//attach function name to button
  button.setPosition(x, y)
    .setSize(w,h)
    .setLabel(label)    
    .setFont(createFont("Arial", 14));
;
  return button;
}

Textfield makeTextfield(ControlP5 controller, String name, int x, int y, int w, int h, String label){
  Textfield field = controller.addTextfield(name)
    .setPosition(x, y)
    .setSize(w, h)
    .setAutoClear(true)
    .setInputFilter(ControlP5.FLOAT)
    .setLabel(label)
    .setFont(createFont("Arial", 30));
    
    field.getCaptionLabel()
      .setColor(color(0))
      .setFont(createFont("Arial", 14))
      .align(ControlP5.CENTER, ControlP5.BOTTOM_OUTSIDE);

      return field;
}

Textlabel makeTextlabel(ControlP5 controller, String name, int x, int y, String text){
  Textlabel label = controller.addTextlabel(name)
    .setPosition(x, y)
    .setText(text);
    label.setColor(color(0));
    label.setFont(createFont("Arial", 14));
  
  return label;
}

Slider makeSlider(ControlP5 controller, String name, int x, int y, int w, int h, float start, float end, String label){
  Slider slider = controller.addSlider(name)
    .setPosition(x,y)
    .setSize(w,h)
    .setRange(start, end)
    .setTriggerEvent(Slider.RELEASE);
    
    slider.getCaptionLabel()
      .setColor(color(0))
      .setFont(createFont("Arial", 14))
      .setText(label);
  
  
  return slider;
}
