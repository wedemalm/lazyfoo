#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH  = 640;

//Texture wrapper class
class LTexture {
public:
  //Initializes variables
  LTexture();

  //Deakkicates memory
  ~LTexture();

  //Loads image at specified path
  bool loadFromFile(std::string path);

  #ifdef _SDL_TTF_H
  //Creates image from font string
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
  #endif

  //Deallocates texture
  void free();

  //Set color modulation
  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  //Set blending
  void setBlendMode(SDL_BlendMode blending);

  //Set alpha modulation
  void setAlpha(Uint8 alpha);

  //Renders Texture at given point
  void render(int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  //Gets image dimensions
  int getWidth();
  int getHeight();

private:
  //The actual hardware texture
  SDL_Texture *m_texture;

  //Image dimensions
  int m_width;
  int m_height;
};

//The application time based timer
class LTimer {
public:
  //Initializes variables
  LTimer();

  //The various clock actions
  void start();
  void stop();
  void pause();
  void unpause();

  //Get the timer's time
  Uint32 getTicks();

  //Checks the status of the timer
  bool isStarted();
  bool isPaused();

private:
  //The clock time when the timer started
  Uint32 m_startTicks;

  //The ticks stored when the timer was paused
  Uint32 m_pausedTicks;

  //The timer status
  bool m_paused;
  bool m_started;
};

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

LTexture g_FPSTextTexture;

//global font
TTF_Font *g_font = NULL;

LTexture::LTexture() {
  //Initialize
  m_texture = NULL;
  m_width   = 0;
  m_height  = 0;
}

LTexture::~LTexture() {
  //Deallocate
  free();
}

bool LTexture::loadFromFile(std::string path) {
  //Get rid of preexisting texture
  free();

  //The final texture
  SDL_Texture *newTexture = NULL;
  
  //Load image at specified path
  SDL_Surface *loadedSurface = IMG_Load(path.c_str());
  if(loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  } else {
    //Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(g_renderer, loadedSurface);
    if(newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    } else {
      //Get image dimensions
      m_width  = loadedSurface->w;
      m_height = loadedSurface->h;
    }
    //Get rid of ol loaded surface
    SDL_FreeSurface(loadedSurface);
  }
  //Return success
  m_texture = newTexture;
  return m_texture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
  bool success = true;
  //Get rid of preexisting texture
  free();

  //Render text surface
  SDL_Surface *l_textSurface = TTF_RenderText_Solid(g_font, textureText.c_str(), textColor);
  if(l_textSurface == NULL) {
    printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  } else {
    //create texture from surface pixels
    m_texture = SDL_CreateTextureFromSurface(g_renderer, l_textSurface);
    if(m_texture == NULL) {
      printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
      success = false;
    } else {
      //Get image dimensions
      m_width  = l_textSurface->w;
      m_height = l_textSurface->h;
    }
    //Get rid of old surface
    SDL_FreeSurface(l_textSurface);
  }
  return success;
}
#endif

void LTexture::free() {
  //Free texture if it exists
  if(m_texture != NULL) {
    SDL_DestroyTexture(m_texture);
    m_texture = NULL;
    m_width   = 0;
    m_height  = 0;
  }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  //Modulate texture
  SDL_SetTextureColorMod(m_texture, red, green, blue);
}

void LTexture::render(int x, int y, SDL_Rect *clip, double angle, 
		      SDL_Point *center, SDL_RendererFlip flip) {
  //Set rendering space and render to screen
  SDL_Rect renderQuad = {x, y, m_width, m_height};

  //Set clip rendering dimensions
  if(clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //Render to screen
  SDL_RenderCopyEx(g_renderer, m_texture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
  return m_width;
}

int LTexture::getHeight() {
  return m_height;
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
  //Set blending function
  SDL_SetTextureBlendMode(m_texture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
  //Modulate texture alpha
  SDL_SetTextureAlphaMod(m_texture, alpha);
}

LTimer::LTimer() {
  //Initialize the variables
  m_startTicks  = 0;
  m_pausedTicks = 0;

  m_paused  = false;
  m_started = false;
}

void LTimer::start() {
  //Start the timer
  m_started = true;

  //Unpause the timer
  m_paused = false;

  //Get the current clock time 
  m_startTicks  = SDL_GetTicks();
  m_pausedTicks = 0;
}

void LTimer::stop() {
  //Stop the timer
  m_started = false;

  //Unpause the timer
  m_paused = false;

  //Clear tick variables
  m_startTicks  = 0;
  m_pausedTicks = 0;
}

void LTimer::pause() {
  //If the timer is running and isn't already paused
  if(m_started && !m_paused) {
    //Pause the timer
    m_paused = true;

    //Calculate the paused ticks
    m_pausedTicks = SDL_GetTicks() - m_startTicks;
    m_startTicks = 0;
  }
}

void LTimer::unpause() {
  //If the timer is running and paused
  if(m_started && m_paused) {
    //Unpause the timer
    m_paused = false;

    //Reset the starting ticks
    m_startTicks = SDL_GetTicks() - m_pausedTicks;

    //Reset the paused ticks
    m_pausedTicks = 0;
  }
}

Uint32 LTimer::getTicks() {
  //The actual timer time
  Uint32 time = 0;

  //If the timer is running
  if(m_started) {
    //If the timer is paused
    if(m_paused) {
      //Return the number of ticks when the timer was paused
      time = m_pausedTicks;
    } else {
      //Return the current time minus the start time
      time = SDL_GetTicks() - m_startTicks;
    }
  }
  return time;
}

bool LTimer::isStarted() {
  //Timer is running and paused or unpaused
  return m_started;
}

bool LTimer::isPaused() {
  //Timer is running and paused
  return m_paused && m_started;
}

bool loadMedia() {
 //Loading success flag
  bool success = true;
  
  //Open the font
  g_font = TTF_OpenFont("lazy.ttf", 28);
  if(g_font == NULL) {
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }
  return success;
}

bool init() {
  bool l_success = true;
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n",
	   SDL_GetError());
    l_success = false;
  } else {
    //Set texture filtering to linear
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
      //Creates vsynced renderer for window
      g_renderer = SDL_CreateRenderer(g_window, -1,
				      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if(g_renderer == NULL) {
	printf("Renderer could not be created! SDL Error:%s\n",
	       SDL_GetError());
	l_success = false;
      } else {
	//Initialize render color
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
	  printf("SDL_image could not initialize! SDL_image Error: %s\n",
		 IMG_GetError());
	  l_success = false;
	}

	//Initialize SDL_ttf
	if(TTF_Init() == -1 ) {
	  printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
	  l_success = false;
	}
      }
    }
  }
  return l_success;
}

void close() {
  //Free loaded images
  g_FPSTextTexture.free();

  //Free global font
  TTF_CloseFont(g_font);
  g_font = NULL;

  //Destroy window
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;

  //Quit SDL subsystem
  IMG_Quit();
  SDL_Quit();
  TTF_Quit();
}

int main() {
  if(!init()) {
    printf("Failed to initialize!\n");
    return -1;
  }

  if(!loadMedia()) {
    printf("Failed to load media!\n");
    return -1;
  }

  //Main loop flag
  bool quit = false;

  //Event handler
  SDL_Event e;

  //Set tet color as black
  SDL_Color textColor = {0, 0, 0, 255};

  //The application timer
  LTimer fpsTimer;

  //In memory text stream
  std::stringstream timeText;

  //Start counting frames per second
  int countedFrames = 0;
  fpsTimer.start();

  //While application is running
  while(!quit) {
    //Handle events on queue
    while(SDL_PollEvent(&e) != 0) {
      //User request quit
      if(e.type == SDL_QUIT) {
	quit = true;
      }
    }

    //Calculate and correct fps
    float avgFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
    if(avgFPS > 2000000) {
      avgFPS = 0;
    }
    
    //Set text to be rendered
    timeText.str("");
    timeText << "Average  Frames Per Second " << avgFPS;
    
    //Render text
    if(!g_FPSTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
      printf("Unable to render FPS texture!\n");
    }

    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);
    
    //Render current texture
    g_FPSTextTexture.render((SCREEN_WIDTH - g_FPSTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - g_FPSTextTexture.getHeight()) / 2);
    
    //Update screen
    SDL_RenderPresent(g_renderer);
    ++countedFrames;
  }
  close();
  return 0;
}
