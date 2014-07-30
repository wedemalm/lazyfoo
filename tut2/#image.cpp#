#include <SDL2/SDL.h>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

//Starts up SDL and creates window
bool init();

//Loads Media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The surface contained by the window
SDL_Surface *g_screenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface *g_helloWorld = NULL;

int main() {
  if(!init()) {
    printf("Failed to initialize! \n");
  } else {
    if(!loadMedia()) {
      printf("Failed to load media!\n");
    } else {
      SDL_BlitSurface(g_helloWorld, NULL, g_screenSurface, NULL);
    }
  }
  SDL_UpdateWindowSurface(g_window);

  SDL_Delay(2000);
  close();

  return 0;
}

bool init() {
  bool l_success = true;
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    l_success = false;
  } else {
    g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
				SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(g_window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n",
	     SDL_GetError());
      l_success = false;
    } else {
      g_screenSurface = SDL_GetWindowSurface(g_window);
    }
  }
  return l_success;
}

bool loadMedia() {
  bool l_success =  true;
  g_helloWorld = SDL_LoadBMP("Hello.bmp");
  if(g_helloWorld == NULL ) {
    printf("Unable to load image %s! SDL_Error: %s\n",
	   "Hello.bmp", SDL_GetError());
    l_success = false;
  }
  return l_success;
}

void close() {
  SDL_FreeSurface(g_helloWorld);
  g_helloWorld = NULL;

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}
