#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

#define SCREEN_HEIGHT 1280
#define SCREEN_WIDTH  720

bool loadMedia();

void close();

bool init();

//Loads individual image as texture
SDL_Texture *loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

//Current display texture
SDL_Texture *g_texture = NULL;

int main() {
  bool l_quit = false;
  SDL_Event l_event;

  if(!init()) {
    printf("Failed to init\n");
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
	//Clear scren
	SDL_RenderClear(g_renderer);
	
	//Render texture to screen
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	
	//Update screen
	SDL_RenderPresent(g_renderer);
      }
    }
  }
  return 0;
}

bool init() {
  bool l_success = true;

  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n",
	   SDL_GetError());
    l_success = false;
  } else {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled");
    }  
    //Create window
    g_window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, 
				SDL_WINDOWPOS_UNDEFINED, SCREEN_HEIGHT,
				SCREEN_WIDTH, SDL_WINDOW_SHOWN);
  
    if(g_window == NULL) {
      printf("Window could not be created! SDL Error: %s\n",
	     SDL_GetError());
      l_success = false;
    } else {
      //Creates renderer for window
      g_renderer = SDL_CreateRenderer(g_window, -1,
				      SDL_RENDERER_ACCELERATED);
      if(g_renderer == NULL) {
	printf("Renderer could not be created! SDL Error:%s\n",
	       SDL_GetError());
	l_success = false;
      } else {
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
	  printf("SDL_image could not initialize! SDL_image Error: %s\n",
		 IMG_GetError());
	  l_success = false;
	}
      }
    }
  }
  return l_success;
}

SDL_Texture *loadTexture(std::string path) {
  SDL_Texture *l_newTexture = NULL;

  SDL_Surface *l_loadedSurface = IMG_Load(path.c_str());
  if(l_loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n",
	   path.c_str(), IMG_GetError());
  } else {
    l_newTexture = SDL_CreateTextureFromSurface(g_renderer, 
						l_loadedSurface);
    if(l_newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n",
	     path.c_str(), SDL_GetError());
    }
    SDL_FreeSurface(l_loadedSurface);
  }
  return l_newTexture;
}

bool loadMedia() {
  bool l_success = true;
  
  g_texture = loadTexture("texture.png");
  if(g_texture == NULL) {
    printf("Failed to load texture image!\n");
    l_success = false;
  }
  return l_success;
}

void close() {
  SDL_DestroyTexture(g_texture);
  g_texture = NULL;

  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;
  
  IMG_Quit();
  SDL_Quit();
}
