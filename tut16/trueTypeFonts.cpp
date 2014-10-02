#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <cmath>

#define SCREEN_HEIGHT 480
#define SCREEN_WIDTH  640

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

  //Creates image from font string
  bool loadFromRendererdText(std::string textureText, SDL_Color textColor);

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

//Scenee texture
LTexture g_modulatedTexture;
LTexture g_backgroundTexture;

//Rendered texture
LTexture g_texture;

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

  //Open the font
  g_font = TTF_OpenFont("lazy.ttf", 28);
  if(g_font == NULL) {
    printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
    success = false;
  } else {
    //Render text
    SDL_Color textColor {0, 0, 0};
    if(!g_texture.loadFromRendererdText("The quick brown fox jumps over the lazy dog", textColor)) {
      printf("Failed to render text texture!\n");
      success = false;
    }
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
  //Free loaded images
  g_texture.free();

  //Destroy window
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;

  //Quit SDL subsystem
  IMG_Quit();
  TTF_Quit();
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
    }
    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);
    
    //Render current frame
    g_texture.render((SCREEN_WIDTH - g_texture.getWidth() ) / 2, 
		     ( SCREEN_HEIGHT - g_texture.getHeight() ) / 2);
    
    //Update screen
    SDL_RenderPresent(g_renderer);
  }
  close();
  return 0;
}

