#pragma once

#include <Arduino.h>

class ROB12629{
  private:
    uint8_t pin_;//one digital pin
    volatile unsigned long count_;
    unsigned long lastcount_, lastTime_;
    double rps_, distance_; //revs per second

    static constexpr float COUNTS_PER_REV_ = 4.0f; 
    static constexpr float CIRCUMFERENCE_ = 20.4;
  public:
    ROB12629(uint8_t pin) : pin_(pin), count_(0), lastcount_(0), lastTime_(0), rps_(0), distance_(0) {
      //empty
    }

    //requires us to feed an ISR to it
    void begin(void (*ISR)()){
      if(pin_ != 2 && pin_ != 3){
        Serial.println("You're not using interrupt pins !!!");
      }

      lastTime_ = micros();
      pinMode(pin_, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(pin_), ISR, RISING);
    }
    //NOTE: the reason you can' have an ISR method is because C++ always implicitly passes *this. E.g. Class::func() calls will always be fed Class::func(this)
    //so it autogenerates a parameter. An ISR must have no parameters so needs to be a "free" function (not attached to an instance of a class)

    void increment(){
    //we'll use this inside of a globally defined ISR
      count_++;
    }

    unsigned long count() const{
      noInterrupts();
      auto count = count_;
      interrupts();
      return count;
    }

    unsigned long lastCount() const{
      return lastcount_;
    }

    void update(unsigned long interval_microseconds){
      auto now = micros();
      unsigned long dt_m = now - lastTime_;

      noInterrupts();
      unsigned long count = count_;
      interrupts();

      updateDistance(count);
      if(dt_m >= interval_microseconds){
        
        unsigned long dc = count - lastcount_;

        // Serial.print("dc = ");
        // Serial.println(dc);

        lastTime_ = now;
        lastcount_ = count;

        if(dc == 0){
          rps_ = 0;
          return;
        }

        double revs = (double)dc / COUNTS_PER_REV_;
        double dt_s = dt_m * 1e-6f;//convert micros to seconds;
        rps_ = (revs/dt_s);
      }
    }

    double revsPerSecond() const{
      return rps_;
    }

    double cmPerSecond() const{
      return rps_ * CIRCUMFERENCE_;
    }
    
    double distance() const{
      return distance_;
    }

    void updateDistance(unsigned long count){
      distance_ = (double)count * (CIRCUMFERENCE_ / COUNTS_PER_REV_);
    }

    void resetDistance(){
      distance_ = 0;
    }

    void resetCount(){
      count_ = 0;
      lastcount_ = 0;
    }

    void reset(){
      resetCount();
      resetDistance();
    }

  private:

};
