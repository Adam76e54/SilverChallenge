#pragma once
#include <Arduino.h>

class CD4021 {
  private: 
    uint8_t clock_, data_, latch_; 

    uint8_t count_, lastCount_; 
    unsigned long lastTime_;
    double rps_, distance_;

  public:

    static constexpr float COUNTS_PER_REV_ = 4.0f; // apparently there's 8 counts per revolution 
    static constexpr float CIRCUMFERENCE_ = 20.4;

    CD4021(uint8_t clock, uint8_t data, uint8_t latch) : 
      clock_(clock)
      , data_(data)
      , latch_(latch)
      , count_(0)
      , lastCount_(0)
      , rps_(0)
      , distance_(0) {}

    void begin() {
      pinMode(clock_, OUTPUT);
      pinMode(latch_, OUTPUT);
      pinMode(data_, INPUT);
      digitalWrite(clock_, LOW);
      digitalWrite(latch_, LOW);
      lastTime_ = micros();
    }

    auto lastCount() const {
      return lastCount_;
    }

    auto count() const{
      return count_;
    }
    double revsPerSecond() const{
      return rps_;
    }

    double cmPerSecond() const{
      return rps_ * CIRCUMFERENCE_;
    }

    void update(unsigned long interval_microseconds){
      noInterrupts();
      count_ = shiftIn();
      interrupts();
      // Serial.print("Count = "); Serial.print(count_);
      // Serial.print("   Last count = "); Serial.print(lastCount_);

      updateDistance(count_);


      updateRevs(count_, interval_microseconds);

      // Serial.print("   cm/s = "); Serial.println(cmPerSecond());
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

    void updateRevs(uint8_t count, unsigned long interval_microseconds){
      auto now = micros();
      unsigned long dt_micro = now - lastTime_;



      if(dt_micro >= interval_microseconds){
        uint8_t dc = count - lastCount_;

        // Serial.print("  [updateRevs] dt (seconds)="); Serial.print(dt_micro * 1e-6f);
        // Serial.print("[CD4021 update] dc = "); Serial.print(dc);
        // Serial.print("  lastCount_ = "); Serial.print(lastCount_);
        // Serial.print("  count = "); Serial.print(count);
        Serial.print(" [CD4021] Distance = "); Serial.print(distance_);

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

        // Serial.print(" cm/s = "); Serial.println(rps_ * CIRCUMFERENCE_);
      }
    }

    uint8_t shiftIn(){
      // This is the custom function the lecturer suggested using (although I've made it cleaner)
      
      // - Set up the CD4021 -
      // Get snapshot from CD4040. The clockPin speed is much faster than arduino so there's no need for using delays
      digitalWrite(latch_, 1);

      delayMicroseconds(20);
      // Force the CD4021 to stop changing for a little bit
      digitalWrite(latch_, 0);
      
      // - Perform the actual shift in -
      uint8_t data = 0;
      for (int8_t i = 7; i >= 0; --i){
        digitalWrite(clock_, 0);
        // delayMicroseconds(1); // We probably don't need this because the CD4021 is much faster than sam anyway
        uint8_t bit = digitalRead(this->data_);
        delayMicroseconds(1);
        // Flip the appropriate bit
        data = data | (bit << i);

        digitalWrite(clock_, 1); 

        // Debugging
        // char statement[64];
        // sprintf(statement, "Bit %d should be %d: ", i, bit);
        // Serial.print(statement);
        // Serial.println(data, BIN);
      }
      return data;
    }
};