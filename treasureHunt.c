#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 5
#define TREASURES 3
#define TRAPS 3

char grid[SIZE][SIZE];  // Gizli harita
char visibleGrid[SIZE][SIZE];  // Oyuncunun gördüğü harita
int playerX = 0, playerY = 0;
int treasuresCollected = 0;
int moves = 0; // Oyuncunun toplam hamle sayısını tutar

void initializeGrid();
void printGrid();
void placeRandomly(char symbol, int count);
void movePlayer(char direction);
int countNearby(char symbol);
void saveGame(char *filename, char *player);
void loadGame(char *filename, char *player);
void updateLeaderboard(char *player, int moves);
void showLeaderboard();
void debugPrintGrid();

void initializeGrid() {
    srand(time(NULL)); // Rastgelelik için zaman tabanlı seed
    memset(grid, '\0', sizeof(grid)); // Gizli haritayı boş başlat
    memset(visibleGrid, '?', sizeof(visibleGrid)); // Oyuncunun gördüğü haritayı '?' ile doldur
    grid[0][0] = 'P'; // Oyuncu başlangıç pozisyonu
    visibleGrid[0][0] = 'P';

    placeRandomly('T', TREASURES); // Hazineleri yerleştir
    placeRandomly('X', TRAPS);     // Tuzakları yerleştir

    //debugPrintGrid(); // Haritayı yazdır
}

void placeRandomly(char symbol, int count) {
    int placed = 0;
    int attempts = 0; // Deneme sayısını takip eder

    while (placed < count) {
        int x = rand() % SIZE;
        int y = rand() % SIZE;

        // Debug: Rastgele seçilen hücreyi yazdır
        //printf("Trying to place %c at (%d, %d)\n", symbol, x, y);

        // Eğer hücre boşsa ve başlangıç pozisyonu değilse, sembolü yerleştir
        if (grid[x][y] == '\0' && !(x == 0 && y == 0)) {
            grid[x][y] = symbol;
            placed++;

            // Debug: Başarıyla yerleştirildiğini göster
           // printf("%c placed at (%d, %d). Total placed: %d\n", symbol, x, y, placed);
        } else {
            // Debug: Pozisyon doluysa veya başlangıç pozisyonuysa
            //printf("Position (%d, %d) is already occupied or invalid.\n", x, y);
        }

        attempts++;
        if (attempts > SIZE * SIZE * 5) { // Çok fazla deneme yapıldıysa
            printf("Error: Could not place all %c symbols. Placed: %d\n", symbol, placed);
            break;
        }
    }
}

void printGrid() {
    printf("\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", visibleGrid[i][j]);
        }
        printf("\n");
    }
    printf("Treasures collected: %d\n", treasuresCollected);
    printf("Nearby treasures: %d, Nearby traps: %d\n", countNearby('T'), countNearby('X'));
}

void debugPrintGrid() {
    printf("\nDebug: Full Grid\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j] == '\0' ? '.' : grid[i][j]); // Boş hücreleri '.' olarak göster
        }
        printf("\n");
    }
    printf("Player position: (%d, %d)\n", playerX, playerY);
}

int countNearby(char symbol) {
    int count = 0;

    // Oyuncunun çevresindeki 1 karakter yarıçapındaki kareleri kontrol et
    for (int i = playerX - 1; i <= playerX + 1; i++) {
        for (int j = playerY - 1; j <= playerY + 1; j++) {
            // Harita sınırları içinde olup olmadığını kontrol et
            if (i >= 0 && i < SIZE && j >= 0 && j < SIZE) {
                // Oyuncunun bulunduğu hücreyi atla
                if (i == playerX && j == playerY) {
                    continue;
                }
                // Eğer hücre belirtilen sembolü içeriyorsa, sayacı artır
                if (grid[i][j] == symbol) {
                    count++;
                }
            }
        }
    }

    return count;
}

void movePlayer(char direction) {
    int newX = playerX, newY = playerY;

    // Hareket yönüne göre yeni pozisyonu belirle
    if (direction == 'u' && playerX > 0) newX--;          // Yukarı
    else if (direction == 'd' && playerX < SIZE - 1) newX++; // Aşağı
    else if (direction == 'l' && playerY > 0) newY--;       // Sol
    else if (direction == 'r' && playerY < SIZE - 1) newY++; // Sağ
    else {
        printf("Invalid move. You can't go outside the grid!\n");
        return;
    }

    // Yeni pozisyonda bir tuzak var mı?
    if (grid[newX][newY] == 'X') {
        printf("You hit a trap! Game over.\n");
        printf("Total moves: %d\n", moves); // Toplam hamle sayısını göster
        exit(0); // Oyunu sonlandır
    }

    // Yeni pozisyonda bir hazine var mı?
    if (grid[newX][newY] == 'T') {
        treasuresCollected++;
        printf("You found a treasure! Total treasures collected: %d\n", treasuresCollected);

        // Hazineyi haritadan kaldır
        grid[newX][newY] = '\0';
    }

    // Eski pozisyonu güncelle
    visibleGrid[playerX][playerY] = '='; // Eski pozisyonu ziyaret edilmiş olarak işaretle

    // Yeni pozisyonu güncelle
    playerX = newX;
    playerY = newY;
    visibleGrid[playerX][playerY] = 'P'; // Yeni pozisyonda oyuncuyu göster
}

void saveGame(char *filename, char *player) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not save the game.\n");
        return;
    }

    // Oyuncu adını kaydet
    fprintf(file, "%s\n", player);

    // Oyuncunun pozisyonunu, toplanan hazine sayısını ve toplam hamle sayısını kaydet
    fprintf(file, "%d %d %d %d\n", playerX, playerY, treasuresCollected, moves);

    // Haritayı kaydet
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fprintf(file, "%c", grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Game saved successfully to %s.\n", filename);
}

void loadGame(char *filename, char *player) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not load the game. Starting a new game.\n");
        initializeGrid();
        return;
    }

    char savedPlayer[50];
    fscanf(file, "%s", savedPlayer);

    // Oyuncu adı kontrolü
    if (strcmp(savedPlayer, player) != 0) {
        printf("This game was saved by another player (%s). Starting a new game.\n", savedPlayer);
        fclose(file);
        initializeGrid();
        return;
    }

    // Oyuncunun pozisyonunu, toplanan hazine sayısını ve toplam hamle sayısını yükle
    fscanf(file, "%d %d %d %d", &playerX, &playerY, &treasuresCollected, &moves);

    // Haritayı yükle
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fscanf(file, " %c", &grid[i][j]);
        }
    }

    // Oyuncunun görünen haritasını güncelle
    memset(visibleGrid, '?', sizeof(visibleGrid));
    visibleGrid[playerX][playerY] = 'P';

    fclose(file);
    printf("Game loaded successfully from %s.\n", filename);
}

void updateLeaderboard(char *player, int moves) {
    FILE *file = fopen("C:\\Users\\HAMZA\\Desktop\\coding\\treasure_hunt\\leaderboard.txt", "a");
    if (!file) {
        printf("Error: Could not update the leaderboard.\n");
        return;
    }

    // Oyuncu adı ve hamle sayısını dosyaya ekle
    fprintf(file, "%s %d\n", player, moves);
    fclose(file);
    printf("Leaderboard updated successfully.\n");
}

void showLeaderboard() {
    FILE *file = fopen("leaderboard.txt", "r");
    if (!file) {
        printf("No leaderboard found.\n");
        return;
    }

    // Geçici bir yapı ile oyuncuları ve skorları oku
    char players[100][50];
    int scores[100];
    int count = 0;

    while (fscanf(file, "%s %d", players[count], &scores[count]) != EOF) {
        count++;
    }
    fclose(file);

    // Skorları sıralama (küçükten büyüğe)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[i] > scores[j]) {
                // Skorları değiştir
                int tempScore = scores[i];
                scores[i] = scores[j];
                scores[j] = tempScore;

                // Oyuncu adlarını değiştir
                char tempPlayer[50];
                strcpy(tempPlayer, players[i]);
                strcpy(players[i], players[j]);
                strcpy(players[j], tempPlayer);
            }
        }
    }

    // Sıralı liderlik tablosunu yazdır
    printf("\nLeaderboard:\n");
    for (int i = 0; i < count; i++) {
        printf("%s: %d moves\n", players[i], scores[i]);
    }
}

int main(int argc, char *argv[]) {
    srand(time(NULL)); // Rastgelelik için zaman tabanlı seed

    if (argc < 3 || strcmp(argv[1], "p") != 0) {
        printf("Usage: %s p <player_name> [load <filename>]\n", argv[0]);
        return 1;
    }

    char playerName[50];
    strcpy(playerName, argv[2]); // Oyuncu adını komut satırı argümanından al

    if (argc >= 4 && strcmp(argv[3], "load") == 0) {
        if (argc < 5) {
            printf("Error: Please specify a file to load.\n");
            return 1;
        }
        loadGame(argv[4], playerName); // Kaydedilmiş oyunu yükle
    } else {
        // Yeni bir oyun başlat
        initializeGrid();
    }

    printf("Welcome to Treasure Hunt, %s!\n", playerName);
    printf("Your goal is to collect all %d treasures while avoiding traps.\n", TREASURES);
    printf("Use 'u' (up), 'd', 'l', 'r' to move.\n");
    printf("You can save the game anytime by entering 's'.\n");

    printGrid();

    while (treasuresCollected < TREASURES) {
        char move;
        printf("Move (u/d/l/r) or save (s): ");
        scanf(" %c", &move);

        // Geçersiz hareket kontrolü
        if (move != 'u' && move != 'd' && move != 'l' && move != 'r' && move != 's') {
            printf("Invalid move! Please enter u, d, l, r, or s.\n");
            continue;
        }

        if (move == 's') {
            char filename[50];
            printf("Enter save file name: ");
            scanf("%s", filename);
            saveGame(filename, playerName);
            continue;
        }

        movePlayer(move);
        moves++; // Hamle sayısını artır
        printGrid();

        // Oyuncu kazandı mı?
        if (treasuresCollected == TREASURES) {
            printf("Congratulations, you found all treasures!\n");
            printf("Total moves: %d\n", moves); // Toplam hamle sayısını göster
            updateLeaderboard(playerName, moves); // Hamle sayısını liderlik tablosuna kaydet
            break; // Döngüden çık
        }
    }

    return 0;
}
