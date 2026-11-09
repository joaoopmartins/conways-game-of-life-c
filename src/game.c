
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "game.h"

LifeGame g_game;
Config g_cfg;

/* Internal utility functions */

static void clearScreen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void clearInputBuffer(void) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF) {
        /* Discard remaining input */
    }
}

void delaySeconds(float seconds) {
    if (seconds <= 0.0f) {
        return;
    }

#ifdef _WIN32
    Sleep((DWORD)(seconds * 1000.0f));
#else
    struct timespec request;

    request.tv_sec = (time_t)seconds;
    request.tv_nsec = (long)((seconds - request.tv_sec) * 1000000000L);

    nanosleep(&request, NULL);
#endif
}

static int isWithinBounds(int row, int col) {
    return (row >= 0 && row < g_cfg.rows &&
            col >= 0 && col < g_cfg.cols);
}

static int countAliveCells(void) {
    int total = 0;

    for (int r = 0; r < g_cfg.rows; r++) {
        for (int c = 0; c < g_cfg.cols; c++) {
            if (g_game.grid[r][c] == 1) {
                total++;
            }
        }
    }

    return total;
}

static void updatePopulationStats(void) {
    int alive = countAliveCells();

    if (!g_game.statsInitialized) {
        g_game.maxAlive = alive;
        g_game.genMaxAlive = g_game.generation;

        g_game.minAlive = alive;
        g_game.genMinAlive = g_game.generation;

        g_game.statsInitialized = 1;
        return;
    }

    if (alive > g_game.maxAlive) {
        g_game.maxAlive = alive;
        g_game.genMaxAlive = g_game.generation;
    }

    if (alive < g_game.minAlive) {
        g_game.minAlive = alive;
        g_game.genMinAlive = g_game.generation;
    }
}

/* Game initialization and board management */

void initGame(void) {
    g_cfg.rows = MAX_SIZE;
    g_cfg.cols = MAX_SIZE;
    g_cfg.secsPerGen = 0.5f;
    g_cfg.showGridOnAdvance = 1;
    g_cfg.showStatsOnAdvance = 0;

    clearGrid();
}

void clearGrid(void) {
    for (int r = 0; r < MAX_SIZE; r++) {
        for (int c = 0; c < MAX_SIZE; c++) {
            g_game.grid[r][c] = 0;
            g_game.ages[r][c] = 0;
        }
    }

    g_game.generation = 0;
    g_game.maxAlive = 0;
    g_game.genMaxAlive = 0;
    g_game.minAlive = 0;
    g_game.genMinAlive = 0;
    g_game.statsInitialized = 0;
}

void printGrid(void) {
    clearScreen();

    printf("   ");

    for (int c = 0; c < g_cfg.cols; c++) {
        printf("%2d ", c);
    }

    printf("\n");

    for (int r = 0; r < g_cfg.rows; r++) {
        printf("%2d ", r);

        for (int c = 0; c < g_cfg.cols; c++) {
            if (g_game.grid[r][c] == 1) {
                printf(" * ");
            } else {
                printf(" . ");
            }
        }

        printf("\n");
    }
}

/* Cell management */

int insertSingleCell(void) {
    int row, col;
    int valid = 0;

    do {
        printf("\nEnter cell coordinates (row col) [0-%d 0-%d]: ",
               g_cfg.rows - 1, g_cfg.cols - 1);

        int inputResult = scanf("%d %d", &row, &col);

        if (inputResult == EOF) {
            return 0;
        }

        if (inputResult != 2) {
            clearInputBuffer();
            printf("Invalid input! Please enter two numbers separated by space (e.g. 4 5).\n");
            continue;
        }

        clearInputBuffer();

        if (!isWithinBounds(row, col)) {
            printf("Out of bounds! Please enter valid coordinates within grid limits.\n");
        } else if (g_game.grid[row][col] == 1) {
            printf("This cell is already alive!\n");
        } else {
            valid = 1;
        }

    } while (!valid);

    g_game.grid[row][col] = 1;
    g_game.ages[row][col] = 1;

    updatePopulationStats();

    printf("Cell successfully inserted at (%d, %d)!\n", row, col);
    delaySeconds(1.0f);

    return 1;
}

int removeSingleCell(void) {
    int row, col;
    int valid = 0;

    do {
        printf("\nEnter cell coordinates to remove (row col) [0-%d 0-%d]: ",
               g_cfg.rows - 1, g_cfg.cols - 1);

        int inputResult = scanf("%d %d", &row, &col);

        if (inputResult == EOF) {
            return 0;
        }

        if (inputResult != 2) {
            clearInputBuffer();
            printf("Invalid input! Please enter two numbers separated by space (e.g. 4 5).\n");
            continue;
        }

        clearInputBuffer();

        if (!isWithinBounds(row, col)) {
            printf("Coordinates out of bounds!\n");
        } else if (g_game.grid[row][col] == 0) {
            printf("This cell is already empty! Please select a live cell.\n");
        } else {
            valid = 1;
        }

    } while (!valid);

    g_game.grid[row][col] = 0;
    g_game.ages[row][col] = 0;

    updatePopulationStats();

    printf("Cell at (%d, %d) successfully removed!\n", row, col);
    delaySeconds(1.0f);

    return 1;
}

void insertMultipleCells(void) {
    char choice;

    do {
        if (!insertSingleCell()) {
            return;
        }

        printf("\nDo you want to insert another cell? (y/n): ");

        if (scanf(" %c", &choice) != 1) {
            clearInputBuffer();
            return;
        }

        clearInputBuffer();

        while (choice != 'y' && choice != 'Y' &&
               choice != 'n' && choice != 'N') {

            printf("Invalid option! Enter y or n: ");

            if (scanf(" %c", &choice) != 1) {
                clearInputBuffer();
                return;
            }

            clearInputBuffer();
        }

    } while (choice == 'y' || choice == 'Y');
}

void removeMultipleCells(void) {
    char choice;

    do {
        if (!removeSingleCell()) {
            return;
        }

        printf("\nDo you want to remove another cell? (y/n): ");

        if (scanf(" %c", &choice) != 1) {
            clearInputBuffer();
            return;
        }

        clearInputBuffer();

        while (choice != 'y' && choice != 'Y' &&
               choice != 'n' && choice != 'N') {

            printf("Invalid option! Enter y or n: ");

            if (scanf(" %c", &choice) != 1) {
                clearInputBuffer();
                return;
            }

            clearInputBuffer();
        }

    } while (choice == 'y' || choice == 'Y');
}

/* Simulation */

static int countToroidalNeighbors(int row, int col) {
    int count = 0;

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) {
                continue;
            }

            int nRow = (row + dr + g_cfg.rows) % g_cfg.rows;
            int nCol = (col + dc + g_cfg.cols) % g_cfg.cols;

            if (g_game.grid[nRow][nCol] == 1) {
                count++;
            }
        }
    }

    return count;
}

static void evolveStep(void) {
    int nextGrid[MAX_SIZE][MAX_SIZE];
    int nextAges[MAX_SIZE][MAX_SIZE];

    for (int r = 0; r < g_cfg.rows; r++) {
        for (int c = 0; c < g_cfg.cols; c++) {
            int neighbors = countToroidalNeighbors(r, c);
            int alive = g_game.grid[r][c];

            if (alive) {
                /* Survival: 2 or 3 neighbors */
                if (neighbors == 2 || neighbors == 3) {
                    nextGrid[r][c] = 1;
                    nextAges[r][c] = g_game.ages[r][c] + 1;
                } else {
                    /* Death: underpopulation or overpopulation */
                    nextGrid[r][c] = 0;
                    nextAges[r][c] = 0;
                }
            } else {
                /* Reproduction: exactly 3 neighbors */
                if (neighbors == 3) {
                    nextGrid[r][c] = 1;
                    nextAges[r][c] = 1;
                } else {
                    nextGrid[r][c] = 0;
                    nextAges[r][c] = 0;
                }
            }
        }
    }

    for (int r = 0; r < g_cfg.rows; r++) {
        for (int c = 0; c < g_cfg.cols; c++) {
            g_game.grid[r][c] = nextGrid[r][c];
            g_game.ages[r][c] = nextAges[r][c];
        }
    }

    g_game.generation++;

    updatePopulationStats();
}

void advanceGenerations(int totalGens) {
    if (totalGens <= 0) {
        return;
    }

    for (int i = 0; i < totalGens; i++) {
        evolveStep();

        if (g_cfg.showGridOnAdvance) {
            printGrid();
        }

        if (g_cfg.showStatsOnAdvance) {
            printStatistics();
        }

        if (i < totalGens - 1) {
            delaySeconds(g_cfg.secsPerGen);
        }
    }
}

/* Statistics */

void printStatistics(void) {
    int currentAlive = countAliveCells();
    int oldestAge = 0;
    int youngestAge = 999999;

    for (int r = 0; r < g_cfg.rows; r++) {
        for (int c = 0; c < g_cfg.cols; c++) {
            if (g_game.grid[r][c] == 1) {
                int age = g_game.ages[r][c];

                if (age > oldestAge) {
                    oldestAge = age;
                }

                if (age < youngestAge) {
                    youngestAge = age;
                }
            }
        }
    }

    printf("\n=================== GAME STATISTICS ===================\n");
    printf(" Current Generation: %d\n", g_game.generation);
    printf(" Currently Alive Cells: %d\n", currentAlive);
    printf(" Maximum Alive Cells: %d (Occurred in Generation %d)\n",
           g_game.maxAlive, g_game.genMaxAlive);
    printf(" Minimum Alive Cells: %d (Occurred in Generation %d)\n",
           g_game.minAlive, g_game.genMinAlive);

    if (currentAlive > 0) {
        printf(" Oldest Alive Cell: Age %d (Born in Generation %d)\n",
               oldestAge, g_game.generation - (oldestAge - 1));

        printf(" Youngest Alive Cell: Age %d (Born in Generation %d)\n",
               youngestAge, g_game.generation - (youngestAge - 1));

        int *ageDistribution = calloc((size_t)oldestAge + 1,
                                      sizeof(int));

        if (ageDistribution != NULL) {
            for (int r = 0; r < g_cfg.rows; r++) {
                for (int c = 0; c < g_cfg.cols; c++) {
                    if (g_game.grid[r][c] == 1) {
                        int age = g_game.ages[r][c];

                        if (age > 0 && age <= oldestAge) {
                            ageDistribution[age]++;
                        }
                    }
                }
            }

            printf("\n--- Cell Age Histogram ---\n");

            for (int age = 1; age <= oldestAge; age++) {
                if (ageDistribution[age] > 0) {
                    printf(" Age %2d (%2d cells): ",
                           age, ageDistribution[age]);

                    for (int b = 0; b < ageDistribution[age]; b++) {
                        printf("#");
                    }

                    printf("\n");
                }
            }

            free(ageDistribution);
        } else {
            printf("\nUnable to allocate memory for age histogram.\n");
        }
    } else {
        printf(" No live cells currently present on the grid.\n");
    }

    printf("=======================================================\n");
}

/* Configuration */

void configureDimensions(void) {
    int newRows, newCols;

    printf("\nEnter new grid dimensions (rows cols): ");
    printf("\nMinimum size: 3x3 | Maximum size: %dx%d\n",
           MAX_SIZE, MAX_SIZE);

    if (scanf("%d %d", &newRows, &newCols) != 2) {
        clearInputBuffer();
        printf("\nInvalid input! Please enter two valid numbers.\n");
        delaySeconds(1.5f);
        return;
    }

    clearInputBuffer();

    if (newRows < 3 || newRows > MAX_SIZE ||
        newCols < 3 || newCols > MAX_SIZE) {

        printf("\nInvalid dimensions! Values must be between 3 and %d.\n",
               MAX_SIZE);
        delaySeconds(1.5f);
        return;
    }

    if (newRows == g_cfg.rows && newCols == g_cfg.cols) {
        printf("\nThe grid already has these dimensions. No changes made.\n");
        delaySeconds(1.5f);
        return;
    }

    g_cfg.rows = newRows;
    g_cfg.cols = newCols;

    clearGrid();

    printf("\nGrid dimensions successfully changed to %dx%d.\n",
           g_cfg.rows, g_cfg.cols);
    printf("The grid has been cleared and the simulation reset.\n");

    delaySeconds(1.5f);
}

void configureSpeed(void) {
    float secs;
    int valid = 0;

    do {
        printf("\nEnter delay between generations (0-60 seconds): ");

        int inputResult = scanf("%f", &secs);

        if (inputResult == EOF) {
            return;
        }

        if (inputResult != 1) {
            clearInputBuffer();
            printf("Invalid input! Please enter a numeric value.\n");
            continue;
        }

        clearInputBuffer();

        if (!isfinite(secs)) {
            printf("Invalid value! The delay must be a finite number.\n");
        } else if (secs < 0.0f || secs > 60.0f) {
            printf("Invalid value! The delay must be between 0 and 60 seconds.\n");
        } else {
            valid = 1;
        }

    } while (!valid);

    g_cfg.secsPerGen = secs;

    printf("Simulation speed successfully configured to %.2f seconds per generation!\n",
           g_cfg.secsPerGen);

    delaySeconds(1.0f);
}

void toggleShowGrid(void) {
    g_cfg.showGridOnAdvance = !g_cfg.showGridOnAdvance;

    printf("\nDisplay grid on advance: %s\n",
           g_cfg.showGridOnAdvance ? "ENABLED" : "DISABLED");

    delaySeconds(2.0f);
}

void toggleShowStatistics(void) {
    g_cfg.showStatsOnAdvance = !g_cfg.showStatsOnAdvance;

    printf("\nDisplay statistics on advance: %s\n",
           g_cfg.showStatsOnAdvance ? "ENABLED" : "DISABLED");

    delaySeconds(2.0f);
}

/* Preset patterns */

void loadGliderPattern(void) {
    if (g_cfg.rows < 4 || g_cfg.cols < 4) {
        printf("\nGrid is too small to load the Glider pattern!\n");
        delaySeconds(1.5f);
        return;
    }

    clearGrid();

    g_game.grid[1][2] = 1;
    g_game.grid[2][3] = 1;
    g_game.grid[3][1] = 1;
    g_game.grid[3][2] = 1;
    g_game.grid[3][3] = 1;

    g_game.ages[1][2] = 1;
    g_game.ages[2][3] = 1;
    g_game.ages[3][1] = 1;
    g_game.ages[3][2] = 1;
    g_game.ages[3][3] = 1;

    updatePopulationStats();

    printf("\nGlider pattern loaded successfully!\n");
    delaySeconds(1.0f);
}

void loadBlinkerPattern(void) {
    if (g_cfg.rows < 3 || g_cfg.cols < 4) {
        printf("\nGrid is too small to load the Blinker pattern!\n");
        delaySeconds(1.5f);
        return;
    }

    clearGrid();

    g_game.grid[2][1] = 1;
    g_game.grid[2][2] = 1;
    g_game.grid[2][3] = 1;

    g_game.ages[2][1] = 1;
    g_game.ages[2][2] = 1;
    g_game.ages[2][3] = 1;

    updatePopulationStats();

    printf("\nBlinker pattern loaded successfully!\n");
    delaySeconds(1.0f);
}

void loadBlockPattern(void) {
    if (g_cfg.rows < 4 || g_cfg.cols < 4) {
        printf("\nGrid is too small to load the Block pattern!\n");
        delaySeconds(1.5f);
        return;
    }

    clearGrid();

    g_game.grid[2][2] = 1;
    g_game.grid[2][3] = 1;
    g_game.grid[3][2] = 1;
    g_game.grid[3][3] = 1;

    g_game.ages[2][2] = 1;
    g_game.ages[2][3] = 1;
    g_game.ages[3][2] = 1;
    g_game.ages[3][3] = 1;

    updatePopulationStats();

    printf("\nBlock pattern loaded successfully!\n");
    delaySeconds(1.0f);
}

void loadBoatPattern(void) {
    if (g_cfg.rows < 5 || g_cfg.cols < 5) {
        printf("\nGrid is too small to load the Boat pattern!\n");
        delaySeconds(1.5f);
        return;
    }

    clearGrid();

    g_game.grid[2][2] = 1;
    g_game.grid[2][3] = 1;
    g_game.grid[3][2] = 1;
    g_game.grid[3][4] = 1;
    g_game.grid[4][3] = 1;

    g_game.ages[2][2] = 1;
    g_game.ages[2][3] = 1;
    g_game.ages[3][2] = 1;
    g_game.ages[3][4] = 1;
    g_game.ages[4][3] = 1;

    updatePopulationStats();

    printf("\nBoat pattern loaded successfully!\n");
    delaySeconds(1.0f);
}

/* Menus */

int showMainMenu(void) {
    int choice;

    printf("*-------------------------------------------------------------------------------------*\n");
    printf("|       _                                 _            __      __  _       _          |\n");
    printf("|      | |                               | |           \\ \\    / / (_)     | |         |\n");
    printf("|      | |   ___     __ _    ___       __| |   __ _     \\ \\  / /   _    __| |   __ _  |\n");
    printf("|  _   | |  / _ \\   / _` |  / _ \\     / _` |  / _` |     \\ \\/ /   | |  / _` |  / _` | |\n");
    printf("| | |__| | | (_) | | (_| | | (_) |   | (_| | | (_| |      \\  /    | | | (_| | | (_| | |\n");
    printf("|  \\____/   \\___/   \\__, |  \\___/     \\__,_|  \\__,_|       \\/     |_|  \\__,_|  \\__,_| |\n");
    printf("|                    __/ |                                                            |\n");
    printf("|                   |___/                                                             |\n");
    printf("|*************************************************************************************|\n");
    printf("|*********************************** MAIN MENU ***************************************|\n");
    printf("|************************************ Options ****************************************|\n");
    printf("|*************************************************************************************|\n");
    printf("|**      {1} - Insert cell              **       {6} - Advance one generation       **|\n");
    printf("|**                                     **                                          **|\n");
    printf("|**      {2} - Remove cell              **       {7} - Advance n generations        **|\n");
    printf("|**                                     **                                          **|\n");
    printf("|**      {3} - Insert multiple cells    **       {8} - Statistics                   **|\n");
    printf("|**                                     **                                          **|\n");
    printf("|**      {4} - Remove multiple cells    **       {9} - Settings                     **|\n");
    printf("|**                                     **                                          **|\n");
    printf("|**      {5} - Clear grid               **       {0} - Exit                         **|\n");
    printf("|******************************** TEST PATTERNS **************************************|\n");
    printf("|**      {10} - Glider                  **       {11} - Blinker                     **|\n");
    printf("|**      {12} - Block                   **       {13} - Boat                        **|\n");
    printf("*-------------------------------------------------------------------------------------*\n");
    printf("*-------------------------------------------------------------------------------------*\n");
    printf("Choose an option: ");

    int inputResult = scanf("%d", &choice);

    if (inputResult == EOF) {
        return -2;
    }

    if (inputResult != 1) {
        clearInputBuffer();
        return -1;
    }

    clearInputBuffer();

    return choice;
}


int showConfigMenu(void) {
    int choice;

    clearScreen();

    printf("*-------------------------------------------------------------------------------------*\n");
    printf("|********************************* SETTINGS MENU *************************************|\n");
    printf("|************************************ Options ****************************************|\n");
    printf("|*************************************************************************************|\n");
    printf("|**      {1} - Grid dimensions          **       {3} - Display grid on step         **|\n");
    printf("|**      {2} - Simulation speed         **       {4} - Display stats on step        **|\n");
    printf("|**                                                                                   **|\n");
    printf("|**                       {0} - Return to main menu                                 **|\n");
    printf("*-------------------------------------------------------------------------------------*\n");
    printf("Choose an option: ");

    int inputResult = scanf("%d", &choice);

    if (inputResult == EOF) {
        return -2;
    }

    if (inputResult != 1) {
        clearInputBuffer();
        return -1;
    }

    clearInputBuffer();

    return choice;
}