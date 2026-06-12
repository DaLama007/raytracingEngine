#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  char* type;
  int startX;
  int startY;
  int endX;
  int endY;
}Shape;

typedef struct{
  int x;
  int y;
}Point;

typedef struct{
  double x;
  double y;
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

void DrawSource(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius, Ray *rays, int num_rays)
{
  for (int i = -90; i < num_rays; i++) {
    float angle = 2.0 * M_PI * i / num_rays; // radians
    int x = (int)(centreX+ radius * cos(angle));
    int y = (int)(centreY + radius * sin(angle));
    SDL_RenderDrawPoint(renderer, x, y); // Draw point
    int newX=x+radius*10*cos(angle);
    int newY=y+radius*10*sin(angle);
    
    Point p = {x, y};
    Vector v = {cos(angle), sin(angle)};
    rays[i+90] = (Ray){p,v}; // origin, direction
  }
}

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
  for (int i = -90; i < 360; i++) {
    float angle = 2.0 * M_PI * i / 360; // radians
    int x = (int)(centreX+ radius * cos(angle));
    int y = (int)(centreY + radius * sin(angle));
    SDL_RenderDrawPoint(renderer, x, y); // Draw point
  }
}

void DrawRays(SDL_Renderer * renderer, Ray *rays, int num_rays, Shape *shapes, int num_shapes, double time, double max_time)
{
  for (int i = 0; i<num_rays; i++) {
    // save origin points in vars
    int originX = rays[i].origin.x;
    int originY = rays[i].origin.y;


    //calculate entering and exiting times between x1 and x2 of shapes
    double minT = time;
    for (int j = 0; j< num_shapes; j++) {
      
      Shape curr = shapes[j];
      // check for rectangle intersection
      if(strcmp("rectangle", curr.type)==0){
        double tendX = (curr.endX - originX)/ rays[i].direction.x;
        double tstartX = (curr.startX - originX)/ rays[i].direction.x;
        double tminX = fmin(tstartX, tendX);
        double tmaxX= fmax(tstartX, tendX);
        double tendY = (curr.endY - originY)/ rays[i].direction.y;
        double tstartY = (curr.startY - originY)/ rays[i].direction.y;
        double tmaxY = fmax(tstartY, tendY);
        double tminY = fmin(tstartY, tendY);

        // calc entry and exit times 
        double tentry = fmax(tminX, tminY);
        double texit = fmin(tmaxX, tmaxY);

        // check if entry time is smaller than exit time
        if(tentry<=texit && tentry>=0){
          minT= fmin(minT, tentry);
        }
      }

      else{
        // circle is defined by (x-C_x)^2 + (y-C_y)^2 = r^2
        //
        // so lets insert the formula of a ray: P(x,y) = O(O_x, O_y) + t*D;
        // into the seperate x and y forms of the formula
        //
        // (O + t*D - C)^2 = r^2
        //
        // For simplification: L = O-C
        // => (L + t*D)^2 = r^2
        // 
        // L^2 + 2L*t*D + (D)t^2 = r^2
        // <=> D^2*t^2 + 2LD*t + L^2 - r^2 = 0
        //
        // And the nthe determinant and calculate based on result
        
        Point C = {curr.startX+(curr.endX-curr.startX)/2, curr.startY+(curr.endY - curr.startY)/2};
        double Lx = originX - C.x;
        double Ly = originY - C.y;
        double Dx = rays[i].direction.x;
        double Dy = rays[i].direction.y;
        double r = (curr.endX - curr.startX)/ 2.0;
        double a = Dx*Dx + Dy*Dy;
        double b = 2*(Lx*Dx + Ly*Dy);
        double c = Lx*Lx + Ly*Ly - r*r;
        double discriminant = b*b - 4 * a * c;

        // handle different det values:
        if(discriminant > 0){
          double t1 = (-b - sqrt(discriminant))/(2 * a);
          double t2 = (-b + sqrt(discriminant))/(2 * a);
          if (t1 >= 0) minT = fmin(minT, t1);
          if (t2 >= 0) minT = fmin(minT, t2);
        }
        else if(discriminant == 0){
          double t1 = -b/(2 * a);

          if(t1 >= 0) minT = fmin(t1, minT);
        }
        else continue;
      }
    }

    // either draw with the time of collision or with the basic value
    for (int j = 0;j<minT;j+=1.0) {
      int x = (int)(rays[i].origin.x + j*rays[i].direction.x);
      int y = (int)(rays[i].origin.y + j*rays[i].direction.y);

      //set rgb coloring for gradient
      double rgb = 255.0/max_time;
      double currRgb = (max_time-j)* rgb;
      SDL_SetRenderDrawColor(renderer,currRgb ,currRgb ,currRgb , 255);
      SDL_RenderDrawPoint(renderer, x, y);
    }
  }
}

void generateShapes(SDL_Renderer * renderer, Shape *shapes, int shapeCount, int panelWidth, int panelHeight){
  for (int i=0;i<shapeCount;i++) {
    int width = rand()%150;
    int height = rand()%150;
    int startX = rand()%(panelWidth - width);
    int startY = rand()%(panelHeight - height);
    int endX = startX+width;
    int endY = startY+height;

    // select one of the available shapes randomly
    int det = rand()%3;
    if(det == 0) shapes[i] = (Shape){"rectangle", startX, startY, endX, endY};
    else shapes[i] = (Shape){"circle", startX, startY, startX+width, startY+width};
  }
}

void drawShapes(SDL_Renderer * renderer, Shape *shapes, int shapeCount){
  for (int i=0;i<shapeCount;i++) {
    Shape curr = shapes[i];
    if(strcmp(curr.type, "circle") == 0){
      double radius = (curr.endX -curr.startX)/2.0;
      int centerX = curr.startX+radius;
      int centerY = curr.startY+radius;
      DrawCircle(renderer, centerX,centerY,radius);
    }
    else{
      Point start = {curr.startX, curr.startY};
      int width = curr.endX-curr.startX;
      int height = curr.endY - curr.startY;
      DrawRect(renderer, start, width, height);
    }
  }
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  int num_rays = 360;
  int num_shapes = 32;
  double time_max = 300;
  Ray *rays = malloc(num_rays * sizeof(Ray));
  Shape *shapes = malloc(num_shapes * sizeof(Shape));
  int time = 0;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow(
    "RayTracing Engine",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1620,
    500,
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

  generateShapes(renderer, shapes, num_shapes, pw, ph);

  while (running) {
    // increase time if max not reached
    if(time_max > time) time++;

    // check for keyboard inputs to move light source
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

    // background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // shapes and light source
    SDL_SetRenderDrawColor(renderer,255,255,255, 255);
    drawShapes(renderer, shapes, num_shapes);
    DrawSource(renderer, centerX, centerY, 50, rays, num_rays);
    DrawRays(renderer, rays, num_rays, shapes, num_shapes, time, time_max);

    SDL_RenderPresent(renderer);
  }
  free(rays);
  free(shapes);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}


