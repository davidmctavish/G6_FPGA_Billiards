#include <stdio.h>
#include <map>
#include <bitset>
#include "physics.hpp"
#include <iostream>
#include <fstream>

//=======================================//
//     DERIVATIONS - Angles, speed
//=======================================//

unsigned int getSpeed(Ball ball_a){
  return GetMagnitude(ball_a.speed_x, ball_a.speed_y);
}

int collisionAngle(Ball ball_a, Ball ball_b) {
  int pos_angle, delta_x, delta_y;

  delta_x = ball_b.pos_x - ball_a.pos_x;
  delta_y = ball_b.pos_y - ball_a.pos_y;
  pos_angle = GetAngle(delta_y, delta_x);

  return pos_angle;
}

// collision angle of the two balls relative to the velocity vector
// of the moving ball.
// Note: This is only used in the event of a glancing, one moving,
// one stationary collision.
int relativeAngle(Ball ball_a, Ball ball_b, int hit_angle){
  int v_angle, net_angle;
  v_angle = GetAngle(ball_a.speed_y, ball_a.speed_x);

  net_angle = hit_angle - v_angle;

  return net_angle;
}

int getNormalAngle(int hit_angle, Ball ball_a){
  int v_angle;
  v_angle = GetAngle(ball_a.speed_y, ball_a.speed_x);
  if (v_angle == 0 && hit_angle > 180)
    v_angle = 360;

  if (v_angle > hit_angle)          return (hit_angle + 90);
  else if (v_angle < hit_angle)     return (hit_angle - 90);
  else                              return hit_angle;
  // this case shouldn't ever occur...
}
//==========================================//
//    COLLISION CALCULATIONS
//=========================================//
bool isCircleCollision(int x, int y){
  int x_sq, y_sq, r_sq;
  x_sq = x * x;
  y_sq = y * y;
  r_sq = RADIUS * RADIUS;   // should be (2*RADIUS)^2 ?

  if (x_sq + y_sq <= r_sq)      return true;
  else                          return false;
}

bool isCollision(Ball *ball_a, Ball *ball_b){
  if (isStutter(ball_a, ball_b->id))    return false;
// check is exist??
  int delta_x, delta_y;
  delta_x = ball_a->pos_x - ball_b->pos_x;
  delta_y = ball_a->pos_y - ball_b->pos_y;
  if ((delta_x <= 2*RADIUS) && (delta_y <= 2*RADIUS) &&
    (-delta_x <= 2*RADIUS) && (-delta_y <= 2*RADIUS)) {

    if (isCircleCollision(delta_x, delta_y)) {
      ball_a->col_id = ball_b->id;
      ball_b->col_id = ball_a->id;

      setCollisionBit(ball_a, ball_b->id);
      setCollisionBit(ball_b, ball_a->id);
      return true;
    }
    else    return false;
  }
  else  return false;
}

bool isStutter(Ball *ball_a, int id){   // true if "stutter" collision
  int bit = (ball_a->prev_col_id >> id) & 0x1;
  if (bit == 0)       return false;
  else                return true;
}

void bufferPreviousCollisions(Ball *ball_a){
  ball_a->prev_col_id = ball_a->col_id;
  ball_a->col_id = 0;
}

void setCollisionBit(Ball *ball_a, int id){
  ball_a->col_id = (ball_a->col_id | (0x1 << id));
  std::bitset<21> x(ball_a->col_id);
  std::bitset<21> y((0x1 << id));
}

void imminentCollision(Ball * ball_a, Ball * ball_b){
  int dx, dy, d_vx, d_vy;
  long long int a, b, c, det;

  d_vx = ball_a->speed_x - ball_b->speed_x;
  d_vy = ball_a->speed_y - ball_b->speed_y;
  dx   = (ball_a->pos_x - ball_a->speed_x) - (ball_b->pos_x - ball_b->speed_x);
  dy   = (ball_a->pos_y - ball_a->speed_y) - (ball_b->pos_y - ball_b->speed_y);

  a    = (long long int)d_vx * d_vx + d_vy * d_vy;
  b    = 2 * ((long long int)dx * d_vx + dy * d_vy);
  c    = (long long int)dx * dx + dy * dy - 4 * RADIUS * RADIUS;

  det  = b * b - 4 * a * c;
  ball_a->t  = (int)((-b - LongSqRt(det))/(2 * a >> 10));
}

void headOnCollision(Ball* ball_a, Ball* ball_b){
#if !FPGA
    cout << "\tHead on collision\n";
#endif
    ball_a->col_speed_x  += ball_b->speed_x;
    ball_a->col_speed_y  += ball_b->speed_y;
    ball_b->col_speed_x  += ball_a->speed_x;
    ball_b->col_speed_y  += ball_a->speed_y;
}

// A is the moving ball
void statCollision(Ball* ball_a, Ball* ball_b, int rel_angle, int hit_angle){
#if !FPGA
  cout << "\tGlancing 1 Ball Stationary collision\n";
#endif
  int speed = intSqRt(ball_a->speed_x * ball_a->speed_x
          + ball_a->speed_y * ball_a->speed_y);
  int bspeed = Abs(Cos(rel_angle)) * speed;    // just a magnitude
  int aspeed = Abs(Sin(rel_angle)) * speed;

  ball_b->col_speed_x += (bspeed * Cos(hit_angle)) >> (10 + SHIFT);
  ball_b->col_speed_y += (bspeed * Sin(hit_angle)) >> (10 + SHIFT);

  int normal;
  normal = getNormalAngle(hit_angle, *ball_a);

  ball_a->col_speed_x += (aspeed * Cos(normal)) >> (10 + SHIFT);
  ball_a->col_speed_y += (aspeed * Sin(normal)) >> (10 + SHIFT);
}

// Collisions for 2 moving balls with non-zero hit angle
// Hit_angle defined as Cartesian angle connecting the two centres
// with respect to ball_a
// Angle_a refers to angle of velocity vector for ball_a
void dynamicCollision(Ball* ball_a, Ball* ball_b, int hit_angle){
  int angle_a, angle_b;
  unsigned int speed_a, speed_b;
#if !FPGA
  cout << "\tDynamic Collision (2ball moving glancing)\n";
#endif
  angle_a = GetAngle(ball_a->speed_y, ball_a->speed_x);
  angle_b = GetAngle(ball_b->speed_y, ball_b->speed_x);

  speed_a = getSpeed(*ball_a);
  speed_b = getSpeed(*ball_b);

  // Note that Cos(x + 90) = -Sin(x)
  //           Sin(x + 90) = Cos(x)
  ball_a->col_speed_x += speed_b * Cos(angle_b - hit_angle) * Cos(hit_angle)
                  + speed_a * Sin(angle_a - hit_angle) * -Sin(hit_angle);
  ball_a->col_speed_y += speed_b * Cos(angle_b - hit_angle) * Sin(hit_angle)
                  + speed_a * Sin(angle_a - hit_angle) * Cos(hit_angle);
  // Reverse hit_angle to perform calculations from B's perspective
  hit_angle = Normalize(hit_angle + 180);
  ball_b->col_speed_x += speed_a * Cos(angle_a - hit_angle) * Cos(hit_angle)
                  + speed_b * Sin(angle_b - hit_angle) * -Sin(hit_angle);
  ball_b->col_speed_y += speed_a * Cos(angle_a - hit_angle) * Sin(hit_angle)
                  + speed_b * Sin(angle_b - hit_angle) * Cos(hit_angle);

  ball_a->col_speed_x = ball_a->col_speed_x >> (10 + SHIFT);
  ball_a->col_speed_y = ball_a->col_speed_y >> (10 + SHIFT);
  ball_b->col_speed_x = ball_b->col_speed_x >> (10 + SHIFT);
  ball_b->col_speed_y = ball_b->col_speed_y >> (10 + SHIFT);
}

// Master Collision event handler
// hit_angle -> the angle create by connecting the centers of the balls
// rel_angle -> the angle between hit_angle and the velocity of the moving ball
//      -> only used for 1-moving ball collisions
//      -> simply = hit_angle - velocity_angle
void collisionForce(Ball *ball_a, Ball *ball_b){
  int hit_angle, rel_angle;  //hit angle - velocity angle

  hit_angle = collisionAngle(*ball_a, *ball_b);

  if (hit_angle == 0)
    headOnCollision(ball_a, ball_b);
  else{
    rel_angle = relativeAngle(*ball_a, *ball_b, hit_angle);
    if (rel_angle == 0 || rel_angle == 180) // Head On
      headOnCollision(ball_a, ball_b);
    // A is stationary
    else if (ball_a->speed_x == 0 && ball_a->speed_y == 0){
      rel_angle = relativeAngle(*ball_b, *ball_a, hit_angle);
      statCollision(ball_b, ball_a, rel_angle, hit_angle);    //first param is moving ball
    }
    // B is stationary
    else if (ball_b->speed_x == 0 && ball_b->speed_y == 0){
      rel_angle = relativeAngle(*ball_a, *ball_b, hit_angle);
      statCollision(ball_a, ball_b, rel_angle, hit_angle);
    }
    // Both balls moving, glancing collision
    else
      dynamicCollision(ball_a, ball_b, hit_angle);
  }
}

//=============================//
//    GAME RULES: SCORING
//=============================//

bool isInPocket(Ball ball_a, int upper, int lower){
  return (ball_a.pos_x > lower && ball_a.pos_x < upper);
}

void incrementScore(GameState *g){
  g->score[g->turn_id]++;
}

bool isPlayersBall(GameState *g, int ball_id){
  if (g->ball[ball_id]->colour == g->turn_id)
    return true;
  return false;
}
// if we always check EAST/WEST first, then to score we must first
// hit EAST/WEST wall.
// Note: North/South pockets also reside on EAST/WEST walls
bool isScore(Ball ball_a, WALL_DIR wall){
  bool isScore = false, s1 = false, s2 = false, s3 = false;

  if (wall == NORTH || wall == SOUTH) {
    s1 = isInPocket(ball_a, P1_UPPER, P1_LOWER);
    s2 = isInPocket(ball_a, P2_UPPER, P2_LOWER);
    s3 = isInPocket(ball_a, P3_UPPER, P3_LOWER);
  }
  else  return false;

  isScore = s1 || s2 || s3;
  return isScore;
}

//==========================================//
//      GAME STATE STATUS
//==========================================//

bool isWon(GameState g){        return g.done;  }
bool isEndTurn(GameState g){    return (g.num_movs == 0);  }
bool isMoving(Ball ball_a){
  if (ball_a.speed_x == 0 && ball_a.speed_y == 0)
    return false;
  return true;
}

//=======================================//
//      BALL STATUS
//======================================//

void updateVelocity(Ball *ball_a, GameState *g){
  if (ball_a->num_cols > 0){
    ball_a->speed_x = ball_a->col_speed_x / ball_a->num_cols;
    ball_a->speed_y = ball_a->col_speed_y / ball_a->num_cols;
  }

  if (ball_a->speed_x - FRICTION > 0)         ball_a->speed_x -= FRICTION;
  else if (ball_a->speed_x + FRICTION < 0)    ball_a->speed_x += FRICTION;
  else                                        ball_a->speed_x = 0;

  if (ball_a->speed_y - FRICTION > 0)         ball_a->speed_y -= FRICTION;
  else if (ball_a->speed_y + FRICTION < 0)    ball_a->speed_y += FRICTION;
  else                                        ball_a->speed_y = 0;

  if (ball_a->speed_y == 0 && ball_a->speed_x == 0)
    removeMoveListEntry(g, ball_a->id);
}

void updatePosition(Ball *ball_a, GameState *g){
  ball_a->pos_x += ball_a->speed_x;
  ball_a->pos_y += ball_a->speed_y;

  boundPosition(ball_a, g);
}

//========================================//
//    MOVING BALL LIST MANAGER
//=======================================//

void addMoveListEntry(GameState *g, int id){
  int i;
  for (i = 0; i < g->num_movs; i++){
    if (g->mov_ids[i] == id)
      return;
  }
  g->num_movs++;
  g->mov_ids[g->num_movs -1] = id;
}

void removeMoveListEntry(GameState *g, int id){
  int i;
  for (i = 0; i < g->num_movs; i++){
    if (g->mov_ids[i] == id){
      while (i+1 < g->num_movs){
        g->mov_ids[i] = g->mov_ids[i+1];
        i++;
      }
      g->num_movs--;
    }
  }
}

// TODO: Test score keeping for players
void removeBall(GameState *g, int id, int turn){
	  g->ball[id]->exist = false;
	  g->num_balls--;
	  if (isPlayersBall(g, id)) {
		  g->scored = true;
	  } else {
		  g->scratch = true;
	  }
	  if (g->num_balls == 0)
	    g->done = true;
	  removeMoveListEntry(g, id);
}

//==========================================//
//      COLLISION LIST MANAGER
//==========================================//
// stores number of collisions encountered during this frame/step
void addColListEntry(Ball * ball_a){
  ball_a->num_cols++;
}

void resetColList(GameState *g){
  int i;
  for (i = 0; i < NUM_BALLS; i++){
    g->ball[i]->num_cols = 0;
    g->ball[i]->col_speed_x = 0;
    g->ball[i]->col_speed_y = 0;
  }
}

//==========================================//
//      BOUND BALL PROPERTIES
//==========================================//

// Ensure ball does not leave the table
void boundPosition(Ball *ball_a, GameState *g){
  if (ball_a->pos_y < SOUTH_WALL){
    if (isStutter(ball_a, 19))      return;
    setCollisionBit(ball_a, 19);

    if (isScore(*ball_a, SOUTH)){
#if !FPGA
      cout << "\t*****Scored!!!\n";
#endif
      removeBall(g, ball_a->id, g->turn_id);
    }
    else {
#if !FPGA
      cout << "\tHit SOUTH wall\n";
#endif
      ball_a->pos_y = SOUTH_WALL + SOUTH_WALL - ball_a->pos_y;
      ball_a->speed_y = -ball_a->speed_y;
    }
  }
  else if (ball_a->pos_y > NORTH_WALL){
    if (isStutter(ball_a, 17))      return;
    setCollisionBit(ball_a, 17);

    if (isScore(*ball_a, NORTH)){
#if !FPGA
      cout << "\t*****Scored!!!\n";
#endif
      removeBall(g, ball_a->id, g->turn_id);
    }
    else  {
#if !FPGA
      cout << "\tHit NORTH wall\n";
#endif
      ball_a->pos_y = NORTH_WALL + NORTH_WALL - ball_a->pos_y;
      ball_a->speed_y = -ball_a->speed_y;
    }
  }

  if (ball_a->pos_x < WEST_WALL){
    if (isStutter(ball_a, 20))      return;
    setCollisionBit(ball_a, 20);
#if !FPGA
      cout << "\tHit WEST wall\n";
#endif
    ball_a->pos_x = WEST_WALL + WEST_WALL - ball_a->pos_x;
    ball_a->speed_x = -ball_a->speed_x;
  }
  else if (ball_a->pos_x > EAST_WALL){
    if (isStutter(ball_a, 18))      return;
    setCollisionBit(ball_a, 18);
#if !FPGA
      cout << "\tHit EAST wall\n";
#endif
    ball_a->pos_x = EAST_WALL + EAST_WALL - ball_a->pos_x;
    ball_a->speed_x = -ball_a->speed_x;
  }
}

// Ensure ball does not move too fast for our rendering purposes
void boundSpeed(Ball *ball_a){
  if (ball_a->speed_x > MAX_SPEED)          ball_a->speed_x =  MAX_SPEED;
  else if (ball_a->speed_x < -MAX_SPEED)    ball_a->speed_x = -MAX_SPEED;
  if (ball_a->speed_y > MAX_SPEED)          ball_a->speed_y =  MAX_SPEED;
  else if (ball_a->speed_y < -MAX_SPEED)    ball_a->speed_y = -MAX_SPEED;
}

//=============================================//
//      POOL CUE
//=============================================//
#if !POS_LOC_IGNORE

#define CUE_POSITIONS_STORED 3

void cueSpeed(int * x_pos, int * y_pos, Ball *cue_ball){
  // Computer the average speed over whcih the points were collected
  cue_ball->speed_x = (x_pos[CUE_POSITIONS_STORED-1] - x_pos[0]) / (CUE_POSITIONS_STORED-1);
  cue_ball->speed_y = (y_pos[CUE_POSITIONS_STORED-1] - y_pos[0]) / (CUE_POSITIONS_STORED-1);
  #if !FPGA
  cout << "cue ball speed: " << std::dec << cue_ball->speed_x << std::endl;
  cout << "cue ball speed: " << std::dec << cue_ball->speed_y << std::endl;
  cout << std::endl << std::endl;
  #endif
  boundSpeed(cue_ball);
  #if !FPGA
  cout << "bounded cue ball speed: " << std::dec << cue_ball->speed_x << std::endl;
  cout << "bounded cue ball speed: " << std::dec << cue_ball->speed_y << std::endl;
  cout << std::endl << std::endl;
  #endif
}

void cuePollPosition(Ball *cue_ball, GameState *g)
{
  int x_pos[CUE_POSITIONS_STORED], y_pos[CUE_POSITIONS_STORED], i;

  unsigned int positionLocatorValue;
  int cur_x, cur_y;

  i = 0;

  // Record a number of consecutive valid positions.
  // Once the cue stick hits the cue ball, give the cue ball
  //  it's speed and exit the function.
  do {
    // poll for a new valid cue position
    positionLocatorValue = pollPositionLocator();
    cur_x = POSLOC_getX(positionLocatorValue);
    cur_y = POSLOC_getY(positionLocatorValue);
    #if !FPGA
    cout << "camera x position: " << std::dec << cur_x << std::endl;
    cout << "camera y position: " << std::dec << cur_y << std::endl;
    #endif

    // if the position is NOT on the table, discard all current values
    if (!positionIsOnTable(cur_x, cur_y)) {
      i = 0;
      g->cue_down = false;
      #if FPGA
      DrawState(g);
      #endif
    } else { // if it is on the table, update the list of recent points
      // convert to table coordinates
      convertCameraCordinateToTable(&cur_x, &cur_y);
      g->cue_pos_x = cur_x;
      g->cue_pos_y = cur_y;
      g->cue_down = true;
      #if FPGA
      DrawState(g);
      #endif
      // add to the list
      if (i == CUE_POSITIONS_STORED) {
	    for (int j=0; j < CUE_POSITIONS_STORED-1; j++) {
          x_pos[j] = x_pos[j+1];
	      y_pos[j] = y_pos[j+1];
        }
        x_pos[CUE_POSITIONS_STORED-1] = cur_x;
        y_pos[CUE_POSITIONS_STORED-1] = cur_y;
      } else {
        x_pos[i] = cur_x;
        y_pos[i] = cur_y;
        i++;
      }
    }
    // End the loop when:
    //  a.) at least CUE_POSITIONS_STORED points are collected AND
    //  b.) The most recent point is hitting the cue ball
    #if !FPGA
    cout << "cue positions tracked: " << i << std::endl;
    cout << std::endl;
    #endif
  } while ( (i < CUE_POSITIONS_STORED) || !cueHit(cur_x, cur_y, cue_ball) );

  cueSpeed(x_pos, y_pos, cue_ball);
}

bool cueHit(int cur_x, int cur_y, Ball *cue_ball){
  // Check if (cur_x, cur_y) overlaps with the cueball
  int x_sq, y_sq, r_sq;
  int x = cue_ball->pos_x - cur_x;
  int y = cue_ball->pos_y - cur_y;
  x_sq = x * x;
  y_sq = y * y;
  r_sq = RADIUS * RADIUS;   // should be (2*RADIUS)^2 ?

  if (x_sq + y_sq <= r_sq)      return true;
  else                          return false;
}

//=================================//
//    Replacing of Cue Ball
//================================//

// the player will move the cue over where they want it, then flip the switch
void replaceCueBall(Ball *cue_ball, GameState *g)
{
  unsigned int positionLocatorValue;
  int cur_x, cur_y;
  bool BallPosValid = false;
  cue_ball->exist = false;

  do {
    positionLocatorValue = pollPositionLocator();
    cur_x = POSLOC_getX(positionLocatorValue);
    cur_y = POSLOC_getY(positionLocatorValue);
    if (positionIsOnTable(cur_x, cur_y)) {
      convertCameraCordinateToTable(&cur_x, &cur_y);
      cue_ball->pos_x = cur_x;
      cue_ball->pos_y = cur_y;
      // check for collisions with other balls
      // not necessary to check with cue ball
      BallPosValid = true;
      for (int i=1; i<NUM_BALLS; i++) {
        if (g->ball[i]->exist &&
            isCircleCollision(cue_ball->pos_x - g->ball[i]->pos_x,
                              cue_ball->pos_y - g->ball[i]->pos_y)) {
          // if collision, cannot put cue_ball here
          BallPosValid = false;
        }
      }
			// TODO: also check if on table
    }
  } while (!BallPosValid);
  cue_ball->exist = true;

  // before leaving this function, have to make sure the cue has been removed from the cue ball first
  // Wait for the cue to be in a valid position not colliding with the cue ball
  bool onTable;
  do {
    onTable = false;
    positionLocatorValue = pollPositionLocator();
    cur_x = POSLOC_getX(positionLocatorValue);
    cur_y = POSLOC_getY(positionLocatorValue);
    if (positionIsOnTable(cur_x, cur_y)) {
      onTable = true;
      convertCameraCordinateToTable(&cur_x, &cur_y);
    }
  } while (onTable && cueHit(cur_x, cur_y, cue_ball));
}

#endif  // !POS_LOC_IGNORE


//=================================//
//    PER FRAME OPERATION
//================================//

// Processes all infomration for each frame
// Iterates over all the moving balls (list)
void Step(GameState *g){
  int i, j, a, iterations;
  Ball * my_ball;
  iterations = g->num_movs;   // otherwise, if a ball stops, we decrement this
                              // and lose a cycle of movement
  // Update Positions
  for (i = 0; i < g->num_movs; i++){
    a = g->mov_ids[i];
    if (!g->ball[a]->exist) {
      removeMoveListEntry(g,a);
      i--;
      continue;
    }
    updatePosition(g->ball[a] , g);
  }

  // Check for Collisions - calculate min t
  iterations = g->num_movs;
  for (i = 0; i < iterations; i++){
    a = g->mov_ids[i];
    for (j = 0; j < NUM_BALLS; j++){
      if (a != j && g->ball[j]->exist){
        if (isCollision(g->ball[a], g->ball[j]))
          imminentCollision(g->ball[a], g->ball[j]);
      }
    }
  }

  // Apply min t
  iterations = g->num_movs;
  for (i = 0; i < iterations; i++){
    my_ball = g->ball[g->mov_ids[i]];
    if (my_ball->t != 0) {
      my_ball->t -= 1024;
#if !FPGA
    cout << "Applying min T to ball " << my_ball->id << " with time: "
      << my_ball->t << endl;
#endif
      g->ball[g->mov_ids[i]]->pos_x = my_ball->pos_x
        + (my_ball->speed_x * my_ball->t >> 10);
      g->ball[g->mov_ids[i]]->pos_y = my_ball->pos_y
        + (my_ball->speed_y * my_ball->t >> 10);
    }
  }

  // Calculate Collisions
  iterations = g->num_movs;
  for (i = 0; i < iterations; i++){
    my_ball = g->ball[g->mov_ids[i]];
    if (my_ball->t != 0) {
      for (j = 0; j < NUM_BALLS; j++){
      // calculate the collision
        if ((my_ball->col_id >> j) & 0x1){
          addMoveListEntry(g, my_ball->id);
          addMoveListEntry(g, g->ball[j]->id);
          addColListEntry(my_ball);
          addColListEntry(g->ball[j]);
          collisionForce(my_ball, g->ball[j]);
          if (my_ball->id == 0 || g->ball[j]->id == 0) {
          	if ((my_ball->id == 8 || g->ball[j]->id == 8) && g->can_hit_eight_ball[g->turn_id]) {
          		g->cue_first_hit = true;
          		break;
          	} else if (my_ball->colour == g->turn_id || g->ball[j]->colour == g->turn_id){
          		g->cue_first_hit = true;
          		break;
          	} else {
          		g->cue_first_hit = true;
          		g->scratch = true;
          		break;
          	}
          }
        }
      }
      my_ball->t = 0;
    }
  }

  // Update Velocity
  iterations = g->num_movs;
  for (i = 0; i < iterations; i++){
    updateVelocity(g->ball[g->mov_ids[i]], g);
    bufferPreviousCollisions(g->ball[g->mov_ids[i]]);
#if !FPGA
    printBall(g->ball[g->mov_ids[i]]);
#endif
  }
  resetColList(g);
}

//====================================//
//      VALIDATE INPUT
//====================================//

bool dataValid(int address, int bits){
  if ((address & bits) == 0)
    return false;
  return true;
}

//====================================//
//      INITIALIZE GAME
//====================================//

void initGameBalls(GameState *g){
  g->num_balls = NUM_BALLS;
#if !FPGA
  int i = 0;
  fstream infile("init.txt", ios_base::in);
  for (i = 0; i < NUM_BALLS; i++){
    g->ball[i] = new Ball();
    g->ball[i]->id = i;
    g->ball[i]->col_id = i;    // maybe don't need this anymore?
    readLine(infile, g->ball[i]);
    printPosition(g->ball[i]);
  }
#elif FPGA
  g->ball[0] = new Ball();
  g->ball[0]->id = 0;
  g->ball[0]->col_id = 0;
  g->ball[0]->pos_x = 100 * NORMAL;
  g->ball[0]->pos_y = 100 * NORMAL;
  g->ball[0]->speed_x = 1 * NORMAL;
  g->ball[0]->speed_y = 1 * NORMAL;

  g->ball[1] = new Ball();
  g->ball[1]->id = 1;
  g->ball[1]->col_id = 1;
  g->ball[1]->pos_x = 180 * NORMAL;
  g->ball[1]->pos_y = 175 * NORMAL;
  g->ball[1]->speed_x = -1 * NORMAL;
  g->ball[1]->speed_y = -1 * NORMAL;
#endif
}

//==================================//
//    Debug Code
//==================================//

// Populate the move list
void initMoveList(GameState *g){
  int i;

  for (i = 0; i < NUM_BALLS; i++){
    if (g->ball[i]->exist){
      if (isMoving(*g->ball[i])){
        g->mov_ids[g->num_movs] = i;
        g->num_movs++;
      }
    }
  }
}

void scratched_cue(GameState *g) {
	if (!(g->ball[0]->exist)){
		g->scratch = true;
	}
}

bool all_balls_sunk (GameState *g, int turn_id){
	for (int i = 0; i < NUM_BALLS; i++) {
		if ((g->ball[i]->colour == turn_id) && (g->ball[i]->exist)){
			return false;
		}
	}
	return true;
}
#if !FPGA
void printVelocity(Ball *ball_a){
    cout << "\tVelocity of Ball " << ball_a->id << ": (" << ball_a->speed_x
        << ", " << ball_a->speed_y << ")\n";
}
void printPosition(Ball *ball_a){
    cout << "\tPosition of Ball " << ball_a->id << ": (" << ball_a->pos_x
        << ", " << ball_a->pos_y << ")\n";
}

void printBall(Ball *ball_a){
    cout << "\tPosition of Ball " << ball_a->id << ": (" << ball_a->pos_x
        << ", " << ball_a->pos_y << ") \t\t\t";
    cout << "\tVelocity of Ball " << ball_a->id << ": (" << ball_a->speed_x
        << ", " << ball_a->speed_y << ")\n";
}

void printScore(GameState g, int id){
  cout << "\tPlayer " << id << " has " << g.score[id] << " points\n";
}

void readLine(fstream &infile, Ball *ball){
  infile >> ball->pos_x >> ball->pos_y >> ball->speed_x >> ball->speed_y;
}
#endif
/* TODO LIST:
 *
 * Implement Collision Tracking
 * - Divide into collision zones (so we don't have O(n^2) )
 */
