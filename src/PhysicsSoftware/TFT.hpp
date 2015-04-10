#ifndef TFT_H_INCLUDED
#define TFT_H_INCLUDED

#define TFT_IGNORE false

#include "physics.hpp"
#include "position_locator.hpp"

void init_tft ();
void DrawRectangle(int xstart, int ystart, int width, int height, int colour);
void DrawCircle(int x0, int y0, int radius, int colour);
void DrawState (GameState *g);
void DrawBackground (int colour);
void DrawNumber(int x0, int y0, int number);
void DrawM(int x0, int y0);
void DrawT(int x0, int y0);
void DrawB(int x0, int y0);
void DrawUR(int x0, int y0);
void DrawBR(int x0, int y0);
void DrawUL(int x0, int y0);
void DrawBL(int x0, int y0);
void DrawPockets ();
void DrawScore(int xcenter, int ycenter, int p1_score, int p2_score, GameState *g);

#endif // TFT_H_INCLUDED
