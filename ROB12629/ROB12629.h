#pragma once

#include <Arduino.h>

class ROB12629{
  private:
    uint8_t pin_;//one digital pin
    volatile unsigned long count_;
    unsigned long lastCount_, lastTime_;
    volatile unsigned long lastISRTime_;
    double rps_, distance_; //revs per second

    static constexpr float COUNTS_PER_REV_ = 4.0f; 
    static constexpr float CIRCUMFERENCE_ = 20.4;
    static constexpr unsigned int DEBOUNCE_ = 15000;
  public:
    ROB12629(uint8_t pin) : pin_(pin), count_(0), lastCount_(0), lastTime_(0), lastISRTime_(0), rps_(0), distance_(0) {
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

    void onInterrupt() {
    // Called only from the ISR:
      auto now = micros();
      if(now - lastISRTime_ >= DEBOUNCE_){
      count_++;

        lastISRTime_ = now;
      }
    }

    unsigned long count() const{
      noInterrupts();
      auto count = count_;
      interrupts();
      return count;
    }

    unsigned long lastCount() const{
      return lastCount_;
    }

    void update(unsigned long interval_microseconds){
      auto now = micros();
      unsigned long dt_micro = now - lastTime_;

      noInterrupts();
      unsigned long count = count_;
      interrupts();
      // Serial.print("Count = "); Serial.println(count);

      updateDistance(count);
      if(dt_micro >= interval_microseconds){
        unsigned int dc = count - lastCount_;

        // Serial.print("  [updateRevs] dt (seconds)="); Serial.print(dt_micro * 1e-6f);
        Serial.print("[encoder update] dc = "); Serial.print(dc);
        // Serial.print("  lastCount_ = "); Serial.print(lastCount_);
        // Serial.print("  count = "); Serial.print(count);

        if(dc == 0){
          rps_ = 0;
        }

        double dt_s = dt_micro * 1e-6f;//convert micros to seconds;
        if(dt_s != 0){
          float revs = (float)dc / COUNTS_PER_REV_;
          rps_ = (revs/dt_s);
        }    

        lastCount_ = count;
        lastTime_ = now;

        // Serial.print(" Distance = "); Serial.print(distance_);
        Serial.print(" cm/s = "); Serial.println(rps_ * CIRCUMFERENCE_);
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
      lastCount_ = 0;
    }

    void reset(){
      resetCount();
      resetDistance();
    }

  private:

};
