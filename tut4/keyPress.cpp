#include <SDL2/SDL.h>
#include <string>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

//Key press surface constants
enum KeyPressSurfaces {
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};

//Starts up SDL and creates window
bool init();

//Loads Media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Loads individual image
SDL_Surface *loadSurface(std::string path);

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The surface contained by the window
SDL_Surface *g_screenSurface = NULL;

//The image that correspond to a keypress
SDL_Surface *g_keyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

//Current displayed image
SDL_Surface *g_currentSurface = NULL;

//Bool for quiting out of gameloop
bool g_quit = false;

SDL_Event e;

int main() {
  if(!init()) {
    printf("Failed to initialize! \n");
  } else {
    if(!loadMedia()) {
      printf("Failed to load media!\n");
    } else {
      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
      while(!g_quit) {
	while (SDL_PollEvent(&e) != 0) {
	  if(e.type == SDL_QUIT) {
	    g_quit = true;
	  } else if(e.type == SDL_KEYDOWN) {
	    //Select surface based on key press
	    switch(e.key.keysym.sym) {
	    case SDLK_UP:
	      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_UP];
	      break;
	    case SDLK_DOWN:
	      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
	      break;
	    case SDLK_LEFT:
	      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
	      break;
	    case SDLK_RIGHT:
	      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
	      break;
	    default:
	      g_currentSurface = g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
	      break;
	    }
	  }
	}
	SDL_BlitSurface(g_currentSurface, NULL, g_screenSurface, NULL);
	SDL_UpdateWindowSurface(g_window);
      }
    }
  }
  
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

  //Load default surface
  g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = 
    loadSurface("press.bmp");
  if(g_keyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
    printf("Failed to load default image! \n");
    l_success = false;
  }
  //Load up surface
  g_keyPressSurfaces[ KEY_PRESS_SURFACE_UP ] = 
    loadSurface("up.bmp");
  if( g_keyPressSurfaces[ KEY_PRESS_SURFACE_UP ] == NULL )
    {
      printf( "Failed to load up image!\n" );
      l_success = false;
    }
  
  //Load down surface
  g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = 
    loadSurface("down.bmp");
  if(g_keyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
    {
      printf("Failed to load down image!\n");
      l_success = false;
    }
  
  //Load left surface
  g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = 
    loadSurface("left.bmp");
  if(g_keyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
    {
      printf( "Failed to load left image!\n" );
      l_success = false;
    }
  
  //Load right surface
  g_keyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = 
    loadSurface("right.bmp");
  if(g_keyPressSurfaces[ KEY_PRESS_SURFACE_RIGHT] == NULL)
    {
      printf("Failed to load right image!\n");
      l_success = false;
    }

  return l_success;
}

void close() {
  //Deallocate surfaces
  for(int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++) {
    SDL_FreeSurface(g_keyPressSurfaces[i]);
    g_keyPressSurfaces[i] = NULL;
  }

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}

SDL_Surface *loadSurface(std::string path) {
  //Load image at specified path
  SDL_Surface *l_loadedSurface = SDL_LoadBMP(path.c_str());
  if(l_loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s", path.c_str(),
	   SDL_GetError());
  }
  return l_loadedSurface;
}
