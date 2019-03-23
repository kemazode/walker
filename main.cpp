#include <boost/filesystem.hpp>
#include <signal.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include "window.hpp"
#include "scene.hpp"
#include "utils.hpp"

enum { HOOKS_MENU_MAIN = 1, HOOKS_GAME, HOOKS_MENU };
enum { MENU_MAIN  = 1, MENU_GAME, MENU_OK, MENU_CREATE, MENU_MAP_SIZES };
enum { TEXT_MENU  = 1, TEXT_CREATE, TEXT_MAP_SIZES, TEXT_MAP_CHOOSE };
enum { C_MAIN, C_GAME, C_GAME_MENU, C_OK, C_CREATE, C_MAP_SIZES, C_END};

#define POS_FULL 0, 0, LINES, COLS
#define POS_AVER COLS/8, LINES/8, LINES - LINES/4, COLS - COLS/4
#define POS_SMALL COLS/4, LINES/4, LINES/2, COLS/2
#define POS_BOTTOM COLS/8 , LINES - LINES/4 + 2, LINES/4 - 2, COLS - COLS/4

#define CONSTRUCT(pos, me, ho, te) \
    Window::Constructor(pos, menus[me].m, menus[me].s, hooks[ho].h, hooks[ho].s, texts[te])

static Window::Constructor constructors[C_END];

// At any given time, we employ only one scenario
static Scenario sc;

// errno
static Text error;

inline void init_constructors();

// Functions for working with the scenario
inline void sc_start(const Args args);
inline void sc_movepx(const Args args);
inline void sc_movepy(const Args args);
inline void sc_movevx(const Args args);
inline void sc_movevy(const Args args);
inline void sc_gen(const Args args);
inline void sc_load();

inline void set_window(const Args args);

static Menu m_main[] =
{
    {"Start New Game", Action(sc_load)},
    {"Create Map",     Action(Fa(Window::push), Args(&constructors[C_CREATE]))},
    {"Scoreboard",     Action()},
    {"Options",        Action()},
    {"Exit",           Action(Window::clear)},
};

static Menu m_game[] =
{
    {"Continue",       Action(Window::pop)},
    {"Exit",           Action(set_window, Args(&constructors[C_MAIN]))},
};

static Menu m_ok[] =
{
    {"OK",       Action(Window::pop)},
};

static Menu m_create[] =
{
    {"Generate",       Action(Fa(Window::push), Args(&constructors[C_MAP_SIZES]))},
};


static Menu m_map_sizes[] =
{
    {"100x100", Action(Fa(sc_gen), Args(100))},
    {"250x250", Action(Fa(sc_gen), Args(250))},
    {"500x500", Action(Fa(sc_gen), Args(500))},
};

static Hook h_game[] =
{
    {'Q',       Action(Fa(Window::apush), Args(&constructors[C_GAME_MENU]))},
    {'q',       Action(Fa(Window::apush), Args(&constructors[C_GAME_MENU]))},

    // Player moving
    {KEY_DOWN,  Action(Fa(sc_movepy), Args(1))},
    {KEY_UP,    Action(Fa(sc_movepy), Args(-1))},
    {KEY_LEFT,  Action(Fa(sc_movepx), Args(-1))},
    {KEY_RIGHT, Action(Fa(sc_movepx), Args(1))},

    // Map moving
    {'k', Action(Fa(sc_movevy), Args(1))},
    {'i', Action(Fa(sc_movevy), Args(-1))},
    {'j', Action(Fa(sc_movevx), Args(-1))},
    {'l', Action(Fa(sc_movevx), Args(1))},
    {'K', Action(Fa(sc_movevy), Args(1))},
    {'I', Action(Fa(sc_movevy), Args(-1))},
    {'J', Action(Fa(sc_movevx), Args(-1))},
    {'L', Action(Fa(sc_movevx), Args(1))},
};

static Hook h_main_menu[] =
{
    {KEY_DOWN, Action(Fa(Window::amenu_driver), Args(REQ_DOWN_ITEM))},
    {KEY_UP,   Action(Fa(Window::amenu_driver), Args(REQ_UP_ITEM))},
    {'\n',     Action(Fa(Window::amenu_driver), Args(REQ_EXEC_ITEM))}
};

static Hook h_menu[] =
{
    {'q',      Action(Window::pop)},
    {'Q',      Action(Window::pop)},
    {KEY_DOWN, Action(Fa(Window::amenu_driver), Args(REQ_DOWN_ITEM))},
    {KEY_UP,   Action(Fa(Window::amenu_driver), Args(REQ_UP_ITEM))},
    {'\n',     Action(Fa(Window::amenu_driver), Args(REQ_EXEC_ITEM))}
};

static struct {
    Hook *h;
    size_t s;
} hooks[] = {
    {nullptr, 0},
    {h_main_menu, SIZE(h_main_menu)}, // HOOKS_MENU
    {h_game, SIZE(h_game)}, // HOOKS_GAME
    {h_menu, SIZE(h_menu)}, // HOOKS_GAME_MENU
};

static struct {
    Menu *m;
    size_t s;
} menus[] = {
    {nullptr, 0},
    {m_main, SIZE(m_main)},    // MENU_MAIN
    {m_game, SIZE(m_game)},    // MENU_GAME
    {m_ok, SIZE(m_ok)},  // MENU_OK
    {m_create, SIZE(m_create)}, // MENU_CREATE
    {m_map_sizes, SIZE(m_map_sizes)}, // MENU_MAP_SIZES
};

static Text texts [] = {
    Text(""),
    Text("Welcome!", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),
    Text("Create your own map.", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),
    Text("Choose map size:", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK)),
    Text("Choose scenario:", A_BOLD|PAIR(MYCOLOR, COLOR_BLACK))
};

extern int LINES;
extern int COLS;

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
    init_constructors();

    for (short i = 1; i <= 64; ++i)
        init_pair(i, (i - 1)%8, (i - 1)/8);

    Window::push(constructors[C_MAIN]);

    while(Window::top())
        Window::exechook(getch());

    Window::clear();
    endwin();
}

void init_constructors() {
    constructors[C_MAIN]      = CONSTRUCT(POS_FULL,   MENU_MAIN,      HOOKS_MENU_MAIN, TEXT_MENU);
    constructors[C_GAME]      = CONSTRUCT(POS_AVER,   0,              HOOKS_GAME,       0);
    constructors[C_GAME_MENU] = CONSTRUCT(POS_SMALL, MENU_GAME,      HOOKS_MENU,       0);
    constructors[C_OK]        = CONSTRUCT(POS_SMALL, MENU_OK,        HOOKS_MENU,       0);
    constructors[C_CREATE]    = CONSTRUCT(POS_FULL,   MENU_CREATE,    HOOKS_MENU,      TEXT_CREATE);
    constructors[C_MAP_SIZES] = CONSTRUCT(POS_SMALL, MENU_MAP_SIZES, HOOKS_MENU,      TEXT_MAP_SIZES);
}

void set_window(const Args args)
{
    Window::clear();
    Window::apush(args);
}

void sc_start(const Args args)
{
    const char *scen_load = (const char *) args.ptr;

    sc.clear();

    if (!sc.load(scen_load, error)) {
        Window::push(constructors[C_OK]);
        Window::print(error);
        return;
    }

    set_window(Args(&constructors[C_GAME]));
    sc.set_display(Window::getfreelines(), Window::getfreecols());
    Window::print(sc.get_render_map(), sc.getx(), sc.gety(), int(sc.size()));
}

void sc_movepx(const Args args)
{
    sc.move_player(args.num, 0);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety(), int(sc.size()));
}

void sc_movepy(const Args args)
{
    sc.move_player(0, args.num);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety(), int(sc.size()));
}

void sc_movevx(const Args args)
{
    sc.move_view(args.num, 0);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety(), int(sc.size()));
}

void sc_movevy(const Args args)
{
    sc.move_view(0, args.num);
    Window::print(sc.get_render_map(), sc.getx(), sc.gety(), int(sc.size()));
}

void sc_gen(const Args args)
{
    String fil;
    bool success = sc.gen(size_t(args.num), error, fil);

    Window::pop(); // Remove C_MAP_SIZES
    Window::push(constructors[C_OK]);

    if (success)
        Window::print("Map was successfully generated to " + fil);
    else
        Window::print(error);
}

namespace bf = boost::filesystem;

inline void sc_load()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        Window::push(constructors[C_OK]);
        Window::print("HOME environment variable not set.");
        return;
    }

    bf::path pwd(bf::operator/(home, F_SCENARIOS));

    // Count the number of regular files
    long cnt = 0;

    if (!bf::exists(pwd)) {
        bf::create_directory(pwd);
    } else {
        cnt = std::count_if(
                    bf::directory_iterator(pwd),
                    bf::directory_iterator(),
                    static_cast<bool(*)(const bf::path&)>(bf::is_regular_file));
    }

    if (cnt == 0) {
        Window::push(constructors[C_OK]);
        Window::print("No files in \"" + pwd.string() + "\".");
        return;
    }

    Menu* load_menu = new Menu[size_t(cnt)];
    String* files = new String[size_t(cnt)];

    // Fill an array of files with files from the scenario directory
    int i = 0;
    for (auto &p : bf::directory_iterator(pwd)) {
        files[i] = p.path().string();

        load_menu[i] = Menu(p.path().filename().string(), Action(Fa(sc_start), files[i].c_str()));
        ++i;
    }

    Window::Constructor cload(POS_SMALL, load_menu, size_t(cnt), hooks[HOOKS_MENU].h,
                             hooks[HOOKS_MENU].s, texts[TEXT_MAP_CHOOSE]);

    auto wptr = Window::push(cload);

    // Exit when a choice window a scenario will close
    // and may be free of memory
    while(Window::has(wptr))
        Window::exechook(getch());

    delete [] files;
    delete [] load_menu;
}
