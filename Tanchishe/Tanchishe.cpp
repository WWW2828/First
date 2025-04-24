#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define U 1
#define D 2
#define L 3
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE {
    int x;
    int y;
    struct SNAKE* next;
} snake;

typedef struct USER {
    char username[20];
    char password[20];
    struct USER* next;
} User;

typedef struct GAMELOG {
    char username[50];
    time_t start_time;
    int duration;
    int score;
    struct GAMELOG* next;
} GameLog;

// 游戏全局变量
int score = 0, add = 10;
int status, sleeptime = 200;
snake* head, *food;
snake* q;
int endgamestatus = 0;

// 用户和日志全局变量
User* userList = NULL;
GameLog* gameLogList = NULL;
char currentUser[20] = "";

// 函数声明
void Pos(int x, int y);
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart();
void freeSnake();

// 用户和日志函数
void registerUser();
int loginUser();
void saveUsersToFile();
void loadUsersFromFile();
void saveLogsToFile();
void loadLogsFromFile();
void addGameLog(time_t start, int duration, int score);
void showGameLogs();
void userMenu();
int confirmLogout();
void deleteCurrentUser();

void Pos(int x, int y) {
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, pos);
}

void creatMap() {
    int i;
    for (i = 0; i < 58; i += 2) {
        Pos(i, 0);
        printf("■");
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i < 26; i++) {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

void initsnake() {
    snake* tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++) {
        head = (snake*)malloc(sizeof(snake));
        head->next = tail;
        head->x = 24 + 2 * i;
        head->y = 5;
        tail = head;
    }
    while (tail != NULL) {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}

int biteself() {
    snake* self;
    self = head->next;
    while (self != NULL) {
        if (self->x == head->x && self->y == head->y) {
            return 1;
        }
        self = self->next;
    }
    return 0;
}

void createfood() {
    snake* food_1;
    srand((unsigned)time(NULL));
    food_1 = (snake*)malloc(sizeof(snake));

    do {
        food_1->x = (rand() % 26 + 1) * 2;
        food_1->y = rand() % 24 + 1;

        int overlap = 0;
        q = head;
        while (q != NULL) {
            if (q->x == food_1->x && q->y == food_1->y) {
                overlap = 1;
                break;
            }
            q = q->next;
        }
        if (!overlap) break;
    } while (1);

    Pos(food_1->x, food_1->y);
    food = food_1;
    printf("■");
}

void cantcrosswall() {
    if (head->x <= 0 || head->x >= 56 || 
        head->y <= 0 || head->y >= 26) {
        endgamestatus = 1;
    }
}

void snakemove() {
    // 在移动前检测游戏状态
    if (endgamestatus != 0) return;

    cantcrosswall();

    // 碰撞后立即终止移动
    if (endgamestatus != 0) return;
    
    snake* nexthead;
    nexthead = (snake*)malloc(sizeof(snake));
     // 统一移动逻辑（优化重复代码）
     switch(status) {
        case U: 
            nexthead->x = head->x;
            nexthead->y = head->y - 1;
            break;
        case D:
            nexthead->x = head->x;
            nexthead->y = head->y + 1;
            break;
        case L:
            nexthead->x = head->x - 2;
            nexthead->y = head->y;
            break;
        case R:
            nexthead->x = head->x + 2;
            nexthead->y = head->y;
            break;
    }

    // 食物检测
    if (nexthead->x == food->x && nexthead->y == food->y) {
        // 吃到食物逻辑
        nexthead->next = head;
        head = nexthead;
        score += add;
        
        // 刷新蛇身显示
        snake* q = head;
        while(q) {
            Pos(q->x, q->y);
            printf("■");
            q = q->next;
        }
        createfood();
    } else {
        // 普通移动逻辑
        nexthead->next = head;
        head = nexthead;
        
        // 更新蛇头显示
        Pos(head->x, head->y);
        printf("■");
        
        // 删除蛇尾
        snake* tail = head;
        while(tail->next->next) tail = tail->next;
        Pos(tail->next->x, tail->next->y);
        printf("  ");
        free(tail->next);
        tail->next = NULL;
    }

    // 自检碰撞检测（立即终止）
    if (biteself() == 1) {
        endgamestatus = 2;
        free(nexthead);  // 释放内存
        return;
    }
}

void pause() {
    while (1) {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE)) {
            break;
        }
    }
}

void gamecircle() {
    Pos(64, 4);
    printf("玩家: %s", currentUser);
    Pos(64, 10);
    printf("控制按键：");
    Pos(64, 11);
    printf("↑ ↓ ←- -→ - 移动方向");
    Pos(64, 12);
    printf("Q - 加速，W - 减速");
    Pos(64, 13);
    printf("空格 - 暂停");
    Pos(64, 14);
    printf("ESC - 退出游戏");
    status = R;

    while (endgamestatus == 0) {
        Pos(64, 6);
        printf("当前得分：%d  ", score);
        Pos(64, 7);
        printf("食物加分：%d 分", add);
        Pos(64, 8);
        printf("移动速度：%d ms/步", sleeptime);

        if (GetAsyncKeyState(VK_UP) && status != D) {
            status = U;
            while (GetAsyncKeyState(VK_UP) & 0x8000);
        }
        else if (GetAsyncKeyState(VK_DOWN) && status != U) {
            status = D;
            while (GetAsyncKeyState(VK_DOWN) & 0x8000);
        }
        else if (GetAsyncKeyState(VK_LEFT) && status != R) {
            status = L;
            while (GetAsyncKeyState(VK_LEFT) & 0x8000);
        }
        else if (GetAsyncKeyState(VK_RIGHT) && status != L) {
            status = R;
            while (GetAsyncKeyState(VK_RIGHT) & 0x8000);
        }
        else if (GetAsyncKeyState(VK_SPACE)) {
            pause();
        }
        else if (GetAsyncKeyState(VK_ESCAPE)) {
            endgamestatus = 3;
            break;
        }
        else if (GetAsyncKeyState('Q')) {
            if (sleeptime >= 50) {
                sleeptime = sleeptime - 30;
                add = add + 2;
                if (sleeptime == 320) {
                    add = 2;
                }
            }
        }
        else if (GetAsyncKeyState('W')) {
            if (sleeptime < 350) {
                sleeptime = sleeptime + 30;
                add = add - 2;
                if (sleeptime == 350) {
                    add = 1;
                }
            }
        }
        Sleep(sleeptime);
        
        // 移动前先检测状态
        if (endgamestatus != 0) break;
        snakemove();

         // 移动后立即检测状态
         if (endgamestatus != 0) break;
    }
}

void welcometogame() {
    Pos(40, 12);
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 13);
    printf("玩家: %s", currentUser);
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑,↓,←-,-→分别控制蛇的移动， Q 为加速，W 为减速\n");
    Pos(25, 13);
    printf("加速将能得到更高的分数。\n");
    // system("pause");
    _getch();            // 等待任意按键
    system("cls");       // 立即清屏
}

void endgame() {
    system("cls");
    Pos(24, 12);
    switch (endgamestatus) {
        case 1: printf("对不起，您撞到墙了。游戏结束!"); break;
        case 2: printf("对不起，您咬到自己了。游戏结束!"); break;
        case 3: printf("您已经主动退出游戏"); break;
    }
    Pos(24, 13);
    printf("您的得分是%d\n", score);
    Pos(24, 15);
    printf("按任意键返回菜单...");
   
    while (_kbhit()) {
        _getch();
    }
    Sleep(100);  // 100ms 延迟
    _getch();            // 等待任意按键
    system("cls");       // 立即清屏

    endgamestatus = 0;   // 重置游戏状态
    sleeptime = 200;   // 重置游戏速度
    add = 10;          // 重置分数系数

    freeSnake();        // 强制释放蛇体内存
}

// 内存释放函数
void freeSnake() {
    snake* current = head;
    while (current != NULL) {
        snake* temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    
    if (food != NULL) {
        free(food);
        food = NULL;
    }
}

void gamestart() {
    freeSnake(); // 清除之前的蛇数据
    system("mode con cols=100 lines=30");
    welcometogame();
    creatMap();
    initsnake();
    createfood();
}

void saveUsersToFile() {
    FILE* file = fopen("users.dat", "wb");
    if (file == NULL) return;

    User* current = userList;
    while (current != NULL) {
        fwrite(current, sizeof(User), 1, file);
        current = current->next;
    }
    fclose(file);
}

void loadUsersFromFile() {
    FILE* file = fopen("users.dat", "rb");
    if (file == NULL) return;

    User temp;
    while (fread(&temp, sizeof(User), 1, file)) {
        User* newUser = (User*)malloc(sizeof(User));
        strcpy(newUser->username, temp.username);
        strcpy(newUser->password, temp.password);
        newUser->next = userList;
        userList = newUser;
    }
    fclose(file);
}

void saveLogsToFile() {
    FILE* file = fopen("logs.dat", "wb");
    if (file == NULL) return;

    GameLog* current = gameLogList;
    while (current != NULL) {
        fwrite(current, sizeof(GameLog), 1, file);
        current = current->next;
    }
    fclose(file);
}

void loadLogsFromFile() {
    FILE* file = fopen("logs.dat", "rb");
    if (file == NULL) return;

    GameLog temp;
    while (fread(&temp, sizeof(GameLog), 1, file)) {
        GameLog* newLog = (GameLog*)malloc(sizeof(GameLog));
        strcpy(newLog->username, temp.username);
        newLog->start_time = temp.start_time;
        newLog->duration = temp.duration;
        newLog->score = temp.score;
        newLog->next = gameLogList;
        gameLogList = newLog;
    }
    fclose(file);
}

void registerUser() {
    system("cls");
    char username[20], password[20];

    Pos(40, 10);
    printf("用户注册");
    Pos(35, 12);
    printf("用户名: ");
    scanf("%s", username);

    User* current = userList;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            Pos(35, 15);
            printf("用户名已存在!");
            Sleep(1000);
            return;
        }
        current = current->next;
    }

    Pos(35, 13);
    printf("密码: ");
    scanf("%s", password);

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = userList;
    userList = newUser;

    saveUsersToFile();

    Pos(35, 15);
    printf("注册成功!");
    Sleep(1000);
}

int loginUser() {
    system("cls");
    char username[20], password[20];

    Pos(40, 10);
    printf("用户登录");
    Pos(35, 12);
    printf("用户名: ");
    scanf("%s", username);

    Pos(35, 13);
    printf("密码: ");
    scanf("%s", password);

    User* current = userList;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0 &&
            strcmp(current->password, password) == 0) {
            strcpy(currentUser, username);
            return 1;
        }
        current = current->next;
    }

    Pos(35, 15);
    printf("用户名或密码错误!");
    Sleep(1000);
    return 0;
}

void addGameLog(time_t start, int duration, int score) {
    GameLog* newLog = (GameLog*)malloc(sizeof(GameLog));
    strcpy(newLog->username, currentUser);
    newLog->start_time = start;
    newLog->duration = duration;
    newLog->score = score;
    newLog->next = gameLogList;
    gameLogList = newLog;

    saveLogsToFile();
}

void showGameLogs() {
    // 先清屏再设置窗口大小，确保干净的状态
    system("cls");
    system("mode con cols=80 lines=30");

    printf("\n\n    ============ 游戏日志 ============\n\n");

    GameLog* current = gameLogList;
    int count = 0;
    int linesPrinted = 5; // 初始空行和标题占用的行数

    if (current == NULL) {
        printf("    暂无游戏记录\n");
        linesPrinted++;
    } else {
        while (current != NULL && count < 20) {
            printf("    用户: %s\n", current->username);

            char timeStr[30];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&current->start_time));
            printf("    开始时间: %s\n", timeStr);
            printf("    持续时间: %d秒\n", current->duration);
            printf("    得分: %d\n", current->score);
            printf("    ------------------------------\n\n");

            current = current->next;
            count++;
            linesPrinted += 6; // 每条记录大约6行

            // 如果接近屏幕底部，暂停等待用户按键
            if (linesPrinted >= 25) { // 30行终端减去5行缓冲
                printf("    [按任意键继续显示...]");
                _getch();
                system("cls");
                printf("\n\n    ============ 游戏日志(续) ============\n\n");
                linesPrinted = 5; // 重置行计数器
            }
        }

        if (current != NULL) {
            printf("    ...更多记录请查看日志文件...\n");
        }
    }

    printf("\n    按任意键返回菜单...");
    _getch();
    system("cls");
    system("mode con cols=80 lines=30");
}

int confirmLogout() {
    system("cls");
    printf("\n\n    确定要永久删除用户 [%s] 吗？(Y/N)\n", currentUser);
    printf("    ?? 警告：此操作不可恢复！");
    char ch = _getch();
    return (ch == 'Y' || ch == 'y');
}

void deleteCurrentUser() {
    if (userList == NULL) return;

    User* prev = NULL;
    User* current = userList;
    
    // 查找目标用户
    while (current != NULL) {
        if (strcmp(current->username, currentUser) == 0) {
            // 处理链表连接
            if (prev == NULL) {
                userList = current->next;
            } else {
                prev->next = current->next;
            }
            // 释放内存
            free(current);
            // 更新存储文件
            saveUsersToFile();
            return;
        }
        prev = current;
        current = current->next;
    }
}

void showGameRules() {
    system("cls");
    printf("\n\n    ====== 游戏规则 ======\n\n");
    printf("    1. 使用方向键↑、↓、←-、-→控制蛇的移动\n");
    printf("    2. 按Q加速（得分更高），W减速\n");
    printf("    3. 蛇头撞墙或咬到自身游戏结束\n");
    printf("    4. 空格键暂停，ESC键退出游戏\n");
    printf("\n    按任意键返回菜单...");
    _getch();
    system("cls");
}

void userMenu() {
    while (1) {
        system("cls");
        while(kbhit()) getch();
        system("mode con cols=80 lines=30");

        int choice;

        printf("\n\n    ====== 贪吃蛇游戏 ======\n\n");
        if (strlen(currentUser) == 0) {
            // 未登录状态菜单
            printf("    1. 用户注册\n");
            printf("    2. 用户登录\n");
            printf("    3. 查看游戏日志\n");
            printf("    4. 游戏规则\n");
            printf("    5. 退出系统\n\n");
            printf("    请选择: ");  

            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    if (loginUser()) {
                        // 登录成功后循环会显示新菜单
                    }
                    break;
                case 3:
                    showGameLogs();
                    break;
                case 4:
                    showGameRules();
                    break;
                case 5: 
                    exit(0);
                default:
                    printf("\n    无效选择，请重新输入!");
                    Sleep(1000);
            }
        } else {
            // 已登录状态菜单
            printf("    当前用户: %s\n\n", currentUser);
            printf("    1. 开始游戏\n");
            printf("    2. 注销\n");
            printf("    3. 查看游戏日志\n");
            printf("    4. 游戏规则\n");
            printf("    5. 退出登录\n\n");
            printf("    请选择: ");  

            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    {
                        time_t startTime = time(NULL);
                        gamestart();
                        gamecircle();
                        time_t endTime = time(NULL);
                        addGameLog(startTime, (int)(endTime - startTime), score);
                        endgame();
                        // 重置游戏状态
                        score = 0;
                        add = 10;
                        endgamestatus = 0;
                    }
                    break;
                case 2:
                    if (confirmLogout()) {
                        deleteCurrentUser();
                        strcpy(currentUser, "");
                        printf("\n    用户已成功注销！");
                    } else {
                        printf("\n    已取消注销操作\n");
                        printf("\n    按任意键返回菜单...");
                        _getch();            // 等待任意按键
                        system("cls");       // 立即清屏
                    }
                    Sleep(1500);
                    break;
                case 3:
                    showGameLogs();
                    break;
                case 4:
                    showGameRules();
                    break;
                case 5: 
                    strcpy(currentUser, "");
                    break;
                default:
                    printf("\n    无效选择，请重新输入!");
                    Sleep(1000);
            }
        }       
    }
}

int main() {
    // 加载用户和日志数据
    loadUsersFromFile();
    loadLogsFromFile();

    // 显示用户菜单
    userMenu();

    return 0;
}
