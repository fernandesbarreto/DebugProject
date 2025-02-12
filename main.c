#include "raylib.h"
#include "criaturas.h"
#include "player.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "armas.h"
#include "mapa.h"
#include "menu.h"
#include "time.h"
#include "animacao.h"
#include "loja.h"

#define miraRaio 5

//P virou a tecla de fechar o programa para S
int hudFramecounter = 0;
bala armaPrincipal[256];
bala armaSecundaria[1024];
Rectangle grid[50];
int qtdDeParedes = 45;
int timerPosition = 0;
double oldangulo;

int main()
{

    InitWindow(GetScreenWidth(), GetScreenHeight(), "TheBugger");
    ToggleFullscreen();
    int wid = GetScreenWidth();
    int hei = GetScreenHeight();
    InitAudioDevice();
    Music music = LoadMusicStream("musica/TheBuggerOST.mp3");
    Sound tiro;
    tiro = LoadSound("sfx/sfxTiro.mp3");
    PlayMusicStream(music);
    int acao = 0;
    int acaoAntiga = 1;
    Rectangle criaturaRec[2];
    Rectangle criaturaRecMorte[2];
    //Loading texture menu
    Image menuPng = LoadImage("etc/MENU.png");
    //Loading texturas
    Texture2D miraTex = LoadTexture("Assets/assets/mira2.png");
    Texture2D mapa = abrirMapa();
    Texture2D shadow = LoadTexture("etc/personagens/principal/shadow.png");
    //------Criatura Bug Red
    Texture2D criatura1TexRed = LoadTexture("etc/personagens/principal/criatura1red.png");
    Texture2D criatura1TexRedMorte = LoadTexture("etc/personagens/principal/criatura1redxplosion.png");
    criaturaRecMorte[1] = (Rectangle){0.0f, 0.0f, criatura1TexRedMorte.width / 8, criatura1TexRedMorte.height / 2};
    criaturaRec[1] = (Rectangle){0.0f, 0.0f, criatura1TexRed.width / 2, criatura1TexRed.height / 2};
    //------Criatura Bug Yellow
    Texture2D criatura1TexYellow = LoadTexture("etc/personagens/principal/criatura1yellow.png");
    Texture2D criatura1TexYellowMorte = LoadTexture("etc/personagens/principal/criatura1yellowxplosion.png");
    //------Criatura Bug Green
    Texture2D criatura1TexGreen = LoadTexture("etc/personagens/principal/criatura1green.png");
    Texture2D criatura1TexGreenMorte = LoadTexture("etc/personagens/principal/criatura1greenxplosion.png");
    //------Criatura Skeleton Warrior
    Texture2D criatura2Tex = LoadTexture("etc/personagens/principal/skeletonwalk.png");
    Texture2D criatura2TexAttack = LoadTexture("etc/personagens/principal/skeletonAttack2.png");
    Texture2D criatura2TexMorte = LoadTexture("etc/personagens/principal/skeletonDeath.png");
    criaturaRec[2] = (Rectangle){0.0f, 0.0f, criatura2Tex.width / 10, criatura2Tex.height / 2};
    criaturaRecMorte[2] = (Rectangle){0.0f, 0.0f, criatura2TexAttack.width / 17, criatura2TexAttack.height / 2};
    //spritesheets load;
    spritesheet player = {
        8,
        0,
        12,
        0,
        1,
        0.0f,
        LoadTexture("etc/personagens/principal/runroboclorblue_.png"),
        LoadTexture("etc/personagens/principal/runrobocolororange_.png"),
        (Rectangle){0.0f, 0.0f, player.textura1.width / player.quantFrames, player.textura1.height / 4},
        (Vector2){0.0f, 0.0f}};
    spritesheet plasma = {
        8,
        0,
        18,
        0,
        1,
        0.0f,
        LoadTexture("etc/personagens/principal/plasmaBlue.png"),
        LoadTexture("etc/personagens/principal/plasmaRed.png"),
        (Rectangle){0.0f, 0.0f, plasma.textura1.width / plasma.quantFrames, plasma.textura1.height},
        (Vector2){0.0f, 0.0f}};
    spritesheet plasmaHUD = {
        8,
        0,
        18,
        0,
        1,
        180.0f,
        LoadTexture("etc/personagens/principal/plasmaBlue.png"),
        LoadTexture("etc/personagens/principal/plasmaRed.png"),
        (Rectangle){0.0f, 0.0f, plasma.textura1.width / plasma.quantFrames, plasma.textura1.height},
        (Vector2){0.0f, 0.0f}};
    spritesheet pcHUD = {
        2,
        0,
        12,
        0,
        1,
        0.0f,
        LoadTexture("etc/pcpc.png"),
        (Texture2D){0},
        (Rectangle){0.0f, 0.0f, pcHUD.textura1.width / pcHUD.quantFrames, pcHUD.textura1.height},
        (Vector2){0.0f, 0.0f}};
    spritesheet heart = {
        5,
        0,
        10,
        0,
        1,
        0.0f,
        LoadTexture("etc/heart.png"),
        (Texture2D){0},
        (Rectangle){0.0f, 0.0f, heart.textura1.width / heart.quantFrames, heart.textura1.height},
        (Vector2){0.0f, 0.0f}};
    //Setando camera
    Camera2D cameraJogador;
    cameraJogador.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
    cameraJogador.target = (Vector2){0.0f, 0.0f};
    cameraJogador.rotation = 0.0f;
    cameraJogador.zoom = 1.0f;

    Camera2D menu;
    menu.zoom = GetScreenWidth() / 1920.0f;
    menu.target = (Vector2){0.0f, 0.0f};
    menu.offset = (Vector2){0.0f, 0.0f};
    menu.rotation = 0.0f;
    //Iniciando o game
    SetTargetFPS(60);
    //Setando os grids do collision map
    ColisionMap(grid, mapa);

    while (!WindowShouldClose())
    {
        //tela de carregamento
        int contadorTempo = 0;
        clock_t prevTime = clock();
        int flag = 0;

        if (menuInicial(&menuPng) == 1)
        {
            //tela de carregamento (abre uma tela rosa por 60s antes do jogo começar)
            while (contadorTempo < 60000 && flag == 0)
            {
                // BeginDrawing();
                // BeginMode2D(menu);
                clock_t currentTime = clock() - prevTime;
                contadorTempo = currentTime * 1000 / CLOCKS_PER_SEC;
                telaCarregamento();
                if (IsKeyPressed(KEY_T))
                {
                    flag = 1;
                }
                // EndDrawing();
                // EndMode2D();
            }

            mob *Criaturas;

            nerdola jogador;
            int balasGastasPrincipal = 0, balasGastasSecundaria = 0, criaturasVivas = 0, wave = 1, armaAtiva = 1, modRifle = 0, modPistola = 0;
            int pontuacao = 0, moeda = 0;
            Vector2 miraPosicao = {-100.0f, -100.0f};

            inicializaPlayer(&jogador);
            while (IsKeyUp(KEY_ESCAPE))
            {
                //Atualizando a stream da música
                HideCursor();

                UpdateMusicStream(music);

                criarWave(wave * 5, &criaturasVivas, &Criaturas, mapa.width, mapa.height, criaturaRec, criaturaRecMorte);
                criaturasVivas = wave * 5;
                while (criaturasVivas > 0)
                {
                    DisableCursor();
                    //Atualizando a stream da música
                    UpdateMusicStream(music);

                    if(IsKeyDown(KEY_P)) pause(wid, hei);

                    //Criando a parte de imagem
                    BeginDrawing();
                    //Iniciando a camera
                    BeginMode2D(cameraJogador);
                    ClearBackground(BLACK);
                    //Desenhando o mapa
                    DrawTextureEx(mapa, (Vector2){0.0f, 0.0f}, 0.0f, 3.0f, WHITE);
                    DrawTextureEx(shadow, (Vector2){(float)cameraJogador.target.x - 16 * 2.75 * 6, (float)cameraJogador.target.y - 16 * 3 * 8}, 0.0f, 3.0f, WHITE);
                    moverCriatura(&Criaturas, jogador.colisao.x, jogador.colisao.y, grid, qtdDeParedes, wave * 5);
                    //Animando criatura
                    double AnguloAt;
                    for (int i = 0; i < wave * 5; i++)
                    {

                        //DrawRectangleRec(Criaturas[i].ataque, RED);
                        //DrawRectangleRec(Criaturas[i].colisao, PINK);
                        if (Criaturas[i].animaMorte.morreu == 1 && Criaturas[i].tipo == 1)
                        {
                            Criaturas[i].animaMorte.frameCounter++;
                            AnimarCriatura1(&Criaturas[i].animaMorte, &criatura1TexYellowMorte, 96, 96);
                        }
                        if (Criaturas[i].animaMorte.morreu == 1 && Criaturas[i].tipo == 2)
                        {
                            Criaturas[i].animaMorte.frameCounter++;
                            AnimarCriatura1(&Criaturas[i].animaMorte, &criatura1TexGreenMorte, 96, 96);
                        }
                        if (Criaturas[i].animaMorte.morreu == 1 && Criaturas[i].tipo == 3)
                        {
                            Criaturas[i].animaMorte.frameCounter++;
                            AnimarCriatura1(&Criaturas[i].animaMorte, &criatura1TexRedMorte, 96, 96);
                        }
                        if (Criaturas[i].animaMorte.morreu == 1 && Criaturas[i].tipo == 4)
                        {
                            Criaturas[i].animaMorte.frameCounter++;
                            if (Criaturas[i].animaMorte.flagAnimMorte == 0)
                                AnimarCriatura2(&Criaturas[i].animaMorte, &criatura2TexMorte, &criaturaRecMorte[2], criatura2TexMorte.width / (6 * 3.5), criatura2TexMorte.height / (3.5), 96 + 32 + 128, 64 + 128 - 32); //(6*3.5), criatura2TexMorte.height/(3.5), 96+32, 64);
                        }
                        if (Criaturas[i].vida > 0 && Criaturas[i].tipo == 1)
                        {
                            Criaturas[i].anima.lrTimer++;
                            if (Criaturas[i].anima.lrTimer > 20)
                            {
                                Criaturas[i].anima.delta = Criaturas[i].anima.oldposition.x - Criaturas[i].anima.position.x;
                                Criaturas[i].anima.oldposition.x = Criaturas[i].anima.position.x;
                                Criaturas[i].anima.lrTimer = 0;
                            }
                            Criaturas[i].anima.frameCounter++;
                            Criaturas[i].anima.position.x = Criaturas[i].colisao.x;
                            Criaturas[i].anima.position.y = Criaturas[i].colisao.y;
                            AnimarCriatura1(&Criaturas[i].anima, &criatura1TexYellow, 96, 96);
                            Criaturas[i].anima.oldposition.y = Criaturas[i].anima.position.y;
                            Criaturas[i].animaMorte.position.x = Criaturas[i].anima.position.x;
                            Criaturas[i].animaMorte.position.y = Criaturas[i].anima.position.y;
                            if (Criaturas[i].animaMorte.dirOuEsq)
                                Criaturas[i].anima.dirOuEsq = 0;
                            else
                                Criaturas[i].anima.dirOuEsq = 1;
                        }
                        if (Criaturas[i].vida > 0 && Criaturas[i].tipo == 2)
                        {
                            Criaturas[i].anima.lrTimer++;
                            if (Criaturas[i].anima.lrTimer > 20)
                            {
                                Criaturas[i].anima.delta = Criaturas[i].anima.oldposition.x - Criaturas[i].anima.position.x;
                                Criaturas[i].anima.oldposition.x = Criaturas[i].anima.position.x;
                                Criaturas[i].anima.lrTimer = 0;
                            }
                            Criaturas[i].anima.frameCounter++;
                            Criaturas[i].anima.position.x = Criaturas[i].colisao.x;
                            Criaturas[i].anima.position.y = Criaturas[i].colisao.y;
                            AnimarCriatura1(&Criaturas[i].anima, &criatura1TexGreen, 96, 96);
                            Criaturas[i].anima.oldposition.y = Criaturas[i].anima.position.y;
                            Criaturas[i].animaMorte.position.x = Criaturas[i].anima.position.x;
                            Criaturas[i].animaMorte.position.y = Criaturas[i].anima.position.y;
                            if (Criaturas[i].animaMorte.dirOuEsq)
                                Criaturas[i].anima.dirOuEsq = 0;
                            else
                                Criaturas[i].anima.dirOuEsq = 1;
                        }
                        if (Criaturas[i].vida > 0 && Criaturas[i].tipo == 3)
                        {
                            Criaturas[i].anima.lrTimer++;
                            if (Criaturas[i].anima.lrTimer > 20)
                            {
                                Criaturas[i].anima.delta = Criaturas[i].anima.oldposition.x - Criaturas[i].anima.position.x;
                                Criaturas[i].anima.oldposition.x = Criaturas[i].anima.position.x;
                                Criaturas[i].anima.lrTimer = 0;
                            }
                            Criaturas[i].anima.frameCounter++;
                            Criaturas[i].anima.position.x = Criaturas[i].colisao.x;
                            Criaturas[i].anima.position.y = Criaturas[i].colisao.y;
                            AnimarCriatura1(&Criaturas[i].anima, &criatura1TexRed, 96, 96);
                            Criaturas[i].anima.oldposition.y = Criaturas[i].anima.position.y;
                            Criaturas[i].animaMorte.position.x = Criaturas[i].anima.position.x;
                            Criaturas[i].animaMorte.position.y = Criaturas[i].anima.position.y;
                            if (Criaturas[i].animaMorte.dirOuEsq)
                                Criaturas[i].anima.dirOuEsq = 0;
                            else
                                Criaturas[i].anima.dirOuEsq = 1;
                        }
                        if (Criaturas[i].vida > 0 && Criaturas[i].tipo == 4)
                        {
                            AnguloAt = atan2(jogador.colisao.y - Criaturas[i].colisao.y, jogador.colisao.x - Criaturas[i].colisao.x);
                            Criaturas[i].anima.angulo = AnguloAt;
                            (Criaturas[i]).ataque.x = Criaturas[i].colisao.x + (float)cos(AnguloAt) * 130;
                            (Criaturas[i]).ataque.y = Criaturas[i].colisao.y + (float)sin(AnguloAt) * 130;
                            Criaturas[i].anima.lrTimer++;
                            if (Criaturas[i].anima.lrTimer > 30)
                            {
                                Criaturas[i].anima.delta = Criaturas[i].anima.oldposition.x - Criaturas[i].anima.position.x;
                                Criaturas[i].anima.oldposition.x = Criaturas[i].anima.position.x;
                                Criaturas[i].anima.lrTimer = 0;
                            }
                            Criaturas[i].anima.frameCounter++;
                            if (Criaturas[i].prontoPraAtacar == 1)
                            {
                                (Criaturas[i]).ataque.width = 120;
                                (Criaturas[i]).ataque.height = 120;
                                //DrawRectangleRec((Criaturas[i]).ataque, RED);
                                achouOplayer(&Criaturas[i], &jogador);
                            }
                            else
                            {
                                //DrawRectangleRec((Criaturas[i]).ataque, RED);
                                Criaturas[i].frameTimer++;
                                if (Criaturas[i].frameTimer > 170)
                                {
                                    //printf("\n%d\n", Criaturas[i].frameTimer);
                                    Criaturas[i].frameTimer = 0;
                                    Criaturas[i].prontoPraAtacar = 1;
                                    Criaturas[i].velocidade = 2;
                                }
                            }
                            Criaturas[i].anima.position.x = Criaturas[i].colisao.x;
                            Criaturas[i].anima.position.y = Criaturas[i].colisao.y;
                            Criaturas[i].anima.oldposition.y = Criaturas[i].anima.position.y;
                            Criaturas[i].animaMorte.position.x = Criaturas[i].anima.position.x;
                            Criaturas[i].animaMorte.position.y = Criaturas[i].anima.position.y;
                            Criaturas[i].animaMorte.dirOuEsq = Criaturas[i].anima.dirOuEsq;
                            //DrawRectangleRec(Criaturas[i].anima.position.x);
                            if (Criaturas[i].anima.estaAtacando == 0)
                            {
                                AnimarCriatura2(&Criaturas[i].anima, &criatura2Tex, &criaturaRec[2], criatura2Tex.width / 35, criatura2Tex.height / 7, 64 + 128, 32 + 128 - 32);
                            }
                            else
                            {
                                if (Criaturas[i].anima.currentFrame == 7)
                                    atingiuOPlayer2(&Criaturas[i], &jogador);
                                Criaturas[i].anima.delayAnimacao++;
                                if (Criaturas[i].anima.delayAnimacao < 20)
                                {
                                    Criaturas[i].anima.frameSpeed = 2;
                                }
                                if (Criaturas[i].anima.delayAnimacao > 20)
                                {

                                    Criaturas[i].anima.frameSpeed = 3;
                                    AnimarCriatura2(&Criaturas[i].anima, &criatura2TexAttack, &criaturaRecMorte[2], 2 * criatura2TexAttack.width / (17 * 3.5), criatura2TexAttack.height / (3.5), 96 + 32 + 128, 64 + 128 - 32);
                                    if (Criaturas[i].anima.currentFrame == 7 || Criaturas[i].anima.currentFrame == 8)
                                        atingiuOPlayer2(&Criaturas[i], &jogador);
                                }
                                else
                                {

                                    AnimarCriatura2(&Criaturas[i].anima, &criatura2Tex, &criaturaRec[2], 2 * criatura2Tex.width / 70, 2 * criatura2Tex.height / 14, 64 + 128, 32 + 128 - 32);
                                    Criaturas[i].anima.frameSpeed = 2;
                                }
                            }
                        }
                    }
                    //Movimentando player
                    acao = movimentarPlayer(&jogador, grid, qtdDeParedes);
                    //Animando player
                    switch (acao)
                    {
                    case 0: //Direita
                        acaoAntiga = 0;
                        break;
                    case 1: //Esquerda
                        acaoAntiga = 1;
                        break;
                    case 5:
                        acao = acaoAntiga;
                        break;
                    case 3:
                        if (acaoAntiga == 0)
                            acao = 3;
                        else
                            acao = 2;
                        break;
                    }
                    player.frameCounter++;
                    player.position.x = jogador.colisao.x;
                    player.position.y = jogador.colisao.y;
                    if (armaAtiva == 1)
                    {
                        player.ativo = 1;
                        player.frameSpeed = 12;
                        plasma.ativo = 1;
                    }
                    if (armaAtiva == 2)
                    {
                        player.ativo = 2;
                        player.frameSpeed = 18;
                        plasma.ativo = 2;
                    }
                    //DrawRectangleRec(jogador.colisao, WHITE);
                    AnimarPlayer(&player, acao);
                    //Atualizando a camera
                    cameraJogador.target = (Vector2){jogador.colisao.x, jogador.colisao.y};
                    //cameraJogador.zoom = 1.1f; //0.75 paradao

                    //Atualizando a mira
                    SetMouseScale(1.0f, 1.0f);
                    miraPosicao = GetMousePosition();
                    miraPosicao.x += cameraJogador.target.x - cameraJogador.offset.x;
                    miraPosicao.y += cameraJogador.target.y - cameraJogador.offset.y;
                    DrawTextureEx(miraTex, miraPosicao, 0.0f, 0.15f, BLUE);
                    //Atualizando os pontos de colisao do mapa

                    if (IsKeyDown(KEY_ONE))
                    {
                        armaAtiva = 1;
                        jogador.velocidade = 8;
                    }
                    if (IsKeyDown(KEY_TWO))
                    {
                        armaAtiva = 2;
                        jogador.velocidade = 15;
                    }

                    if (balasGastasPrincipal < 256 && armaAtiva == 1)
                        playerEstaAtirando(&armaPrincipal[balasGastasPrincipal], jogador, &balasGastasPrincipal, tiro, miraPosicao, armaAtiva, modRifle, modPistola);
                    if (balasGastasSecundaria < 1024 && armaAtiva == 2)
                        playerEstaAtirando(&armaSecundaria[balasGastasSecundaria], jogador, &balasGastasSecundaria, tiro, miraPosicao, armaAtiva, modRifle, modPistola);
                    for (int i = 0; i < wave * 5; i++)
                        if (Criaturas[i].vida > 0 && Criaturas[i].tipo != 4)
                            atingiuOPlayer(&Criaturas[i], &jogador);
                    for (int i = 0; i < 256; i++)
                    {
                        if (armaPrincipal[i].viva == 0)
                            continue;
                        armaPrincipal[i].frameCounter++;
                        movimentarProjetil(&armaPrincipal[i], grid);
                        plasma.position = (Vector2){armaPrincipal[i].colisao.x, armaPrincipal[i].colisao.y};
                        plasma.ativo = 1;
                        plasma.angulo = armaPrincipal[i].angulo;
                        plasma.frameCounter = armaPrincipal[i].frameCounter;
                        AnimarBala(&plasma);
                        armaPrincipal[i].frameCounter = plasma.frameCounter;
                        criaturasVivas -= acertouACriatura(&armaPrincipal[i], &Criaturas, wave * 5, &pontuacao, &moeda);
                    }
                    for (int i = 0; i < 1024; i++)
                    {
                        if (armaSecundaria[i].viva == 0)
                            continue;
                        armaSecundaria[i].frameCounter++;
                        movimentarProjetil(&armaSecundaria[i], grid);
                        plasma.position = (Vector2){armaSecundaria[i].colisao.x, armaSecundaria[i].colisao.y};
                        plasma.ativo = 2;
                        plasma.angulo = armaSecundaria[i].angulo;
                        plasma.frameCounter = armaSecundaria[i].frameCounter;
                        AnimarBala(&plasma);
                        armaSecundaria[i].frameCounter = plasma.frameCounter;
                        criaturasVivas -= acertouACriatura(&armaSecundaria[i], &Criaturas, wave * 5, &pontuacao, &moeda);
                    }
                    //********************************************   HUD DO JOGO   ***********************************************************************

                    //BeginMode2D(menu);
                    plasmaHUD.frameCounter++;
                    pcHUD.frameCounter++;
                    heart.frameCounter++;
                    AnimarHud(&pcHUD, pcHUD.textura1, wid / 10, wid / 12, cameraJogador.target.x + wid / 2.8, cameraJogador.target.y - hei / 2.3 - hei / 16);
                    DrawText(TextFormat("%d Breakpoints", pontuacao), cameraJogador.target.x + wid / 8, cameraJogador.target.y - hei / 2.3, 30, PURPLE);
                    DrawText(TextFormat("Aperte 'E' para abrir a loja ou 'P' para pausar o jogo"), cameraJogador.target.x - wid / 2.1, cameraJogador.target.y - hei / 2 + 10, 30, GOLD);
                    DrawText(TextFormat("Moedas: %d", moeda), cameraJogador.target.x - wid / 2.1, cameraJogador.target.y - hei / 2.5, 30, GOLD);

                    DrawText(TextFormat("Vida: %d", jogador.vida), cameraJogador.target.x - wid / 2.5, cameraJogador.target.y + hei / 2.5, 35, RED);
                    AnimarHud(&heart, heart.textura1, wid / 40, wid / 36, cameraJogador.target.x - wid / 2.2, cameraJogador.target.y + hei / 2.6);
                    switch (armaAtiva)
                    {
                    case (1):
                        AnimarHud(&plasmaHUD, plasmaHUD.textura1, hei / 10, hei / 10, cameraJogador.target.x + wid / 2.4, cameraJogador.target.y + hei / 2.2);
                        DrawText(TextFormat("%i", 256 - balasGastasPrincipal), cameraJogador.target.x + wid / 2.3, cameraJogador.target.y + hei / 2.2 - hei / 16, 35, BLUE);
                        break;
                    case (2):
                        AnimarHud(&plasmaHUD, plasmaHUD.textura2, hei / 10, hei / 10, cameraJogador.target.x + wid / 2.4, cameraJogador.target.y + hei / 2.2);
                        DrawText(TextFormat("%i", 1024 - balasGastasSecundaria), cameraJogador.target.x + wid / 2.3, cameraJogador.target.y + hei / 2.2 - hei / 16, 35, (Color){255, 58, 0, 255});
                        //DrawRectangleLines(cameraJogador.target.x + wid/2, cameraJogador.target.y + hei/2 + 54 , 150, 50, (Color){255, 58, 0, 255});
                        break;
                    }
                    EndMode2D();
                    EndDrawing();
                    //********************************************   HUD DO JOGO   ***********************************************************************
                    if (IsKeyDown(KEY_E))
                        abrirLoja(&moeda, &modPistola, &modRifle, &jogador, &armaPrincipal, &armaSecundaria, 256, 1024, &balasGastasPrincipal, &balasGastasSecundaria);

                    if (jogador.vida <= 0)
                    {
                        while (IsKeyUp(KEY_SPACE))
                        {
                            BeginMode2D(menu);
                            free(Criaturas);
                            ClearBackground(BLACK);
                            DrawText("VOCE MORREU!", 345, 400, 150, RED);
                            DrawText("Pressione espaço para comecar novamente ou p para sair", 490, 600, 30, LIGHTGRAY);
                            DrawText(TextFormat("pontuação: %i", pontuacao), 600, 700, 90, YELLOW);
                            EndDrawing();
                            wave = 1;
                            armaAtiva = 1;
                            modRifle = 0;
                            modPistola = 0;
                            criaturasVivas = 0;
                            moeda = 0;
                            criarWave(wave * 5, &criaturasVivas, &Criaturas, mapa.width, mapa.height, criaturaRec, criaturaRecMorte);
                            inicializaPlayer(&jogador);
                            EndMode2D();
                            if (IsKeyDown(KEY_P))
                            {
                                UnloadImage(menuPng);
                                UnloadTexture(criatura1TexYellow);
                                UnloadTexture(criatura1TexYellowMorte);
                                UnloadTexture(criatura1TexGreen);
                                UnloadTexture(criatura1TexGreenMorte);
                                UnloadTexture(criatura2Tex);
                                UnloadTexture(criatura2TexAttack);
                                UnloadTexture(criatura2TexMorte);
                                UnloadTexture(shadow);
                                UnloadTexture(plasma.textura1);
                                UnloadTexture(plasma.textura2);
                                UnloadTexture(player.textura1);
                                UnloadTexture(player.textura2);
                                UnloadTexture(criatura1TexRed);
                                UnloadTexture(criatura1TexRedMorte);
                                UnloadTexture(mapa);
                                UnloadMusicStream(music);
                                CloseAudioDevice();
                                CloseWindow();
                                return 0;
                            }
                            if (IsKeyDown(KEY_SPACE))
                                pontuacao = 0;
                        }
                    }
                }
                free(Criaturas);
                wave++;
            }
        }
    }
    UnloadImage(menuPng);
    UnloadTexture(criatura1TexYellow);
    UnloadTexture(criatura1TexYellowMorte);
    UnloadTexture(criatura1TexGreen);
    UnloadTexture(criatura1TexGreenMorte);
    UnloadTexture(criatura2Tex);
    UnloadTexture(criatura2TexAttack);
    UnloadTexture(criatura2TexMorte);
    UnloadTexture(shadow);
    UnloadTexture(plasma.textura1);
    UnloadTexture(plasma.textura2);
    UnloadTexture(plasmaHUD.textura1);
    UnloadTexture(plasmaHUD.textura2);
    UnloadTexture(player.textura1);
    UnloadTexture(player.textura2);
    UnloadTexture(criatura1TexRed);
    UnloadTexture(criatura1TexRedMorte);
    UnloadTexture(mapa);
    UnloadMusicStream(music);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
