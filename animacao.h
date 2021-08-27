#ifndef ANIMACAO
#define ANIMACAO

#include <raylib.h>
typedef struct{
    int quantFrames;
    int frameCounter;
    int frameSpeed;
    int currentFrame;
    Texture2D textura;
    Rectangle frame; 
    Vector2 position;
} spritesheet;

void playAnimation(spritesheet *anim);

void AnimarPlayer(spritesheet *anim, int acao);

#endif