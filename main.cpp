/* This file is part of Walker.
 * 
 * Walker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Walker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Walker.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <csignal>
#include <cerrno>
#include "scene.hpp"
#include "utils.hpp"
#include "ui.hpp"

// At any given time, we employ only one scenario
static std::unique_ptr<Scenario> scenario = nullptr;

static void sig_winch(const int signo);
static void init_dirs();
static void mkdir_parents(const char *dir);

int main()
{    
    initscr();
    signal(SIGWINCH, sig_winch);
    curs_set(FALSE);
    keypad(stdscr, true);
    noecho();
    start_color();

    for (short i = 1; i <= 64; ++i)
        init_pair(i, (i - 1)%8, (i - 1)/8);

    /* Create config directories if they did not exist */
    init_dirs();

    W::push(builder.at(Builder::main));

    while(W::top())
        W::hook();

    W::clear();
    endwin();
}

void sig_winch(const int signo)
{
    (void) signo;
    struct winsize size {};
    ioctl(fileno(stdout), TIOCGWINSZ, reinterpret_cast<char *>(&size));
    resizeterm(size.ws_row, size.ws_col);
}

void init_dirs()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        W::push(builder.at(Builder::error) | "HOME variable is not set.");
        return;
    }

    string scens = home,
           gener = home;

    scens = scens + '/' + F_SCENARIOS;
    gener = gener + '/' + F_GENERATIONS;

    std::ifstream f;

    for (auto &fn : { scens, gener })
    {
        f.open(fn);
        if (f.is_open())
            f.close();
        else mkdir_parents(fn.c_str());
    }

}

void mkdir_parents(const char *dir)
{
    char tmp[PATH_MAX];
    char *p = nullptr;
    size_t len;

    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);

    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for(p = tmp + 1; *p; p++)
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }

    mkdir(tmp, S_IRWXU);
}


void scenario_init(Arg arg)
{
    const string &scene_load = *reinterpret_cast<const string *>(arg);

    auto location = W::getlocation(builder.at(Builder::game).p);

    try {
        scenario.reset(new Scenario(scene_load, location.lines, location.cols));

    } catch(const game_error &er)  {                
        W::push(builder.at(Builder::error) | er.what());
        return;
    }
    
    W::set(builder.at(Builder::game));
    scenario->render();
}

void scenario_move_player_x(Arg arg)
{ scenario->move_player(int(arg), 0); }

void scenario_move_player_y(Arg arg)
{ scenario->move_player(0, int(arg)); }

void scenario_move_view_x(Arg arg)
{ scenario->move_view(int(arg), 0); }

void scenario_move_view_y(Arg arg)
{ scenario->move_view(0, int(arg)); }

void scenario_generate(Arg arg)
{
    string fil;

    /* Remove C_SIZES */
    W::pop();

    try {

        /* Square-shaped map */
        fil = Map::generate(int(arg), int(arg));

    } catch (const game_error& er) {
        W::push(builder.at(Builder::error) | er.what());
        return;
    }
    
    W::push(builder.at(Builder::okay) | "Map was successfully generated to " + fil);
}

void scenario_load()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        W::push(builder.at(Builder::error) | "HOME variable is not set.");
        return;
    }

    string dir = home;
    dir = dir + '/' + F_SCENARIOS;

    vector<string> files;

    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(dir.c_str())) == nullptr) {
        W::push(builder.at(Builder::error) | string(strerror(errno)) + " \"" + dir + "\".");
        return;
    }
    while  ( (dirp = readdir(dp)) != nullptr )
        if (dirp->d_type & DT_REG)
            files.emplace_back(dir + dirp->d_name);

    closedir(dp);

    if (files.empty()) {
        W::push(builder.at(Builder::error) | "No files in \"" + dir + "\".");
        return;
    }

    vector<W::Menu> load_menu;
    load_menu.reserve(files.size());

    for (const auto &f : files)
        load_menu.emplace_back(f.substr(f.rfind('/') + 1), ActionAV(scenario_init, Arg(&f)));

    auto wptr = W::push(W::Builder(W::small, load_menu,
                             hooks.at(Hooks::menu),
                             "Choose scenario:",
                             "Scenaries"));

    /* Exit when a choice window a scenario will close
     * and may be free of memory */
    while(W::has(wptr))
        W::hook();
}
