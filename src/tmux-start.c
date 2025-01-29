/*
 * tmux-start - Interactive tmux session manager
 *
 * (C) Copyright 2025 A.B. Carroll III <ben@hl9.net>
 *
 * Licensed under the MIT License or the BSD 2-Clause License, at your option.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "tmux-start.h"

// Function to initialize the session list
struct tmux_session_list *init_session_list(unsigned int count) {
    struct tmux_session_list *list = malloc(sizeof(struct tmux_session_list));
    list->count = count;
    list->sessions = malloc(count * sizeof(struct tmux_session));
    return list;
}

// Function to free the session list
void free_session_list(struct tmux_session_list *list) {
    for (unsigned int i = 0; i < list->count; i++) {
        free(list->sessions[i].shortcut);
        free(list->sessions[i].label);
    }
    free(list->sessions);
    free(list);
}

/**
 * What a mess, ... not sure of the cleaner way to do this without all the escape non-sense.
 */
void printHeader(void) {
    printf(" _                                            _                 \n");
    printf("| |_ _ __ ___  _   ___  __  ___  ___  ___ ___(_) ___  _ __  ___ \n");
    printf("| __| '_ ` _ \\| | | \\ \\/ / / __|/ _ \\/ __/ __| |/ _ \\| '_ \\/ __|\n");
    printf("| |_| | | | | | |_| |>  <  \\__ \\  __/\\__ \\__ \\ | (_) | | | \\__ \\\n");
    printf(" \\__|_| |_| |_|\\__,_/_/\\_\\ |___/\\___||___/___/_|\\___/|_| |_|___/\n");
    printf("                                                                \n");
    printf(" Use the up/down arrows, or h/j to navigate existing sessions   \n");
    printf(" Press enter to attach to an existing session, or enter the session number\n\n");
}


// Function to disable canonical mode and echo for the terminal
void disableCanonicalMode(struct termios *old_attr) {
    struct termios new_attr;
    tcgetattr(STDIN_FILENO, old_attr);
    new_attr = *old_attr;
    new_attr.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);
}

// Function to restore terminal attributes
void restoreCanonicalMode(struct termios *old_attr) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_attr);
}

// Function to populate the session list by running "tmux list-sessions"
struct tmux_session_list *getTmuxSessionList() {
    FILE *fp = popen("tmux list-sessions 2>/dev/null", "r");
    if (!fp) {
        fprintf(stderr, "Failed to get tmux sessions.\n");
        return NULL;
    }

   // Allocate memory for the session list
    struct tmux_session_list *list = malloc(sizeof(struct tmux_session_list));
    list->count = 0;
    list->sessions = malloc(MAX_SESSIONS * sizeof(struct tmux_session));

    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, fp) && list->count < MAX_SESSIONS) {
        // Remove newline character
        line[strcspn(line, "\n")] = '\0';

        // Extract the shortcut (e.g., "0") and the full label
        char *shortcut = strtok(strdup(line), ":");
        char *label = strdup(line); // Full line as label

        // Populate the session struct
        list->sessions[list->count].shortcut = shortcut;
        list->sessions[list->count].label = label;

        list->count++;
    }

    pclose(fp);
    return list;
}

// Function to attach to a selected tmux session
void attachToSession(const char *tmux_id) {
    char command[300];
    snprintf(command, sizeof(command), "tmux attach-session -t '%s'", tmux_id);
    if (system(command) != 0) {
        fprintf(stderr, "Failed to attach to session: %s\n", tmux_id);
    }
}


// Function to create a new session
void createNewSession(int ask_name) {
    char session_name[256] = {0};

    if (ask_name) {
        printf("\nEnter a name for the new session (ESC to cancel, enter for none): ");
        struct termios old_attr;
        disableCanonicalMode(&old_attr);

        int index = 0;
        char c;
        while ((c = getchar()) != '\n') {
            if (c == 27) { // ESC key
                restoreCanonicalMode(&old_attr);
                printf("\nReturning to menu...\n");
                return;
            }
            if (index < sizeof(session_name) - 1) {
                session_name[index++] = c;
                putchar(c); // Echo back the character
            }
        }
        session_name[index] = '\0';
        restoreCanonicalMode(&old_attr);
        printf("\n");
    }

    char command[300];
    if (strlen(session_name) == 0) {
        snprintf(command, sizeof(command), "tmux new-session -d");
    } else {
        snprintf(command, sizeof(command), "tmux new-session -d -s '%s'", session_name);
    }

    if (system(command) != 0) {
        fprintf(stderr, "Failed to create session\n");
        return;
    }

    printf("Created new session: %s\n", session_name[0] ? session_name : "[Unnamed]");

    // Retrieve the session ID of the last created session
    FILE *fp = popen("tmux display-message -p \"#{session_id}\"", "r");
    if (fp) {
        char session_id[100];
        if (fgets(session_id, sizeof(session_id), fp)) {
            session_id[strcspn(session_id, "\n")] = '\0';  // Remove newline
            pclose(fp);
            attachToSession(session_id);
            return;
        }
        pclose(fp);
    }

    // Fallback: If session ID retrieval failed, try attaching the best way possible
    fprintf(stderr, "Warning: Failed to determine session ID. Attaching to last session.\n");
    attachToSession(session_name[0] ? session_name : ".");
}



void displayMenu(struct tmux_session_list *session_list, int current_selection, const char *filter) {
    system("clear");
    printHeader();
    printf("Select a tmux session (ESC to quit):\n");

    // Highlight the current selection
    for (int i = 0; i < session_list->count + 1; i++) {
        if (i == current_selection) {
            printf("\033[1;32m-> ");
        } else {
            printf("   ");
        }
        if (i == 0) {
            printf("Create a new session\n");
        } else {
            printf("%s\n", session_list->sessions[i - 1].label); // Show the full session details
        }

        if (i == current_selection) {
            printf("\033[0m"); // Reset text formatting
        }
    }

    if (filter) {
        printf("\nAttach to session: %s\n", filter);
    }
}

// Main interactive menu
void interactiveMenu(struct tmux_session_list *session_list, int ask_name) {
    int current_selection = 0;
    struct termios old_attr;
    disableCanonicalMode(&old_attr);

    char filter[256] = {0};
    int filter_len = 0;

    while (1) {
        displayMenu(session_list, current_selection, filter_len > 0 ? filter : NULL);

        char input = getchar();
        if (input == '\033') { // Arrow key prefix
            getchar(); // Skip the '['
            switch (getchar()) {
                case 'A': // Up arrow
                    if (current_selection > 0) current_selection--;
                    break;
                case 'B': // Down arrow
                    if (current_selection < session_list->count) current_selection++;
                    break;
            }
        } else if(input == 'j') { 
            if (current_selection > 0) current_selection--;
        } else if(input == 'k') { 
            if (current_selection < session_list->count) current_selection++;
        } else if (input == '\n') { // Enter key
            restoreCanonicalMode(&old_attr);
            if (current_selection == 0) {
                createNewSession(ask_name);
            } else {
                attachToSession(session_list->sessions[current_selection - 1].shortcut);
            }
            return;
        } else if (input == 27) {
            restoreCanonicalMode(&old_attr);
            printf("\nExiting...\n");
            return;
        } else {
            if (filter_len < sizeof(filter) - 1) {
                filter[filter_len++] = input;
                filter[filter_len] = '\0';

                for (int i = 0; i < session_list->count; i++) {
                    if (strncmp(filter, session_list->sessions[i].shortcut, filter_len) == 0) {
                        if (strlen(session_list->sessions[i].shortcut) == filter_len) {
                            restoreCanonicalMode(&old_attr);
                            attachToSession(session_list->sessions[i].shortcut);
                            return;
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int ask_name = 1; // Default to asking for session names

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-ask-session-name") == 0) {
            ask_name = 0;
        }
    }

    // Get the session list
    struct tmux_session_list *session_list = getTmuxSessionList();
    if (!session_list || session_list->count == 0) {
        printf("No existing sessions. Creating a new one...\n");
        createNewSession(ask_name);
    } else {
        interactiveMenu(session_list, ask_name);
    }

    // Free the session list
    if (session_list) {
        free_session_list(session_list);
    }

    return 0;
}
