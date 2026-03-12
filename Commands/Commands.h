#pragma once

namespace comm
{
  constexpr char DELIMITER = ':';//commands will look like "LEFT:SPEED:12.3"
  constexpr char END = '\n';

  constexpr const char* LEFT = "L";
  constexpr const char* RIGHT = "R";
  constexpr const char* CHANGE_SPEED = "O";
  constexpr const char* ULTRA_SONIC = "U";

  constexpr const char* DIRECTION = "Q";
  constexpr const char* FORWARD = "W";
  constexpr const char* TURN_RIGHT = "D";
  constexpr const char* TURN_LEFT = "A";
  constexpr const char* BACKWARD = "S";

  constexpr const char* STOP = "K";
  constexpr const char* START = "B";

  constexpr const char* CHANGE_TO_MANUAL = "M";
  constexpr const char* CHANGE_TO_MAPPING = "F";

  constexpr const char* EVENT = "E";

  constexpr const char* REMOVED_MESSAGE = "Obstacle removed";
  constexpr const char* OBSTACLE_MESSAGE = "Stopped for obstacle";
}
