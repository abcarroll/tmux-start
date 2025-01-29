# tmux-start

`tmux-start` is a terminal-based utility that enhances working with [tmux](https://github.com/tmux/tmux) sessions. It provides an interactive menu to list, navigate, and attach to tmux sessions, or create new ones, with a user-friendly interface.

It's really not much of a utility, but it works well with my current workflow.  I currently use `tmux-start` as the default application for `guake`, so when I create a new `guake` tab, it will launch `tmux-start`.  This way, I can use `tmux` for the vast majority of features, but still have `guake` tabs and visibility.

If you've never tried it, just a shout-out to a great project: Give [guake](https://github.com/Guake/guake) a try if you've never tried it.  I suggest changing the toggle hotkey to something such as `Alt+\``.  

---

## Features

- Displays all existing tmux sessions with detailed information.
- Navigate using **arrow keys**, **Vim-style keys (`j/k`)**, or type the session number for quick selection.
- Create new sessions with optional session name prompts.
- Compatible with `tmux` as a dependency.

---

## Installation

### Prerequisites

- Ensure you have [tmux](https://github.com/tmux/tmux) installed on your system, and if you are planning on building it, you will need `gcc` or `clang`, and `cmake`.
  - Arch Linux: `sudo pacman -S tmux base-devel cmake`
  - Debian/Ubuntu: `sudo apt install tmux build-essential cmake`

### From Source

1. Clone the repository:
   ```bash
   git clone https://github.com/abcarroll/tmux-start.git
   cd tmux-start
   
   cmake .
   make
   sudo make install  
   ```
   
2. This installs the `tmux-start` binary to `/usr/local/bin`.
   

---

## Usage

Run `tmux-start` from your terminal:

```bash
tmux-start
```

### Command-Line Options

- `--no-ask-session-name`: Disables the prompt for entering a session name when creating a new tmux session. If no name is provided, the session defaults to `default_session`.
- `--no-ask-session-name-when-first`: Disables the prompt for entering a session name, but only if it's the _first_ session.  That is, this allows you to use `tmux-start`, and if no existing sessions are found, then it will act as if you just launched `tmux` normally.  If existing sessions exist, however, normal behavior is used, and you will be prompted for an attachment, and for a session name if new.
- `--new-session-last`: Moves the new session option to the last option, below attachable sessions.

## Packaging Support

This project includes scaffolding to package it for various Linux distributions. Ensure `tmux` is listed as a dependency for all package managers.  The `packages/` directory contains packaging files for Arch Linux (PKGBUILD), Debian-based distributions (deb), and RHEL-based distributions (rpm).  

These, however, don't necessarily work -- yet.

## Bugs 

 - If the session ID has more than two digits, there is no way to access it via numeric shortcut.  It will always assume one digit is being typed.  To fix this, I would like to check for this condition and if it occurs, then require the enter key be pressed after entering the session number.

## License

This project is dual-licensed under the **MIT License** and **BSD License**.

```
(C) Copyright 2025 A.B. Carroll III <ben@hl9.net>
```

You are free to use, modify, and distribute this software under the terms of either license.