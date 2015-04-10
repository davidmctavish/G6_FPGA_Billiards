#ifndef PHYSICS_H
#define PHYSICS_H   true

#define FPGA              true
#define DEBUG_COL_ID      false
#define DEBUG_COL_ANGLE   false
#define DEBUG_ANGLE       false
#define DEBUG_COLLISIONS  false
#define DEBUG_MOV_LIST    false
#define DEBUG_TIME        true
#define POS_LOC_IGNORE false

#if !FPGA
#include <stdio.h>
#include <fstream>
#include <assert.h>
using namespace std;
#endif

#include <map>
#include "angles.hpp"

#if !POS_LOC_IGNORE
#include "position_locator.hpp"
#endif  // !POS_LOC_IGNORE

#define NUM_BALLS   16
#define NORMAL      128
#define SHIFT       7    // remember to change this to log2(NORMAL)
#define TRIG_SHIFT  10

#define FRICTION    1    // what should it be?
#define FRICTION_FRAMES   3
//#define MAX_SPEED   (1     * NORMAL)
#define MAX_SPEED   (200)

#define RADIUS      (10     * NORMAL)

#define EAST_WALL   ((640   * NORMAL) - RADIUS)
#define WEST_WALL   ((0     * NORMAL) + RADIUS)
#define NORTH_WALL  ((480   * NORMAL) - RADIUS)
#define SOUTH_WALL  ((0     * NORMAL) + RADIUS)

// Pocket boundaries
#define P1_UPPER  (640     * NORMAL)
#define P1_LOWER  (620     * NORMAL - 2*RADIUS)
#define P2_UPPER  (330     * NORMAL+ RADIUS)
#define P2_LOWER  (310     * NORMAL- RADIUS)
#define P3_UPPER  (20       * NORMAL + 2*RADIUS)
#define P3_LOWER  (0       * NORMAL)

enum OBJECT_TYPE  {SOLIDS,    STRIPES,  EIGHT,     CUE,     TEST};
enum GAME_STATE   {P1_TURN,   P2_TURN,  CALIBRATION};
enum WALL_DIR     {NORTH,     EAST,     SOUTH,      WEST};
//int MAX_READS = 2;
class GameState;
class Ball {

public:
  // Physics properties
  int pos_x;
  int pos_y;
  int speed_x;
  int speed_y;

  // Other properties
  OBJECT_TYPE colour  = SOLIDS;
  int id;

  // State
  bool isMove       = false;
  bool exist        = true;

  // Double-buffered Drawing positions
  int t             = -1;  // For 'hit prediction'

  // Collision Handling ~ Pseudo Hardware
  int col_id        = 0;  // Each bit's index represents a ball
  int prev_col_id   = 0;  // Doubled buffered: previous frame's collisions
  // 17 - North Wall | 18 - East Wall | 19 - South Wall | 20 - West Wall

  // Multi-collisions
  int col_speed_x   = 0;
  int col_speed_y   = 0;
  int num_cols      = 0;  // use this to divide net momentum

  int friction_count= 0;  // used to determine when to apply friction
};

class GameState {
public:
  GAME_STATE state;
  bool done = false;      // if game is finished
  int num_balls;
  Ball *ball[NUM_BALLS];   // List of pointers to all game balls

  // Data for frame updates
  int mov_ids[NUM_BALLS];         // List of moving balls
  int num_movs = 0;

  // Player States
  int turn_id = 0;
  int score[2] = {0,0};

  // Cue states for drawing the cue's position
  int cue_pos_x, cue_pos_y;
  bool cue_down = false;
  int cue_colour = 4;

  // Turn Statues
  bool scratch = false;
  bool scored = false;
  bool cue_first_hit = false;
  bool can_hit_eight_ball[2] = {false, false};
};


// Derivations
unsigned int getSpeed(Ball ball_a);   // perhaps should force this to int?
int collisionAngle(Ball ball_a, Ball ball_b);
int relativeAngle(Ball ball_a, Ball ball_b, int hit_angle);
void getXYComponents(int &x, int &y, int angle, int magnitude);
int getNormalAngle(int hit_angle, Ball ball_a);

// Collision Calculations
bool isCircleCollision(int x, int y);
bool isCollision(Ball *ball_a, Ball *ball_b);
bool isStutter(Ball *ball_a, int id);
//void imminentCollision(Ball * ball_a, Ball *ball_b);
void bufferPreviousCollisions(Ball *ball_a);
void setCollisionBit(Ball *ball_a, int id);

void headOnCollision(Ball *ball_a, Ball *ball_b);
void statCollision(Ball *ball_a, Ball *ball_b, int relative_angle, int hit_angle);
void dynamicCollision(Ball *ball_a, Ball *ball_b, int hit_angle);
void collisionForce(Ball *ball_a, Ball *ball_b);

// Game Rules: Scoring
bool isInPocket(Ball ball_a, int upper, int lower);
void incrementScore(GameState *g);
bool isPlayersBall(GameState *g, int ball_id);
bool isScore(Ball ball_a, WALL_DIR wall);

// Game State Status
bool isWon(GameState g);
bool isEndTurn(GameState g);
bool isMoving(Ball ball_a);

// Ball Status
void updateVelocity(Ball *ball_a, GameState *g);  // plus friction!
void updatePosition(Ball *ball_a, GameState *g);

// Moving Ball List Manager
void addMoveListEntry(GameState *g, int id);
void updateMoveList(GameState *g);
void removeMoveListEntry(GameState *g, int id);
void removeBall(GameState *g, int id, int turn);

// Collision List Manager
void addColListEntry(Ball *ball_a);
void removeColList(GameState *g);

// Bound Ball Properties
void boundSpeed(Ball *ball_a);
void boundPosition(Ball *ball_a, GameState *g);

// Pool Cue
void cueSpeed(int * x_pos, int * y_pos, Ball *cue_ball);
bool cueHit(int cut_x, int cur_x, Ball *cue_ball);
void cuePollPosition(Ball *cue_ball, GameState *g);
void replaceCueBall(Ball *cue_ball, GameState *g);

// After calling "cuePollPosition", the cue ball will be moving.
// Calling it at the start of each players turn should be all that is needed.

// Per Frame Operation
void Step(GameState *g);

// Data Validation
bool dataValid(int address, int bits);

// Initialize Game
//#if !FPGA
//void initGameBalls(GameState *g, string filename);
//#elif FPGA
void initGameBalls(GameState *g);
//#endif

// Manage Scoring
void incrementScore(GameState *g);

void initMoveList(GameState *g);
void scratched_cue(GameState *g);
bool all_balls_sunk (GameState *g, int turn_id);
#if !FPGA
// Debug functions
void printPosition(Ball *ball_a);
void printVelocity(Ball *ball_a);
void printBall(Ball *ball_a);
void printScore(GameState g, int id);
void readLine(fstream &infile, Ball *ball);
#endif

#endif	// PHYSICS_H
