#include "sdlgraph.h"

#include "SDL.h"

int SDLGraph_Startup(OOTint width,OOTint height)
{
    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );
    
    //Quit SDL
    SDL_Quit();
    
    return 0;    
}