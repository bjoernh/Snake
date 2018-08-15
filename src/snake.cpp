#include "snake.h"
//general
#include <stdio.h>
#include <algorithm>
#include <iterator>
#include <cmath>

Snake::Snake() : CubeApp(40){

}

bool Snake::loop(){
  static long loopcount = 0;

  loopcount++;
  return true;
}
