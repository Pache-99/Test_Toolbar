#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL3/SDL.h> 
#include <SDL3_image/SDL_image.h>

// Start Screen dimensions
#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900 

// Number of Buttons (enum Button Property)
#define BUTTON_NUM 6
// Button Status (enum ButtonStatus)
#define BUTTON_STAT 3

// Number of Side Buttons
#define SIDE_BUTTON_NUM 2

// Image Path 
#define BACKGROUND_IMAGE_PATH "../image/basic-bg2.jpg"
#define LEFTSLIDER_IMAGE_PATH "../image/button_2/bg-left.png"
#define RIGHTSLIDER_IMAGE_PATH "../image/button_2/bg-right.png"
#define UPSLIDER_IMAGE_PATH "../image/button_2/bg-bottom-down.png"
#define DOWNSLIDER_IMAGE_PATH "../image/button_2/bg-bottom-up.png"

enum ButtonProperty
{
    PIN,
    LOGIN,
    MULTI,
    MINI,
    MAX,
    EXIT
};

enum ButtonStatus
{
    CLICK,
    DEFAULT,
    HOVER
};

typedef struct
{
    SDL_FRect *backRect;
    SDL_FRect *buttonRect[BUTTON_NUM];
    SDL_FRect *sideRect[SIDE_BUTTON_NUM];
    SDL_FRect *downRect; // For seek
    SDL_FRect *upRect; // For hide 
} ToolbarComp;

ToolbarComp* toolbarComp;

ToolbarComp* createToolbarComp(SDL_FRect* backRect, SDL_FRect buttonRect[], SDL_FRect sideRect[],
SDL_FRect* downRect, SDL_FRect* upRect)
{
    toolbarComp = (ToolbarComp*)malloc(sizeof(ToolbarComp));
    if (toolbarComp != NULL){
        toolbarComp->backRect = backRect;
        
        for (int i=0; i<BUTTON_NUM; i++)
        {
            toolbarComp->buttonRect[i] = &buttonRect[i];
        }

        for (int i=0; i<SIDE_BUTTON_NUM; i++)
        {
            toolbarComp->sideRect[i] = &sideRect[i];
        }

        toolbarComp->downRect = downRect;
        toolbarComp->upRect = upRect;
    }

    return toolbarComp;
}

// Change Button Icons
SDL_Texture* buttonIcon(enum ButtonProperty button, enum ButtonStatus status, SDL_Renderer *renderer);
// Check Button Properties
int checkButton(enum ButtonProperty button, SDL_Window *window, SDL_Renderer *renderer, bool toolActivate);
// Print Toolbar
SDL_Renderer* printToolbar(SDL_Renderer* renderer, bool toolActivate);
// Resize Window 
int resizeWindow(SDL_Window* window, bool toolMoving);
// Hide Toolbar
int hideToolbar();
// Seek Toolbar
int seekToolbar();

int test();

int main(int argc, char* argv[]){
   
    (void) argc;
    (void) argv;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not be initialized!\n"
               "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }
    
    test();
}

int test()
{
    SDL_Window *window = SDL_CreateWindow("Basic C SDL project",
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_RESIZABLE); 
    if(!window)
    {
        printf("Window could not be created!\n"
               "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
        if(!renderer)
        {
            printf("Renderer could not be created!\n"
                   "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            // Background rect of window 
            SDL_FRect backRect;
            backRect.w = SCREEN_WIDTH;
            backRect.h = SCREEN_HEIGHT;
            backRect.x = 0;
            backRect.y = 0;

            // Buttons Array 
            SDL_FRect buttonArr[BUTTON_NUM];

            int button_W, button_X, button_H, button_Y;
            button_W = SCREEN_WIDTH / 32;
            button_H = (button_W * 5) / 6; // ratio w:h = 6:5
            button_Y = 0; 

            // Button tray
            // * This is a calculator.
            SDL_Rect squareRect;
            squareRect.w = button_W * 6;
            squareRect.h = button_H;
            squareRect.x = (SCREEN_WIDTH / 2) - (button_W * (BUTTON_NUM / 2));
            squareRect.y = 0;

            // Activate Double buffering 
            SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

            // Render BackGround image
            SDL_Texture* backTexture = IMG_LoadTexture(renderer, BACKGROUND_IMAGE_PATH); 
            SDL_RenderTexture(renderer, backTexture, NULL, &backRect); 
            
            // Render Buttons image 
            for (int i=0; i<BUTTON_NUM; i++)
            {
                button_X = squareRect.x + (i * button_W);
                buttonArr[i] = (SDL_FRect){button_X, button_Y, button_W, button_H};

                SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, &buttonArr[i]);
            }

            // Side Slider Array
            SDL_FRect sideSliderArr[2]; 
            int side_W, side_H, side_Y;
            side_H = button_H;
            side_W = side_H / 5; // ratio w:h = 1:5
            side_Y = 0;

            // Down Slider
            SDL_FRect downSlider; // 67 x 12
            int down_W, down_H, down_X, down_Y;
            down_H = SCREEN_HEIGHT / 75;
            down_W = (67 * down_H) / 12; // ratio w:h = 67:12 
            down_X = (SCREEN_WIDTH / 2) - (down_W / 2);
            down_Y = button_Y + button_H;

            // Up Slider
            SDL_FRect upSlider;
            int up_W, up_H, up_X, up_Y;
            up_W = down_W;
            up_H = down_H;
            up_Y = 0;
            up_X = down_X;

            //Side Sliders Texture
            sideSliderArr[0] = (SDL_FRect){(squareRect.x - side_W), side_Y, side_W, side_H};

            sideSliderArr[1] = (SDL_FRect){(squareRect.x + squareRect.w), side_Y, side_W, side_H};

            // Down and Up Sliders Texture 
            downSlider = (SDL_FRect){down_X, down_Y, down_W, down_H};
            upSlider = (SDL_FRect){up_X, up_Y, up_W, up_H};

            toolbarComp = createToolbarComp(&backRect, buttonArr, sideSliderArr, &downSlider, &upSlider);

            // Toolbar activate status 
            bool toolActivate = true;

            renderer = printToolbar(renderer, toolActivate);
            SDL_RenderPresent(renderer);

            bool quit = false;
            float mouseX, mouseY;

            bool clickLeft = false;
            bool clickRight = false;
            bool clickDown = false;
            bool clickUp = false;

            while(!quit)
            {
                SDL_Event e;
                SDL_WaitEvent(&e);

                if(e.type == SDL_EVENT_QUIT)
                {
                    quit = true;
                }
                else if (e.type == SDL_EVENT_KEY_DOWN)
                {
                    if (e.key.scancode == SDL_SCANCODE_ESCAPE) // scancode is physical key event.
                    {
                        quit = true;
                    }
                }
                else if (e.type == SDL_EVENT_WINDOW_RESIZED)
                {
                    resizeWindow(window, false);

                    renderer = printToolbar(renderer, toolActivate);
                    SDL_RenderPresent(renderer);
                }
                else
                {
                    SDL_GetMouseState(&mouseX, &mouseY);

                    SDL_FPoint mousePos = (SDL_FPoint){mouseX, mouseY} ;

                    switch(e.type)
                    {
                        case SDL_EVENT_MOUSE_BUTTON_DOWN:
                            renderer = printToolbar(renderer, toolActivate);

                            // Button Array
                            for (int i=0; i<BUTTON_NUM; i++)
                            {
                                SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, &buttonArr[i]);

                                if (SDL_PointInRectFloat(&mousePos, &buttonArr[i])) {
                                    SDL_RenderTexture(renderer, buttonIcon(i, CLICK, renderer), NULL, &buttonArr[i]);
                                }
                            }

                            // Side Sliders
                            for (int i = 0; i<SIDE_BUTTON_NUM; i++)
                            {
                                if (SDL_PointInRectFloat(&mousePos, &sideSliderArr[0])){
                                    clickLeft = true;
                                }
                                else if (SDL_PointInRectFloat(&mousePos, &sideSliderArr[1])){
                                    clickRight = true;
                                }
                                else{
                                    clickLeft = false;
                                    clickRight = false;
                                }
                            }

                            if (SDL_PointInRectFloat(&mousePos, &downSlider)){
                                clickDown = true;
                            }
                            else{
                                clickDown = false;
                            }
                            
                            if (SDL_PointInRectFloat(&mousePos, &upSlider)){
                                clickUp = true;
                            }
                            else{
                                clickUp = false;
                            }

                            SDL_RenderPresent(renderer);
                            break;

                        case SDL_EVENT_MOUSE_BUTTON_UP:  
                            renderer = printToolbar(renderer, toolActivate);

                            if (SDL_PointInRectFloat(&mousePos, &downSlider) && clickDown){
                                toolActivate = false; // For Hide
                                hideToolbar(window, renderer);
                                SDL_Log("Click Down Slider");
                                printToolbar(renderer, toolActivate);
                            }
                            else if (SDL_PointInRectFloat(&mousePos, &upSlider) && clickUp && !toolActivate){
                                seekToolbar(window, renderer); // For Seek 
                                SDL_Log("Click Up Slider");
                                toolActivate = true;
                                printToolbar(renderer, toolActivate);
                                SDL_RenderPresent(renderer); 
                            }
                            else{
                                for (int i=0; i<BUTTON_NUM; i++)
                                {
                                    SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, &buttonArr[i]);

                                    if (SDL_PointInRectFloat(&mousePos, &buttonArr[i])) {

                                        SDL_RenderTexture(renderer, buttonIcon(i, HOVER, renderer), NULL, &buttonArr[i]);

                                        if (!toolActivate){
                                            break;
                                        }
                                        else{
                                            printToolbar(renderer, toolActivate);

                                            if (checkButton(i, window, renderer, toolActivate) == EXIT){
                                                return 0;
                                            }
                                        }                                  
                                    }
                                }
                            }

                            if (clickLeft || clickRight){
                                clickLeft = false;
                                clickRight = false;
                            }

                            SDL_RenderPresent(renderer);
                            break;

                        case SDL_EVENT_MOUSE_MOTION: 
                            renderer = printToolbar(renderer, toolActivate);

                            for (int i=0; i<BUTTON_NUM; i++)
                            {
                                if (SDL_PointInRectFloat(&mousePos, &buttonArr[i])) {
                                    SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, &buttonArr[i]);
                                    SDL_RenderTexture(renderer, buttonIcon(i, HOVER, renderer), NULL, &buttonArr[i]);
                                }
                                else{
                                    SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, &buttonArr[i]);
                                }
                            }

                            // TODO: 현재 윈도우 사이즈를 얻을 수 있는 것
                            // 현재 윈도우 사이즈를 얻어 툴바가 이동 가능한 공간 넓이를 얻는 것.  

                            if (clickLeft){
                                SDL_Log("CHECK LEFT");
                                
                            }
                            else if (clickRight){
                                SDL_Log("CHECK RIGHT");
                            }

                            SDL_RenderPresent(renderer);
                            break;
                    }
                }

            }
            free(toolbarComp);
            SDL_DestroyRenderer(renderer);
        }
        SDL_DestroyWindow(window);
    }
    SDL_Quit();

    return 0;
}

SDL_Texture* buttonIcon(enum ButtonProperty button, enum ButtonStatus status, SDL_Renderer *renderer) {
    char* pinName = "1_pin";
    char* loginName = "2_login";
    char* multiName = "3_multi";
    char* miniName = "4_mini";
    char* maxName = "5_max";
    char* exitName = "6_exit";
    
    char* clickEnd = "_click.png";
    char* defaultEnd = "_default.png";
    char* hoverEnd = "_hover.png";

    char* frontChar;
    char* backChar;

    switch(button)
    {
        case PIN:
            frontChar = pinName;
            break;
        case LOGIN:
            frontChar = loginName;
            break;
        case MULTI:
            frontChar = multiName;
            break;
        case MINI:
            frontChar = miniName;
            break;
        case MAX:
            frontChar = maxName;
            break;
        case EXIT:
            frontChar = exitName;
            break;
    }

    switch(status)
    {
        case CLICK:
            backChar = clickEnd;
            break;
        case DEFAULT:
            backChar = defaultEnd;
            break;
        case HOVER:
            backChar = hoverEnd;
            break;
    }

    char* rootPath = "../image/button_1/";
    char* charLink = (char*)malloc(strlen(rootPath) + strlen(frontChar) + strlen(backChar) + 1);

    strcpy(charLink, rootPath);
    strcat(charLink, frontChar);
    strcat(charLink, backChar); 

    SDL_Texture* iconTexture = IMG_LoadTexture(renderer, charLink);
    return iconTexture;
}

int checkButton(enum ButtonProperty button, SDL_Window *window, SDL_Renderer *renderer, bool toolActivate)
{
    static bool maxStat = false; // True mean max window, False mean normal size window 

    switch(button)
    {
        case PIN:
            SDL_Log("Click Pin");
            break;
        case LOGIN:
            SDL_Log("Click Login");
            break;
        case MULTI:
            SDL_Log("Click Multi");
            break;
        case MINI:
            SDL_MinimizeWindow(window);
            printToolbar(renderer, true);
            break;
        case MAX:
            if (!maxStat){
                SDL_SetWindowFullscreen(window, true);
                maxStat = true;
            }
            else{
                SDL_SetWindowFullscreen(window, false);
                maxStat = false;
            }
            break;
        case EXIT:
            SDL_Quit();
            return EXIT;
            break;
    }

    return 1;
}

int resizeWindow(SDL_Window* window, bool toolMoving)
{
    int width, height;
                        
    SDL_GetWindowSize(window, &width, &height);
    SDL_Log("WINDOW SIZE: w=> %d h=> %d", width, height);

    // Resizing Background 
    float back_X, back_Y, back_W, back_H;
    back_W = width;
    back_H = height;
    back_X = (width / 2) - (back_W / 2);
    back_Y = 0;

    *(toolbarComp->backRect) = (SDL_FRect){back_X, back_Y, back_W, back_H};

    // Resizing Button Array 
    float button_X, button_Y, button_W, button_H;
    button_W = width / 32;
    button_H = (button_W * 5) / 6;

    if (!toolMoving){
        button_Y = 0;
    }
    else{
        button_Y = toolbarComp->buttonRect[0]->y;
    }

    // Resizing Button tray
    // * This is a calculator.
    SDL_Rect squareRect;
    squareRect.w = button_W * 6;
    squareRect.h = button_H;
    squareRect.x = (width / 2) - (button_W * (BUTTON_NUM / 2));
    squareRect.y = 0;

    for (int i=0; i<BUTTON_NUM; i++)
    {
        button_X = squareRect.x + (i * button_W);
        *(toolbarComp->buttonRect[i]) = (SDL_FRect){button_X, button_Y, button_W, button_H};
    }

    // Resizing Side Slider Array
    SDL_FRect sideSliderArr[2]; 
    int side_W, side_H, side_Y;
    side_H = button_H;
    side_W = side_H / 5; // ratio w:h = 1:5
    side_Y = 0;
    
    *(toolbarComp->sideRect[0]) = (SDL_FRect){(squareRect.x - side_W), side_Y, side_W, side_H};
    *(toolbarComp->sideRect[1]) = (SDL_FRect){(squareRect.x + squareRect.w), side_Y, side_W, side_H};

    // Resizing Down Slider
    int down_W, down_H, down_X, down_Y;
    down_H = height / 75;
    down_W = (67 * down_H) / 12; // ratio w:h = 67:12 
    down_X = (width / 2) - (down_W / 2);
    down_Y = button_Y + button_H;

    *(toolbarComp->downRect) = (SDL_FRect){down_X, down_Y, down_W, down_H};

    // Resizing Up Slider
    int up_W, up_H, up_X, up_Y;
    up_W = down_W;
    up_H = down_H;
    up_Y = 0;
    up_X = down_X;

    *(toolbarComp->upRect) = (SDL_FRect){up_X, up_Y, up_W, up_H};

    return 0; 
}

int hideToolbar(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_Texture* backTexture = IMG_LoadTexture(renderer, BACKGROUND_IMAGE_PATH); 
    SDL_RenderTexture(renderer, backTexture, NULL, toolbarComp->backRect); 

    SDL_RenderPresent(renderer);

    for (int i=0; i<BUTTON_NUM; i++)
    {
        toolbarComp->buttonRect[i]->y -= toolbarComp->buttonRect[i]->h;
    }

    for (int i=0; i<SIDE_BUTTON_NUM; i++)
    {
        toolbarComp->sideRect[i]->y -= toolbarComp->sideRect[i]->h;
    }

    return 0;
}

int seekToolbar(SDL_Window* window, SDL_Renderer* renderer)
{
    SDL_Texture* backTexture = IMG_LoadTexture(renderer, BACKGROUND_IMAGE_PATH); 
    SDL_RenderTexture(renderer, backTexture, NULL, toolbarComp->backRect); 

    SDL_RenderPresent(renderer);

    for (int i=0; i<BUTTON_NUM; i++)
    {
        toolbarComp->buttonRect[i]->y += toolbarComp->buttonRect[i]->h;
    }

    for (int i=0; i<SIDE_BUTTON_NUM; i++)
    {
        toolbarComp->sideRect[i]->y += toolbarComp->sideRect[i]->h;
    }

    return 0;
}

SDL_Renderer* printToolbar(SDL_Renderer* renderer, bool toolActivate)
{
    SDL_Texture* backTexture = IMG_LoadTexture(renderer, BACKGROUND_IMAGE_PATH); 
    SDL_RenderTexture(renderer, backTexture, NULL, toolbarComp->backRect); 

    for (int i = 0; i<BUTTON_NUM; i++)
    {
        SDL_RenderTexture(renderer, buttonIcon(i, DEFAULT, renderer), NULL, toolbarComp->buttonRect[i]);
    }

    SDL_Texture* leftSliderTexture = IMG_LoadTexture(renderer, LEFTSLIDER_IMAGE_PATH);
    SDL_RenderTexture(renderer, leftSliderTexture, NULL, toolbarComp->sideRect[0]); 

    SDL_Texture* rightSliderTexture = IMG_LoadTexture(renderer, RIGHTSLIDER_IMAGE_PATH);
    SDL_RenderTexture(renderer, rightSliderTexture, NULL, toolbarComp->sideRect[1]); 

    if (toolActivate){
        SDL_Texture* downSliderTexture = IMG_LoadTexture(renderer, DOWNSLIDER_IMAGE_PATH);
        SDL_RenderTexture(renderer, downSliderTexture, NULL, toolbarComp->downRect);
    }
    else{
        SDL_Texture* upSliderTexture = IMG_LoadTexture(renderer, UPSLIDER_IMAGE_PATH);
        SDL_RenderTexture(renderer, upSliderTexture, NULL, toolbarComp->upRect);
    }

    return renderer;
}
