#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>

//Screen domension constants
const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH  = 640;

const int TOTAL_DATA = 10;

//Data points
Sint32 g_data[TOTAL_DATA];

//A circele structure
struct Circle {
  int x, y;
  int r;
};

//Window Class
class LWindow {
public:
  //Initializes internals
  LWindow();

  //Creates window
  bool init();
  
  //Creates renderer from internal window
  SDL_Renderer *createRenderer();
  
  //Handles window events
  void handleEvent(SDL_Event &e);
  
  //Dealocates internals
  void free();

  //Window dimensions
  int getWidth();
  int getHeight();

  //Window focii
  bool hasMouseFocus();
  bool hasKeyboardFocus();
  bool isMinimized();

private:
  //Window data
  SDL_Window *m_window;
  
  //Window dimensions
  int m_width;
  int m_height;

  //Window focus
  bool m_mouseFocus;
  bool m_keyboardFocus;
  bool m_fullScreen;
  bool m_minimized;
};

//Texture wrapper class
class LTexture {
public:
  //Initializes variables
  LTexture();

  //Deallicates memory
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

//Start up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media nad shits down SDL
void close();

//Circle/Circle collision detector
bool checkCollision(Circle &a, Circle &b);

//Circle/Box collision detector
bool checkCollision(Circle &a, SDL_Rect &b);

//Calcilates distance squared between two points
double distanceSquared(int x1, int y1, int x2, int y2);

//The window renderer
SDL_Renderer *g_renderer = NULL;

//Globally used font
TTF_Font *g_font = NULL;

//Scene textures
LTexture g_sceneTexture;

//Our custom window
LWindow g_window;

LWindow::LWindow() {
  //Initialize non-existant window
  m_window        = NULL;
  m_mouseFocus    = false;
  m_keyboardFocus = false;
  m_fullScreen    = false;
  m_minimized     = false;
  m_width  = 0;
  m_height = 0;
}

bool LWindow::init() {
  //Create window
  m_window = SDL_CreateWindow("SDL Tutorial 35", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			      SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if(m_window != NULL) {
    m_mouseFocus = true;
    m_keyboardFocus = true;
    m_width = SCREEN_WIDTH;
    m_height = SCREEN_HEIGHT;
  }
  return m_window != NULL;
}

SDL_Renderer *LWindow::createRenderer() {
  return SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event &e) {
  //Window event occured
  if(e.type == SDL_WINDOWEVENT) {
    //Caption update flag
    bool updateCaption = false;
    
    switch(e.window.event) {
      //Get new dimensions and repaint on window size change
    case SDL_WINDOWEVENT_SIZE_CHANGED:
      m_width  = e.window.data1;
      m_height = e.window.data2;
      SDL_RenderPresent(g_renderer);
      break;
      
      //Repaint on exposure
    case SDL_WINDOWEVENT_EXPOSED:
      SDL_RenderPresent(g_renderer);
      break;

      //Mouse entered window
    case SDL_WINDOWEVENT_ENTER:
      m_mouseFocus  = true;
      updateCaption = true;
      break;

      //Mouse left window
    case SDL_WINDOWEVENT_LEAVE:
      m_mouseFocus  = false;
      updateCaption = true;
      
      //Window has keyboard focus
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      m_keyboardFocus  = true;
      updateCaption    = true;
      break;

      //Window lost keyboard focus
    case SDL_WINDOWEVENT_FOCUS_LOST:
      m_keyboardFocus  = false;
      updateCaption    = true;
      break;

      //Window minimized
    case SDL_WINDOWEVENT_MINIMIZED:
      m_minimized = true;
      break;

      //Window maximized
    case SDL_WINDOWEVENT_MAXIMIZED:
      m_minimized = false;
      break;

      //Window restored
    case SDL_WINDOWEVENT_RESTORED:
      m_minimized = false;
      break;
    }
    //Update window caption with new data
    if(updateCaption) {
      std::stringstream caption;
      caption << "SDL Tutorial - MouseFocus:" << ((m_mouseFocus) ? "On" : "Off") << "KeyboardFocus:" << ((m_keyboardFocus) ? "On" : "Off");
      SDL_SetWindowTitle(m_window, caption.str().c_str());
    }
  } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
    if(m_fullScreen) {
      SDL_SetWindowFullscreen(m_window, SDL_FALSE);
      m_fullScreen = false;
    } else {
      SDL_SetWindowFullscreen(m_window, SDL_TRUE);
      m_fullScreen = true;
      m_minimized  = false;
    }
  }
}

void LWindow::free() {
  if(m_window != NULL) {
    SDL_DestroyWindow(m_window);
  }
  
  m_mouseFocus    = false;
  m_keyboardFocus = false;
  m_width  = 0;
  m_height = 0;
}

int LWindow::getWidth() {
    return m_width;
}

int LWindow::getHeight() {
    return m_height;
}

bool LWindow::hasMouseFocus() {
    return m_mouseFocus;
}

bool LWindow::hasKeyboardFocus() {
    return m_keyboardFocus;
}

bool LWindow::isMinimized() {
    return m_minimized;
}

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

bool loadMedia() {
  //Loading success flag
  bool success = true;

  //Initialize scene texture
  if(!g_sceneTexture.loadFromFile("window.png")) {
    printf("Failed to load window texture!\n");
    success = false;
  }

  return success;
}

bool checkCollision(Circle &a, Circle &b) {
  //Calculate total radius squared
  int totalRadiusSquared = a.r + b.r;
  totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

  //If the distance between the centers of the circles is less than the sum of their radii
  if(distanceSquared(a.x, a.y, b.x, b.y) < totalRadiusSquared) {
    //The circles have collided
    return true;
  }
  //If not
  return false;
}

bool checkCollision(Circle &a, SDL_Rect &b) {
  //Closest point on collision box
  int cX, cY;
  
  //Find closest x offset
  if(a.x < b.x) {
    cX = b.x;
  } else if (a.x > b.x + b.w) {
    cX = b.x + b.w;
  } else {
    cX = a.x;
  }

  //Find closest y offset
  if (a.y < b.y) {
    cY = b.y;
  } else if(a.y > b.y + b.h) {
    cY = b.y + b.h;
  } else {
    cY = a.y;
  }

  //If the closest point is inside the circle
  if(distanceSquared(a.x, a.y, cX, cY) < a.r * a.r) {
    //This box and the circle have collided
    return true;
  }

  //If the shapes have not collided
  return false;
}

double distanceSquared(int x1, int y1, int x2, int y2) {
  int deltaX = x2 - x1;
  int deltaY = y2 - y1;
  return deltaX * deltaX + deltaY * deltaY;
}

bool init() {
  bool l_success = true;
  
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n",
	   SDL_GetError());
    l_success = false;
  } else {
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled");
    }  

    //Create window
    if(!g_window.init()) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      l_success = false;
    } else {
      //Create renderer for window
      g_renderer = g_window.createRenderer();
      if(g_renderer == NULL) {
	printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
	l_success = false;
      } else {
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
	  printf("SDL_image could not initialize! SDL_image Error: %s\n",
		 IMG_GetError());
	  l_success = false;
	}
	//Initialize SDL_ttf
	if(TTF_Init() == -1) {
	  printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
	  l_success = false;
	}
      }
    }
  }
  return l_success;
}

void close() {
  //Free loadded images
  g_sceneTexture.free();
  
  //Destroy window
  SDL_DestroyRenderer(g_renderer);
  g_window.free();

  //Quit SDL subsystem
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
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

  bool quit = false;
  SDL_Event e;

  //While application is running
  while(!quit) {
    //Handle events on queue
    while(SDL_PollEvent(&e) != 0) {
      //User request quit
      if(e.type == SDL_QUIT) {
	quit = true;
      } 
      //Handle window events
      g_window.handleEvent(e);
    }
    //Only draw when not minimized
    if(!g_window.isMinimized()) {
      //Clear screen
      SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
      SDL_RenderClear(g_renderer);

      //Render text textures
      g_sceneTexture.render((g_window.getWidth() - g_sceneTexture.getWidth()) / 2,
				 (g_window.getHeight() - g_sceneTexture.getHeight()) / 2 );

      //Update screen
      SDL_RenderPresent(g_renderer);
    }
  }
  close();
  return 0;
}
