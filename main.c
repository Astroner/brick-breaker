#include <SDL2/SDL.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define PLAYER_WIDTH 100
#define PLAYER_HEIGHT 20
#define PLAYER_SPEED 9

#define BALL_SIZE 20
#define BALL_SPEED 5

#define BRICK_WIDTH 100
#define BRICK_HEIGHT 20
#define BRICKS_ROWS 5
#define BRICKS_COLS 10
#define BRICKS_VERTICAL_GAP 20
#define BRICKS_TOP_PADDING 50

#define FPS 120
#define MS_PER_FRAME 1000 / FPS

int playerDirection = 0;
SDL_Rect player = {
    .h = PLAYER_HEIGHT,
    .w = PLAYER_WIDTH,
    .x = (WINDOW_WIDTH - PLAYER_WIDTH) / 2,
    .y = WINDOW_HEIGHT - PLAYER_HEIGHT
};

struct {
    SDL_Rect pos;
    SDL_Point speed;
} ball = {
    .pos = {
        .h = BALL_SIZE,
        .w = BALL_SIZE,
        .x = (WINDOW_WIDTH - BALL_SIZE) / 2,
        .y = WINDOW_HEIGHT - PLAYER_HEIGHT - BALL_SIZE
    },
    .speed = {
        .x = BALL_SPEED,
        .y = -BALL_SPEED
    }
};

#define ARR_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

SDL_Rect bricks[BRICKS_ROWS * BRICKS_COLS];
int bricksNumber = BRICKS_ROWS * BRICKS_COLS;

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "Brick Breaker",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int horizontalGap = (WINDOW_WIDTH - BRICK_WIDTH * BRICKS_COLS) / (BRICKS_COLS + 1);
    
    for(size_t i = 0; i < BRICKS_ROWS; i++) {
        for(size_t j = 0; j < BRICKS_COLS; j++) {
            bricks[i * BRICKS_COLS + j].w = BRICK_WIDTH;
            bricks[i * BRICKS_COLS + j].h = BRICK_HEIGHT;
            bricks[i * BRICKS_COLS + j].x = horizontalGap + j * (horizontalGap + BRICK_WIDTH);
            bricks[i * BRICKS_COLS + j].y = BRICKS_TOP_PADDING + i * (BRICKS_VERTICAL_GAP + BRICK_HEIGHT);
        }
    }

    SDL_Event e;
    int quit = 0;
    while(!quit) {
        Uint64 start = SDL_GetPerformanceCounter();

        if(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
                
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym) {
                        case SDLK_a:
                            playerDirection = -1;
                            break;
                        
                        case SDLK_d:
                            playerDirection = 1;
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch(e.key.keysym.sym) {
                        case SDLK_d:
                        case SDLK_a:
                            playerDirection = 0;
                            break;
                    }
                    break;
            }
        }

        if(ball.pos.x <= 0 || ball.pos.x + ball.pos.w >= WINDOW_WIDTH) {
            ball.speed.x *= -1;
        }

        if(ball.pos.y <= 0) {
            ball.speed.y *= -1;
        }

        SDL_Rect intersection;
        if(SDL_IntersectRect(&ball.pos, &player, &intersection)) {
            ball.pos.y -= intersection.h;
            ball.speed.y *= -1;
        }

        for(size_t i = 0; i < ARR_SIZE(bricks); i++) {
            if(bricks[i].w > 0 && SDL_IntersectRect(&ball.pos, bricks + i, &intersection)) {

                bricks[i].w = 0;
                bricksNumber--;

                if(intersection.w > intersection.h) {
                    ball.speed.y *= -1;
                } else {
                    ball.speed.x *= -1;
                }
            }
        }

        if(bricksNumber == 0) {
            printf("WIN\n");
            quit = 1;
        }

        if(ball.pos.y + ball.pos.h >= WINDOW_HEIGHT) {
            printf("LOSE\n");
            quit = 1;
        }

        int nextX = player.x + playerDirection * PLAYER_SPEED;
        if(nextX >= 0 && nextX + PLAYER_WIDTH <= WINDOW_WIDTH) player.x = nextX;

        ball.pos.x += ball.speed.x;
        ball.pos.y += ball.speed.y;



        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        SDL_RenderFillRect(renderer, &player);
        SDL_RenderFillRect(renderer, &ball.pos);

        for(size_t i = 0; i < ARR_SIZE(bricks); i++) {
            if(bricks[i].w > 0) SDL_RenderDrawRect(renderer, bricks + i);
        }

        SDL_RenderPresent(renderer);

        Uint64 end = SDL_GetPerformanceCounter();

        Uint64 duration = (end - start) * 1000 / SDL_GetPerformanceFrequency();

        if(MS_PER_FRAME > duration) {
            SDL_Delay(MS_PER_FRAME - duration);
        }
    }



    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}