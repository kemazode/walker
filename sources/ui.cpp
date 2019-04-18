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
#include <memory>
#include <dirent.h>

#include "map.hpp"
#include "ui.hpp"
#include "scene.hpp"
#include "images.hpp"

using std::unique_ptr;

/* Wrappers */
static void map_generate(arg_t);
static void scenario_menu();
static void scenario_init(arg_t);

static item menu_main[] =
{
  item("Start a new scenario", "You can choose a scenario from the list and start its passage.", {fun_t(scenario_menu), 0}),
  item("Create map", "Here you can generate your own ASCII map.", {window_push, BUILD_MAP_CREATOR}),
  item("Controlling", "Keyboard shortcuts.",               {window_push, BUILD_CONTROL}),
  item("Exit","Just quitting the game.",                          {fun_t(window_clear), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_game[] =
{
  item("Continue", "Continue the game.", {fun_t(window_pop), 0}),
  item("Exit", "Back to menu.",          {window_set, BUILD_MAIN}),
  {nullptr, {nullptr , 0}}
};

static item menu_okay[] =
{
  item("OK",           {fun_t(window_pop), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_return[] =
{
  item("Back", "Back to menu.",  {fun_t(window_pop), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_map_creator[] =
{
  item("Generate", "Create a map of ASCII characters.",   {window_push, BUILD_MAP_SIZES}),
  item("Back",     "Back to menu.",                        {fun_t(window_pop), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_map_sizes[] =
{
  item("w: 100, h: 100", {map_generate, 100}),
  item("w: 250, h: 250", {map_generate, 250}),
  item("w: 500, h: 500", {map_generate, 500}),
  {nullptr, {nullptr , 0}}
};

item *menus[] =
{
  menu_main,
  menu_game,
  menu_okay,
  menu_return,
  menu_map_creator,
  menu_map_sizes,
};

static hook hooks_main[] =
{
  hook(KEY_DOWN, {fun_t(window_menu_driver), REQ_DOWN_ITEM}),
  hook(KEY_UP,   {fun_t(window_menu_driver), REQ_UP_ITEM}),
  hook('\n',     {fun_t(window_menu_driver), REQ_EXEC_ITEM}),
  {0, {nullptr, 0}}
};

static hook hooks_game[] =
{
  hook('Q',       {window_push, BUILD_GAME_MENU}),
  hook('q',       {window_push, BUILD_GAME_MENU}),

  // Player moving
  hook(KEY_DOWN,  {scenario_move_player_y,  1}),
  hook(KEY_UP,    {scenario_move_player_y, -1}),
  hook(KEY_LEFT,  {scenario_move_player_x, -1}),
  hook(KEY_RIGHT, {scenario_move_player_x,  1}),

  // map moving
  hook('k', {scenario_move_view_y,  1}),
  hook('i', {scenario_move_view_y, -1}),
  hook('j', {scenario_move_view_x, -1}),
  hook('l', {scenario_move_view_x,  1}),
  hook('K', {scenario_move_view_y,  1}),
  hook('I', {scenario_move_view_y, -1}),
  hook('J', {scenario_move_view_x, -1}),
  hook('L', {scenario_move_view_x,  1}),
  {0, {nullptr, 0}}
};

static hook hooks_menu[] =
{
  hook('q',      {fun_t(window_pop), 0}),
  hook('Q',      {fun_t(window_pop), 0}),
  hook(KEY_DOWN, {fun_t(window_menu_driver), REQ_DOWN_ITEM}),
  hook(KEY_UP,   {fun_t(window_menu_driver), REQ_UP_ITEM}),
  hook('\n',     {fun_t(window_menu_driver), REQ_EXEC_ITEM}),
  {0, {nullptr, 0}}
};

//static hook hooks_event_dialog[] =
//{
//  hook('\n',     {fun_t(window_menu_driver), REQ_EXEC_ITEM}),
//  hook('\n',     {fun_t(window_pop), 0}),
//  hook(KEY_DOWN, {fun_t(window_menu_driver), REQ_DOWN_ITEM}),
//  hook(KEY_UP,   {fun_t(window_menu_driver), REQ_UP_ITEM}),
//  {0, {nullptr, 0}}
//};

hook *hooks[] =
{
  hooks_main,
  hooks_game,
  hooks_menu,
  //hooks_event_dialog,
};

const static text descs[] =
{
  "You are welcome!",
  "Create your own map.",
  "Select map size:",

  text("General", PAIR(NEUTRAL_COLOR, COLOR_BLACK)|A_BOLD) + ":\n\
  Q/q      - Close the window (not the main menu).\n\
  Enter    - Choosing.\n\
  Up, Down - Menu navigation.\n\n" +
  text("Game", PAIR(NEUTRAL_COLOR, COLOR_BLACK)|A_BOLD) + ":\n\
  i/I, j/J, k/K, l/L    - Map view moving.\n\
  Up, Down, Right, Left - Player moving.\n\
  Q/q                   - Open game menu.\n\
  Enter                 - Close the message window.",
};

const static text titles[] =
{
  "Menu",
  "Message",
  "Map Creator",
  "Map Sizes",
  "Error",
  "Controlling",
};

builder build[] =
{
  builder /* BUILD_MAIN */
  (
  POSITION_FULL,
  menus[MENU_MAIN],
  hooks[HOOKS_MAIN],
  descs[DESC_MAIN],
  titles[TITLE_MENU],
  OPTION_NORMAL,
  FORMAT_CENTER,
  images[IMAGE_HORSEBACK_FIGHT]
  ),

  builder /* BUILD_GAME */
  (
  POSITION_FULL,
  nullptr,
  hooks[HOOKS_GAME],
  nullptr,
  nullptr,
  OPTION_BORDERLESS
  ),

  builder /* BUILD_GAME_MENU */
  (
  POSITION_SMALL,
  menus[MENU_GAME],
  hooks[HOOKS_MENU],
  nullptr,
  titles[TITLE_MENU]
  ),

  builder /* BUILD_OKAY */
  (
  POSITION_SMALL,
  menus[MENU_OKAY],
  hooks[HOOKS_MENU],
  nullptr,
  titles[TITLE_MESSAGE]
  ),

  builder /* BUILD_MAP_CREATOR */
  (
  POSITION_FULL,
  menus[MENU_MAP_CREATOR],
  hooks[HOOKS_MENU],
  descs[DESC_MAP_CREATOR],
  titles[TITLE_MAP_CREATOR],
  OPTION_NORMAL,
  FORMAT_CENTER,
  images[IMAGE_MOUNTAINS]
  ),

  builder /* BUILD_MAP_SIZES */
  (
  POSITION_SMALL,
  menus[MENU_MAP_SIZES],
  hooks[HOOKS_MENU],
  descs[DESC_MAP_SIZES],
  titles[TITLE_MAP_SIZES]
  ),

  builder /* BUILD_ERROR */
  (
  POSITION_SMALL,
  menus[MENU_OKAY],
  hooks[HOOKS_MENU],
  nullptr,
  titles[TITLE_ERROR]
  ),

  builder /* BUILD_CONTROL */
  (
  POSITION_FULL,
  menus[MENU_BACK],
  hooks[HOOKS_MENU],
  descs[DESC_CONTROL],
  titles[TITLE_CONTROL],
  OPTION_NORMAL,
  FORMAT_RIGHT
  ),
};


void scenario_init(arg_t arg)
{
  char* scene = reinterpret_cast<char *>(arg);
  auto location = window_get_location(build[BUILD_GAME].position);

  try {
    scenario_create_from_config(scene, window_print, location.lines, location.cols);

  } catch(const game_error &error)  {  
    window_push(BUILD_ERROR, error.what());    
    return;
  }

  window_set(BUILD_GAME);
  scenario_render();
}

void map_generate(arg_t arg)
{
  string fil;

  /* Remove C_SIZES */
  window_pop();
  try {
    /* Square-shaped map */
    fil = map::generate(int(arg), int(arg));

  } catch (const game_error& error) {
    window_push(BUILD_ERROR, error.what());
    return;
  }

  window_push(BUILD_OKAY, "Map was successfully generated to " + fil);
}

void scenario_menu()
{
  constexpr size_t extension_size = sizeof(".yaml");
  const char * home = std::getenv("HOME");

  if (home == nullptr)
    {
      window_push(BUILD_ERROR, "HOME variable is not set.");
      return;
    }

  string dir = home;
  dir = dir + '/' + FILE_SCENARIOS;

  vector<unique_ptr<char[]>> paths;
  vector<unique_ptr<char[]>> names;

  DIR *dp;
  struct dirent *dirp;

  if((dp  = opendir(dir.c_str())) == nullptr)
    {
      window_push(BUILD_ERROR, string(strerror(errno)) + " \"" + dir + "\".");
      return;
    }
  while  ( (dirp = readdir(dp)) != nullptr )
    if (dirp->d_type & DT_REG)
      {
        string path = dir + dirp->d_name;
        if (path.rfind(".yaml") != path.size() + 1 - extension_size)
          continue;

        paths.emplace_back(new char[path.size() + 1]);
        strcpy(paths.back().get(), path.c_str());

        auto name_lenght = strlen(dirp->d_name) - extension_size + 1;

        names.emplace_back (new char[name_lenght + 1]);
        strncpy(names.back().get(), dirp->d_name, name_lenght);
        names.back().get()[name_lenght] = '\0';
      }

  closedir(dp);

  if (paths.empty())
    {
      window_push(BUILD_ERROR, "No scenarios in \"" + dir + "\".");
      return;
    }

  unique_ptr<item[]> load_menu(new item[paths.size() + 1]);

  for (size_t i = 0; i < paths.size(); ++i)
    load_menu.get()[i] =
    {
      names[i].get(),
      { fun_t(scenario_init), arg_t(paths[i].get()) }
    };
  load_menu.get()[paths.size()] = {};

  window *wptr = window_push(builder(POSITION_SMALL, load_menu.get(), hooks[HOOKS_MENU],
                                     "Select the scenario:",
                                     "Scenarios",
                                     OPTION_NORMAL,
                                     FORMAT_CENTER,
                                     images[IMAGE_OPEN_BOOK],
                                     IMAGE_POSITION_LEFT));

  /* Exit when a choice window a scenario will close
     * and may be free of memory */
  while(window_has(wptr))
    window_hook();
}
