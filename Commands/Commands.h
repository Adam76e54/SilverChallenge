#pragma once

//COMMANDS_H
namespace comm
{
  constexpr char DELIMITER = ':';//commands will look like "LEFT:SPEED:12.3"
  constexpr char END = '\n';

  constexpr const char* LEFT = "L";
  constexpr const char* RIGHT = "R";
  constexpr const char* SPEED = "O";
  constexpr const char* IR = "I";
  constexpr const char* ULTRA_SONIC = "U";

  constexpr const char* DIRECTION = "Q";
  constexpr const char* W = "W";
  constexpr const char* D = "D";
  constexpr const char* A = "A";
  constexpr const char* S = "S";
  constexpr const char* STOP = "K";
  constexpr const char* START = "B";

  constexpr const char* MANUAL = "M";
  constexpr const char* TRACK = "T";
  constexpr const char* FOLLOW = "F";

  constexpr const char* EVENT = "E";

  constexpr const char* REMOVED_MESSAGE = "Obstacle removed";
  constexpr const char* OBSTACLE_MESSAGE = "Stopped for obstacle";
}
