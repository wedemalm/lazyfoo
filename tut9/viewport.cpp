#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH  1280

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

//Texture
SDL_Texture *g_texture = NULL;

bool init();

bool loadMedia();

int main() {  
  //Main loop flag
  bool l_quit = false;
  
  if(!init()) {
    printf("Failed to initialize!\n");
  } else {
    if(!loadMedia()) {
      printf("Failed to load media!\n");
    } else {
      //Event handler
      SDL_Event e;

      //While application is running
      while(!l_quit) {
	while(SDL_PollEvent(&e) != 0) {
	  if(e.type == SDL_QUIT) {
	    l_quit = true;
	  }
	}

	SDL_Rect topLeftViewPort;
	topLeftViewPort.x = 0;
	topLeftViewPort.y = 0;
	topLeftViewPort.h = SCREEN_HEIGHT / 2;
	topLeftViewPort.w = SCREEN_WIDTH  / 2;
	
	SDL_RenderSetViewport(g_renderer, & topLeftViewPort);
	
	//Render texture to screen
	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);
	
	//Top right viewport
	SDL_Rect topRightViewport;
	topRightViewport.x = SCREEN_WIDTH / 2;
	topRightViewport.y = 0;
	topRightViewport.w = SCREEN_WIDTH / 2;
	topRightViewport.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport( g_renderer, &topRightViewport );

	SDL_RenderCopy(g_renderer, g_texture, NULL, NULL);

	//Bottom viewport
	SDL_Rect bottom;
	bottom.x = 0;
	bottom.y = SCREEN_HEIGHT / 2;
	bottom.w = SCREEN_WIDTH;
	bottom.h = SCREEN_HEIGHT / 2;
	SDL_RenderSetViewport(g_renderer, &bottom);
	
	//Render texture to screen
	SDL_RenderCopy( g_renderer, g_texture, NULL, NULL );

	//Update screen
	SDL_RenderPresent(g_renderer);
      }
    }
  }
  return 0;
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
				SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
				SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  
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









