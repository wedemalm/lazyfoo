#include <SDL2/SDL.h>
#include <string>

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

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

//current displayed image
SDL_Surface *g_stretchedSurface = NULL;

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
       while(!g_quit) {
	while (SDL_PollEvent(&e) != 0) {
	  if(e.type == SDL_QUIT) {
	    g_quit = true;
	  }
	}
	//Apply the image stretched
	SDL_Rect stretchRect;
	stretchRect.x = 0;
	stretchRect.y = 0;
	stretchRect.w = SCREEN_WIDTH;
	stretchRect.h = SCREEN_HEIGHT;

	SDL_BlitScaled(g_stretchedSurface, NULL, g_screenSurface,
		       &stretchRect);

	//Update the surface
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

  //Load stretching surface
  g_stretchedSurface = loadSurface("stretch.bmp");
  if(g_stretchedSurface == NULL) {
    printf("Failed to load stretching image! \n");
    l_success = false;
  }
  return l_success;
}

void close() {
  //Deallocate surfaces
  SDL_FreeSurface(g_stretchedSurface);
  g_stretchedSurface = NULL;

  SDL_DestroyWindow(g_window);
  g_window = NULL;

  SDL_Quit();
}

SDL_Surface *loadSurface(std::string path) {
  //The final optimized image
  SDL_Surface *l_optimizedSurface = NULL;

  //Load image at specified path
  SDL_Surface *l_loadedSurface = SDL_LoadBMP(path.c_str());
  if(l_loadedSurface == NULL) {
    printf("Unable to load image %s! SDL Error: %s", path.c_str(),
	   SDL_GetError());
  } else {
    l_optimizedSurface = SDL_ConvertSurface(l_loadedSurface,
					  g_screenSurface->format,
					  NULL);
    if(l_optimizedSurface == NULL) {
      printf("Unable to optimize image %s! SDL Error: %s\n",
	     path.c_str(), SDL_GetError());
    }
    //Get rid of old loaded surface
    SDL_FreeSurface(l_loadedSurface);
  }
  return l_optimizedSurface;
}
