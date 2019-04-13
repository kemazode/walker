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
#include <memory>
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

    window_push(BUILD_MAIN);

    while(window_top())
        window_hook();

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
        window_push(BUILD_ERROR, "HOME variable is not set.");
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


void scenario_init(arg_t arg)
{
    char* scene_load = reinterpret_cast<char *>(arg);

    auto location = window_get_location(build[BUILD_GAME].position);

    try {        
        scenario.reset(new Scenario(scene_load, location.lines, location.cols));

    } catch(const game_error &error)  {
        window_push(BUILD_ERROR, error.what());
        return;
    }
    
    window_push(BUILD_GAME);
    scenario->render();
}

void scenario_move_player_x(arg_t arg)
{ scenario->move_player(int(arg), 0); }

void scenario_move_player_y(arg_t arg)
{ scenario->move_player(0, int(arg)); }

void scenario_move_view_x(arg_t arg)
{ scenario->move_view(int(arg), 0); }

void scenario_move_view_y(arg_t arg)
{ scenario->move_view(0, int(arg)); }

void scenario_generate(arg_t arg)
{
    string fil;

    /* Remove C_SIZES */
    window_pop();

    try {

        /* Square-shaped map */
        fil = Map::generate(int(arg), int(arg));

    } catch (const game_error& error) {
      window_push(BUILD_ERROR, error.what());
      return;
    }
    
    window_push(BUILD_OKAY, "Map was successfully generated to " + fil);
}

void scenario_load()
{
    const char * home = std::getenv("HOME");

    if (home == nullptr) {
        window_push(BUILD_ERROR, "HOME variable is not set.");
        return;
    }

    string dir = home;
    dir = dir + '/' + F_SCENARIOS;

    vector<unique_ptr<char[]>> paths;
    vector<unique_ptr<char[]>> names;

    DIR *dp;
    struct dirent *dirp;

    if((dp  = opendir(dir.c_str())) == nullptr) {
        window_push(BUILD_ERROR, string(strerror(errno)) + " \"" + dir + "\".");
        return;
    }
    while  ( (dirp = readdir(dp)) != nullptr )
        if (dirp->d_type & DT_REG) {
            //files.emplace_back(dir + dirp->d_name);
            string path = dir + dirp->d_name;
            paths.emplace_back (new char[path.size() + 1]);
            strcpy(paths.back().get(), path.c_str());

            names.emplace_back (new char[strlen(dirp->d_name) + 1]);
            strcpy(names.back().get(), dirp->d_name);
          }

    closedir(dp);

    if (paths.empty()) {
        window_push(BUILD_ERROR, "No files in \"" + dir + "\".");
        return;
      }

    shared_ptr<item[]> load_menu(new item[paths.size() + 1]);


    for (size_t i = 0; i < paths.size(); ++i)
        load_menu.get()[i] =
        {
          names[i].get(),
          { fun_t(scenario_init), arg_t(paths[i].get()) }
        };
    load_menu.get()[paths.size()] = {};

    window *wptr = window_push(builder(POSITION_SMALL, load_menu.get(), hooks[HOOKS_MENU],
                                       "Select the scenario:",
                                       "Scenarios"));

    /* Exit when a choice window a scenario will close
     * and may be free of memory */
    while(window_has(wptr))
        window_hook();
}
