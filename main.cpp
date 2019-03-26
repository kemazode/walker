#include <csignal>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <dirent.h>
#include <cerrno>
#include "window.hpp"
#include "scene.hpp"
#include "utils.hpp"

//namespace bf = boost::filesystem;

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

// At any given time, we employ only one scenario
static Scenario sc;
static Text error;

// Functions for working with the scenario
static inline void sc_mvpx(Args args);
static inline void sc_mvpy(Args args);
static inline void sc_mvvx(Args args);
static inline void sc_mvvy(Args args);
static void sc_start(Args args);
static void sc_gen(Args args);
static void sc_load();

static inline void w_push(Args args);
static inline void w_set(Args args);
static inline void w_menu_driver(Args args);

static vector<vector<W::Menu>> menus =
{
    // MENU_NULL
    {},

    { // MENU MAIN
      W::Menu("Start New Game", Action(sc_load)),
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
      W::Menu("100x100", Action(Fa(sc_gen), Args(100))),
      W::Menu("250x250", Action(Fa(sc_gen), Args(250))),
      W::Menu("500x500", Action(Fa(sc_gen), Args(500))),
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
      W::Hook(KEY_DOWN,  Action(Fa(sc_mvpy), Args(1))),
      W::Hook(KEY_UP,    Action(Fa(sc_mvpy), Args(-1))),
      W::Hook(KEY_LEFT,  Action(Fa(sc_mvpx), Args(-1))),
      W::Hook(KEY_RIGHT, Action(Fa(sc_mvpx), Args(1))),

      // Map moving
      W::Hook('k', Action(Fa(sc_mvvy), Args(1))),
      W::Hook('i', Action(Fa(sc_mvvy), Args(-1))),
      W::Hook('j', Action(Fa(sc_mvvx), Args(-1))),
      W::Hook('l', Action(Fa(sc_mvvx), Args(1))),
      W::Hook('K', Action(Fa(sc_mvvy), Args(1))),
      W::Hook('I', Action(Fa(sc_mvvy), Args(-1))),
      W::Hook('J', Action(Fa(sc_mvvx), Args(-1))),
      W::Hook('L', Action(Fa(sc_mvvx), Args(1))),
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

static void _sig_winch(const int signo)
{
    (void) signo;
    struct winsize size {};
    ioctl(fileno(stdout), TIOCGWINSZ, reinterpret_cast<char *>(&size));
    resizeterm(size.ws_row, size.ws_col);
}

int main()
{    
    initscr();
    signal(SIGWINCH, _sig_winch);
    curs_set(FALSE);
    keypad(stdscr, true);
    noecho();
    start_color();

    for (short i = 1; i <= 64; ++i)
        init_pair(i, (i - 1)%8, (i - 1)/8);

    W::push(ctrs[C_MAIN]);

    while(W::top())
        W::exechook(getch());

    W::clear();
    endwin();
}

void w_set(Args args)
{ W::set(ctrs[size_t(args.num)]); }

void w_push(Args args)
{ W::push(ctrs[size_t(args.num)]); }

void w_menu_driver(Args args)
{ W::menu_driver(args.num); }

void sc_start(Args args)
{
    const char *scen_load = reinterpret_cast<const char *>(args.ptr);

    sc.clear();

    if (!sc.load(scen_load, error)) {
        W::push(ctrs[C_OK]);
        W::print(error);
        return;
    }

    W::set(ctrs[C_GAME]);
    sc.set_display(W::getfreelines(), W::getfreecols());
    W::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvpx(Args args)
{
    sc.move_player(args.num, 0);
    W::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvpy(Args args)
{
    sc.move_player(0, args.num);
    W::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvvx(Args args)
{
    sc.move_view(args.num, 0);
    W::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvvy(Args args)
{
    sc.move_view(0, args.num);
    W::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_gen(Args args)
{
    String fil;
    bool success = sc.gen(size_t(args.num), error, fil);

    W::pop(); // Remove C_SIZES
    W::push(ctrs[C_OK]);

    if (success)
        W::print("Map was successfully generated to " + fil);
    else
        W::print(error);
}

void sc_load()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        W::push(ctrs[C_OK]);
        W::print("HOME environment variable not set.");
        return;
    }

    String dir = home;
    dir = dir + '/' + F_SCENARIOS;

    vector<String> files;

    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(dir.c_str())) == nullptr) {
        W::push(ctrs[C_OK]);
        W::print("Error(" + String(strerror(errno)) + ") opening " + dir);
        return;
    }
    while  ( (dirp = readdir(dp)) != nullptr )
        if (dirp->d_type & DT_REG)
            files.emplace_back(dir + dirp->d_name);

    closedir(dp);

    if (files.empty()) {
        W::push(ctrs[C_OK]);
        W::print("No files in \"" + dir + "\".");
        return;
    }

    vector<W::Menu> load_menu;
    load_menu.reserve(files.size());

    for (auto &f : files)
        load_menu.emplace_back(f.substr(f.rfind('/') + 1), Action(sc_start, f.c_str()));

    auto wptr = W::push(Ctrs(W::Pos::POS_SMLL, load_menu, hooks[HOOKS_MENU], texts[TEXT_MAPS]));

    // Exit when a choice window a scenario will close
    // and may be free of memory
    while(W::has(wptr))
        W::exechook(getch());
}
