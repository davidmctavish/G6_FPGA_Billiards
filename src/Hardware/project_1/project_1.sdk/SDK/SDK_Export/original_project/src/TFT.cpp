#include <stdio.h>

#include "TFT.hpp"

#if !TFT_IGNORE

#if FPGA
#include "xparameters.h"
#endif	// FPGA

#define TEST_FRAMES   600
#define BLUE 0x000000FC
#define NOT_AS_BRIGHT_GREEN 0x00008000
#define CYAN 0x0000FCFC
#define WHITE 0x00FCFCFC
#define RED 0x00FC0000
#define BLACK 0x00000000
#define XPAR_MIG_7SERIES_0_BASEADDR_ALT (XPAR_MIG_7SERIES_0_BASEADDR + 0x04000000)
#define TFT_MASK 0x00000001
#define XCENTER 315
#define YCENTER 10

volatile unsigned int * tftptr = (unsigned int*) XPAR_AXI_TFT_0_BASEADDR;
volatile unsigned int * memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR;
volatile unsigned int * memptr_alt = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR_ALT;
int tft_array_x [NUM_BALLS][2] = {0};
int tft_array_y [NUM_BALLS][2] = {0};
int cue_pos_x_array[2] = {0};
int cue_pos_y_array[2] = {0};

void init_tft () {
    tftptr[0] = (unsigned int) memptr_alt;
    tftptr[1] = 0x00000001;
}

void DrawBackground (int colour)
{
	DrawRectangle (0, 0, 640, 480, colour);
	memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR_ALT;
	DrawRectangle (0, 0, 640, 480, colour);
	memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR;
}

void DrawScore(int xcenter, int ycenter, int p1_score, int p2_score, GameState *g)
{

	int right_center, left_center;
	right_center = xcenter + 20;
	left_center = xcenter - 20;
	DrawRectangle ((left_center - 5), (ycenter - 4), 10, 9, 1);
	DrawRectangle ((right_center - 5), (ycenter - 4), 10, 9, 1);
	DrawCircle((left_center - 10), ycenter, 2, 3);
	DrawCircle((right_center + 10), ycenter, 2, 3);
	DrawNumber(left_center, ycenter, p1_score);
	DrawNumber(right_center, ycenter, p2_score);

	if (isEndTurn(*g)) {
		if (g->turn_id == 0) {
			if (g->scratch){
				DrawCircle((left_center - 10), ycenter, 2, 0);
			} else if (all_balls_sunk(g, g->turn_id)) {
				DrawCircle((left_center - 10), ycenter, 2, 4);
			} else {
				DrawCircle((left_center - 10), ycenter, 2, 1);
			}
		}	else {
			if (g->scratch){
				DrawCircle((right_center + 10), ycenter, 2, 0);
			} else if (all_balls_sunk(g, g->turn_id)) {
				DrawCircle((right_center + 10), ycenter, 2, 4);
			} else {
				DrawCircle((right_center + 10), ycenter, 2, 2);
			}
		}
	}
}

void DrawPockets ()
{
	DrawRectangle (0, 0, 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
	DrawRectangle ((640 - (RADIUS >> SHIFT)), 0, 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
	DrawRectangle (0, 480 - (RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
	DrawRectangle (640 - (RADIUS >> SHIFT), 480 - (RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
	DrawRectangle (315 - (RADIUS >> SHIFT), 480 - (RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
	DrawRectangle (315 - (RADIUS >> SHIFT), 0, 2*(RADIUS >> SHIFT), 2*(RADIUS >> SHIFT), 0);
}

void DrawRectangle (int xstart, int ystart, int width, int height, int colour)
{


    int x,y;
    for (y = ystart; y < (ystart + height); y++) {
        for (x = xstart; x < (xstart + width); x++) {
        	if (colour == 0){
        		memptr[(1024*y) + (x)] = BLACK;
        	} else if (colour == 1 ){
        		memptr[(1024*y) + (x)] = NOT_AS_BRIGHT_GREEN;
        	} else if (colour == 2){
        		memptr[(1024*y) + (x)] = BLUE;
        	} else if (colour == 3){
        		memptr[(1024*y) + (x)] = CYAN;
        	} else if (colour == 4){
        		memptr[(1024*y) + (x)] = RED;
        	} else {
        		memptr[(1024*y) + (x)] = WHITE;
        	}
        }
    }
}

void DrawM(int x0, int y0) {
	memptr[(1024*y0) + (x0)] = WHITE;
	memptr[(1024*y0) + (x0 - 1)] = WHITE;
	memptr[(1024*y0) + (x0 + 1)] = WHITE;
	memptr[(1024*y0) + (x0 - 2)] = WHITE;
	memptr[(1024*y0) + (x0 + 2)] = WHITE;

}
void DrawT(int x0, int y0){
	memptr[(1024* (y0 - 4)) + (x0)] = WHITE;
	memptr[(1024* (y0 - 4)) + (x0 - 1)] = WHITE;
	memptr[(1024* (y0 - 4)) + (x0 + 1)] = WHITE;
	memptr[(1024* (y0 - 4)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 - 4)) + (x0 + 2)] = WHITE;
}
void DrawB(int x0, int y0) {
	memptr[(1024* (y0 + 4)) + (x0)] = WHITE;
	memptr[(1024* (y0 + 4)) + (x0 - 1)] = WHITE;
	memptr[(1024* (y0 + 4)) + (x0 + 1)] = WHITE;
	memptr[(1024* (y0 + 4)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 + 4)) + (x0 + 2)] = WHITE;

}
void DrawUR(int x0, int y0) {
	memptr[(1024* (y0 - 4)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 - 3)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 - 2)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 - 1)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0)) + (x0 + 2)] = WHITE;
}
void DrawBR(int x0, int y0) {
	memptr[(1024* (y0 + 4)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 + 3)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 + 2)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0 + 1)) + (x0 + 2)] = WHITE;
	memptr[(1024* (y0)) + (x0 + 2)] = WHITE;
}
void DrawUL(int x0, int y0) {
	memptr[(1024* (y0 - 4)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 - 3)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 - 2)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 - 1)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0)) + (x0 - 2)] = WHITE;
}
void DrawBL(int x0, int y0) {
	memptr[(1024* (y0 + 4)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 + 3)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 + 2)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0 + 1)) + (x0 - 2)] = WHITE;
	memptr[(1024* (y0)) + (x0 - 2)] = WHITE;
}

void DrawNumber(int x0, int y0, int number)
{
	switch (number) {
	case 0:
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		DrawUL(x0, y0);
		DrawBL(x0, y0);
		break;
	case 1:
		DrawUR(x0 - 2, y0);
		DrawBR(x0 - 2, y0);
		break;
	case 2:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUR(x0, y0);
		DrawBL(x0, y0);
		break;
	case 3:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		break;
	case 4:
		DrawM(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		DrawUL(x0, y0);
		break;
	case 5:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUL(x0, y0);
		DrawBR(x0, y0);
		break;
	case 6:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUL(x0, y0);
		DrawBL(x0, y0);
		DrawBR(x0, y0);
		break;
	case 7:
		DrawT(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		break;
	case 8:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		DrawUL(x0, y0);
		DrawBL(x0, y0);
		break;
	case 9:
		DrawM(x0, y0);
		DrawT(x0, y0);
		DrawB(x0, y0);
		DrawUR(x0, y0);
		DrawBR(x0, y0);
		DrawUL(x0, y0);
		break;
	case 10:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawT(x0 + 1, y0);
		DrawB(x0 + 1, y0);
		DrawUR(x0 + 1, y0);
		DrawBR(x0 + 1, y0);
		DrawUL(x0 + 1, y0);
		DrawBL(x0 + 1, y0);
	break;
	case 11:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawUR(x0 + 1, y0);
		DrawBR(x0 + 1, y0);
	break;
	case 12:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawM(x0 + 1, y0);
		DrawT(x0 + 1, y0);
		DrawB(x0 + 1, y0);
		DrawUR(x0 + 1, y0);
		DrawBL(x0 + 1, y0);
	break;
	case 13:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawM(x0 + 1, y0);
		DrawT(x0 + 1, y0);
		DrawB(x0 + 1, y0);
		DrawUR(x0 + 1, y0);
		DrawBR(x0 + 1, y0);
	break;
	case 14:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawM(x0 + 1, y0);
		DrawUR(x0 + 1, y0);
		DrawBR(x0 + 1, y0);
		DrawUL(x0 + 1, y0);
	break;
	case 15:
		DrawUR(x0 - 5, y0);
		DrawBR(x0 - 5, y0);
		DrawM(x0 + 1, y0);
		DrawT(x0 + 1, y0);
		DrawB(x0 + 1, y0);
		DrawBR(x0 + 1, y0);
		DrawUL(x0 + 1, y0);
	break;
	}
}

void DrawCircle(int x0, int y0, int radius, int colour)
{
  int x = radius;
  int y = 0;
  int radiusError = 1-x;

  while(x >= y)
  {
	// Draw a line at height y (and -y) from -x to +x
	int col = -x;
	while (col <= x) {
        if (colour == 0){
            memptr[x0+col + 1024*(y0+y)] = WHITE;
            memptr[x0+col + 1024*(y0-y)] = WHITE;
            memptr[x0+y + 1024*(y0+col)] = WHITE;
            memptr[x0-y + 1024*(y0-col)] = WHITE;
        } else if (colour == 1) {
            memptr[x0+col + 1024*(y0+y)] = BLUE;
            memptr[x0+col + 1024*(y0-y)] = BLUE;
            memptr[x0+y + 1024*(y0+col)] = BLUE;
            memptr[x0-y + 1024*(y0-col)] = BLUE;
		} else if (colour == 2){
            memptr[x0+col + 1024*(y0+y)] = CYAN;
            memptr[x0+col + 1024*(y0-y)] = CYAN;
            memptr[x0+y + 1024*(y0+col)] = CYAN;
            memptr[x0-y + 1024*(y0-col)] = CYAN;
        } else if (colour == 3) {
        	memptr[x0+col + 1024*(y0+y)] = NOT_AS_BRIGHT_GREEN;
        	memptr[x0+col + 1024*(y0-y)] = NOT_AS_BRIGHT_GREEN;
        	memptr[x0+y + 1024*(y0+col)] = NOT_AS_BRIGHT_GREEN;
        	memptr[x0-y + 1024*(y0-col)] = NOT_AS_BRIGHT_GREEN;
        } else if (colour == 4) {
        	memptr[x0+col + 1024*(y0+y)] = BLACK;
        	memptr[x0+col + 1024*(y0-y)] = BLACK;
        	memptr[x0+y + 1024*(y0+col)] = BLACK;
        	memptr[x0-y + 1024*(y0-col)] = BLACK;
        }
		col++;
	}

    /*memptr[(x + x0) + 1024*(y + y0)] = 0x0000FCFC;
    memptr[(y + x0) + 1024*(x + y0)] = 0x0000FCFC;
    memptr[(-x + x0) + 1024*(y + y0)] = 0x0000FCFC;
    memptr[(-y + x0) + 1024*(x + y0)] = 0x0000FCFC;
    memptr[(-x + x0) + 1024*(-y + y0)] = 0x0000FCFC;
    memptr[(-y + x0) + 1024*(-x + y0)] = 0x0000FCFC;
    memptr[(x + x0) + 1024*(-y + y0)] = 0x0000FCFC;
    memptr[(y + x0) + 1024*(-x + y0)] = 0x0000FCFC;*/
    y++;
    if (radiusError<0)
    {
      radiusError += 2 * y + 1;
    }
    else
    {
      x--;
      radiusError += 2 * (y - x) + 1;
    }
  }
}

void DrawState (GameState *g){


	volatile unsigned int * memptr_swp;
	unsigned int status;
	int i, x_center, y_center;
    //DrawRectangle(west_wall, south_wall, east_wall, north_wall);
    //DrawRectangle (0, 0, 640, 480);
    //loop through NUM_BALLS
    //Check if ball exists

    // un-draw the balls
    for (i = 0; i < NUM_BALLS; i++) {
    	DrawCircle(tft_array_x[i][1], tft_array_y[i][1], (RADIUS >> SHIFT), 3);
    }

    // undraw the cue posisition
    DrawCircle(cue_pos_x_array[1], cue_pos_y_array[1], 2, 3);

    // Save the old positions
    for (i = 0; i < NUM_BALLS; i++) {
        x_center = ((g->ball[i]->pos_x) >> SHIFT);
        y_center = ((g->ball[i]->pos_y) >> SHIFT);
        tft_array_x[i][1] = tft_array_x[i][0];
        tft_array_x[i][0] = x_center;
        tft_array_y[i][1] = tft_array_y[i][0];
        tft_array_y[i][0] = y_center;
    }

    // draw all the balls
    for (i = 0; i < NUM_BALLS; i++) {
        if (g->ball[i]->exist) {
            x_center = ((g->ball[i]->pos_x) >> SHIFT);
            y_center = ((g->ball[i]->pos_y) >> SHIFT);
            if ( g->ball[i]->colour == CUE) {
                DrawCircle(x_center, y_center, (RADIUS >> SHIFT), 0);
            } else if ( g->ball[i]->colour == SOLIDS) {
                DrawCircle(x_center, y_center, (RADIUS >> SHIFT), 1);
                DrawNumber(x_center, y_center, g->ball[i]->id);
            } else if ( g->ball[i]->colour == STRIPES) {
                DrawCircle(x_center, y_center, (RADIUS >> SHIFT), 2);
                DrawNumber(x_center, y_center, g->ball[i]->id);
            } else if ( g->ball[i]->colour == EIGHT) {
                DrawCircle(x_center, y_center, (RADIUS >> SHIFT), 4);
                DrawNumber(x_center, y_center, g->ball[i]->id);
            }
        }
    }
    // draw cue_position
    int cue_pos_x_center = (g->cue_pos_x) / NORMAL;
    int cue_pos_y_center = (g->cue_pos_y) / NORMAL;
    cue_pos_x_array[1] = cue_pos_x_array[0];
    cue_pos_x_array[0] = cue_pos_x_center;
    cue_pos_y_array[1] = cue_pos_y_array[0];
    cue_pos_y_array[0] = cue_pos_y_center;
    if (g->cue_down) {
    	DrawCircle(cue_pos_x_center, cue_pos_y_center, 2, g->cue_colour);
    }

    DrawPockets ();
    DrawScore(XCENTER, YCENTER, 1, 2, g);

    // Swap buffers
    memptr_swp = memptr;
    memptr = memptr_alt;
    memptr_alt = memptr_swp;
    init_tft();
    status = tftptr[2] & TFT_MASK;
    while (status == 0) {
    	status = tftptr[2] & TFT_MASK;
    }
}


GameState *g = new GameState();

// should probably make this the new main....
void runSimulation(){
  g->scored = false;
  g->scratch = false;
  g->cue_first_hit = false;
  g->can_hit_eight_ball[g->turn_id] = all_balls_sunk(g, g->turn_id);

  while ( 1) {
#if !FPGA
    cout << "\nFrame " << counter << endl;
#endif
    DrawState(g);
    Step(g);
    if (isEndTurn(*g)){
#if !FPGA
      cout << "Balls have stopped moving\n";
#endif
      break;
    }
  }


  DrawState(g);
}

int main(){
  init_tft();
  //initPositionLocator();
  DrawBackground(1);
  initGameBalls(g);
  initMoveList(g);
  DrawState(g);
  bool continueGame = true;
  while(continueGame) {
	DrawState(g);
    cuePollPosition(g->ball[0], g);
    runSimulation();
    // OTHER GAME LOGIC HERE
    if (g->ball[8]->exist == false) {
    	if (g->scratch) {
    		if (g->turn_id == 0) {
    			DrawBackground(3);
    			continueGame = false;
    		} else {
    			DrawBackground(2);
    			continueGame = false;
    		}
    	}	else {
    		if (g->turn_id == 0) {
    			DrawBackground(2);
    			continueGame = false;
    		} else {
    			DrawBackground(3);
    			continueGame = false;
    		}
    	}
    } else {
    	if (g->scratch) {
    		g->turn_id = (g->turn_id == 0);
    		DrawState(g);
    		replaceCueBall(g->ball[0], g);
    		g->scratch = false;
    	} else if (!(g->scored)) {
    		g->turn_id = (g->turn_id == 0);
    	}
    }
  }

}

#endif	// !TFT_IGNORE
