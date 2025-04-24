#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>
#include<string.h>
#include<conio.h>

#define U 1
#define D 2
#define L 3
#define R 4       //�ߵ�״̬��U���� ��D���£�L:�� R����

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

// ��Ϸȫ�ֱ���
int score = 0, add = 10;
int status, sleeptime = 200;
snake* head, *food;
snake* q;
int endgamestatus = 0;

// �û�����־ȫ�ֱ���
User* userList = NULL;
GameLog* gameLogList = NULL;
char currentUser[20] = "";

// ��������
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

// �û�����־����
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
        printf("��");
        Pos(i, 26);
        printf("��");
    }
    for (i = 1; i < 26; i++) {
        Pos(0, i);
        printf("��");
        Pos(56, i);
        printf("��");
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
        printf("��");
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
    printf("��");
}

void cantcrosswall() {
    if (head->x <= 0 || head->x >= 56 || 
        head->y <= 0 || head->y >= 26) {
        endgamestatus = 1;
    }
}

void snakemove() {
    // ���ƶ�ǰ�����Ϸ״̬
    if (endgamestatus != 0) return;

    cantcrosswall();

    // ��ײ��������ֹ�ƶ�
    if (endgamestatus != 0) return;
    
    snake* nexthead;
    nexthead = (snake*)malloc(sizeof(snake));
     // ͳһ�ƶ��߼����Ż��ظ����룩
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

    // ʳ����
    if (nexthead->x == food->x && nexthead->y == food->y) {
        // �Ե�ʳ���߼�
        nexthead->next = head;
        head = nexthead;
        score += add;
        
        // ˢ��������ʾ
        snake* q = head;
        while(q) {
            Pos(q->x, q->y);
            printf("��");
            q = q->next;
        }
        createfood();
    } else {
        // ��ͨ�ƶ��߼�
        nexthead->next = head;
        head = nexthead;
        
        // ������ͷ��ʾ
        Pos(head->x, head->y);
        printf("��");
        
        // ɾ����β
        snake* tail = head;
        while(tail->next->next) tail = tail->next;
        Pos(tail->next->x, tail->next->y);
        printf("  ");
        free(tail->next);
        tail->next = NULL;
    }

    // �Լ���ײ��⣨������ֹ��
    if (biteself() == 1) {
        endgamestatus = 2;
        free(nexthead);  // �ͷ��ڴ�
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
    printf("���: %s", currentUser);
    Pos(64, 10);
    printf("���ư�����");
    Pos(64, 11);
    printf("�� �� ��- -�� - �ƶ�����");
    Pos(64, 12);
    printf("Q - ���٣�W - ����");
    Pos(64, 13);
    printf("�ո� - ��ͣ");
    Pos(64, 14);
    printf("ESC - �˳���Ϸ");
    status = R;

    while (endgamestatus == 0) {
        Pos(64, 6);
        printf("��ǰ�÷֣�%d  ", score);
        Pos(64, 7);
        printf("ʳ��ӷ֣�%d ��", add);
        Pos(64, 8);
        printf("�ƶ��ٶȣ�%d ms/��", sleeptime);

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
        
        // �ƶ�ǰ�ȼ��״̬
        if (endgamestatus != 0) break;
        snakemove();

         // �ƶ����������״̬
         if (endgamestatus != 0) break;
    }
}

void welcometogame() {
    Pos(40, 12);
    printf("��ӭ����̰ʳ����Ϸ��");
    Pos(40, 13);
    printf("���: %s", currentUser);
    Pos(40, 25);
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("�á�,��,��-,-���ֱ�����ߵ��ƶ��� Q Ϊ���٣�W Ϊ����\n");
    Pos(25, 13);
    printf("���ٽ��ܵõ����ߵķ�����\n");
    // system("pause");
    _getch();            // �ȴ����ⰴ��
    system("cls");       // ��������
}

void endgame() {
    system("cls");
    Pos(24, 12);
    switch (endgamestatus) {
        case 1: printf("�Բ�����ײ��ǽ�ˡ���Ϸ����!"); break;
        case 2: printf("�Բ�����ҧ���Լ��ˡ���Ϸ����!"); break;
        case 3: printf("���Ѿ������˳���Ϸ"); break;
    }
    Pos(24, 13);
    printf("���ĵ÷���%d\n", score);
    Pos(24, 15);
    printf("����������ز˵�...");
   
    while (_kbhit()) {
        _getch();
    }
    Sleep(100);  // 100ms �ӳ�
    _getch();            // �ȴ����ⰴ��
    system("cls");       // ��������

    endgamestatus = 0;   // ������Ϸ״̬
    sleeptime = 200;   // ������Ϸ�ٶ�
    add = 10;          // ���÷���ϵ��

    freeSnake();        // ǿ���ͷ������ڴ�
}

// �ڴ��ͷź���
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
    freeSnake(); // ���֮ǰ��������
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
    printf("�û�ע��");
    Pos(35, 12);
    printf("�û���: ");
    scanf("%s", username);

    User* current = userList;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            Pos(35, 15);
            printf("�û����Ѵ���!");
            Sleep(1000);
            return;
        }
        current = current->next;
    }

    Pos(35, 13);
    printf("����: ");
    scanf("%s", password);

    User* newUser = (User*)malloc(sizeof(User));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    newUser->next = userList;
    userList = newUser;

    saveUsersToFile();

    Pos(35, 15);
    printf("ע��ɹ�!");
    Sleep(1000);
}

int loginUser() {
    system("cls");
    char username[20], password[20];

    Pos(40, 10);
    printf("�û���¼");
    Pos(35, 12);
    printf("�û���: ");
    scanf("%s", username);

    Pos(35, 13);
    printf("����: ");
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
    printf("�û������������!");
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
    // �����������ô��ڴ�С��ȷ���ɾ���״̬
    system("cls");
    system("mode con cols=80 lines=30");

    printf("\n\n    ============ ��Ϸ��־ ============\n\n");

    GameLog* current = gameLogList;
    int count = 0;
    int linesPrinted = 5; // ��ʼ���кͱ���ռ�õ�����

    if (current == NULL) {
        printf("    ������Ϸ��¼\n");
        linesPrinted++;
    } else {
        while (current != NULL && count < 20) {
            printf("    �û�: %s\n", current->username);

            char timeStr[30];
            strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&current->start_time));
            printf("    ��ʼʱ��: %s\n", timeStr);
            printf("    ����ʱ��: %d��\n", current->duration);
            printf("    �÷�: %d\n", current->score);
            printf("    ------------------------------\n\n");

            current = current->next;
            count++;
            linesPrinted += 6; // ÿ����¼��Լ6��

            // ����ӽ���Ļ�ײ�����ͣ�ȴ��û�����
            if (linesPrinted >= 25) { // 30���ն˼�ȥ5�л���
                printf("    [�������������ʾ...]");
                _getch();
                system("cls");
                printf("\n\n    ============ ��Ϸ��־(��) ============\n\n");
                linesPrinted = 5; // �����м�����
            }
        }

        if (current != NULL) {
            printf("    ...�����¼��鿴��־�ļ�...\n");
        }
    }

    printf("\n    ����������ز˵�...");
    _getch();
    system("cls");
    system("mode con cols=80 lines=30");
}

int confirmLogout() {
    system("cls");
    printf("\n\n    ȷ��Ҫ����ɾ���û� [%s] ��(Y/N)\n", currentUser);
    printf("    ?? ���棺�˲������ɻָ���");
    char ch = _getch();
    return (ch == 'Y' || ch == 'y');
}

void deleteCurrentUser() {
    if (userList == NULL) return;

    User* prev = NULL;
    User* current = userList;
    
    // ����Ŀ���û�
    while (current != NULL) {
        if (strcmp(current->username, currentUser) == 0) {
            // ������������
            if (prev == NULL) {
                userList = current->next;
            } else {
                prev->next = current->next;
            }
            // �ͷ��ڴ�
            free(current);
            // ���´洢�ļ�
            saveUsersToFile();
            return;
        }
        prev = current;
        current = current->next;
    }
}

void showGameRules() {
    system("cls");
    printf("\n\n    ====== ��Ϸ���� ======\n\n");
    printf("    1. ʹ�÷��������������-��-�������ߵ��ƶ�\n");
    printf("    2. ��Q���٣��÷ָ��ߣ���W����\n");
    printf("    3. ��ͷײǽ��ҧ��������Ϸ����\n");
    printf("    4. �ո����ͣ��ESC���˳���Ϸ\n");
    printf("\n    ����������ز˵�...");
    _getch();
    system("cls");
}

void userMenu() {
    while (1) {
        system("cls");
        while(kbhit()) getch();
        system("mode con cols=80 lines=30");

        int choice;

        printf("\n\n    ====== ̰������Ϸ ======\n\n");
        if (strlen(currentUser) == 0) {
            // δ��¼״̬�˵�
            printf("    1. �û�ע��\n");
            printf("    2. �û���¼\n");
            printf("    3. �鿴��Ϸ��־\n");
            printf("    4. ��Ϸ����\n");
            printf("    5. �˳�ϵͳ\n\n");
            printf("    ��ѡ��: ");  

            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    registerUser();
                    break;
                case 2:
                    if (loginUser()) {
                        // ��¼�ɹ���ѭ������ʾ�²˵�
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
                    printf("\n    ��Чѡ������������!");
                    Sleep(1000);
            }
        } else {
            // �ѵ�¼״̬�˵�
            printf("    ��ǰ�û�: %s\n\n", currentUser);
            printf("    1. ��ʼ��Ϸ\n");
            printf("    2. ע��\n");
            printf("    3. �鿴��Ϸ��־\n");
            printf("    4. ��Ϸ����\n");
            printf("    5. �˳���¼\n\n");
            printf("    ��ѡ��: ");  

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
                        // ������Ϸ״̬
                        score = 0;
                        add = 10;
                        endgamestatus = 0;
                    }
                    break;
                case 2:
                    if (confirmLogout()) {
                        deleteCurrentUser();
                        strcpy(currentUser, "");
                        printf("\n    �û��ѳɹ�ע����");
                    } else {
                        printf("\n    ��ȡ��ע������\n");
                        printf("\n    ����������ز˵�...");
                        _getch();            // �ȴ����ⰴ��
                        system("cls");       // ��������
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
                    printf("\n    ��Чѡ������������!");
                    Sleep(1000);
            }
        }       
    }
}

int main() {
    // �����û�����־����
    loadUsersFromFile();
    loadLogsFromFile();

    // ��ʾ�û��˵�
    userMenu();

    return 0;
}
