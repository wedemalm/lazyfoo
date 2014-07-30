#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH  1280

SDL_Window *g_window        = NULL;
SDL_Surface *g_screenSurface = NULL;
SDL_Surface *g_pngSurface    = NULL;

bool loadMedia();

void close();

bool init();

SDL_Surface *loadSurface(std::string path);

int main() {
  bool l_quit = false;
  SDL_Event l_event;

  if(!init()){
    printf("Failed to initialize!\n");
  } else {
    if(!loadMedia()) {
      printf("Failed to load media!\n");
    } else {
      while(!l_quit) {
	while(SDL_PollEvent(&l_event) != 0) {
	  if(l_event.type == SDL_QUIT) {
	    l_quit = true;
	  }
	}
	SDL_BlitSurface(g_pngSurface, NULL, g_screenSurface, NULL);
	SDL_UpdateWindowSurface(g_window);
      }
    }
  }
  close();
  return 0;
}

bool init(){
  bool l_success = true;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    l_success = false;
  } else {
    g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
				SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(g_window == NULL) {
      printf("Window could not be created! SDL Error: %s\n", 
	     SDL_GetError());
      l_success = false;
    } else {
      int imgFlags = IMG_INIT_PNG;
      if(!(IMG_Init(imgFlags) & imgFlags)) {
	printf("SDL_image could not initialize! SDL_image Error: %s\n",
	       IMG_GetError());
	l_success = false;
      } else {
	g_screenSurface = SDL_GetWindowSurface(g_window);
      }
    }
  }
  return l_success;
}

SDL_Surface *loadSurface(std::string path) {
  //The final optimized image
  SDL_Surface *l_optimizedSurface = NULL;
  
  //Load image at specified path
  SDL_Surface *l_loadedSurface = IMG_Load(path.c_str());
  if(l_loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s!", 
	   path.c_str());
  } else {
    l_optimizedSurface = SDL_ConvertSurface(l_loadedSurface, 
					    g_screenSurface->format,
					    NULL);
    if(l_optimizedSurface == NULL) {
      printf("Unable to optimize image %s! SDL Error: %s\n",
	     path.c_str(), SDL_GetError());
    }
    SDL_FreeSurface(l_loadedSurface);
  }
  return l_optimizedSurface;
}

bool loadMedia() {
  bool l_success =  true;

  //Load stretching surface
  g_pngSurface = loadSurface("image.png");
  if(g_pngSurface == NULL) {
    printf("Failed to load image.png! \n");
    l_success = false;
  }
  return l_success;
}

void close() {
  //Deallocate surfaces
  SDL_FreeSurface(g_pngSurface);
  g_pngSurface = NULL;

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}
