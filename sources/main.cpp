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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <cstring>
#include <csignal>
#include <cerrno>
#include "ui.hpp"

static void sig_winch(const int signo);
static void mkdir_parents(const char *dir);
static void init_dirs();

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
  const char *home = getenv("HOME");

  if (!home)
    {
      window_push(BUILD_ERROR, "HOME variable is not set.");
      return;
    }

  char path[FILE_COUNT][PATH_MAX] {};

  for (int i = 0; i < FILE_COUNT; ++i)
    {
      strcat(path[i], home);
      strcat(path[i], "/");
    }

  strcat(path[0], FILE_SCENARIOS);
  strcat(path[1], FILE_GENERATIONS);

  FILE *f;

  for (int i = 0; i < FILE_COUNT; ++i)
    {
      f = fopen(path[i], "r");
      if (f)
        fclose(f);
      else mkdir_parents(path[i]);
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
    if (*p == '/')
      {
        *p = 0;
        mkdir(tmp, S_IRWXU);
        *p = '/';
      }

  mkdir(tmp, S_IRWXU);
}

