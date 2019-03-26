#include <boost/filesystem.hpp>
#include <csignal>
#include <sys/ioctl.h>
#include "window.hpp"
#include "scene.hpp"
#include "utils.hpp"

using Menu = Window::Menu;
using Hook = Window::Hook;

namespace bf = boost::filesystem;

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

using Ctrs = Window::Constructor;
using Pos = Window::Pos;

// At any given time, we employ only one scenario
static Scenario sc;
static Text error;

// Functions for working with the scenario
static inline void sc_mvpx(const Args args);
static inline void sc_mvpy(const Args args);
static inline void sc_mvvx(const Args args);
static inline void sc_mvvy(const Args args);
static void sc_start(const Args args);
static void sc_gen(const Args args);
static void sc_load();

static inline void w_push(const Args args);
static inline void w_set(const Args args);
static inline void w_menu_driver(const Args args);

static vector<vector<Menu>> menus =
{
    // MENU_NULL
    {},

    { // MENU MAIN
      Menu("Start New Game", Action(sc_load)),
      Menu("Create Map",     Action(w_push, Args(C_CREATE))),
      Menu("Scoreboard",     Action()),
      Menu("Options",        Action()),
      Menu("Exit",           Action(Window::clear)),
    },

    { // MENU_GAME
      Menu("Continue",       Action(Window::pop)),
      Menu("Exit",           Action(w_set, Args(C_MAIN))),
    },

    { // MENU_OKAY
      Menu("OK",       Action(Window::pop)),
    },

    { // MENU_CRTE
      Menu("Generate",       Action(w_push, Args(C_SIZES))),
    },

    { // MENU_SIZS
      Menu("100x100", Action(Fa(sc_gen), Args(100))),
      Menu("250x250", Action(Fa(sc_gen), Args(250))),
      Menu("500x500", Action(Fa(sc_gen), Args(500))),
    },
};

static vector<vector<Hook>> hooks =
{
    // HOOKS_NULL
    {},

    { // HOOKS_MAIN
      Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
    },

    { // HOOKS_GAME
      Hook('Q',       Action(w_push, Args(C_GAME_MENU))),
      Hook('q',       Action(w_push, Args(C_GAME_MENU))),

      // Player moving
      Hook(KEY_DOWN,  Action(Fa(sc_mvpy), Args(1))),
      Hook(KEY_UP,    Action(Fa(sc_mvpy), Args(-1))),
      Hook(KEY_LEFT,  Action(Fa(sc_mvpx), Args(-1))),
      Hook(KEY_RIGHT, Action(Fa(sc_mvpx), Args(1))),

      // Map moving
      Hook('k', Action(Fa(sc_mvvy), Args(1))),
      Hook('i', Action(Fa(sc_mvvy), Args(-1))),
      Hook('j', Action(Fa(sc_mvvx), Args(-1))),
      Hook('l', Action(Fa(sc_mvvx), Args(1))),
      Hook('K', Action(Fa(sc_mvvy), Args(1))),
      Hook('I', Action(Fa(sc_mvvy), Args(-1))),
      Hook('J', Action(Fa(sc_mvvx), Args(-1))),
      Hook('L', Action(Fa(sc_mvvx), Args(1))),
    },

    { // HOOKS_MENU
      Hook('q',      Action(Window::pop)),
      Hook('Q',      Action(Window::pop)),
      Hook(KEY_DOWN, Action(w_menu_driver, Args(REQ_DOWN_ITEM))),
      Hook(KEY_UP,   Action(w_menu_driver, Args(REQ_UP_ITEM))),
      Hook('\n',     Action(w_menu_driver, Args(REQ_EXEC_ITEM)))
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

static vector<Window::Constructor> ctrs =
{
    Ctrs(Pos::POS_FULL, menus[MENU_MAIN], hooks[HOOKS_MAIN], texts[TEXT_MENU]),
    Ctrs(Pos::POS_AVER, menus[MENU_NULL], hooks[HOOKS_GAME], texts[TEXT_NULL]),
    Ctrs(Pos::POS_SMLL, menus[MENU_GAME], hooks[HOOKS_MENU], texts[TEXT_NULL]),
    Ctrs(Pos::POS_SMLL, menus[MENU_OKAY], hooks[HOOKS_MENU], texts[TEXT_NULL]),
    Ctrs(Pos::POS_FULL, menus[MENU_CRTE], hooks[HOOKS_MENU], texts[TEXT_CRTE]),
    Ctrs(Pos::POS_SMLL, menus[MENU_SIZS], hooks[HOOKS_MENU], texts[TEXT_SIZS]),
};

static void _sig_winch(const int signo)
{
    (void) signo;
    struct winsize size;
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

    Window::push(ctrs[C_MAIN]);

    while(Window::top())
        Window::exechook(getch());

    Window::clear();
    endwin();
}

void w_set(const Args args)
{ Window::set(ctrs[size_t(args.num)]); }

void w_push(const Args args)
{ Window::push(ctrs[size_t(args.num)]); }

void w_menu_driver(const Args args)
{ Window::menu_driver(args.num); }

void sc_start(const Args args)
{
    const char *scen_load = reinterpret_cast<const char *>(args.ptr);

    sc.clear();

    if (!sc.load(scen_load, error)) {
        Window::push(ctrs[C_OK]);
        Window::print(error);
        return;
    }

    Window::set(ctrs[C_GAME]);
    sc.set_display(Window::getfreelines(), Window::getfreecols());
    Window::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvpx(const Args args)
{
    sc.move_player(args.num, 0);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvpy(const Args args)
{
    sc.move_player(0, args.num);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvvx(const Args args)
{
    sc.move_view(args.num, 0);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_mvvy(const Args args)
{
    sc.move_view(0, args.num);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety());
}

void sc_gen(const Args args)
{
    String fil;
    bool success = sc.gen(size_t(args.num), error, fil);

    Window::pop(); // Remove C_SIZES
    Window::push(ctrs[C_OK]);

    if (success)
        Window::print("Map was successfully generated to " + fil);
    else
        Window::print(error);
}

void sc_load()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        Window::push(ctrs[C_OK]);
        Window::print("HOME environment variable not set.");
        return;
    }

    bf::path pwd(bf::operator/(home, F_SCENARIOS));

    // Count the number of regular files
    ulong cnt = 0;

    if (!bf::exists(pwd)) {
        bf::create_directory(pwd);
    } else {
        cnt = ulong(std::count_if(
                    bf::directory_iterator(pwd),
                    bf::directory_iterator(),
                    static_cast<bool(*)(const bf::path&)>(bf::is_regular_file)));
    }

    if (cnt == 0) {
        Window::push(ctrs[C_OK]);
        Window::print("No files in \"" + pwd.string() + "\".");
        return;
    }

    vector<Menu> load_menu;
    load_menu.reserve(cnt);

    vector<String> files;
    files.reserve(cnt);

    // Fill an array of files with files from the scenario directory
    for (auto &p : bf::directory_iterator(pwd)) {
        files.emplace_back(p.path().string());
        load_menu.emplace_back(p.path().filename().string(), Action(Fa(sc_start), files.back().c_str()));
    }

    auto wptr = Window::push(Window::Constructor(Pos::POS_SMLL, load_menu, hooks[HOOKS_MENU], texts[TEXT_MAPS]));

    // Exit when a choice window a scenario will close
    // and may be free of memory
    while(Window::has(wptr))
        Window::exechook(getch());


}
