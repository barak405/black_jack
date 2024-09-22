#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "data.h"

const char *rank_card_name[] = {"ace",
                                 "2" , "3" , "4" , "5" , "6" , "7" , "8" , "9" , "10" 
                                 , "jack" , "queen" , "king"};
const char *suit_card_name[] = { "Heart", "Club", "Diamond", "Spade"};

// Initialize the game with an empty deck and set initial cash
void game_init(GameState *game) {
    srand(time(NULL));  // random number generator

    init_cardlist(&game->deck);
    init_cardlist(&game->player_hand);
    init_cardlist(&game->dealer_hand);

    fill_deck(&game->deck);
    game->cash = 1000;
    game->pot = 0;
    game->player_value = 0;
    game->dealer_value = 0;
}

// Main function to run the game state machine
void run_game(GameState *game) {
    GameStateType current_state = STATE_BETTING;

    while (1) {
        switch (current_state) {
            case STATE_BETTING:
                betting_phase(game);
                current_state = STATE_DEALING;
                break;
            case STATE_DEALING:
                dealing_phase(game);
                current_state = STATE_BLACKJACK_CHECK;
                break;
            case STATE_BLACKJACK_CHECK:
                blackjack_check_phase(game);
                if ( game->player_value == 21){
                    current_state = STATE_RESULT;
                } else {
                    current_state = STATE_PLAYER_TURN;
                }
                
                break;
            case STATE_PLAYER_TURN:
                player_turn_phase(game);

                if (game->player_value > 21) {
                    current_state = STATE_RESULT;
                }else {
                    current_state = STATE_DEALER_TURN;
                }
                break;
            case STATE_DEALER_TURN:
                dealer_turn_phase(game);
                current_state = STATE_RESULT;
                break;
            case STATE_RESULT:
                result_phase(game);


                if ((game->cash < 10) && (game->pot == 0 )) {
                    current_state = STATE_GAME_OVER;

                }else {
                    current_state = STATE_RESET;
                }
                break;

            case STATE_RESET:
                reset_phase(game);
                current_state = STATE_BETTING;
                break;


            case STATE_GAME_OVER:
                game_over_phase(game);
                break;
        }
    }
}


void print_menu() {
    puts("----------------------welcome to my black jack game ----------------------\n");
    puts("---------------------------------enjoy------------------------------------\n");
    
}


//Display_hand function to print Rank of Suit
void display_hand(CardList *hand, const char *owner, int hide_second_card) {
    printf("%s's hand: ", owner);

    Card *current = hand->head;
    int index = 0;
    while (current != NULL) {
        if (hide_second_card && index == 1) {
            printf("??????");  // Hide the second card
        } else {
            int rank = get_rank(current) - 1;  // Adjust rank to match array indexing
            int suit = get_suit(current);      // Extract suit from the card

            // Find the correct suit based on the suit bit value
            const char *suit_name = NULL;
            switch (suit)
            {
            case 1:
                suit_name = suit_card_name[0]; //Heart
                break;
            case 2:
                suit_name = suit_card_name[1];//Club
                break;
            case 4:
                suit_name = suit_card_name[2];//Diamond
                break;
            case 8:
                suit_name = suit_card_name[3];//Spade
                break;
            
            default:
                break;
            }
            

            printf("%s of %s", rank_card_name[rank], suit_name);
        }

        current = current->next;
        index++;

        if (current != NULL) {
            printf(", ");
        }
    }

    printf("\n");
}







// Betting phase where player places a bet
void betting_phase(GameState *game) {
    printf("Your cash: %d\n", game->cash);
    printf("Current pot: %d\n", game->pot);

    if (game->cash < 10 && game->pot == 0) {
        printf("You're out of cash! Game Over.\n");
        exit(0);
    }

    int bet;
    do {
        printf("How much would you like to bet? (multiples of 10 if its a tie you can add 0): ");
        scanf("%d", &bet);
        if( (bet == 0) && (game->pot == 0)) {
            printf(" You cant bet zero amount, dont be a cheat and give me your money:) ! \n");
        }
        if( (bet %10 != 0)) {
            printf(" You can only bet in multiples of 10 please enter a new bet \n");
        }
        if( (bet > game->cash)) {
            printf(" You dont have that amount of money, please enter a right amount \n");
        }

    } while ((bet % 10 != 0 )|| (bet < 0) || (bet > game->cash) || (bet == 0 && game->pot == 0));

    game->cash -= bet;
    game->pot += bet;
}

// Dealing phase where cards are dealt to player and dealer
void dealing_phase(GameState *game) {
    card_push(&game->player_hand, card_draw(&game->deck));
    card_push(&game->player_hand, card_draw(&game->deck));

    card_push(&game->dealer_hand, card_draw(&game->deck));
    card_push(&game->dealer_hand, card_draw(&game->deck));

    // Display player and dealer hands
    display_hand(&game->dealer_hand, "Dealer", 1);
    display_hand(&game->player_hand, "player", 0);
}

// Blackjack check phase to check if the player has 21
void blackjack_check_phase(GameState *game) {
    // Simplified hand value calculation
    int A_rank = get_rank(game->player_hand.head);
    int B_rank = get_rank(game->player_hand.head->next);

    game->player_value =(A_rank + B_rank);

    if ((A_rank == 1 && B_rank == 10 ) || (B_rank == 1 && A_rank == 10 )) { //change player card val to  black jack
        game->player_value = 21;
    }

    if (game->player_value == 21 && game->player_hand.len == 2) {
        printf("Black Jack!");
        game->cash += game->pot + (game->pot * 1.5);
        game->pot =0;
    }
}

// Player's turn to hit or stand
void player_turn_phase(GameState *game) {
    char action[10];
    while (1) {
        printf("Do you want to hit or stand? ");
        scanf("%s", action);

        if (strcmp(action, "hit") == 0) {
            card_push(&game->player_hand, card_draw(&game->deck));
            display_hand(&game->player_hand, "Player", 0);
            game->player_value += get_rank(game->player_hand.head);  // scoring

            if (game->player_value > 21) {
                printf("Bust! You lose.\n");
                game->pot = 0;
                return;
            }
        } else if (strcmp(action, "stand") == 0) {
            break;
        } else {
            printf("Invalid input. Please enter 'hit' or 'stand'.\n");
        }
    }
}

// Dealer's turn to draw cards until 17 or more
void dealer_turn_phase(GameState *game) {

    game->dealer_value = get_rank(game->dealer_hand.head) + get_rank(game->dealer_hand.head->next);

    while (game->dealer_value < 17) {
        printf("Dealer draws...\n"); 
        card_push(&game->dealer_hand, card_draw(&game->deck)); //alert whenever the dealr draw a card
        game->dealer_value += get_rank(game->dealer_hand.head);
        
        
    }

}

// Phase to determine the result of the round
void result_phase(GameState *game) {
    if (game->dealer_value > 21) {
         printf("Dealer busts! You win.\n");
         game->cash += game->pot *2;
         game->pot = 0; 
         return; 


    }
    if( game->dealer_value == game->player_value) {
        printf("its a tie! pot remins\n");
        return;

    }
    
    
    if ((game->dealer_value < game->player_value) && game->player_value <= 21) {
        printf("You win the round!\n");
        game->cash += game->pot * 2;
        game->pot = 0;
        return;
    }
    
    
    if(game->dealer_value > game->player_value) {
        printf("Dealer Wins! you lose\n");
        display_hand(&game->dealer_hand, "The win hand of the dealer", 0);
        game->pot = 0;
        return;
        }
}
// Reset the game state for a new round
void reset_phase(GameState *game) {
    free_cardlist(&game->player_hand);
    free_cardlist(&game->dealer_hand);
    init_cardlist(&game->player_hand);
    init_cardlist(&game->dealer_hand);

    if (game->cash < 10 && game->pot == 0)  {
        printf("Game Over. You are out of cash.\n");
        exit(0);
    }
    fill_deck(&game->deck);
    game->player_value = 0;
    game->dealer_value = 0;


    printf("Do you want to play again? (yes/no):\n ");
    char response[10];
    scanf("%s", response);
    if (strcmp(response, "no") == 0) {
        printf("Thank you for playing! come again\n");
        exit(0);
    }
}

// Game over phase when player runs out of cash
void game_over_phase(GameState *game) {
    (void)game;  // Explicitly mark 'game' as unused to avoid the warning
    printf("Game Over. You ran out of cash.\n");
    exit(0);
}

// Initialize the card list
void init_cardlist(CardList *cardlist) {
    cardlist->head = NULL;
    cardlist->len = 0;
}

// Create a new card with given rank and suit
Card *card_new(int rank, int suit) {
    Card *new_card = (Card *)malloc(sizeof(Card));
    new_card->data = (rank << 4) | suit;
    new_card->next = NULL;
    return new_card;
}

// Push a card onto a card list
void card_push(CardList *cardlist, Card *card) {
    card->next = cardlist->head;
    cardlist->head = card;
    cardlist->len++;
}

// Fill the deck with all 52 cards (13 ranks x 4 suits)
void fill_deck(CardList *deck) {
    for (int rank = 1; rank <= 13; rank++) {
        for (int suit = 1; suit <= 8; suit <<= 1) {
            Card *new_card = card_new(rank, suit);
            card_push(deck, new_card);
        }
    }
}

// Draw a random card from the deck
Card *card_draw(CardList *deck) {
    if (deck->len == 0) return NULL;

    size_t rand_pos = rand() % deck->len;
    Card *prev = NULL, *current = deck->head;
    for (size_t i = 0; i < rand_pos; i++) {
        prev = current;
        current = current->next;
    }

    // Remove the card from the deck
    if (prev) prev->next = current->next;
    else deck->head = current->next;

    deck->len--;
    current->next = NULL;
    return current;
}

// Get rank of a card
int get_rank(Card *card) {
    int rank = (card->data >> 4);  // gets rank from the high 4 bits
    
    if (rank >=11) {
        return 10; //jack queen king  
    } else if ( rank == 1) {
        return 1;
    }
    return rank; // return the num value of the non royelty cards 
    
}

// Get suit of a card
int get_suit(Card *card) {
    return (card->data & 0xF);  // Extract suit from the low 4 bits
}

// Free all dynamically allocated cards
void free_cardlist(CardList *cardlist) {
    Card *current = cardlist->head;
    while (current != NULL) {
        Card *next = current->next;
        free(current);
        current = next;
    }
    cardlist->head = NULL;
    cardlist->len = 0;
}