#include "L293D.h"
#include <Arduino.h>

  L293D::L293D(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint8_t EN12, uint8_t EN34)
  : A1_(a1), A2_(a2), A3_(a3), A4_(a4), EN12_(EN12), EN34_(EN34), leftSpeed_(0), rightSpeed_(0) {
    //empty
  }

  void L293D::begin(){
    pinMode(A1_, OUTPUT);
    pinMode(A2_, OUTPUT);
    pinMode(A3_, OUTPUT);
    pinMode(A4_, OUTPUT);
    pinMode(EN12_, OUTPUT);
    pinMode(EN34_, OUTPUT);
    //Serial.println("Set everything to output");
  }

  void L293D::drive(float left, float right){
    if(left >= 0){
      leftForward(left);
    } else {
      leftBackward(left);
    }

    if(right >= 0){
      rightForward(right);
    } else {
      rightBackward(right);
    }
  }

  void L293D::forward(){
    leftForward();
    rightForward();
  }

  void L293D::forward(float percentage){
    speed(percentage, percentage);
    leftForward();
    rightForward();
  }

  void L293D::forward(float leftPercentage, float rightPercentage){
    speed(leftPercentage, rightPercentage);
    leftForward();
    rightForward();
  }

  void L293D::backward(){
    leftBackward();
    rightBackward();
  }

  void L293D::backward(float percentage){
    speed(percentage, percentage);
    leftBackward();
    rightBackward();
  }

  void L293D::backward(float leftPercentage, float rightPercentage){
    speed(leftPercentage, rightPercentage);
    leftBackward();
    rightBackward();
  }

  void L293D::brake(unsigned int deadtime){

    speed(1,1);

    writeLeft(HIGH, HIGH);
    writeRight(HIGH, HIGH);

    delay(deadtime);

    coast();
  }

  void L293D::coast(){
    leftCoast();
    rightCoast();
  }

  void L293D::speed(float leftPercentage, float rightPercentage){
    leftSpeed(leftPercentage);
    rightSpeed(rightPercentage);
    // Serial.print("Set both speeds to: ");
    // Serial.println(left);
  }

  void L293D::leftSpeed(float percentage){
    if(percentage < 0) percentage = 0;
    if(percentage > 1) percentage = 1;
    //Use analogWrite() to give Pulse-width modulation
    leftSpeed_ = 255 * percentage;
    analogWrite(EN12_, leftSpeed_);
  }

  void L293D::rightSpeed(float percentage){
    if(percentage < 0) percentage = 0;
    if(percentage > 1) percentage = 1;
    //Use analogWrite() to give Pulse-width modulation
    rightSpeed_ = 255 * percentage;
    analogWrite(EN34_, rightSpeed_);

  }

  void L293D::leftCoast(){
    //1. Set speed to 0: turns off PWM on the enable pin
    leftSpeed(0);
    //2. Set digital pins to actual coasting state
    writeLeft(LOW, LOW);
  }

  void L293D::rightCoast(){
    //1. Set speed to 0: turns off PWM on the enable pin
    rightSpeed(0);
    //2. Set digital pins to actual coasting state
    writeRight(LOW, LOW);
  }

  void L293D::leftBrake(unsigned int deadtime){
    //1. Set PWM to 100% duty cycle to give max braking (this isn't very eligent but probably grand)
    leftSpeed(1);
    //2. Set to brake mode
    writeLeft(HIGH, HIGH);
    //3. Wait before setting speed to 0
    delay(deadtime);
    leftCoast();
  }

  void L293D::rightBrake(unsigned int deadtime){
    //1. Set PWM to 100% duty cycle to give max braking (this isn't very eligent but probably grand)
    rightSpeed(1);
    //2. Set to brake mode
    writeRight(HIGH, HIGH);
    //3. Wait before setting speed to 0
    delay(deadtime);
    rightCoast();
  }

  void L293D::leftBackward(){
    //1.Disable motor to avoid shoot-through
    // leftCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeLeft(HIGH, LOW);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    leftSpeed(leftSpeed_/(255.0));
  }

  void L293D::leftBackward(float percentage){
    //1.Disable motor to avoid shoot-through
    // leftCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeLeft(HIGH, LOW);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    leftSpeed(percentage);
  }

  void L293D::rightBackward(){
    //1.Disable motor to avoid shoot-through
    // rightCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeRight(HIGH, LOW);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    rightSpeed(rightSpeed_/(255.0));
  }

  void L293D::rightBackward(float percentage){
    //1.Disable motor to avoid shoot-through
    // rightCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeRight(HIGH, LOW);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    rightSpeed(percentage);
  }

  void L293D::leftForward(){
    //1.Disable motor to avoid shoot-through
    // leftCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeLeft(LOW, HIGH);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    leftSpeed(leftSpeed_/(255.0));
  }

  void L293D::leftForward(float percentage){
    //1.Disable motor to avoid shoot-through
    // leftCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeLeft(LOW, HIGH);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    leftSpeed(percentage);
  }

  void L293D::rightForward(){
    //1.Disable motor to avoid shoot-through
    // rightCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeRight(LOW, HIGH);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    rightSpeed(rightSpeed_/(255.0));
  }

  void L293D::rightForward(float percentage){
    //1.Disable motor to avoid shoot-through
    // rightCoast();
    // delay(L293D_DEADTIME);
    //2. Set new direction
    writeRight(LOW, HIGH);//NOTE: this may need changing depending on orientation of motor
    //3. Reset speed
    rightSpeed(percentage);
  }

  uint8_t L293D::leftSpeed() const{
    return leftSpeed_;
  }

  uint8_t L293D::rightSpeed() const{
    return rightSpeed_;
  }
  
/************INTERNAL METHODS************/
  void L293D::writeLeft(bool a1, bool a2) const{
    digitalWrite(A1_, a1);
    digitalWrite(A2_, a2);
  }

  void L293D::writeRight(bool a3, bool a4) const{
    digitalWrite(A3_, a3);
    digitalWrite(A4_, a4);
  }
