#pragma once
#include <Arduino.h>

template <uint8_t N> 
class Buffer{
  char buffer_[N];
  uint8_t head_, tail_, count_; 
  
public:
  Buffer() : head_(0), tail_(0), count_(0){}

  bool write(const char* data, uint8_t size){
    if(size > space()) return false;

    for(uint8_t i = 0; i < size; ++i){
      write(data[i]);//note, if(size > space()) guarantees this won't return false
    }
    return true;
  }

  bool write(char data){
    if(count_ == N) return false;

    buffer_[head_] = data;
    head_ = (head_ + 1) % N;
    ++count_;
    return true; 
  }

  uint8_t read(char destination[], uint8_t size){
    char c;
    uint8_t i;

    for(i = 0; i < size - 1; ++i){
      if(!read(c)) break;//break out if the read fails
      destination[i] = c;
      if(c == '\n') break;
    }

    destination[i] = '\0';
    return i;
  }

  bool read(char& destination){
    if(count_ == 0) return false;

    destination = buffer_[tail_];
    tail_ = (tail_ + 1) % N;

    --count_;

    return true;
  }

  String read(){
    String string;
    char c;
    uint8_t i = 0;
    
    while(read(c)){
      if(c == '\n') break;
      string += c;
    }

    return string;
  }

  char peek() const{
    if(count_ == 0) return '\0';
    return buffer_[tail_];
  }

  const char* peekCommand() const{
    if(count_ == 0) return '\0';
    return buffer_[head_];
  }
  uint8_t space() const{return N - count_;}

  uint8_t available() const{return count_;}

};