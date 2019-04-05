#ifndef UI_HPP
#define UI_HPP

#include "utils.hpp"
#include "window.hpp"

using W = Window;
using Ctrs = W::Constructor;

enum
{
    MENU_NULL,
    MENU_MAIN,
    MENU_GAME,
    MENU_OKAY,
    MENU_CRTE,
    MENU_SIZS
};

enum
{
    HOOKS_NULL,
    HOOKS_MAIN,
    HOOKS_GAME,
    HOOKS_MENU
};

enum
{
    TEXT_NULL,
    TEXT_MENU,
    TEXT_CRTE,
    TEXT_SIZS,
    TEXT_MAPS
};

enum
{
    C_MAIN,
    C_GAME,
    C_GAME_MENU,
    C_OK,
    C_CREATE,
    C_SIZES,
    C_END,
};

// Functions for working with the scenario
static inline void scenario_move_player_x(Args args);
static inline void scenario_move_player_y(Args args);
static inline void scenario_move_view_x(Args args);
static inline void scenario_move_view_y(Args args);
static        void scenario_init(Args args);
static        void scenario_generate(Args args);
static        void scenario_load();

static inline void w_push(Args args);
static inline void w_set(Args args);
static inline void w_menu_driver(Args args);

static vector<vector<W::Menu>> menus =
{
    // MENU_NULL
    {},

    { // MENU MAIN
      W::Menu("Start New Game", Action(scenario_load)),
      W::Menu("Create Map",     Action(w_push, Args(C_CREATE))),
      W::Menu("Scoreboard",     Action()),
      W::Menu("Options",        Action()),
      W::Menu("Exit",           Action(W::clear)),
    },

    { // MENU_GAME
      W::Menu("Continue",       Action(W::pop)),
      W::Menu("Exit",           Action(w_set, Args(C_MAIN))),
    },

    { // MENU_OKAY
      W::Menu("OK",       Action(W::pop)),
    },

    { // MENU_CRTE
      W::Menu("Generate",       Action(w_push, Args(C_SIZES))),
    },

    { // MENU_SIZS
      W::Menu("100x100", Action(Fa(scenario_generate), Args(100))),
      W::Menu("250x250", Action(Fa(scenario_generate), Args(250))),
      W::Menu("500x500", Action(Fa(scenario_generate), Args(500))),
    },
};

static vector<vector<W::Hook>> hooks =
{
    // HOOKS_NULL
    {},

    { // HOOKS_MAIN
      W::Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      W::Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      W::Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
    },

    { // HOOKS_GAME
      W::Hook('Q',       Action(w_push, Args(C_GAME_MENU))),
      W::Hook('q',       Action(w_push, Args(C_GAME_MENU))),

      // Player moving
      W::Hook(KEY_DOWN,  Action(Fa(scenario_move_player_y), Args(1))),
      W::Hook(KEY_UP,    Action(Fa(scenario_move_player_y), Args(-1))),
      W::Hook(KEY_LEFT,  Action(Fa(scenario_move_player_x), Args(-1))),
      W::Hook(KEY_RIGHT, Action(Fa(scenario_move_player_x), Args(1))),

      // Map moving
      W::Hook('k', Action(Fa(scenario_move_view_y), Args(1))),
      W::Hook('i', Action(Fa(scenario_move_view_y), Args(-1))),
      W::Hook('j', Action(Fa(scenario_move_view_x), Args(-1))),
      W::Hook('l', Action(Fa(scenario_move_view_x), Args(1))),
      W::Hook('K', Action(Fa(scenario_move_view_y), Args(1))),
      W::Hook('I', Action(Fa(scenario_move_view_y), Args(-1))),
      W::Hook('J', Action(Fa(scenario_move_view_x), Args(-1))),
      W::Hook('L', Action(Fa(scenario_move_view_x), Args(1))),
    },

    { // HOOKS_MENU
      W::Hook('q',      Action(W::pop)),
      W::Hook('Q',      Action(W::pop)),
      W::Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      W::Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      W::Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
    },
};

static vector<Text> texts =
{
    // TEXT_NULL
    Text(),

    // TEXT_MENU
    Text("Welcome!", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),

    // TEXT_CRTE
    Text("Create your own map.", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),

    // TEXT_SIZS
    Text("Choose map size:", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),

    // TEXT_MAP
    Text("Choose scenario:", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK))
};

static vector<W::Constructor> ctrs =
{
    Ctrs(W::Pos::POS_FULL, menus[MENU_MAIN], hooks[HOOKS_MAIN], texts[TEXT_MENU]),
    Ctrs(W::Pos::POS_AVER, menus[MENU_NULL], hooks[HOOKS_GAME], texts[TEXT_NULL]),
    Ctrs(W::Pos::POS_SMLL, menus[MENU_GAME], hooks[HOOKS_MENU], texts[TEXT_NULL]),
    Ctrs(W::Pos::POS_SMLL, menus[MENU_OKAY], hooks[HOOKS_MENU], texts[TEXT_NULL]),
    Ctrs(W::Pos::POS_FULL, menus[MENU_CRTE], hooks[HOOKS_MENU], texts[TEXT_CRTE]),
    Ctrs(W::Pos::POS_SMLL, menus[MENU_SIZS], hooks[HOOKS_MENU], texts[TEXT_SIZS]),
};

void w_set(Args args)
{ W::set(ctrs[size_t(args.num)]); }

void w_push(Args args)
{ W::push(ctrs[size_t(args.num)]); }

void w_menu_driver(Args args)
{ W::menu_driver(args.num); }

#endif // UI_HPP
