#ifndef SCORETABLE_H
#define SCORETABLE_H

#include <ncurses.h>
#include <locale.h>
#include <string.h>
#include <stdio.h>
#include <ncursesw/ncurses.h>

#define MAX_PLAYERS 200
#define COLOR_ORANGE 80
#define COLOR_PINK 81

typedef struct Player {
    char username[50];
    char email[100];
    char password[50];
    int sum_of_scores;
    int number_of_golds;
    int number_of_finished_games;
    int experience;
    char login_datetime[64];
    int rank;
    char special_name[20];
} Player;

int display_score_table(const char *file_path, const char *highlighted_username) {
    setlocale(LC_ALL, "");

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();

    // Ensure ncurses uses the correct encoding
    if (has_colors()) {
        start_color();
    }

    // Clear the screen
    clear();
    getch();

    char username[50];
    strcpy(username, highlighted_username);

    WINDOW *win = newwin(LINES, COLS, 0, 0);
    keypad(stdscr, TRUE);
    
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_WHITE);
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);
    init_pair(4, COLOR_BLACK, COLOR_CYAN);
    init_pair(5, COLOR_MAGENTA, COLOR_WHITE);
    init_color(COLOR_YELLOW, 1000, 1000, 0);
    init_color(COLOR_ORANGE, 900, 500, 0);
    init_color(COLOR_PINK, 900, 0, 750);
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);
    init_pair(7, COLOR_ORANGE, COLOR_BLACK);
    init_pair(8, COLOR_PINK, COLOR_BLACK);

    Player players[MAX_PLAYERS];
    int num_players = 0;

    // Open the user_data.txt file
    FILE *fptr1 = fopen(file_path, "r");

    char header[132];
    fgets(header, sizeof(header), fptr1);
    fgets(header, sizeof(header), fptr1);

    while (fscanf(fptr1, "%49s %99s %49s", players[num_players].username, players[num_players].email, players[num_players].password) == 3) {
        if (num_players >= MAX_PLAYERS) {
            break;
        }
        num_players++;
    }
    fclose(fptr1);

    for (int j = 0; j < num_players; j++) {
        char foldername[200];
        FILE *fptr2;

        snprintf(foldername, sizeof(foldername), "Users' Info/%s/info.txt", players[j].username);
        fptr2 = fopen(foldername, "r");
        if (fscanf(fptr2, "%49s %d %d %d %16s", 
                players[j].username, 
                &players[j].sum_of_scores, 
                &players[j].number_of_golds, 
                &players[j].number_of_finished_games,
                players[j].login_datetime) != 5) {
            // Handle error
        }
        fclose(fptr2);
    }

    // Calculate player ranks based on sum of scores
    for (int i = 0; i < num_players; i++) {
        players[i].rank = 1;
        for (int j = 0; j < num_players; j++) {
            if (players[j].sum_of_scores > players[i].sum_of_scores) {
                players[i].rank++;
            }
        }
    }

    // Sort players by rank
    for (int i = 0; i < num_players - 1; i++) {
        for (int j = i + 1; j < num_players; j++) {
            if (players[i].rank > players[j].rank) {
                Player temp = players[i];
                players[i] = players[j];
                players[j] = temp;
            }
        }
    }

    // For navigation through pages
    int page_size = LINES - 4;
    int current_page = 0;
    int total_pages = (num_players + page_size - 1) / page_size;
    int ch;

    while (1) {
        clear();

        // Header
        attron(COLOR_PAIR(4) | A_BOLD);
        mvprintw(0, COLS / 2 - 45, "   Rank |     Username     | Score | Earned Golds | Finished Games |     Experience     |   Title  \U0001F3C6   ");
        attroff(COLOR_PAIR(4) | A_BOLD);
        attron(COLOR_PAIR(5) | A_BOLD);
        mvprintw(1, COLS / 2 - 45, "--------------------------------------------------------------------------------------------------------");
        attroff(COLOR_PAIR(5) | A_BOLD);
        move(2, 0);

        for (int i = 0; i < page_size && current_page * page_size + i < num_players; i++) {
            int player_index = current_page * page_size + i;

            if (player_index >= num_players) {
                break;
            }

            // Assign special name and color based on rank
            if (players[player_index].rank == 1) {
                strcpy(players[player_index].special_name, "Legend \U0001F947");
                attron(COLOR_PAIR(6));
                attron(A_BOLD);
            } else if (players[player_index].rank == 2) {
                strcpy(players[player_index].special_name, "Champion \U0001F948");
                attron(COLOR_PAIR(7));
                attron(A_BOLD);
            } else if (players[player_index].rank == 3) {
                strcpy(players[player_index].special_name, "Hero \U0001F949");
                attron(COLOR_PAIR(8));
                attron(A_BOLD);
            } else {
                strcpy(players[player_index].special_name, "-");
                attron(COLOR_PAIR(1)); // Default to white for all others
            }

            // Highlight the current user's row
            if (strcmp(players[player_index].username, username) == 0) {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(2 + i, COLS / 2 - 45, "%-8d| %-17s| %-6d| %-13d| %-15d| %-19s| %-12s  <==", players[player_index].rank, players[player_index].username, players[player_index].sum_of_scores, players[player_index].number_of_golds,
                         players[player_index].number_of_finished_games, players[player_index].login_datetime, players[player_index].special_name);
                attroff(COLOR_PAIR(2) | A_BOLD);
            } else {
                mvprintw(2 + i, COLS / 2 - 45, "%-8d| %-17s| %-6d| %-13d| %-15d| %-19s| %-12s", players[player_index].rank, players[player_index].username, players[player_index].sum_of_scores, players[player_index].number_of_golds,
                         players[player_index].number_of_finished_games, players[player_index].login_datetime, players[player_index].special_name);
            }

            attroff(A_BOLD);
            attroff(COLOR_PAIR(6));
            attroff(COLOR_PAIR(7));
            attroff(COLOR_PAIR(8));
            attroff(COLOR_PAIR(1)); // Ensure to reset to default color pair after each rank
        }

        mvprintw(LINES - 2, COLS / 2, "Page %d of %d", current_page + 1, total_pages);
        mvprintw(LINES - 1, COLS / 2 - 11, "Use Arrow keys to scroll, 'q' to quit");
        refresh();

        ch = getch();
        if (ch == KEY_DOWN && current_page < total_pages - 1) {
            current_page++;  // Next page
        } else if (ch == KEY_UP && current_page > 0) {
            current_page--;  // Previous page
        } else if (ch == 'q') {  // Quit
            break;
        }
    }
    endwin();
    return 0;
}


#endif