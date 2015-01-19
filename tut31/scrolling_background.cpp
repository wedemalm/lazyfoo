#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <vector>

//Screen domension constants
const int SCREEN_HEIGHT = 480;
const int SCREEN_WIDTH  = 640;

//A circele structure
struct Circle {
  int x, y;
  int r;
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
  bool loadFromRendererdText(std::string textureText, SDL_Color textColor);
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

//The dot that will move around on the screen
class dot {
public:
  //The dimensions of the dot
  static const int DOT_WIDTH  = 20;
  static const int DOT_HEIGHT = 20;

  //Maximum axis velocity of the dot
  static const int DOT_VEL = 10;

  //Initializes the variables
  dot();

  //Takes key presses and adjusts the dot's velocity
  void handleEvent(SDL_Event &e);

  //Moves the dot
  void move();

  //Shows the dot on the screen
  void render();

private:
  //The X and Y offsets of the dot
  int m_posX;
  int m_posY;

  //The velocity of the dot
  int m_velX;
  int m_velY;
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

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

//Scene textures
LTexture g_dotTexture;
LTexture g_bgTexture;

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
bool LTexture::loadFromRendererdText(std::string textureText, SDL_Color textColor) {
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

dot::dot() {
  //Initialize the offsets
  m_posX = 0;
  m_posY = 0;

  //Initialize the velocity
  m_velX = 0;
  m_velY = 0;
}

void dot::handleEvent(SDL_Event &e) {
  //If a key was pressed
  if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
    //Adjust the velocity
    switch(e.key.keysym.sym) {
    case SDLK_UP: m_velY -= DOT_VEL;
      break;
    case SDLK_DOWN: m_velY += DOT_VEL;
      break;
    case SDLK_LEFT: m_velX -= DOT_VEL;
      break;
    case SDLK_RIGHT: m_velX += DOT_VEL;
      break;
    }
  } else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
    //Adjust the velocity
    switch(e.key.keysym.sym) {
    case SDLK_UP: m_velY += DOT_VEL;
      break;
    case SDLK_DOWN: m_velY -= DOT_VEL;
      break;
    case SDLK_LEFT: m_velX += DOT_VEL;
      break;
    case SDLK_RIGHT: m_velX -= DOT_VEL;
      break;      
    }
  }
}

void dot::move() {
  //Move the dot left or right
  m_posX += m_velX;

  //If the dot went too far to the left or right
  if((m_posX < 0) || (m_posX + DOT_WIDTH > SCREEN_WIDTH)) {
    //Move back
    m_posX -= m_velX;
  }

  //Move the dot up or down
  m_posY += m_velY;

  //If the dot went too far up or down
  if((m_posY < 0) || (m_posY + DOT_HEIGHT > SCREEN_HEIGHT)) {
    //Move back
    m_posY -= m_velY;
  }
}

void dot::render() {
  //Show the dot relative to the camera
  g_dotTexture.render(m_posX, m_posY);
}

bool loadMedia() {
  //Loading success flag
  bool success = true;

  if(!g_dotTexture.loadFromFile("dot.bmp")) {
    printf("Failed to load dot texture!\n");
    success = false;
  }

  if(!g_bgTexture.loadFromFile("bg.png")) {
    printf("Failed to load bg texture!\n");
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
	SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Initialize PNG loading
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

void close() {
  //Free loaded images
  g_dotTexture.free();

  //Destroy window
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;

  //Quit SDL subsystem
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

  //The dot that will be moving around on the screen
  dot theDot;

  //The background scrolling offset
  int scrollingOffset = 0;

  //While application is running
  while(!quit) {
    //Handle events on queue
    while(SDL_PollEvent(&e) != 0) {
      //User request quit
      if(e.type == SDL_QUIT) {
	quit = true;
      } 
      //Handle input for the dot
      theDot.handleEvent(e);
    }

    //Move the dot
    theDot.move();

    //Scrolling background
    --scrollingOffset;
    if(scrollingOffset < -g_bgTexture.getWidth() ) {
      scrollingOffset = 0;
    }

    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    //Render background
    g_bgTexture.render(scrollingOffset, 0);
    g_bgTexture.render(scrollingOffset + g_bgTexture.getWidth(), 0);

    //Render objects
    theDot.render();
    
    //Update screen
    SDL_RenderPresent(g_renderer);
  }
  close();
  return 0;
}
