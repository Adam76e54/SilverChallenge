#pragma once

namespace comm
{
  constexpr char DELIMITER = ':';//commands will look like 'LEFT:SPEED:12.3'
  constexpr char END = '\n';

  constexpr char ULTRA_SONIC = 'A';

  constexpr char FORWARD = 'B';
  constexpr char TURN_RIGHT = 'C';
  constexpr char TURN_LEFT = 'D';

  constexpr char STOP = 'E';
  constexpr char START = 'F';

  constexpr char EVENT = 'G';

  constexpr char MATCH_WHEELS = 'H';

  constexpr char CALIBRATE_LEFT = 'I';
  constexpr char CALIBRATE_RIGHT = 'J';

  constexpr char CHANGE_TARGET = 'K';

  constexpr char LEFT_SPEED = 'L';
  constexpr char RIGHT_SPEED = 'M';

  constexpr char CURRENT_SPEED = 'N';
  constexpr char TOTAL_DISTANCE = 'O';
  constexpr char SAVE_EEPROM = 'P';

  constexpr char RIGHT_FACTOR = 'Q';
  constexpr char LEFT_FACTOR = 'R';

  constexpr char KP = 'S';
  constexpr char KI = 'T';
  constexpr char KD = 'U';
  
  constexpr  const char* REMOVED_MESSAGE = "Obstacle removed";
  constexpr const char* OBSTACLE_MESSAGE = "Stopped for obstacle";
}
