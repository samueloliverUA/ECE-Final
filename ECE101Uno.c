#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define DECK_SIZE 100
#define HAND_SIZE 100
#define HANDNUM 7
#define PLAYER_COUNT 2


typedef struct card_t {
    char name;  
    char color;  
} card;


typedef struct player_t {
    char playerName[20];
    card deck[HAND_SIZE];
    int decksize;
} player;

// Function prototypes
void initializeDeck(card deck[], int deckSize);
void shuffleDeck(card deck[]);
int drawCard(card deck[], int *deckSize, player *p);
int isValidCard(card top, card cand);
void printCard(card c);
void printPlayerHand(player *p);
int handleAND(player *curr, card pileTop, player *other, card deck[], int *deckSize);
int handleOR(player *curr, card pileTop, player *other, card deck[], int *deckSize);
int handleNOT(int *currentIndex);
int handleReverse(int *currentIndex);

int main() {
    card deck[DECK_SIZE];
    int deckSize = DECK_SIZE;
    player players[PLAYER_COUNT];
    card pile[HAND_SIZE];
    int pileSize = 0;
    int numPlayers;
    int i, j;
    int current = 0;
    char playAgain;

    do {
        // create new game
        initializeDeck(deck, DECK_SIZE);
        shuffleDeck(deck);
        deckSize = DECK_SIZE;
        pileSize = 0;

        // player setup
        do {
            printf("Enter number of players: ");
            scanf("%d", &numPlayers);
        } while (numPlayers != PLAYER_COUNT);
        printf("Enter first player's name: ");
        scanf("%19s", players[0].playerName);
        printf("Enter second player's name: ");
        scanf("%19s", players[1].playerName);
        for (i = 0; i < PLAYER_COUNT; i++) players[i].decksize = 0;

        // deal
        for (i = 0; i < HANDNUM; i++)
            for (j = 0; j < PLAYER_COUNT; j++)
                drawCard(deck, &deckSize, &players[j]);

        // initial hands
        printPlayerHand(&players[0]);
        printPlayerHand(&players[1]);
        printf("Card pile is empty.\n\n");

        current = 0;
        // game loop
        while (1) {
            player *p = &players[current];
            player *other = &players[(current + 1) % PLAYER_COUNT];

            if (p->decksize == 0) {
                printf("%s wins!\n", p->playerName);
                break;
            }
            if (deckSize == 0) {
                int winner = (players[0].decksize < players[1].decksize) ? 0 : 1;
                printf("Deck empty. %s wins with fewer cards!\n", players[winner].playerName);
                break;
            }

            // prompt play
            int choice;
            printf("%s, enter which card to play from 0 to %d: ",
                   p->playerName, p->decksize - 1);
            scanf("%d", &choice);
            if (choice < 0 || choice >= p->decksize) {
                printf("Invalid choice, %s does not have %d cards\n\n",
                       p->playerName, p->decksize);
                continue;
            }
            // select card
            card sel = p->deck[choice];
            // validate against pile
            if (pileSize > 0 && !isValidCard(pile[pileSize-1], sel)) {
                printf("Invalid choice, cannot place "); printCard(sel);
                printf(" in "); printCard(pile[pileSize-1]);
                printf("\n\n");
                continue;
            }
            // play
            pile[pileSize++] = sel;
            // remove from hand
            for (i = choice; i < p->decksize - 1; i++)
                p->deck[i] = p->deck[i+1];
            p->decksize--;

            // handle special
            if (sel.name == 'A') {
                handleAND(p, pile[pileSize-2], other, deck, &deckSize);
                current = (current + 1) % PLAYER_COUNT;
            } else if (sel.name == 'O') {
                handleOR(p, pile[pileSize-2], other, deck, &deckSize);
                current = (current + 1) % PLAYER_COUNT;
            } else if (sel.name == 'N') {
                handleNOT(&current);
            } else if (sel.name == 'R') {
                handleReverse(&current);
            } else {
                current = (current + 1) % PLAYER_COUNT;
            }

            // show state
            printPlayerHand(&players[0]);
            printPlayerHand(&players[1]);
            printf("Top of card pile is: "); printCard(pile[pileSize-1]);
            printf("\n\n");
        }

        // play again
        printf("Play again? ");
        scanf(" %c", &playAgain);
        printf("Goodbye\n");
    } while (playAgain == 'Y' || playAgain == 'y');

    return 0;
}

// implementations
void initializeDeck(card deck[], int deckSize) {
    const char cols[] = {'R','Y','G','B'};
    int idx = 0;
    for (int c = 0; c < 4; c++)
        for (int rep = 0; rep < 2; rep++)
            for (char n = '0'; n <= '9'; n++)
                deck[idx++] = (card){n, cols[c]};
    for (int i = 0; i < 5; i++)
        deck[idx++] = (card){'A','S'};
    for (int i = 0; i < 5; i++) deck[idx++] = (card){'O','S'};
    for (int i = 0; i < 5; i++) deck[idx++] = (card){'N','S'};
    for (int i = 0; i < 5; i++) deck[idx++] = (card){'R','S'};
}

void shuffleDeck(card deck[]) {
    for (int i = 0; i < 10000; i++) {
        int a = rand() % DECK_SIZE;
        int b = rand() % DECK_SIZE;
        card t = deck[a]; deck[a] = deck[b]; deck[b] = t;
    }
}

int drawCard(card deck[], int *deckSize, player *p) {
    if (*deckSize <= 0) return 0;
    p->deck[p->decksize++] = deck[--(*deckSize)];
    return 1;
}

int isValidCard(card top, card cand) {
    if (cand.color == 'S') return 1;
    return (cand.color == top.color || cand.name == top.name);
}

void printCard(card c) {
    if (c.color != 'S') {
        const char *col;
        switch (c.color) {
            case 'R': col = "Red"; break;
            case 'Y': col = "Yellow"; break;
            case 'G': col = "Green"; break;
            case 'B': col = "Blue"; break;
            default: col = "?";
        }
        printf("%s %c", col, c.name);
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

int handleAND(player *curr, card pileTop, player *other, card deck[], int *deckSize) {
    int idxs[HAND_SIZE], cnt = 0;
    for (int i = 0; i < curr->decksize; i++)
        if (curr->deck[i].color != 'S') idxs[cnt++] = i;
    printf("Choose the card to play with AND from ");
    for (int i = 0; i < cnt; i++) printf("%d%s", idxs[i], i<cnt-1?", ":": ");
    int choice; scanf("%d", &choice);
    int canMatch = 0;
    for (int i = 0; i < other->decksize; i++)
        if (other->deck[i].color == curr->deck[choice].color && other->deck[i].name == curr->deck[choice].name)
            canMatch = 1;
    if (!canMatch) {
        printf("AND penalty, Draw 4\n\n");
        for (int i = 0; i < 4; i++) drawCard(deck, deckSize, other);
    } else printf("Card Matches, no AND penalty\n\n");
    return canMatch;
}

int handleOR(player *curr, card pileTop, player *other, card deck[], int *deckSize) {
    int idxs[HAND_SIZE], cnt = 0;
    for (int i = 0; i < curr->decksize; i++)
        if (curr->deck[i].color != 'S') idxs[cnt++] = i;
    printf("Choose the card to play with OR from ");
    for (int i = 0; i < cnt; i++) printf("%d%s", idxs[i], i<cnt-1?", ":": ");
    int choice; scanf("%d", &choice);
    int canMatch = 0;
    for (int i = 0; i < other->decksize; i++)
        if (other->deck[i].color == curr->deck[choice].color || other->deck[i].name == curr->deck[choice].name)
            canMatch = 1;
    if (!canMatch) {
        printf("OR penalty, Draw 4\n\n");
        for (int i = 0; i < 4; i++) drawCard(deck, deckSize, other);
    } else printf("Card Matches, no OR penalty\n\n");
    return canMatch;
}

int handleNOT(int *currentIndex) {
    printf("Next Player's turn skipped.\n\n");
    return 0;
}

int handleReverse(int *currentIndex) {
    printf("Reverse played. You go again.\n\n");
    return 0;
}


