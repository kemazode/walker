#include "images.hpp"
#include "utils.hpp"
#include <fstream>

static text cow =
    "                    \n"
    "^__^                \n"
    "(oo)\\_______       \n"
    "(__)\\       )\\/\\ \n"
    "    ||----w |       \n"
    "    ||     ||       \n"
    "                      ";

static text centaur =
    "                     \n"
    "           ,~,       \n"
    "          (((}       \n"
    "          -''-.      \n"
    "         (\\  /\\)   \n"
    "   ~______\\) | `\\  \n"
    "~~~(         |  ')   \n"
    "   | )____(  |       \n"
    "   /|/     ` /|      \n"
    "   \\ \\      / |    \n"
    "    |\\|\\   /| |\\  \n"
    "                       ";

static text horseback_fight =
    "                      \n"
    "        y             \n"
    "       /`'            \n"
    "       \\ O ,         \n"
    "        |,/(\\   /    \n"
    "  -`___-\\  |` ./O    \n"
    "  ''-(  /`--) `\\/\\  \n"
    "     7/`       /|     \n"
    "     \\       /  \\   \n";

static text mountains =
    "                                                                   \n"
    "             _,_           +                   __                  \n"
    "             ','                  /\\          `. `.               \n"
    "       .                        .'  \\    +      \"  |             \n"
    "                               /     \\         /  .'         .    \n"
    "                    .'\\      .'       \\       `\"`               \n"
    "   +             .-'   `.   /          `.                          \n"
    "         .     .'        \\.'             \\                       \n"
    "            .-'           \\               \\   .-`\"`-.      . +  \n"
    "        .'.'               \\               \\.'       `-.         \n"
    "      /                    `.           .-'\\           `-._       \n"
    "     .'                       \\       .-'                   `-.   \n"
    "                                                               `-. \n"
    ".-------------------'''''''''''''''              _.--      .'      \n"
    "                             ___..         _.--''        .'        \n"
    "                       --''''             '            .'          \n"
    "                                                                   \n";

static text pikeman =
    "                                      \n"
    "   ,   A           {}                 \n"
    "  / \\, | ,        .--.               \n"
    " |    =|= >      /.--.\\              \n"
    "  \\ /` | `       |====|              \n"
    "   `   |         |`::`|               \n"
    "       |     .-;`\\..../`;_.-^-._     \n"
    "      /\\\\/  /  |...::..|`   :   `|  \n"
    "      |:'\\ |   /'''::''|   .:.   |   \n"
    "       \\ /\\;-,/\\   ::  |..:::::..| \n"
    "       |\\ <` >  >._::_.| ':::::' |   \n"
    "       | `\"\"`  /   ^^  |   ':'   |  \n"
    "       |       |       \\    :    /   \n"
    "       |       |        \\   :   /    \n"
    "       |       |___/\\___|`-.:.-`     \n"
    "       |        \\_ || _/    `        \n"
    "       |        <_ >< _>              \n"
    "       |        |  ||  |              \n"
    "       |        |  ||  |              \n"
    "       |       _\\.:||:./_            \n"
    "       |      /____/\\____\\          \n";

static text scroll_and_ink_pen =
    "                              \n"
    "(\\                           \n"
    " \'\\                         \n"
    "  \'\\     __________         \n"
    "  / '|   ()_________)         \n"
    "  \\ '/    \\ ~~~~~~~~ \\     \n"
    "    \\       \\ ~~~~~~   \\   \n"
    "    ==).      \\__________\\  \n"
    "   (__)       ()__________)   \n";

static text open_book =
    "                          \n"
    "      ______ ______       \n"
    "    _/      Y      \\_    \n"
    "   // ~~ ~~ | ~~ ~  \\\\  \n"
    "  // ~ ~ ~~ | ~~~ ~~ \\\\ \n"
    " //________.|.________\\\\\n"
    "`----------`-'----------' \n";

const text *images[] =
{
  &cow,
  &centaur,
  &horseback_fight,
  &mountains,
  &pikeman,
  &scroll_and_ink_pen,
  &open_book,
};

using std::fstream;

text images_find(const char *image)
{
  const char * home = std::getenv("HOME");

  if (home == nullptr)
      throw game_error("HOME variable is not set.");

  string path = home;
  path = path + "/" + F_SCENARIOS + image;

  fstream f(path);
  string im;

  if (!f.good())
    throw game_error(string("We can't open image file \"") + image + "\".");

  size_t max_line = 0;
  while (f.good())
    {
      string temp;
      getline(f, temp);
      im += temp;
      im += '\n';
      if (temp.size() > max_line) max_line = temp.size();
    }

  im.insert(im.begin(), '\n');
  im.insert(im.begin(), max_line, ' ');
  return im;
}
