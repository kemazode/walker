#include <fstream>
#include <limits>
#include <unordered_map>
#include "scene.hpp"
#include "perlin.hpp"

const String Scenario::m_gfile = "Generation.txt";

// Add "(count)" to filename (s), example "Generation (1).txt"
static inline String nextgen(const String &s, int count);

static char textures[] = { '~', '#', '\'', '`' };

static std::unordered_map<char, attr_t> color = {
    {'~',  COLOR_PAIR(PAIR(COLOR_BLUE, COLOR_BLACK))|A_INVIS},
    {'#',  COLOR_PAIR(PAIR(COLOR_WHITE, COLOR_BLACK))|A_INVIS},
    {'\'', COLOR_PAIR(PAIR(COLOR_GREEN, COLOR_BLACK))|A_INVIS},
    {'`',  COLOR_PAIR(PAIR(COLOR_GREEN, COLOR_BLACK))|A_INVIS},
};

/* SCENARIO FORMAT:
 * 100      # size of map (100x100)
 * 0 0      # view pos (0, 0)
 * 0 0      # player pos (0, 0)
 * .....    #
 * .....    #
 * .....    # MAP
 * .....    #
 * .....    #
 */
bool Scenario::load(const String &f, Text &err)
{
    m_file = f;
    std::ifstream fl;

    fl.exceptions(std::ios::failbit);
    char *map = nullptr;
    try {
        fl.open(m_file);

        // Read size reading
        fl >> m_size;

        // Map start view coords reading
        fl >> m_x >> m_y;

        // Player start coords reading
        int x, y;
        fl >> x >> y;

        // Add player object to objects list
        m_objects.emplace_back(x, y, Text::cchar('@', A_BOLD));
        m_player = &m_objects.back();

        // Go to position (0, 0) on the map
        fl.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Whole map reading
        m_real_size = m_size*m_size + 1;
        map = new char[m_real_size];
        for (size_t i = 0; i < m_real_size - 1; ++i) {
            int c;
            while ((c = fl.get()) == '\n');
            if (c == EOF) break;
            map[i] = char(c);
        }
        map[m_real_size - 1] = '\0';

    } catch (std::ios_base::failure &) {        
        if (!fl.is_open())
           err = "We can't open file \"" + f + "\"";
        else
           err = "Something went wrong during reading the file \"" + f + "\".";

        return false;
    }

    fl.close();
    m_map_source = Text(map);

    delete [] map;

    try {
        for (size_t i = 0; i < m_map_source.len; ++i)
            m_map_source[i].attr = color.at(m_map_source[i].c);
    } catch (std::out_of_range &) {       
        err = "Sorry, but file \"" + f + "\" has incorrect symbols.";
        return false;
    }

    update_render_map();
    return true;
}

bool Scenario::gen(const String &f, size_t size, Text& err)
{
    srand(unsigned(time(nullptr)));
    perlin_set_seed(rand());

    std::ofstream fil;

    try {
        fil.open(f);

        for (size_t y = 0; y < size; ++y) {
            for (size_t x = 0; x < size; ++x)
            {
                unsigned seed = unsigned(perlin2d(x, y, 0.05f, 10)*10);
                fil << textures[seed % SIZE(textures)];
            }
            fil << '\n';
        }

    } catch (std::ios::failure &f) {
        err = f.what();
        return false;
    }
    return true;
}

bool Scenario::gen(size_t size, Text &err, String &f)
{
    const char *home = getenv("HOME");

    if (home == nullptr) {
        err = "HOME environment variable not set.";
        return false;
    }

    String folder = home;
    folder += '/';
    folder += F_GENERATIONS;

    std::ifstream fil;
    int count = 0;
    String filename = m_gfile;

    do {
        fil.close();
        if (count > 0)
            filename = nextgen(m_gfile, count);
        fil.open(folder + filename);
        ++count;        
    } while (fil.is_open());

    fil.close();
    f = folder + filename;

    return this->gen(f, size, err);
}

String nextgen(const String &str, int count)
{
    String s = str;
    s.insert(s.rfind('.'), " (" + std::to_string(count) + ")");
    return s;
}

void Scenario::update_render_map() {

    m_map_render = m_map_source;

    for (auto &obj : m_objects)
        map_render(obj.getx(), obj.gety()) = obj.getcchar();

    physic_light_render(*m_player);
}

void Scenario::move_player(int x, int y)
{
    int npx = m_player->getx() + x;
    int npy = m_player->gety() + y;

    if (npx < 0 || npx >= int(m_size) || npy < 0 || npy >= int(m_size))
        return;
    else if (!physic_movement_allowed(npx, npy, *m_player))
        return;

    set_view(npx - m_cols/2, npy - m_lines/2);

    m_player->move(x, y);
    update_render_map();
}

void Scenario::set_view(int x, int y)
{
    if (x < 0) m_x = 0;
    else if (x + m_cols > int(m_size))
    {
        if (int(m_size) > m_cols)
            m_x = int(m_size) - m_cols;
    }
    else
        m_x = x;

    if (y < 0) m_y = 0;
    else if (y + m_lines > int(m_size))
    {
        if (int(m_size) > m_lines)
            m_y = int(m_size) - m_lines;
    }
    else
        m_y = y;
}

void Scenario::clear()
{
    m_size = m_real_size = m_x = m_y = m_lines = m_cols = 0;
    m_file.clear();
    m_map_source.clear();
    m_map_render.clear();
    m_objects.clear();
    m_player = nullptr;
}

bool Scenario::physic_movement_allowed(int x, int y, const Object& obj)
{
    const String &p = obj.getpathless();
    char c = map_source(x, y).c;

    if (p.find(c) != p.npos)
        return false;
    else
        return true;
}

void Scenario::physic_light_render(const Object& viewer)
{
    const String not_visible = "#";
    int visr = 16;

    int px = viewer.getx();
    int py = viewer.gety();

    int side = 0;
    while (side < 8)
    {
        for (int i = 0; i <= visr; ++i)
        {
            double k = (i == 0)? -1 : double(visr) / double(i);
            for (int n = 0; n <= visr; ++n)
            {
                int x, y;
                if (side < 4)
                {
                    y = n;
                    x = (std::abs(k + 1) < 0.1)? y : int(double(y/k));
                    if (side > 0 && side < 3)
                        y = -y;
                    if (side > 1 && side < 4)
                        x = -x;
                } else
                {
                    x = n;
                    y = (std::abs(k + 1) < 0.1)? x : int(double(x/k));

                    if (side > 4 && side < 7)
                        y = -y;
                    if (side > 5 && side < 8)
                        x = -x;
                }

                int npx = px + x;
                int npy = py + y;

                if (npy >= 0 && npy < int(m_size) &&
                        npx >= 0 && npx < int(m_size)) {

                    map_source(npx, npy).attr &= ~A_INVIS;
                    map_source(npx, npy).attr |= A_DIM;

                    map_render(npx, npy).attr &= ~A_INVIS;
                    map_render(npx, npy).attr |= A_BOLD;

                    if (not_visible.find(m_map_render.text[npy*int(m_size) + npx].c) != not_visible.npos)
                        break;
                }
            }
        }
        ++side;
    }
}
