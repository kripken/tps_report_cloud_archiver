//mandelbrot with SDL
//from http://pastebin.com/aNkSZWCF
#include <SDL/SDL.h>
#include <cmath>
#include <stdlib.h>

#if EMSCRIPTEN
#include <emscripten.h>
#endif

SDL_Surface* screen;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
SDL_Event keypress;

struct complexNumber
{
    float real;
    float imag;
};

int N = 30; //N = max iterations

inline float getMagnitude(complexNumber b)
{
    return (sqrt(b.real*b.real + b.imag*b.imag));
}

float Re_max = 1.0;
float Re_min = -2.5;
float Im_min = -1.0;
float Im_max = 1.0;

void frame() {
    static int frames = 0;
    frames++;
    if (frames == 200) exit(1);
  
    float Re_max_old = Re_max;
    float Re_min_old = Re_min;
    float Im_min_old = Im_min;
    float Im_max_old = Im_max;

    float t = 0.01;
    Re_min = (1-t)*Re_min_old + t*Re_max_old;
    Re_max = t*Re_min_old + (1-t)*Re_max_old;
    Im_min = (1-t)*Im_min_old + t*Im_max_old;
    Im_max = t*Im_min_old + (1-t)*Im_max_old;

    float delta = (Re_max - Re_min)/SCREEN_WIDTH;
    complexNumber z,xlast,xcurr;

    SDL_LockSurface(screen);

    int *pixels = (int*)screen->pixels;

    int i,j,count;

    //Checks all values on the screen in sync with all values on the Real/Imaginary axis
    for(i=0,z.real = Re_min; i<SCREEN_WIDTH; i++,z.real+=delta)
    {
        for(j=0,z.imag = Im_min; j<SCREEN_HEIGHT; j++,z.imag +=delta)
        {
            count = 0; //resets number of iterations
            xlast.real = xlast.imag = 0.0; //measurement starts at x_0 = 0
            while(getMagnitude(xlast) <= 2.0 && count < N)
            //if the magnitude is larger than 2, it will likely tend to infinity
            //a higher maximum number of iterations N will produce more precise drawings
            {
                //x_n+1 = (x_n)^2 + z

                //for a complex number a+bi, the square of this is (a^2 - b^2) + 2abi
                //the real part of the complex number is stored in the .real component
                //the imaginary part of the complex number is stored in the .imag component
                xcurr.real = (xlast.real*xlast.real - xlast.imag*xlast.imag) + z.real;
                xcurr.imag = (2*xlast.imag*xlast.real) + z.imag;

                //transfers x_n+1 to x_n
                xlast.real = xcurr.real;
                xlast.imag = xcurr.imag;
                count++;
            }
            //if it didn't tend to infinity, we draw a black pixel
            int color = 0;
            if(getMagnitude(xlast) > 2.0)
            {
                //if it did, we draw a pixel of color depending on when it reached tendence to infinity
                color = 255*count/N;
                color = color + (color << 8);
            }
            pixels[(i + j*SCREEN_WIDTH)] = color;
        }
    }
    SDL_UnlockSurface(screen);
#if !EMSCRIPTEN
    SDL_Flip(screen);
#endif
 }

int main(int argc, char* argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_HWSURFACE);
  SDL_WM_SetCaption("Mandelbrot set in C++",NULL);

#if EMSCRIPTEN
  emscripten_set_main_loop(frame, 0, 0);
#else
  while (1) {
    frame();
    //SDL_Delay(1);
  }
#endif

  return 0;
}

