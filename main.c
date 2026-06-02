#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <math.h>

struct shape{
  int startX;
  int startY;
  int endX;
  int endY;
};

typedef struct{
  int x;
  int y;
}Point;

typedef struct{
  int x;
  int y;
}Vector;

typedef struct{
  Point origin;
  Vector direction;
}Ray;

void DrawRect(SDL_Renderer * renderer, Point topLeft, int32_t width, int32_t height)
{
  int x = topLeft.x;
  int y = topLeft.y;
  for(int i = x ; i< x+width; i++){
    SDL_RenderDrawPoint(renderer, i, y);
    SDL_RenderDrawPoint(renderer, i , y+height);
  }

  for(int i = y ; i< y+height; i++){
    SDL_RenderDrawPoint(renderer, x, i);
    SDL_RenderDrawPoint(renderer, x+width , i);
  }
}

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{

  int num_points = 360; // Or more for smoother curve

  for (int i = -90; i < num_points; i++) {
    float angle = 2.0 * M_PI * i / num_points; // radians
    int x = (int)(centreX+ radius * cos(angle));
    int y = (int)(centreY + radius * sin(angle));
    SDL_RenderDrawPoint(renderer, x, y); // Draw point
    int newX=x+radius*10*cos(angle);
    int newY=y+radius*10*sin(angle);
    SDL_RenderDrawLine(renderer,x,y,newX,newY);
  }
}
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
    "RayTracing Engine",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    640,
    480,
    SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
  );

  if (!window) {
    printf("SDL_CreateWindow error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  int running = 1;
  SDL_Event e;
  SDL_Renderer *renderer = SDL_CreateRenderer(
    window,
    -1,
    SDL_RENDERER_ACCELERATED
  );


  int pw, ph;
  SDL_GL_GetDrawableSize(window, &pw, &ph);
  int centerY = 300;
  int centerX = 300;
  printf("Pixel size: %dx%d\n", pw, ph);

  // get sdl surface
  SDL_Surface* pSurface = NULL;
  pSurface = SDL_GetWindowSurface(window);

  // create color bitmap
  Uint32 color =SDL_MapRGB(pSurface->format, 255, 255, 255);
  //create rect at fixed pos 
  SDL_Rect rect;

  //set pos for different rect cords
  Point tL = {20,20};
  int width = 100;
  int height = 100;
  DrawRect(renderer, tL, width, height);
  DrawCircle(renderer,centerX,centerY,100);

  while (running) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = 0;
      }
      else if (e.type == SDL_KEYDOWN) {
        if(e.key.keysym.sym == SDLK_UP){
          centerY-=20;
        }
        else if(e.key.keysym.sym == SDLK_DOWN){
          centerY+=20;
        }
        else if(e.key.keysym.sym == SDLK_RIGHT){
          centerX+=20;
        }
        else if(e.key.keysym.sym == SDLK_LEFT){
          centerX-=20;
        }
      }

    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    DrawRect(renderer, tL, width, height);
    DrawCircle(renderer, centerX, centerY, 50); 

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}


