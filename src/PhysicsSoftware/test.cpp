#include "physics.hpp"
#if !FPGA
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;
#endif

int TEST_FRAMES = 10;
GameState *g = new GameState();

// should probably make this the new main....
void runSimulation(){
  int counter = 0;

  while (counter < TEST_FRAMES) {
#if !FPGA
    cout << "\nFrame " << counter << endl;
#endif
    Step(g);
    // TODO: Chris should put draw function here.
    // GameState object is a pointer, called g.
    if (isEndTurn(*g)){
#if !FPGA
      cout << "Balls have stopped moving\n";
#endif
      break;
    }
    counter++;
  }
}
void testLongSqRt(unsigned long long int num){
  unsigned long long int root;
  root = LongSqRt(num);
  cout << "Root of " << num << " is calculated as " << root
    << " vs " << intSqRt((int)num) << endl;// << " with difference " << root - intSqRt((int)num)<< endl;
}

int main(int argc, char* argv[]){
  string filename = "init.txt";
#if !FPGA
  g->num_balls = 2;
  for (int i = 1; i < argc; i++){
    string arg = argv[i];
    string prev = argv[i-1];
    if (prev.compare("FPS") == 0)             TEST_FRAMES = stoi(arg);
    else if (prev.compare("BALLS") == 0)      g->num_balls = stoi(arg);
    else if (prev.compare("TEST") == 0)       filename = arg;
  }
  /*TEST_FRAMES = 400;
  g->num_balls = 7;
  filename = "testcases/7ball_break.txt";*/
#endif
  // Pysics Tests
  //initGameBalls(g, filename);
  initGameBalls(g);
  initMoveList(g);
  runSimulation();

  // test for position locator
  #if !POS_LOC_IGNORE
  cout << std::endl << std::endl << "PosLoc Test" << std::endl;
  Ball *CueBall = new Ball();
  CueBall->id = 0;
  CueBall->col_id = 0;    // maybe don't need this anymore?
  CueBall->colour = CUE;
  CueBall->pos_x = 100 * NORMAL;
  CueBall->pos_y = 100 * NORMAL;
  CueBall->speed_x = 0 * NORMAL;
  CueBall->speed_y = 0 * NORMAL;
  printPosition(CueBall);
  cuePollPosition(CueBall, g);
  cuePollPosition(CueBall, g);
  #endif
}
