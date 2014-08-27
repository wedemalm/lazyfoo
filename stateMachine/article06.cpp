/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//The headers
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/*Constants*/
//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The frame rate
const int FRAMES_PER_SECOND = 20;

//The dot dimensions
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

//The house dimensions
const int HOUSE_WIDTH = 40;
const int HOUSE_HEIGHT = 40;

//The door dimensions
const int DOOR_WIDTH = 20;
const int DOOR_HEIGHT = 40;

//Game states
enum GameStates
{
    STATE_NULL,
    STATE_INTRO,
    STATE_TITLE,
    STATE_GREEN_OVERWORLD,
    STATE_RED_ROOM,
    STATE_BLUE_ROOM,
    STATE_EXIT,
};

/*Classes*/
//Game state base class
class GameState
{
    public:
    virtual void handle_events() = 0;
    virtual void logic() = 0;
    virtual void render() = 0;
    virtual ~GameState(){};
};

//The dot
class Dot
{
    private:
    //The collision box of the dot
    SDL_Rect box;

    //The velocity of the dot
    int xVel, yVel;

    //The current level dimensions
    int curLvlWidth;
    int curLvlHeight;

    public:
    //Initializes the variables
    Dot();

    //Sets the dot
    void init( int x, int y, int lvlWidth, int lvlHeight );

    //Handles keypress
    void handle_input();

    //Moves the dot
    void move();

    //Shows the dot
    void show();

    //Sets the camera over the dot
    void set_camera();

    //Gets the dot's collision box
    operator SDL_Rect();
};

class House
{
    private:
    //The house graphic
    SDL_Surface *houseGFX;

    //The collision box of the house
    SDL_Rect box;

    public:
    //Initializes variables
    House();

    //Frees graphic
    ~House();

    //Sets the house
    void init( int x, int y, SDL_Surface *housePic );

    //Shows the house
    void show();

    //Gets the collision box
    operator SDL_Rect();
};

class Door
{
    private:
    //The area of the door
    SDL_Rect box;

    public:
    //Initializes variables
    Door();

    //Sets the Door
    void init( int x, int y );

    //Shows the door
    void show();

    //Gets the collision box
    operator SDL_Rect();
};

//The timer
class Timer
{
    private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

    public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    int get_ticks();

    //Checks the status of the timer
    bool is_started();
    bool is_paused();
};

class Intro : public GameState
{
    private:
    //Intro background
    SDL_Surface *background;

    //Intro message
    SDL_Surface *message;

    public:
    //Loads intro resources
    Intro();
    //Frees intro resources
    ~Intro();

    //Main loop functions
    void handle_events();
    void logic();
    void render();
};

class Title : public GameState
{
    private:
    //Title screen background
    SDL_Surface *background;

    //Title screen message
    SDL_Surface *message;

    public:
    //Loads title screen resources
    Title();
    //Frees title resources
    ~Title();

    //Main loop functions
    void handle_events();
    void logic();
    void render();
};

class OverWorld : public GameState
{
    private:
    //Level dimensions
    const static int LEVEL_WIDTH = 1280;
    const static int LEVEL_HEIGHT = 960;

    //Overworld background
    SDL_Surface *background;

    //The houses
    House redHouse;
    House blueHouse;

    public:
    //Loads resources and initializes objects
    OverWorld( int prevState );

    //Frees resources
    ~OverWorld();

    //Main loop functions
    void handle_events();
    void logic();
    void render();
};

class RedRoom : public GameState
{
    private:
    //Level dimensions
    const static int LEVEL_WIDTH = 640;
    const static int LEVEL_HEIGHT = 480;

    //Red room background
    SDL_Surface *background;

    //The exit door
    Door exit;

    public:
    //Loads resources and intializes objects
    RedRoom();

    //Frees resources
    ~RedRoom();

    //Main loop functions
    void handle_events();
    void logic();
    void render();
};

class BlueRoom : public GameState
{
    private:
    //Level dimensions
    const static int LEVEL_WIDTH = 640;
    const static int LEVEL_HEIGHT = 480;

    //Blue room background
    SDL_Surface *background;

    //The exit door
    Door exit;

    public:
    //Loads resources and intializes objects
    BlueRoom();

    //Frees resources
    ~BlueRoom();

    //Main loop functions
    void handle_events();
    void logic();
    void render();
};

/*Functions*/
//Image loader
SDL_Surface *load_image( std::string filename );

//Surface blitter
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL );

//Check collision
bool check_collision( SDL_Rect A, SDL_Rect B );

//Initialization
bool init();

//File loading
bool load_files();

//Clean up
void clean_up();

//State status manager
void set_next_state( int newState );

//State changer
void change_state();

/*Globals*/
//The surfaces
SDL_Surface *dot = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The camera
SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

//The font that's going to be used
TTF_Font *font = NULL;

//The color of the font
SDL_Color textColor = { 0, 0, 0 };

//The dot
Dot myDot;

//State variables
int stateID = STATE_NULL;
int nextState = STATE_NULL;

//Game state object
GameState *currentState = NULL;

/*Class Definitions*/
Dot::Dot()
{
    //Initialize the offsets
    init( 0, 0, 0, 0 );

    //Set dot dimensions
    box.w = DOT_WIDTH;
    box.h = DOT_HEIGHT;

    //Initialize the velocity
    xVel = 0;
    yVel = 0;
}

void Dot::init( int x, int y, int lvlWidth, int lvlHeight )
{
    //Set dot position
    box.x = x;
    box.y = y;

    //Get level dimensions
    curLvlWidth = lvlWidth;
    curLvlHeight = lvlHeight;
}

void Dot::handle_input()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel -= DOT_WIDTH / 2; break;
            case SDLK_RIGHT: xVel += DOT_WIDTH / 2; break;
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel += DOT_WIDTH / 2; break;
            case SDLK_RIGHT: xVel -= DOT_WIDTH / 2; break;
        }
    }
}

void Dot::move()
{
    //Move the dot left or right
    box.x += xVel;

    //If the dot went too far to the left or right
    if( ( box.x < 0 ) || ( box.x + DOT_WIDTH > curLvlWidth ) )
    {
        //move back
        box.x -= xVel;
    }

    //Move the dot up or down
    box.y += yVel;

    //If the dot went too far up or down
    if( ( box.y < 0 ) || ( box.y + DOT_HEIGHT > curLvlHeight ) )
    {
        //move back
        box.y -= yVel;
    }
}

void Dot::show()
{
    //Show the dot
    apply_surface( box.x - camera.x, box.y - camera.y, dot, screen );
}

void Dot::set_camera()
{
    //Center the camera over the dot
    camera.x = ( box.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
    camera.y = ( box.y + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2;

    //Keep the camera in bounds.
    if( camera.x < 0 )
    {
        camera.x = 0;
    }
    if( camera.y < 0 )
    {
        camera.y = 0;
    }
    if( camera.x > curLvlWidth - camera.w )
    {
        camera.x = curLvlWidth - camera.w;
    }
    if( camera.y > curLvlHeight - camera.h )
    {
        camera.y = curLvlHeight - camera.h;
    }
}

Dot::operator SDL_Rect()
{
    return box;
}

House::House()
{
    //Initialize house collision box
    box.x = 0;
    box.y = 0;
    box.w = HOUSE_WIDTH;
    box.h = HOUSE_HEIGHT;

    //Initialize house graphic
    houseGFX = NULL;
}

House::~House()
{
    //Free the house graphic
    SDL_FreeSurface( houseGFX );
}

void House::init( int x, int y, SDL_Surface *housePic )
{
    //Set house position
    box.x = x;
    box.y = y;

    //Get house graphic
    houseGFX = housePic;
}

void House::show()
{
    //Show the house
    apply_surface( box.x - camera.x, box.y - camera.y, houseGFX, screen );
}

House::operator SDL_Rect()
{
    return box;
}

Door::Door()
{
    //Initialize door collision box
    box.x = 0;
    box.y = 0;
    box.w = DOOR_WIDTH;
    box.h = DOOR_HEIGHT;
}

void Door::init( int x, int y )
{
    //Set door position
    box.x = x;
    box.y = y;
}

void Door::show()
{
    //Show the door
    SDL_FillRect( screen, &box, 0 );
}

Door::operator SDL_Rect()
{
    return box;
}

Timer::Timer()
{
    //Initialize the variables
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

void Timer::start()
{
    //Start the timer
    started = true;

    //Unpause the timer
    paused = false;

    //Get the current clock time
    startTicks = SDL_GetTicks();
}

void Timer::stop()
{
    //Stop the timer
    started = false;

    //Unpause the timer
    paused = false;
}

void Timer::pause()
{
    //If the timer is running and isn't already paused
    if( ( started == true ) && ( paused == false ) )
    {
        //Pause the timer
        paused = true;

        //Calculate the paused ticks
        pausedTicks = SDL_GetTicks() - startTicks;
    }
}

void Timer::unpause()
{
    //If the timer is paused
    if( paused == true )
    {
        //Unpause the timer
        paused = false;

        //Reset the starting ticks
        startTicks = SDL_GetTicks() - pausedTicks;

        //Reset the paused ticks
        pausedTicks = 0;
    }
}

int Timer::get_ticks()
{
    //If the timer is running
    if( started == true )
    {
        //If the timer is paused
        if( paused == true )
        {
            //Return the number of ticks when the the timer was paused
            return pausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            return SDL_GetTicks() - startTicks;
        }
    }

    //If the timer isn't running
    return 0;
}

bool Timer::is_started()
{
    return started;
}

bool Timer::is_paused()
{
    return paused;
}

Intro::Intro()
{
    //Load the background
    background = load_image( "introbg.png" );

    //Render the intro message
    message = TTF_RenderText_Solid( font, "Lazy Foo' Productions Presents...", textColor );
}

Intro::~Intro()
{
    //Free the surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( message );
}

void Intro::handle_events()
{
    //While there's events to handle
    while( SDL_PollEvent( &event ) )
    {
        //If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            set_next_state( STATE_EXIT );
        }
        //If the user pressed enter
        else if( ( event.type == SDL_KEYDOWN ) && ( event.key.keysym.sym == SDLK_RETURN ) )
        {
            //Move to the title screen
            set_next_state( STATE_TITLE );
        }
    }
}

void Intro::logic()
{

}

void Intro::render()
{
    //Show the background
    apply_surface( 0, 0, background, screen );

    //Show the message
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );
}

Title::Title()
{
    //Load the background
    background = load_image( "titlebg.png" );

    //Render the title message
    message = TTF_RenderText_Solid( font, "A State Machine Demo.", textColor );
}

Title::~Title()
{
    //Free surfaces
    SDL_FreeSurface( background );
    SDL_FreeSurface( message );
}

void Title::handle_events()
{
    //While there's events to handle
    while( SDL_PollEvent( &event ) )
    {
        //If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            set_next_state( STATE_EXIT );
        }
        //If the user pressed enter
        else if( ( event.type == SDL_KEYDOWN ) && ( event.key.keysym.sym == SDLK_RETURN ) )
        {
            //Go to the overworld
            set_next_state( STATE_GREEN_OVERWORLD );
        }
    }
}

void Title::logic()
{

}

void Title::render()
{
    //Show the background
    apply_surface( 0, 0, background, screen );

    //Show the message
    apply_surface( ( SCREEN_WIDTH - message->w ) / 2, ( SCREEN_HEIGHT - message->h ) / 2, message, screen );
}

OverWorld::OverWorld( int prevState )
{
    //Load the background
    background = load_image( "greenoverworld.png" );

    //Set the houses
    redHouse.init( 0, 0, load_image( "red.bmp" ) );
    blueHouse.init( 1240, 920, load_image( "blue.bmp" ) );

    //If the last state was the red room
    if( prevState == STATE_RED_ROOM )
    {
        //Show up in front of the red house
        myDot.init( 10, 40, LEVEL_WIDTH, LEVEL_HEIGHT );
    }
    //If the last state was the blue room
    else if( prevState == STATE_BLUE_ROOM )
    {
        //Show up in front of the blue house
        myDot.init( 1250, 900, LEVEL_WIDTH, LEVEL_HEIGHT );
    }
    //If the last state was something else
    else
    {
        //Show up in the center of the overworld
        myDot.init( 630, 470, LEVEL_WIDTH, LEVEL_HEIGHT );
    }
}

OverWorld::~OverWorld()
{
    //Free the resources
    SDL_FreeSurface( background );
}

void OverWorld::handle_events()
{
    //While there's events to handle
    while( SDL_PollEvent( &event ) )
    {
        //Handle events for the dot
        myDot.handle_input();

        //If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            set_next_state( STATE_EXIT );
        }
    }
}

void OverWorld::logic()
{
    //If the dot touches the red house
    if( check_collision( myDot, redHouse ) == true )
    {
        //Move to the red room
        set_next_state( STATE_RED_ROOM );
    }
    //If the dot touches the blue house
    else if( check_collision( myDot, blueHouse ) == true )
    {
        //Move to the blue room
        set_next_state( STATE_BLUE_ROOM );
    }

    //Move the dot
    myDot.move();
}

void OverWorld::render()
{
    //Set the camera
    myDot.set_camera();

    //Show the background
    apply_surface( 0, 0, background, screen, &camera );

    //Show the rooms
    redHouse.show();
    blueHouse.show();

    //Show the dot on the screen
    myDot.show();
}

RedRoom::RedRoom()
{
    //Load the background
    background = load_image( "redroom.png" );

    //Set the exit
    exit.init( 310, 440 );

    //Set the dot
    myDot.init( 310, 420, LEVEL_WIDTH, LEVEL_HEIGHT );
}

RedRoom::~RedRoom()
{
    //Free the background
    SDL_FreeSurface( background );
}

void RedRoom::handle_events()
{
    //While there's events to handle
    while( SDL_PollEvent( &event ) )
    {
        //Handle events for the dot
        myDot.handle_input();

        //If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            set_next_state( STATE_EXIT );
        }
    }
}

void RedRoom::logic()
{
    //If the dot went to the exit
    if( check_collision( myDot, exit ) == true )
    {
        //Go to the overworld
        set_next_state( STATE_GREEN_OVERWORLD );
    }

    //Move the dot
    myDot.move();
}

void RedRoom::render()
{
    //Set the camera
    myDot.set_camera();

    //Show the background
    apply_surface( 0, 0, background, screen, &camera );

    //Show the door
    exit.show();

    //Show the dot on the screen
    myDot.show();
}

BlueRoom::BlueRoom()
{
    //Load the background
    background = load_image( "blueroom.png" );

    //Set the exit
    exit.init( 310, 0 );

    //Set the dot
    myDot.init( 310, 40, LEVEL_WIDTH, LEVEL_HEIGHT );
}

BlueRoom::~BlueRoom()
{
    //Free the background
    SDL_FreeSurface( background );
}

void BlueRoom::handle_events()
{
    //While there's events to handle
    while( SDL_PollEvent( &event ) )
    {
        //Handle events for the dot
        myDot.handle_input();

        //If the user has Xed out the window
        if( event.type == SDL_QUIT )
        {
            //Quit the program
            set_next_state( STATE_EXIT );
        }
    }
}

void BlueRoom::logic()
{
    //If the dot went to the exit
    if( check_collision( myDot, exit ) == true )
    {
        //Go to the overworld
        set_next_state( STATE_GREEN_OVERWORLD );
    }

    //Move the dot
    myDot.move();
}

void BlueRoom::render()
{
    //Set the camera
    myDot.set_camera();

    //Show the background
    apply_surface( 0, 0, background, screen, &camera );

    //Show the door
    exit.show();

    //Show the dot on the screen
    myDot.show();
}

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
      optimizedImage = SDL_ConvertSurfaceFormat( loadedImage, &screen->format, NULL );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_TRUE, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool check_collision( SDL_Rect A, SDL_Rect B )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = A.x;
    rightA = A.x + A.w;
    topA = A.y;
    bottomA = A.y + A.h;

    //Calculate the sides of rect B
    leftB = B.x;
    rightB = B.x + B.w;
    topB = B.y;
    bottomB = B.y + B.h;

    //If any of the sides from A are outside of B
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_CreateWindow("My Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    //    SDL_WM_SetCaption( "State Machine Demo", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the dot image
    dot = load_image( "dot.bmp" );

    //If there was a problem in loading the dot
    if( dot == NULL )
    {
        return false;
    }

    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 36 );

    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Delete game state and free state resources
    delete currentState;

    //Free the surfaces
    SDL_FreeSurface( dot );

    //Close the font that was used
    TTF_CloseFont( font );

    //Quit SDL_ttf
    TTF_Quit();

    //Quit SDL
    SDL_Quit();
}

void set_next_state( int newState )
{
    //If the user doesn't want to exit
    if( nextState != STATE_EXIT )
    {
        //Set the next state
        nextState = newState;
    }
}

void change_state()
{
    //If the state needs to be changed
    if( nextState != STATE_NULL )
    {
        //Delete the current state
        if( nextState != STATE_EXIT )
        {
            delete currentState;
        }

        //Change the state
        switch( nextState )
        {
            case STATE_TITLE:
                currentState = new Title();
                break;

            case STATE_GREEN_OVERWORLD:
                currentState = new OverWorld( stateID );
                break;

            case STATE_RED_ROOM:
                currentState = new RedRoom();
                break;

            case STATE_BLUE_ROOM:
                currentState = new BlueRoom();
                break;
        }

        //Change the current state ID
        stateID = nextState;

        //NULL the next state ID
        nextState = STATE_NULL;
    }
}

int main( int argc, char* args[] )
{
    //The frame rate regulator
    Timer fps;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Set the current state ID
    stateID = STATE_INTRO;

    //Set the current game state object
    currentState = new Intro();

    //While the user hasn't quit
    while( stateID != STATE_EXIT )
    {
        //Start the frame timer
        fps.start();

        //Do state event handling
        currentState->handle_events();

        //Do state logic
        currentState->logic();

        //Change state if needed
        change_state();

        //Do state rendering
        currentState->render();

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }

        //Cap the frame rate
        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
        {
            SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
        }
    }

    //Clean up
    clean_up();

    return 0;
}
