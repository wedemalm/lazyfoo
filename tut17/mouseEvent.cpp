#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH  640

//Button constants
const int BUTTON_WIDTH  = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

enum LButtonSprite {
  BUTTON_SPRITE_MOUSE_OUT         = 0,
  BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
  BUTTON_SPRITE_MOUSE_DOWN        = 2,
  BUTTON_SPRITE_MOUSE_UP          = 3,
  BUTTON_SPRITE_TOTAL             = 4
};

class LTexture;
//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

//Globaly used font
TTF_Font *g_font = NULL;

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

//The mouse button
class LButton {
public:
  //Initializes internal variables
  LButton();

  //Sets top left position
  void setPosition(int x, int y);

  //Handles mouse event
  void handleEvent(SDL_Event *e);

  //Shows button sprite
  void render();

private:
  //Top left postition
  SDL_Point m_position;

  //Curently used global sprite
  LButtonSprite m_currentSprite;
};

//Scenee texture
LTexture g_modulatedTexture;
LTexture g_backgroundTexture;

//Rendered texture
LTexture g_texture;

//Mouse button sprites
SDL_Rect g_spriteClips[BUTTON_SPRITE_TOTAL];
LTexture g_buttonsSpriteSheetTexture;

//Buttons objects
LButton g_buttons[ TOTAL_BUTTONS ]; 

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

LButton::LButton() {
  m_position.x = 0;
  m_position.y = 0;

  m_currentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y){
  m_position.x = x;
  m_position.y = y;
}

void LButton::handleEvent(SDL_Event *e){
  //If mouse event happend
  if(e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
    //Get mouse position
    int l_posX, l_posY;
    SDL_GetMouseState(&l_posX, &l_posY);

    //Check if mouse is in button
    bool inside = true;

    //Mouse is left of the button
    if(l_posX < m_position.x) {
      inside = false;
    } //Mouse is right of the button
    else if (l_posX > m_position.x + BUTTON_WIDTH) {
      inside = false; 
    } //Mouse is above the button
    else if (l_posY < m_position.y) {
      inside = false;
    } //Mouse is below the button
    else if (l_posY > m_position.y + BUTTON_HEIGHT) {
      inside = false;
    }

    //Mouse is outside button
    if(!inside) {
      m_currentSprite = BUTTON_SPRITE_MOUSE_OUT;
    } else {
      //Set mouse over sprite
      switch(e->type) {
      case SDL_MOUSEMOTION:
	m_currentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
	break;
      case SDL_MOUSEBUTTONDOWN:
	m_currentSprite = BUTTON_SPRITE_MOUSE_DOWN;
	break;
      case SDL_MOUSEBUTTONUP:
	m_currentSprite = BUTTON_SPRITE_MOUSE_UP;
	break;
      }
    }
  }
}

void LButton::render() {
  //Show current button sprite
  g_buttonsSpriteSheetTexture.render(m_position.x, m_position.y, &g_spriteClips[m_currentSprite]);
}

bool loadMedia() {
  //Loading success flag
  bool success = true;

  //Load sprites
  if(!g_buttonsSpriteSheetTexture.loadFromFile("button.png")) {
    printf("Failed to lead button sprite texture!\n");
    success = false;
  } else {
    //Set sprites
    for(int i = 0; i < BUTTON_SPRITE_TOTAL; i++) {
      g_spriteClips[i].x = 0;
      g_spriteClips[i].y = i * 200;
      g_spriteClips[i].w = BUTTON_WIDTH;
      g_spriteClips[i].h = BUTTON_HEIGHT;
    }
    //Set button in corners
    g_buttons[0].setPosition(0, 0);
    g_buttons[1].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, 0);
    g_buttons[2].setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);
    g_buttons[3].setPosition(SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT);
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
  g_buttonsSpriteSheetTexture.free();

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
  bool quit = false;
  SDL_Event e;

  if(!init()) {
    printf("Failed to initialize!\n");
    return -1;
  }

  if(!loadMedia()) {
    printf("Failed to load media!\n");
    return -1;
  }
  
  //While application is running
  while(!quit) {
    //Handle events on queue
    while(SDL_PollEvent(&e) != 0) {
      //User request quit
      if(e.type == SDL_QUIT) {
	quit = true;
      }

      //Handle button events
      for(int i = 0; i < TOTAL_BUTTONS; i++) {
	g_buttons[i].handleEvent(&e);
      }
    }

    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    //Render buttons
    for(int i = 0; i < TOTAL_BUTTONS; i++) {
      g_buttons[i].render();
    }    
    
    //Update screen
    SDL_RenderPresent(g_renderer);
  }
  close();
  return 0;
}

