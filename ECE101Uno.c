#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 100
#define HAND_SIZE 100
#define HAND 7
#define MAX_PLAYERS 10

typedef struct card_t {
    char name;
    char color;
} card;

typedef struct player_t {
    char playerName[20];
    card deck[HAND_SIZE];
    int decksize;
} player;

void initializeDeck(card deck[], int deckSize);
void shuffleDeck(card deck[]);
int drawCard(card deck[], int *deckSize, player *p);
int isValidCard(card top, card cand);
int handleAND(player *current, card TopCard, player *next, card deck[], int *deckSize);
int handleOR(player *current, card TopCard, player *next, card deck[], int *deckSize);
int handleNOT(int *currentIndex, int numPlayers);
int handleReverse(int *currentIndex, int numPlayers);
void printCard(card c);
void printPlayerHand(player *p);

int main() {
    
    card deck[DECK_SIZE];
    int deckSize = DECK_SIZE;
    player players[MAX_PLAYERS];
    card pile[HAND_SIZE];
    int pileSize = 0;
    int numPlayers;
    int i, j;
    int current = 0;
    char playAgain;

    srand((unsigned)time(NULL));

    do {
        
        initializeDeck(deck, DECK_SIZE);
        shuffleDeck(deck);
        deckSize = DECK_SIZE;
        pileSize = 0;

        do {
            printf("Enter number of players (2-%d): ", MAX_PLAYERS);
            scanf("%d", &numPlayers);
        } while (numPlayers < 2 || numPlayers > MAX_PLAYERS);

        for (i = 0; i < numPlayers; i++) {
            printf("Enter name for player %d: ", i + 1);
            scanf("%s", players[i].playerName);
            players[i].decksize = 0;
        }

        for (i = 0; i < HAND; i++) {
            for (j = 0; j < numPlayers; j++) {
                drawCard(deck, &deckSize, &players[j]);
            }
        }

        for (i = 0; i < numPlayers; i++) {
            printPlayerHand(&players[i]);
        }
        printf("Card pile is empty.\n\n");

        current = 0;
        while (1) {
            player *p = &players[current];
            int nextIndex = (current + 1) % numPlayers;
            player *next = &players[nextIndex];

            if (p->decksize == 0) {
                printf("%s wins!\n", p->playerName);
                break;
            }
            if (deckSize == 0) {
                int winner = 0;
                for (i = 1; i < numPlayers; i++) {
                    if (players[i].decksize < players[winner].decksize) {
                        winner = i;
                    }
                }
                printf("Deck empty. %s wins with fewer cards!\n", players[winner].playerName);
                break;
            }

            int choice;
            printf("%s, enter which card to play (0-%d), or -1 to draw: ",
                   p->playerName, p->decksize - 1);
            scanf("%d", &choice);

            if (choice == -1) {
                drawCard(deck, &deckSize, p);
                printf("%s draws a card.\n\n", p->playerName);
                current = nextIndex;
                continue;
            }
            if (choice < 0 || choice >= p->decksize) {
                printf("Invalid choice, %s does not have %d cards\n\n",
                       p->playerName, p->decksize);
                continue;
            }

            card selection = p->deck[choice];
            if (pileSize > 0 && !isValidCard(pile[pileSize - 1], selection)) {
                printf("Invalid choice, cannot place ");
                printCard(selection);
                printf(" on ");
                printCard(pile[pileSize - 1]);
                printf("\n\n");
                continue;
            }

            pile[pileSize++] = selection;
            for (i = choice; i < p->decksize - 1; i++) {
                p->deck[i] = p->deck[i + 1];
            }
            p->decksize--;

            if (selection.name == 'A') {
                handleAND(p, pile[pileSize - 2], next, deck, &deckSize);
                current = nextIndex;
            } else if (selection.name == 'O') {
                handleOR(p, pile[pileSize - 2], next, deck, &deckSize);
                current = nextIndex;
            } else if (selection.name == 'N') {
                handleNOT(&current, numPlayers);
            } else if (selection.name == 'R') {
                handleReverse(&current, numPlayers);
            } else {
                current = nextIndex;
            }

            for (i = 0; i < numPlayers; i++) {
                printPlayerHand(&players[i]);
            }
            printf("Top of card pile is: ");
            printCard(pile[pileSize - 1]);
            printf("\n\n");
        }

        printf("Play again? (Y/N): ");
        scanf(" %c", &playAgain);
        printf("Goodbye\n");
    } while (playAgain == 'Y' || playAgain == 'y');

    return 0;
}

void initializeDeck(card deck[], int deckSize) {
    const char colors[] = {'R', 'Y', 'G', 'B'};
    int index = 0;
    for (int i = 0; i < 4; i++) {
        for (int rep = 0; rep < 2; rep++) {
            for (char n = '0'; n <= '9'; n++) {
                deck[index++] = (card){n, colors[i]};
            }
        }
    }
    for (int i = 0; i < 5; i++) {
        deck[index++] = (card){'A', 'S'};
    }
    for (int i = 0; i < 5; i++) {
        deck[index++] = (card){'O', 'S'};
    }
    for (int i = 0; i < 5; i++) {
        deck[index++] = (card){'N', 'S'};
    }
    for (int i = 0; i < 5; i++) {
        deck[index++] = (card){'R', 'S'};
    }
}

void shuffleDeck(card deck[]) {
    for (int i = 0; i < 10000; i++) {
        int a = rand() % DECK_SIZE;
        int b = rand() % DECK_SIZE;
        card t = deck[a];
        deck[a] = deck[b];
        deck[b] = t;
    }
}

int drawCard(card deck[], int *deckSize, player *p) {
    if (*deckSize <= 0) {
        return 0;
    }
    p->deck[p->decksize++] = deck[--(*deckSize)];
    return 1;
}

int isValidCard(card top, card cand) {
    if (cand.color == 'S') {
        return 1;
    }
    return (cand.color == top.color || cand.name == top.name);
}

void printCard(card c) {
    if (c.color != 'S') {
        const char *color;
        switch (c.color) {
            case 'R': color = "Red"; break;
            case 'Y': color = "Yellow"; break;
            case 'G': color = "Green"; break;
            case 'B': color = "Blue"; break;
            default: color = "?";
        }
        printf("%s %c", color, c.name);
    } else {
        const char *name;
        switch (c.name) {
            case 'A': name = "AND"; break;
            case 'O': name = "OR"; break;
            case 'N': name = "NOT"; break;
            case 'R': name = "Reverse"; break;
            default: name = "?";
        }
        printf("%s", name);
    }
}

void printPlayerHand(player *p) {
    printf("%s’s hand:\n", p->playerName);
    for (int i = 0; i < p->decksize; i++) {
        printf("[%d] ", i);
        printCard(p->deck[i]);
        printf("\n");
    }
    printf("\n");
}

int handleAND(player *current, card TopCard, player *next, card deck[], int *deckSize) {
    int index_s[HAND_SIZE];
    int count = 0;
    for (int i = 0; i < current->decksize; i++) {
        if (current->deck[i].color != 'S') {
            index_s[count++] = i;
        }
    }
    printf("Choose the card to play with AND from ");
    for (int i = 0; i < count; i++) {
        printf("%d%s", index_s[i], i < count-1 ? ", " : ": ");
    }
    int choice;
    scanf("%d", &choice);
    int canMatch = 0;
    for (int i = 0; i < next->decksize; i++) {
        if (next->deck[i].color == current->deck[choice].color && next->deck[i].name == current->deck[choice].name) {
            canMatch = 1;
        }
    }
    if (!canMatch) {
        printf("AND penalty, Draw 4\n\n");
        for (int i = 0; i < 4; i++) {
            drawCard(deck, deckSize, next);
        }
    } else {
        printf("Card Matches, no AND penalty\n\n");
    }
    return canMatch;
}

int handleOR(player *current, card TopCard, player *next, card deck[], int *deckSize) {
    int index_s[HAND_SIZE];
    int count = 0;
    for (int i = 0; i < current->decksize; i++) {
        if (current->deck[i].color != 'S') {
            index_s[count++] = i;
        }
    }
    printf("Choose the card to play with OR from ");
    for (int i = 0; i < count; i++) {
        printf("%d%s", index_s[i], i < count-1 ? ", " : ": ");
    }
    int choice;
    scanf("%d", &choice);
    int canMatch = 0;
    for (int i = 0; i < next->decksize; i++) {
        if (next->deck[i].color == current->deck[choice].color || next->deck[i].name == current->deck[choice].name) {
            canMatch = 1;
        }
    }
    if (!canMatch) {
        printf("OR penalty, Draw 4\n\n");
        for (int i = 0; i < 4; i++) {
            drawCard(deck, deckSize, next);
        }
    } else {
        printf("Card Matches, no OR penalty\n\n");
    }
    return canMatch;
}

int handleNOT(int *currentIndex, int numPlayers) {
    *currentIndex = (*currentIndex + 1) % numPlayers;
    printf("Next player's turn skipped.\n\n");
    return 0;
}

int handleReverse(int *currentIndex, int numPlayers) {
    printf("Reverse played. You go again.\n\n");
    return 0;
}
