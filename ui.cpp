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

#include "ui.hpp"

static item menu_main[] =
{
  item("Start New Game", {fun_t(scenario_load), 0}),
  item("Create Map",     {window_push, BUILD_MAP_CREATOR}),
  item("Scoreboard"),
  item("Options"),
  item("Exit",           {fun_t(window_clear), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_game[] =
{
  item("Continue",       {fun_t(window_pop), 0}),
  item("Exit",           {window_set, BUILD_MAIN}),
  {nullptr, {nullptr , 0}}
};

static item menu_okay[] =
{
  item("OK",           {fun_t(window_pop), 0}),
  {nullptr, {nullptr , 0}}
};

static item menu_map_creator[] =
{
  item("Generate",      {window_push, BUILD_MAP_SIZES}),
  {nullptr, {nullptr , 0}}
};

static item menu_map_sizes[] =
{
  item("100x100", {scenario_generate, 100}),
  item("250x250", {scenario_generate, 250}),
  item("500x500", {scenario_generate, 500}),
  {nullptr, {nullptr , 0}}
};

item *menus[] =
{
  menu_main,
  menu_game,
  menu_okay,
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

  // Map moving
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
static hook hooks_event_dialog[] =
{
  hook('\n',     {fun_t(window_menu_driver), REQ_EXEC_ITEM}),
  hook(KEY_DOWN, {fun_t(window_menu_driver), REQ_DOWN_ITEM}),
  hook(KEY_UP,   {fun_t(window_menu_driver), REQ_UP_ITEM}),
  {0, {nullptr, 0}}
};

hook *hooks[] =
{
  hooks_main,
  hooks_game,
  hooks_menu,
  hooks_event_dialog,
};

static text descs[] =
{
  "Welcome!",
  "Create your own map.",
  "Select map size:"
};

static text titles[] =
{
  "Menu",
  "Message",
  "Map Creator",
  "Map Sizes",
  "Error",
};

builder build[] =
{
  builder(POSITION_FULL,  menus[MENU_MAIN],        hooks[HOOKS_MAIN], descs[DESC_MAIN],        titles[TITLE_MENU]),
  builder(POSITION_FULL,  nullptr,                 hooks[HOOKS_GAME], nullptr,                 nullptr, OPTION_BORDERLESS),
  builder(POSITION_SMALL, menus[MENU_GAME],        hooks[HOOKS_MENU], nullptr,                 titles[TITLE_MENU]),
  builder(POSITION_SMALL, menus[MENU_OKAY],        hooks[HOOKS_MENU], nullptr,                 titles[TITLE_MESSAGE]),
  builder(POSITION_FULL,  menus[MENU_MAP_CREATOR], hooks[HOOKS_MENU], descs[DESC_MAP_CREATOR], titles[TITLE_MAP_CREATOR]),
  builder(POSITION_SMALL, menus[MENU_MAP_SIZES],   hooks[HOOKS_MENU], descs[DESC_MAP_SIZES],   titles[TITLE_MAP_SIZES]),
  builder(POSITION_SMALL, menus[MENU_OKAY],        hooks[HOOKS_MENU], nullptr,                 titles[TITLE_ERROR]),
};
