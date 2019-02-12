# Game
Game in progress

After cloning:

1. Get correct Windows SDK with "Retarget Solution"

2. Download SDL library files and set them up with Visual Studio

  Set up include, library directories
  
    include: 
    SDL2-devel-2.0.8-VC\SDL2-2.0.8\include
    SDL2_ttf-2.0.14\include
    SDL2_image-devel-2.0.0-VC\include
             
    library: 
    SDL2-devel-2.0.8-VC\SDL2-2.0.8\lib\x86
    SDL2_ttf-2.0.14\lib\x86
    SDL2_image-devel-2.0.0-VC\lib\x86
             
  Set up linker dependencies
  
    SDL2.lib
    SDL2main.lib
    SDL2_image.lib
    SDL2_ttf.lib
    SDL2_mixer.lib
    
Note that different versions of SDL may cause some problems. For example, \_SDL_TTF_H was renamed to SDL_TTF_H
