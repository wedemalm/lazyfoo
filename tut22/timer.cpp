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

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

LTexture g_texture;
LTexture g_timeTextTexture;

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

bool loadMedia() {
 //Loading success flag
  bool success = true;
  
  //Open the font
  g_font = TTF_OpenFont("lazy.ttf", 28);
  if(g_font == NULL) {
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  } else {
    //Set text color as black
    SDL_Color textColor = { 0, 0, 0, 255 };
    
    //Load prompt texture
    if( !g_texture.loadFromRenderedText( "Press Enter to Reset Start Time.", 
						  textColor ) ) {
      printf( "Unable to render prompt texture!\n" );
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
  g_texture.free();
  g_timeTextTexture.free();

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

  //Current time start time
  Uint32 startTime = 0;

  //In memory text stream
  std::stringstream timeText;

  //While application is running
  while(!quit) {
    //Handle events on queue
    while(SDL_PollEvent(&e) != 0) {
      //User request quit
      if(e.type == SDL_QUIT) {
	quit = true;
      } //Reset start time on retun keypress
      else if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
	startTime = SDL_GetTicks();
      }
    }
    
    //Set text to be rendered
    timeText.str("");
    timeText << "Milliseconds since start time " << SDL_GetTicks() - startTime;
    
    //Render text
    if(!g_timeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
      printf("Unable to render time texture!\n");
    }

    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);
    
    //Render current texture
    g_texture.render((SCREEN_WIDTH - g_texture.getWidth()) / 2, 0);
    g_timeTextTexture.render((SCREEN_WIDTH - g_texture.getWidth()) / 2, 
			     (SCREEN_HEIGHT - g_texture.getHeight()) / 2);
    
    //Update screen
    SDL_RenderPresent(g_renderer);
  }
  close();
  return 0;
}
