#include <efi.h>
#include <efilib.h>

UINT32 squareSize = 20;
uint32_t pixels;

uint32_t width;
uint32_t height;
uint32_t playerPosX = 20;
uint32_t playerPosY = 20;

#define RED 0x00FF0000
#define GREEN 0x0000FF00
#define MAGENTA 0x00FF00FF
#define BLACK 0x00000000
#define CYAN 0x0000FFFF
#define YELLOW 0x00FFFF00
#define BLUE 0x000000FF

#define COINS_NUMBER 6
#define WALLS_NUMBER 216
#define ENEMIES_NUMBER 8

typedef struct {
    uint32_t x;
    uint32_t y;
    BOOLEAN collected;
} Coin;

Coin coins[COINS_NUMBER];

typedef struct {
    uint32_t x;
    uint32_t y;
} Wall;

Wall walls[WALLS_NUMBER];

typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t direction;
    uint32_t start;
    uint32_t range;
    BOOLEAN isVertical; 
} Enemy;

Enemy enemies[ENEMIES_NUMBER];

void initEnemies(){
    enemies[0] = (Enemy){40, 80, 1, 40, 1, TRUE};
    enemies[1] = (Enemy){40, 380, 1, 40, 1, TRUE};
    enemies[2] = (Enemy){740, 80, -1, 740, 1, TRUE};
    enemies[3] = (Enemy){740, 380, -1, 740, 1, TRUE};
    enemies[4] = (Enemy){120, 220, -1, 220, 2, FALSE};
    enemies[5] = (Enemy){660, 220, -1, 220, 2, FALSE};
    enemies[6] = (Enemy){400, 140, 1, 400, 2, TRUE};
    enemies[7] = (Enemy){380, 260, -1, 380, 2, TRUE};
}

EFI_RNG_PROTOCOL *rng = NULL;

uint32_t random(){
    EFI_RNG_PROTOCOL *rng;
    EFI_STATUS Status;
    EFI_GUID rngGuid = EFI_RNG_PROTOCOL_GUID;

    Status = uefi_call_wrapper(BS->LocateProtocol, 3, &rngGuid, NULL, (VOID**)&rng);

    uint32_t randomVal = 0;
    if(!EFI_ERROR(Status)){
        uefi_call_wrapper(rng->GetRNG, 4, rng, NULL, sizeof(uint32_t), (UINT8*)&randomVal);
    }
    return randomVal;
}

BOOLEAN isWallAt(uint32_t x, uint32_t y) {
    for (int i = 0; i < WALLS_NUMBER; i++) {
        if (walls[i].x == x && walls[i].y == y) {
            return TRUE;
        }
    }
    return FALSE;
}

void initRandomEnemies(){
    for(int i=0; i<ENEMIES_NUMBER; i++){
        uint32_t x, y;
        do {
            x = (random() % (width / squareSize - 2) + 1) * squareSize;
            y = (random() % (height / squareSize - 2) + 1) * squareSize;
        } while (isWallAt(x, y) || (x == playerPosX && y == playerPosY));

        uint32_t direction = (random() % 2) * 2 - 1;
        uint32_t vertSeed = random() % 2;
        BOOLEAN vert;
        uint32_t start;
        if(vertSeed == 1){
            vert = TRUE;
            start = x;
        }else{
            vert = FALSE;
            start = y;
        }
        uint32_t range = random() % 3 + 1;
        enemies[i] = (Enemy){x, y, direction, start, range, vert};
    }
}

void initRandomCoins(){
    for(int i=0; i<COINS_NUMBER; i++){
        uint32_t x, y;
        do {
            x = (random() % (width / squareSize - 2) + 1) * squareSize;
            y = (random() % (height / squareSize - 2) + 1) * squareSize;
        } while (isWallAt(x, y) || (x == playerPosX && y == playerPosY));

        coins[i] = (Coin){x, y, FALSE};
    }
}

void initWalls(){
    for(int i=0; i<10; i++){
        walls[i] = (Wall){80, 20+20*i};
    }

    for(int i=0; i<10; i++){
        walls[i+10] = (Wall){80, 440-20*i};
    }

    for(int i=0; i<10; i++){
        walls[i+20] = (Wall){700, 20+20*i};
    }

    for(int i=0; i<10; i++){
        walls[i+30] = (Wall){700, 440-20*i};
    }

    for(int i=0; i<16; i++){
        walls[i+40] = (Wall){160, 80+20*i};
    }

    for(int i=0; i<16; i++){
        walls[i+56] = (Wall){620, 80+20*i};
    }

    for(int i=0; i<10; i++){
        walls[i+72] = (Wall){180+i*20, 200};
    }

    for(int i=0; i<10; i++){
        walls[i+82] = (Wall){420+i*20, 200};
    }

    for(int i=0; i<40; i++){
        walls[i+92] = (Wall){0+i*20, 0};
    }

    for(int i=0; i<40; i++){
        walls[i+132] = (Wall){0+i*20, 460};
    }

    for(int i=1;i<23;i++){
        walls[i+172] = (Wall){0, i*20};
    }

    for(int i=1;i<23;i++){
        walls[i+194] = (Wall){780, i*20};
    }
}

void initCoins(){
    coins[0] = (Coin){40, 40, FALSE};
    coins[1] = (Coin){40, 420, FALSE};
    coins[2] = (Coin){740, 40, FALSE};
    coins[3] = (Coin){740, 420, FALSE};
    coins[4] = (Coin){120, 120, FALSE};
    coins[5] = (Coin){660, 120, FALSE};
}

void drawSquare(uint32_t *buffer, uint32_t posX, uint32_t posY, uint32_t color){
    for (uint32_t y = posY; y < posY+squareSize; y++) {
        for (uint32_t x = posX; x < posX+squareSize; x++) {
            buffer[x + (y * pixels)] = color; 
        }
    }
}

void drawFrame(uint32_t *buffer){
    for(uint32_t x = 0; x<width; x+=squareSize){
        drawSquare(buffer, x, 0, BLUE);
        drawSquare(buffer, x, height-squareSize, BLUE);

    }

    for(uint32_t y = 0; y<height; y+=squareSize){
        drawSquare(buffer, 0, y, BLUE);
        drawSquare(buffer, width-squareSize, y, BLUE);
    }

    for(int i=0; i<WALLS_NUMBER; i++){
        drawSquare(buffer, walls[i].x, walls[i].y, BLUE);
    }
}

void drawCircle(uint32_t *buffer, int32_t centerX, int32_t centerY, int32_t radius, uint32_t color) {
    for (int32_t y = -radius; y <= radius; y++) {
        for (int32_t x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                int32_t pixelX = centerX + 10 + x;
                int32_t pixelY = centerY + 10 + y;

                if (pixelX >= 0 && pixelX < (int32_t)width && pixelY >= 0 && pixelY < (int32_t)height) {
                    buffer[pixelX + (pixelY * pixels)] = color;
                }
            }
        }
    }
}

void drawEnemies(uint32_t *buffer){
    for(int i=0; i<ENEMIES_NUMBER; i++){
        drawSquare(buffer, enemies[i].x, enemies[i].y, RED);
    }
}

void drawCoins(uint32_t *buffer){
    for(int i=0; i<COINS_NUMBER; i++){
        if(coins[i].collected == FALSE){
            drawCircle(buffer, coins[i].x, coins[i].y, 8, YELLOW);
        }
    }
}

void pickUpCoin(){
    for(int i=0; i<COINS_NUMBER; i++){
        if(playerPosX == coins[i].x && playerPosY == coins[i].y && !coins[i].collected){
            coins[i].collected = TRUE;
            break;
        }
    }
}

void refresh(uint32_t *buffer){
    drawSquare(buffer, playerPosX, playerPosY, CYAN);
    drawCoins(buffer);
    drawEnemies(buffer);
}

BOOLEAN isMoveAllowed(uint32_t *buffer, uint32_t newX, uint32_t newY){
    if(newX == width - squareSize || newX < squareSize) return FALSE;
    if(newY == height - squareSize || newY < squareSize) return FALSE;

    for(int i=0; i<WALLS_NUMBER; i++){
        if(newX == walls[i].x && newY == walls[i].y){
            return FALSE;
        }
    }

    drawSquare(buffer, playerPosX, playerPosY, BLACK);
    return TRUE;
}

BOOLEAN checkEnemyCollision(){
    for(int i=0;i<ENEMIES_NUMBER;i++){
        if(playerPosX == enemies[i].x && playerPosY == enemies[i].y) return TRUE;
    }
    return FALSE;
}

void restart(uint32_t *buffer){
    for(int i=0; i<COINS_NUMBER; i++){
        coins[i].collected = FALSE;
    }
    playerPosX = 20;
    playerPosY = 20;
    refresh(buffer);
}

BOOLEAN checkFinish(){
    for(int i=0; i<COINS_NUMBER; i++){
        if(!coins[i].collected) return FALSE;
    }
    return TRUE;
}

int menu() {
    UINTN index;
    EFI_INPUT_KEY key;

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);

    Print(L"\r\n--- PACKAGE GUY ---\r\n\r\n");
    Print(L"  (1) STANDARD\r\n");
    Print(L"  (2) RANDOM\r\n");
    Print(L"  (3) EXIT\r\n");

    while(1) {
        uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

        if(key.UnicodeChar == L'1') return 1;
        if(key.UnicodeChar == L'2') return 2;
        if(key.UnicodeChar == L'3') return 3;
    }
}

int endScreen() {
    UINTN index;
    EFI_INPUT_KEY key;

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 0);

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);

    Print(L"\r\n---YOU WON---\r\n\r\n");
    Print(L"  (1) PLAY AGAIN\r\n");
    Print(L"  (2) EXIT\r\n");

    while(1) {
        uefi_call_wrapper(BS->WaitForEvent, 3, 1, &ST->ConIn->WaitForKey, &index);
        uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

        if(key.UnicodeChar == L'1') return 1;
        if(key.UnicodeChar == L'2') return 2;
    }
}

void clearScreen(uint32_t *buffer){
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            buffer[x + (y * pixels)] = 0x00000000; 
        }
    }
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    EFI_GUID rngGuid = EFI_RNG_PROTOCOL_GUID;
    uefi_call_wrapper(BS->LocateProtocol, 3, &rngGuid, NULL, (VOID**)&rng);

    initWalls();

    EFI_STATUS Status;
    EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

    Status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (VOID**)&gop);

    if (EFI_ERROR(Status)) {
        Print(L"Nie udalo sie znalezc GOP! Error: %r\n", Status);
        return Status;
    }

    uint32_t *buffer = (uint32_t *)gop->Mode->FrameBufferBase;
    uint32_t ppsl = gop->Mode->Info->PixelsPerScanLine;
    width = gop->Mode->Info->HorizontalResolution;
    height = gop->Mode->Info->VerticalResolution;

    UINTN i;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
    UINTN sizeOfInfo;
    UINT32 mode = 2;

    Status=uefi_call_wrapper(gop->SetMode, 2, gop, mode);

    buffer = (uint32_t *)gop->Mode->FrameBufferBase;
    ppsl = gop->Mode->Info->PixelsPerScanLine;
    pixels = ppsl;
    width = gop->Mode->Info->HorizontalResolution;
    height = gop->Mode->Info->VerticalResolution;

    clearScreen(buffer);

    int choice = menu();
    clearScreen(buffer);
    if(choice == 3) return  EFI_SUCCESS;
    if(choice == 2) {
        initRandomEnemies();
        initRandomCoins();
    }
    if(choice == 1) {
        initEnemies();
        initCoins();
    }
    drawFrame(buffer);

    drawSquare(buffer, playerPosX, playerPosY, CYAN);
    drawCoins(buffer);

    EFI_INPUT_KEY key;
    UINTN index;

    EFI_EVENT Timer;
    uefi_call_wrapper(BS->CreateEvent, 5, EVT_TIMER, TPL_CALLBACK, NULL, NULL, &Timer);
    uefi_call_wrapper(BS->SetTimer, 3, Timer, TimerPeriodic, 6000000);

    while(1){
        Status = uefi_call_wrapper(BS->CheckEvent, 1, Timer);
        if(Status == EFI_SUCCESS){
            for(int i=0;i <ENEMIES_NUMBER; i++){
                drawSquare(buffer, enemies[i].x, enemies[i].y,  BLACK);
                if(enemies[i].isVertical){
                    if(enemies[i].x+enemies[i].direction*squareSize > enemies[i].start + squareSize * enemies[i].range
                        || enemies[i].x+enemies[i].direction*squareSize < enemies[i].start - squareSize * enemies[i].range
                        || isWallAt(enemies[i].x+enemies[i].direction*squareSize, enemies[i].y)){
                        enemies[i].direction *= -1;
                    }
                    enemies[i].x += enemies[i].direction * squareSize;
                }else{
                    if(enemies[i].y+enemies[i].direction*squareSize > enemies[i].start + squareSize * enemies[i].range 
                        || enemies[i].y+enemies[i].direction*squareSize < enemies[i].start - squareSize * enemies[i].range
                        || isWallAt(enemies[i].x, enemies[i].y+enemies[i].direction*squareSize)){
                        enemies[i].direction *= -1;
                    }
                    enemies[i].y += enemies[i].direction * squareSize;
                }
            }
            drawEnemies(buffer);
        }

        Status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

        if(Status == EFI_SUCCESS){
            if(key.ScanCode == SCAN_RIGHT){
                uint32_t newX = playerPosX + squareSize;
                if(isMoveAllowed(buffer, newX, playerPosY)){
                    playerPosX = newX;
                    pickUpCoin();
                    refresh(buffer);
                }
            }

            if(key.ScanCode == SCAN_LEFT){
                uint32_t newX = playerPosX - squareSize;
                if(isMoveAllowed(buffer, newX, playerPosY)){
                    playerPosX = newX;
                    pickUpCoin();
                    refresh(buffer);
                }
            }

            if(key.ScanCode == SCAN_UP){
                uint32_t newY = playerPosY - squareSize;
                if(isMoveAllowed(buffer, playerPosX, newY)){
                    playerPosY = newY;
                    pickUpCoin();
                    refresh(buffer);
                }
            }

            if(key.ScanCode == SCAN_DOWN){
                uint32_t newY = playerPosY + squareSize;
                if(isMoveAllowed(buffer, playerPosX, newY)){
                    playerPosY = newY;
                    pickUpCoin();
                    refresh(buffer);
                }
            }
        }

        if(checkEnemyCollision()){
            restart(buffer);
        }

        if(checkFinish()){
            clearScreen(buffer);
            int choice = endScreen();
            if(choice == 1){
                clearScreen(buffer);
                int menuChoice = menu();
                if(menuChoice == 1){
                    initEnemies();
                    initCoins();
                    restart(buffer);
                    clearScreen(buffer);
                    drawFrame(buffer);
                    drawSquare(buffer, playerPosX, playerPosY, CYAN);
                    drawCoins(buffer);

                }
                if(menuChoice == 2){
                    initRandomEnemies();
                    initRandomCoins();
                    restart(buffer);
                    clearScreen(buffer);
                    drawFrame(buffer);
                    drawSquare(buffer, playerPosX, playerPosY, CYAN);
                    drawCoins(buffer);
                }
                if(menuChoice == 3) {
                    clearScreen(buffer);
                    return EFI_SUCCESS;
                }
            }
            if(choice == 2){
                clearScreen(buffer);
                return EFI_SUCCESS;
            }
        }
    }


    return EFI_SUCCESS;
}