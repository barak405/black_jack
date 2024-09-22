#ifndef DATA_H
#define DATA_H

#include <stddef.h>
#include <stdint.h>

// Card structure linked list node
typedef struct Card {
    uint8_t data;          
    struct Card *next;     
} Card;

// CardList structure for hands & decks
typedef struct CardList {
    Card *head;            
    size_t len;            
} CardList;

// GameState structure to track game state
typedef struct GameState {
    CardList deck;         // Main deck of cards
    CardList player_hand;  // Player's hand
    CardList dealer_hand;  // Dealer's hand
    int cash;              // Player's cash
    int pot;               // Current bet pot
    int player_value;      // Value of the player's hand
    int dealer_value;      // Value of the dealer's hand
} GameState;

// Enum for  states mecine
typedef enum {
    STATE_BETTING,
    STATE_DEALING,
    STATE_BLACKJACK_CHECK,
    STATE_PLAYER_TURN,
    STATE_DEALER_TURN,
    STATE_RESULT,
    STATE_RESET,
    STATE_GAME_OVER
} GameStateType;

//  Game state Function 
void game_init(GameState *game);
void run_game(GameState *game);
void print_menu();
void display_hand(CardList *hand, const char *owner, int hide_first);
void betting_phase(GameState *game);
void dealing_phase(GameState *game);
void blackjack_check_phase(GameState *game);
void player_turn_phase(GameState *game);
void dealer_turn_phase(GameState *game);
void result_phase(GameState *game);
void reset_phase(GameState *game);
void game_over_phase(GameState *game);

// Card handling Functions
void init_cardlist(CardList *cardlist);
Card *card_new(int rank, int suit);
void card_push(CardList *cardlist, Card *card);
void fill_deck(CardList *deck);
Card *card_draw(CardList *deck);
int get_rank(Card *card);
int get_suit(Card *card);
void free_cardlist(CardList *cardlist);

#endif
