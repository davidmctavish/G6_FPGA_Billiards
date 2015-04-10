#include "angles.hpp"
#include "iostream"
// This is so cool.
// We bit shift by 2, then append a 1. To simulate sqrt.
unsigned long long int LongSqRt(unsigned long long int num){
  if (num <= 0)
    return 0; //can't deal with negatives

  unsigned long long int place = 0x4000000000000000;
  while (place > num)
    place = (place >> 2); // divide by 4

  unsigned long long int root = 0;
  while (place){
    if (num >= root + place){
      num -= root + place;
      root += place * 2;
    }
    root = (root >> 1);
    place = (place >> 2);
  }
  return ((unsigned long long int)(root));
}

int intSqRt(unsigned int num){
  if (num <= 0)
    return 0; //can't deal with negatives

  unsigned int place = 1 << (sizeof(unsigned int)*8 - 2);
  while (place > num)
    place = (place >> 2); // divide by 4

  unsigned int root = 0;
  while (place){
    if (num >= root + place){
      num -= root + place;
      root += place * 2;
    }
    root = (root >> 1);
    place = (place >> 2);
  }
  return ((int)(root));
}

int Round64(int x){
  if( x % GRAIN == 0)
    return x;
  else {
    int q = (int)(x / GRAIN);
    x = q * GRAIN;
    return x;
  }
}

int GetMagnitude(int x_comp, int y_comp){
  int mag;
  unsigned int abs_mag = x_comp * x_comp + y_comp * y_comp;
  mag = intSqRt(abs_mag);
  return mag;
}

int Normalize(int degree){
  while (degree < 0)
    degree += 360;
  while (degree > 360)
    degree -= 360;
  return degree;
}

int Abs(int num){
  if (num < 0)
    return -num;
  return num;
}

int GetAngle(int delta_y, int delta_x){
  if (delta_x == 0 && delta_y == 0)         return 0;
  else if (delta_x == 0 && delta_y > 0)     return 90;
  else if (delta_x == 0 && delta_y < 0)     return 270;

  int x,y;
  if (delta_x < 0)    x = -delta_x;
  else                x = delta_x;
  if (delta_y < 0)    y = -delta_y;
  else                y = delta_y;
  int angle = ATan((int)(1024 * y/x));
  if (delta_x < 0 && delta_y <= 0)           return 180 + angle;
  else if (delta_x < 0 && delta_y > 0)      return 180 - angle;
  else if (delta_x > 0 && delta_y < 0)      return 360 - angle;

  return angle;
}

int Sin(int x){
  if (x < 0 || x > 360)
    x = Normalize(x);

  if (x >= 0 && x <= 90)                    return  sine[x];
  else if (x > 90 && x <= 180)              return  sine[180 - x];
  else if (x > 180 && x <= 270)             return -sine[x - 180];
  else /*if (x > 270 && x <= 360)*/             return -sine[360 - x];
}

int Cos(int x){
  if (x < 0 || x > 360)
    x = Normalize(x);

  if (x >= 0 && x <= 90)                    return  sine[90 - x];
  else if (x > 90 && x <= 180)              return -sine[90 - (180 - x)];
  else if (x > 180 && x <= 270)             return -sine[90 - (x - 180)];
  else /*if (x > 270 && x <= 360)*/             return  sine[90 - (360 - x)];
}

int ATan(int x){
  x = Round64(x);
  if (angle_map.count(x))
    return angle_map[x];

  int count = 0, index = 0, step = GRAIN;
  while (!angle_map.count(x)){
    count++;
    if (x < 2000)   step = 64;
    else if (x > 2000 && x < 5249)  step = 128;
    else if (x > 50000)    return angle_map[58752];
    else if (x > 29000)    return angle_map[29440];
    else if (x > 19000)    return angle_map[19584];
    else if (x > 14000)    return angle_map[14720];
    else if (x > 11000)    return angle_map[11776];
    else if (x > 9000)     return angle_map[9728];
    else if (x > 8000)     return angle_map[8320];
    else if (x > 7000)     return angle_map[7296];
    else if (x > 6000)     return angle_map[6400];
    else if (x > 5248)     return angle_map[5888];
    else                   return 90;

    index = x - count*step;
    if (angle_map.count(index))
      return angle_map[x];

    index = x + count*step;
    if (angle_map.count(index))
      return angle_map[x];
  }
  return angle_map[index];
}
