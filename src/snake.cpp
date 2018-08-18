#include "snake.h"
//general
#include <stdio.h>
#include <algorithm>
#include <iterator>
#include <cmath>

Snake::Snake() : CubeApp(40){
  float startSpeed = 0.2;
  players.push_back(new Player(this, 0, getRandomPointOnScreen(top).cast<float>(), Vector3f(0, startSpeed, 0), Color::green(), 10));
  players.push_back(new Player(this, 1, getRandomPointOnScreen(top).cast<float>(), Vector3f(0, startSpeed, 0), Color::green()+Color::red(), 10));
  players.push_back(new Player(this, 2, getRandomPointOnScreen(top).cast<float>(), Vector3f(0, startSpeed, 0), Color::blue()+Color::red(), 10));
  players.push_back(new Player(this, 3, getRandomPointOnScreen(top).cast<float>(), Vector3f(0, startSpeed, 0), Color::red(), 10));
  for(int i = 0; i < 20; i++){
    food.push_back(new Food(this, getRandomPointOnScreen(front), Color::randomBlue()*2+Color::randomRed()*0.5));
    food.push_back(new Food(this, getRandomPointOnScreen(right), Color::randomBlue()*2+Color::randomRed()*0.5));
    food.push_back(new Food(this, getRandomPointOnScreen(back), Color::randomBlue()*2+Color::randomRed()*0.5));
    food.push_back(new Food(this, getRandomPointOnScreen(left), Color::randomBlue()*2+Color::randomRed()*0.5));
    food.push_back(new Food(this, getRandomPointOnScreen(top), Color::randomBlue()*2+Color::randomRed()*0.5));
    food.push_back(new Food(this, getRandomPointOnScreen(bottom), Color::randomBlue()*2+Color::randomRed()*0.5));
  }
}

bool Snake::loop(){
  static long loopcount = 0;
  clear();
  for(int oversampling = 4; oversampling > 0; oversampling--){
    for(auto player : players){
      player->handleJoystick();
      player->step();
      for(auto player2 : players){
        if(player->collidesWith(player2->iPosition()) && player!=player2){
          player2->die();
          //std::cout << "died" << std::endl;
        }
      }
      if(player->getIsDead())
        player->reset();
      player->render();
    }

    for (auto f : food){
      for (auto p : players){
        if(p->iPosition() == f->getPosition()){
          p->grow(2);
          p->speedUp(1.05);
          f->eat();
          food.push_back(new Food(this, getRandomPointOnScreen(anyScreen)));
        }
      }
      f->render();
    }
    food.erase(std::remove_if(food.begin(),food.end(),[](Food * f){return (f->getIsEaten());}),food.end());
  }

  render();
  loopcount++;
  return true;
}

Snake::Player::Player(CubeApp * renderCube, int joysticknumber, Vector3f setPosition, Vector3f setVelocity, Color setColor, unsigned int length) : joystick(joysticknumber){
  ca = renderCube;
  tail.push_back(position);
  position = setPosition;
  velocity = setVelocity;
  acceleration = Vector3f(0,0,0);
  color = setColor;
  defaultPosition = position;
  defaultVelocity = velocity;
  snakeLength = length;
  defaultSnakeLength = snakeLength;
  defaultColor = color;
  isDying = false;
  isDead = false;
}


void Snake::Player::reset(){
  position = defaultPosition;
  velocity = defaultVelocity;
  color = defaultColor;
  snakeLength = defaultSnakeLength;
  isDying = false;
  isDead = false;
  tail.clear();
}


void Snake::Player::step(){
  if(!isDying){
    accelerate();
    move();
    warp();

    for(unsigned int i = 0; i < 3; i++){
      if(position[i] < 0.01 && position[i] > 0)
        position[i] = 0;
      if(position[i] > VIRTUALCUBEMAXINDEX-0.01)
        position[i] = VIRTUALCUBEMAXINDEX;
    }

    //append to tail
    if(iPosition() != tail.back().cast<int>())
      tail.push_back(iPosition().cast<float>());

    //Check collisions
    Vector3i head = iPosition();
    int colCounter = 0;
    for(auto t : tail)
      if(head == t.cast<int>())
        colCounter++;
    if(colCounter > 1)
      die();

    //cap the tailssize
    if(tail.size() > snakeLength)
      tail.erase(tail.begin(), tail.end() - snakeLength);
  }else{
    if(dieCounter/40%2){
      color = Color::black();
    }else{
      color = Color::white();
    }
    dieCounter++;
    if(dieCounter >= 200){
      isDead = true;
    }
  }
}

void Snake::Player::accelerate(){
  velocity += acceleration;
}

void Snake::Player::move(){
  position += velocity;
}

void Snake::Player::warp(){
  //constrain position values
  for(int i = 0; i < 3; i++)
    position[i] = constrain(position[i], 0.0f, (float)VIRTUALCUBEMAXINDEX);

  Vector3i currentPosition = iPosition();
  EdgeNumber currentEdge = ca->getEdgeNumber(currentPosition);

  if(currentEdge != anyEdge){
    if(currentEdge != lastEdge){
      switch (currentEdge) {
        case topLeft:
        case topRight:
        case bottomRight:
        case bottomLeft:
          std::swap(velocity[2] ,velocity[0]);
        break;
        case topFront:
        case topBack:
        case bottomBack:
        case bottomFront:
          std::swap(velocity[2] ,velocity[1]);
        break;
        case frontRight:
        case backLeft:
        case leftFront:
        case rightBack:
          std::swap (velocity[0],velocity[1]);
        break;
        case anyEdge:
        default:
        break;
      }
      //set position to the rounded position to eliminate being always slightly below the surface due to rounding errors
      position = currentPosition.cast<float>();
      //constrain velocity directions, reflect if neccessary
      if((currentPosition[0] == 0 && velocity[0] < 0) || (currentPosition[0] == VIRTUALCUBEMAXINDEX && velocity[0] > 0)) velocity[0] *= -1;
      if((currentPosition[1] == 0 && velocity[1] < 0) || (currentPosition[1] == VIRTUALCUBEMAXINDEX && velocity[1] > 0)) velocity[1] *= -1;
      if((currentPosition[2] == 0 && velocity[2] < 0) || (currentPosition[2] == VIRTUALCUBEMAXINDEX && velocity[2] > 0)) velocity[2] *= -1;
    }
  }
  lastIPosition = currentPosition;
  lastEdge = currentEdge;
}

void Snake::Player::handleJoystick(){
  float newAxis0 = joystick.getAxis(0);
  if(newAxis0 < 0 && lastAxis0 == 0){
      turnLeft();
  }
  else if(newAxis0 > 0 && lastAxis0 == 0){
      turnRight();
  }
  lastAxis0 = newAxis0;
}

void Snake::Player::render(){
  for(auto t : tail){
      ca->setPixel3D(t[0], t[1], t[2], color);
  }
}

void Snake::Player::turnLeft(){
  if(!isDying && !ca->isOnEdge(iPosition()))
  {
    if (position[2] == 0){
      std::swap(velocity[0],velocity[1]);
      velocity[0] = -velocity[0];
    }
    else if (position[2] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[0],velocity[1]);
      velocity[1] = -velocity[1];
    }
    else if (position[1] == 0){
      std::swap(velocity[0],velocity[2]);
      velocity[2] = -velocity[2];
    }
    else if (position[1] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[0],velocity[2]);
      velocity[0] = -velocity[0];
    }
    else if (position[0] == 0){
      std::swap(velocity[1],velocity[2]);
      velocity[1] = -velocity[1];
    }
    else if (position[0] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[1],velocity[2]);
      velocity[2] = -velocity[2];
    }
    else{
      std::cout << position << std::endl;
    }
  }
}

void Snake::Player::turnRight()
{
  if(!isDying && !ca->isOnEdge(iPosition()))
  {
    if (position[2] == 0){
      std::swap(velocity[0],velocity[1]);
      velocity[1] = -velocity[1];
    }
    else if (position[2] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[0],velocity[1]);
      velocity[0] = -velocity[0];
    }
    else if (position[1] == 0){
      std::swap(velocity[0],velocity[2]);
      velocity[0] = -velocity[0];
    }
    else if (position[1] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[0],velocity[2]);
      velocity[2] = -velocity[2];
    }
    else if (position[0] == 0){
      std::swap(velocity[1],velocity[2]);
      velocity[2] = -velocity[2];
    }
    else if (position[0] == VIRTUALCUBEMAXINDEX){
      std::swap(velocity[1],velocity[2]);
      velocity[1] = -velocity[1];
    }
  }
}

bool Snake::Player::collidesWith(Vector3i point)
{
  for(auto t : tail)
    if(t.cast<int>() == point)
      return true;
  return false;
}

void Snake::Player::grow(unsigned int howMuch){
  snakeLength += howMuch;
}

void Snake::Player::speedUp(float factor){
  if(velocity.norm() < 1.0f)
    velocity *= constrain(factor, 1.0f, 2.0f);
}

void Snake::Player::die(){
  isDying = true;
  color = Color::white();
  dieCounter = 0;
}

bool Snake::Player::getIsDying(){
  return isDying;
}

bool Snake::Player::getIsDead(){
  return isDead;
}

Vector3i Snake::Player::iPosition(){
  return Vector3i(round(position[0]),round(position[1]),round(position[2]));
}


Snake::Food::Food(CubeApp * renderCube, Vector3i setPosition, Color setColor)
{
  isEaten = false;
  position = setPosition;
  color = setColor;
  ca = renderCube;
}

Vector3i Snake::Food::getPosition()
{
  return position;
}

Color Snake::Food::getColor()
{
  return color;
}

bool Snake::Food::getIsEaten()
{
  return isEaten;
}

void Snake::Food::eat()
{
  isEaten = true;
}

void Snake::Food::render()
{
  ca->setPixel3D(position, color);
}
