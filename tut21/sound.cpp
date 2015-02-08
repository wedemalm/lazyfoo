#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <cmath>

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

//The window we'll be rendering to
SDL_Window *g_window = NULL;

//The window renderer
SDL_Renderer *g_renderer = NULL;

LTexture g_texture;

//The music that will be played
Mix_Music *g_music = NULL;

//The sound effects that will be used
Mix_Chunk *g_scratch = NULL;
Mix_Chunk *g_high    = NULL;
Mix_Chunk *g_medium  = NULL;
Mix_Chunk *g_low     = NULL;

//global font
TTF_Font *g_font;

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

bool loadMedia() {
  //Loading success flag
  bool success = true;

  //Load prompt texture
  if(!g_texture.loadFromFile("prompt.png")) {
    printf("Failed to load texture!\n");
    success = false;
  }

  //Load music
  g_music = Mix_LoadMUS("beat.wav");
  if(g_music == NULL) {
    printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  //Load sound effects
  g_scratch = Mix_LoadWAV("scratch.wav");
  if(g_scratch == NULL) {
    printf("Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  g_high = Mix_LoadWAV("high.wav");
  if(g_high == NULL) {
    printf("Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  g_medium = Mix_LoadWAV("medium.wav");
  if(g_medium == NULL) {
    printf("Failed to load medium sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  g_low = Mix_LoadWAV("low.wav");
  if(g_low == NULL) {
    printf("Failed to load low sound effect! SDL_mixer Error: %s\n", Mix_GetError());
    success = false;
  }

  return success;
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
	
	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
	  printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
      }
    }
  }
  return l_success;
}

void close() {
  //Free loaded images
  g_texture.free();

  //Free the sound effects
  Mix_FreeChunk(g_scratch);
  Mix_FreeChunk(g_high);
  Mix_FreeChunk(g_medium);
  Mix_FreeChunk(g_low);
  g_scratch = NULL;
  g_high    = NULL;
  g_medium  = NULL;
  g_low     = NULL;

  //Free the music
  Mix_FreeMusic(g_music);
  g_music = NULL;

  //Destroy window
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  g_window   = NULL;
  g_renderer = NULL;

  //Quit SDL subsystem
  IMG_Quit();
  SDL_Quit();
  Mix_Quit();
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
      } else if(e.type == SDL_KEYDOWN) {
	switch (e.key.keysym.sym) {
	  //Play high sound effect
	case SDLK_1:
	  printf("1 \n");
	  Mix_PlayChannel(-1, g_high, 0);
	  break;

	  //Play medium sound effect
	case SDLK_2:
	  Mix_PlayChannel(-1, g_medium, 0);
	  break;

	  //Play low sound effect
	case SDLK_3:
	  Mix_PlayChannel(-1, g_low, 0);
	  break;

	  //Play scratch sound effect
	case SDLK_4:
	  Mix_PlayChannel(-1, g_scratch, 0);
	  break;

	case SDLK_9:
	  //If there is no music playing
	  if(Mix_PlayingMusic() == 0) {
	    //Play the music
	    Mix_PlayMusic(g_music, -1);
	  }
	  //If music is being played
	  else {
	    //If the music is paused
	    if(Mix_PausedMusic() == 1) {
	      //Resume the music
	      Mix_ResumeMusic();
	    }
	    //If the music is playing
	    else {
	      //Pause the music
	      Mix_PauseMusic();
	    }
	  }
	  break;

	case SDLK_0:
	  //Stop the music
	  Mix_HaltMusic();
	  break;
	}
      }
    }

    //Clear screen
    SDL_SetRenderDrawColor(g_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(g_renderer);

    //Render current texture
    g_texture.render(0, 0);
    
    //Update screen
    SDL_RenderPresent(g_renderer);
  }
  close();
  return 0;
}
