ArrayList<String> lines = new ArrayList<String>();
int maxLines = 10;

final int mode_TRACK = 0;
final int mode_MANUAL = 1;
final int mode_FOLLOW = 2;


int mode = mode_MANUAL;

Client sam;

PFont font;

final int PORT = 80;

final int START = 0;
final int MENU = 1;

final String SAM = "192.168.4.1";

int state = START;

ControlP5 panel;
ArrayList<Controller<?>> menuButtons = new ArrayList<Controller<?>>();

Button downloadButton, manualModeButton, trackModeButton, followModeButton;
Button moveButton, turnLeftButton, turnRightButton;

Slider slider, leftThreshold, rightThreshold, maxDistance;

Textlabel speedlabel, modelabel, leftIRlabel, rightIRlabel, uslabel;
Textlabel distanceInputLabel, angleInputLabel;

Toggle stopToggle;
Textfield distanceTextBox, angleTextBox;
