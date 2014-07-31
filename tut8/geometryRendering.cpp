#include <SDL2/SDL.h>
#include <stdio.h>
#include <string>

#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH  1280

bool loadMedia();

void close();

bool init();

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

int main() {
  bool l_quit = false;
  SDL_Event l_event;

  if(!init()) {
       printf("Failed to init\n");
  } else {
    while(!l_quit) {
      while(SDL_PollEvent(&l_event) != 0) {
	if(l_event.type == SDL_QUIT) {
	  l_quit = true;
	}
      }
      //CLear screen
      SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(g_renderer);

      //Render red filled quad
      SDL_Rect l_fillRect = {SCREEN_WIDTH /4, SCREEN_HEIGHT / 4, 
			     SCREEN_WIDTH /2, SCREEN_HEIGHT / 2};
      SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0xFF);
      SDL_RenderFillRect(g_renderer, &l_fillRect);

      //Render green outlined quad
      SDL_Rect outlineRect = {SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6,
			      SCREEN_WIDTH * 2 / 3, 
			      SCREEN_HEIGHT * 2 / 3};
      SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0xFF); 
      SDL_RenderDrawRect(g_renderer, &outlineRect);

      //Draw blue horizontal line
      SDL_SetRenderDrawColor(g_renderer, 0x00, 0x00, 0xFF, 0xFF);
      SDL_RenderDrawLine(g_renderer, 0, SCREEN_HEIGHT / 2,
			 SCREEN_WIDTH, SCREEN_HEIGHT / 2);

      //Draw vertical line of yellow dots
      SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0x00, 0xFF );
      for(int i = 0; i < SCREEN_HEIGHT; i += 4) {
	SDL_RenderDrawPoint(g_renderer, SCREEN_WIDTH / 2, i);
      }
      
      //Update screen
      SDL_RenderPresent(g_renderer);
    }
  }
  close();
  return 0;
}

void close() {
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;

  SDL_Quit();
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
      }
    }
  }
  return l_success;
}
