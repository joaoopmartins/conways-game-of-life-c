
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

int main(void) {
    int option = -1;
    int quantGens;
    int configOption;
    int inputResult;

    initGame();

    while (option != 0) {
        printGrid();
        option = showMainMenu();

        /* Handle end-of-file in the main menu */
        if (option == -2) {
            printf("\nInput closed. Terminating program...\n");
            return 0;
        }

        switch (option) {
            case 0:
                printf("\nTerminating program... See you soon!\n");
                break;

            case 1:
                insertSingleCell();
                break;

            case 2:
                removeSingleCell();
                break;

            case 3:
                insertMultipleCells();
                break;

            case 4:
                removeMultipleCells();
                break;

            case 5:
                clearGrid();
                printf("\nGrid reset successfully!\n");
                delaySeconds(1.5f);
                break;

            case 6:
                advanceGenerations(1);
                break;

            case 7:
                printf("\nEnter number of generations to advance: ");

                inputResult = scanf("%d", &quantGens);

                if (inputResult == EOF) {
                    printf("\nInput closed. Terminating program...\n");
                    return 0;
                }

                clearInputBuffer();

                if (inputResult == 1 && quantGens > 0) {
                    advanceGenerations(quantGens);
                } else {
                    printf("Invalid quantity! Please enter a positive integer.\n");
                    delaySeconds(1.5f);
                }
                break;

            case 8:
                printStatistics();
                printf("\nPress ENTER to continue...");

                if (getchar() == EOF) {
                    printf("\nInput closed. Terminating program...\n");
                    return 0;
                }
                break;

            case 9:
                configOption = -1;

                while (configOption != 0) {
                    configOption = showConfigMenu();

                    /* Handle end-of-file in the configuration menu */
                    if (configOption == -2) {
                        printf("\nInput closed. Terminating program...\n");
                        return 0;
                    }

                    switch (configOption) {
                        case 0:
                            break;

                        case 1:
                            configureDimensions();
                            break;

                        case 2:
                            configureSpeed();
                            break;

                        case 3:
                            toggleShowGrid();
                            break;

                        case 4:
                            toggleShowStatistics();
                            break;

                        default:
                            printf("\nInvalid option!\n");
                            delaySeconds(1.5f);
                            break;
                    }
                }
                break;

            /* Fast test presets */
            case 10:
                loadGliderPattern();
                break;

            case 11:
                loadBlinkerPattern();
                break;

            case 12:
                loadBlockPattern();
                break;

            case 13:
                loadBoatPattern();
                break;

            default:
                printf("\nInvalid option! Please enter a valid menu option.\n");
                delaySeconds(1.5f);
                break;
        }
    }

    return 0;
}
