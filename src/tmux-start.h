/*
 * tmux-start - Interactive tmux session manager
 *
 * (C) Copyright 2025 A.B. Carroll III <ben@hl9.net>
 *
 * Licensed under the MIT License or the BSD 2-Clause License, at your option.
 *
 * See the LICENSE file for details.
 */

#ifndef TMUX_START_H
#define TMUX_START_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// Constants
#define MAX_SESSIONS 100
#define MAX_LINE_LENGTH 512

// Data structures
struct tmux_session {
    char *shortcut;
    char *label;
};

struct tmux_session_list {
    unsigned int count;
    struct tmux_session *sessions;
};

// Function declarations
struct tmux_session_list *init_session_list(unsigned int count);
void free_session_list(struct tmux_session_list *list);
void printHeader(void);
void disableCanonicalMode(struct termios *old_attr);
void restoreCanonicalMode(struct termios *old_attr);
struct tmux_session_list *getTmuxSessionList(void);
void attachToSession(const char *tmux_id);
void createNewSession(int ask_name);
void displayMenu(struct tmux_session_list *session_list, int current_selection);
void interactiveMenu(struct tmux_session_list *session_list, int ask_name);

#endif // TMUX_START_H
