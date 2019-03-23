#include "window.hpp"

// FOR BORDERS 4
#define SUB_WIN_LINES_RELATIVE 2
#define SUB_WIN_COLS_RELATIVE 2
#define SUB_WIN_Y 1
#define SUB_WIN_X 1

static int text_height(const Text *t, int freecols);
static int waddtext(WINDOW *w, const Text *t);
static int waddcchar(WINDOW *w, const Text::cchar *t);

#define    mvwaddtext(win,y,x,t) \
    (wmove((win),(y),(x)) == ERR ? ERR : waddtext((win),(t)))

// STATIC WINDOW VARIABLES AND CONSTS
Window *Window::topw      = nullptr;

Window::Window(const Constructor &ct)
{
    m_win = newwin(ct.lines, ct.cols, ct.y, ct.x);

    /* Set window decorations */
    wattron(m_win, PAIR(MYCOLOR, COLOR_BLACK)|A_DIM);
    box(m_win, 0, 0);
    //wborder(m_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wattroff(m_win, PAIR(MYCOLOR, COLOR_BLACK)|A_DIM);

   // wbkgd(m_win, COLOR_PAIR(PAIR(COLOR_YELLOW, COLOR_BLACK)));

    /* Free height (lines) space */
    freelines = ct.lines - SUB_WIN_LINES_RELATIVE;
    freecols =  ct.cols  - SUB_WIN_COLS_RELATIVE;

    /* Text height, position computation */
    int th = 0;

    if (ct.t && ct.t->len != 0)
    {
        th = text_height(ct.t, freecols);

        /* Text drawing */
        m_sub_t = derwin(m_win, th, freecols, SUB_WIN_Y, SUB_WIN_X);
        //mvwaddwstr(m_sub_t, 0, 0, t);
        mvwaddtext(m_sub_t, 0, 0, ct.t);

    } else
        m_sub_t = nullptr;

    int menu_height = 0;
    int menu_y = 0;

    if (ct.m)
    {
        /* Menu height, position computation */
        menu_height = freelines - th;
        menu_y = SUB_WIN_Y + th;

        /* Create subwindow for menu */
        m_sub_m = derwin(m_win, menu_height, freecols, menu_y, SUB_WIN_X);

        /* Create items from Menu* m */
        m_items = new ITEM *[ct.msize + 1];
        for (size_t i = 0; i < ct.msize; ++i) {
            m_items[i] = new_item(ct.m[i].label.c_str(), " ");
            if (ct.m[i].act.empty())
                item_opts_off(m_items[i], O_SELECTABLE);
            else
                m_items[i]->userptr = &ct.m[i];

        }
        m_items[ct.msize] = nullptr;

        /* Link menu & window */
        m_menu = new_menu(m_items);
        set_menu_win(m_menu, m_win);
        set_menu_sub(m_menu, m_sub_m);

        /* For menu scrolling */
        set_menu_format(m_menu, menu_height, 1);

        /* Set menu decorations */
        set_menu_mark(m_menu, " * ");
        set_menu_grey(m_menu, A_DIM);
        set_menu_fore(m_menu, PAIR(MYCOLOR, COLOR_BLACK)|A_BOLD);
        set_menu_back(m_menu, PAIR(COLOR_WHITE, COLOR_BLACK));

        /* Attaching */
        post_menu(m_menu);

    } else {
        m_menu  = nullptr;
        m_items = nullptr;
        m_sub_m = nullptr;
    }

    /* Set hooks */
    m_hooks = ct.h;
    m_hooks_size = ct.hsize;

    /* Panel creation */
    m_pan = new_panel(m_win);
    set_panel_userptr(m_pan, this);
    topw = this;

    this->refresh();
    update_panels();    
}

Window::~Window()
{
    /* Set the top window */
    auto pan_ptr = panel_below(topw->m_pan);
    if (pan_ptr)
        topw = reinterpret_cast<Window *>(const_cast<void *>(panel_userptr(pan_ptr)));
    else
        topw = nullptr;

    if (m_menu) {
        unpost_menu(m_menu);
        int n = item_count(m_menu);
        free_menu(m_menu);
        for (int i = 0; i < n; ++i)
            free_item(m_items[i]); 
        free_item(m_items[n]);
    }

    delete [] m_items;
    del_panel(m_pan);
    delwin(m_sub_m);
    delwin(m_sub_t);
    delwin(m_win);

    update_panels();

    // Refresh previous window to show it, if it's exists
    if (topw)
        topw->refresh();
}

void Window::_refresh() const
{
    wnoutrefresh(this->m_sub_t);
    wnoutrefresh(this->m_sub_m);
    wnoutrefresh(this->m_win);
    doupdate();
}

void Window::_menu_driver(int act) const
{
    Menu *temp = static_cast<Menu *>(item_userptr(current_item(m_menu)));
    switch (act) {
    case REQ_EXEC_ITEM:
        if (temp)
            temp->act.exec();
        return;
    }

    ::menu_driver(m_menu, act);
    this->refresh();
}

void Window::_exechook(int key) const
{
    for (size_t i = 0; i < m_hooks_size; ++i)
        if (m_hooks[i].key == key)
            m_hooks[i].act.exec();
}

// РЕЗЕРВИРУЕТ ВСЁ РОСТРАНСТВА ОКНА ПОД ТЕКСТОВОЕ ПОДОКНО, ЕСЛИ
// ОТСУТСТВУЕТ ПОДОКНО ДЛЯ МЕНЮ
void Window::_print(const Text &t, int x, int y, int size)
{    
    if (t.len == 0 || m_sub_m) return;

    int cth = m_sub_t? getmaxy(m_sub_t) - getbegy(m_sub_t) + 1 : 0;

    int xend = x + freecols;
    int yend = y + freelines;

    if (cth != freelines) {
        if (m_sub_t)
            wresize(m_sub_t, freelines, freecols);
        else
            m_sub_t = derwin(m_win, freelines, freecols, SUB_WIN_Y, SUB_WIN_X);
    }

    wmove(m_sub_t, 0, 0);
    for (int i = y; i < yend; ++i)
        for (int j = x; j < xend; ++j)
        {
            if (i >= size || j >= size) {
                waddch(m_sub_t, '\n');
                break;
            }
            waddcchar(m_sub_t, &t.text[i*size + j]);
        }


    refresh();
}

void Window::_print(const Text &t)
{
    int cth = m_sub_t? getmaxy(m_sub_t) - getbegy(m_sub_t) + 1 : 0;
    int th = text_height(&t, freecols);

    if (th != cth) {
        if (m_sub_m) {
            int new_menu_height = freelines - th;

            unpost_menu(m_menu);
            delwin(m_sub_m);

           /* Moving subwindows is allowed, but should be avoided */
           //  wresize(m_sub_m, new_menu_height, freecols);
           //  mvwin(m_sub_m, text_height + SUB_WIN_Y, SUB_WIN_X);

            m_sub_m = derwin(m_win, new_menu_height,
                             freecols, th + SUB_WIN_Y,
                             SUB_WIN_X);

            set_menu_sub(m_menu, m_sub_m);
            post_menu(m_menu);
        }

        if (m_sub_t) {
            wresize(m_sub_t, th, freecols);
        } else {
            m_sub_t = derwin(m_win, th, freecols, SUB_WIN_Y, SUB_WIN_X);
            //wclear(m_sub_t);
        }
    }

    mvwaddtext(m_sub_t, 0, 0, &t);
    refresh();
}

int text_height(const Text *t, int freecols)
{
    size_t old = 0;
    int text_height = 1, templen;

    // Text("")
    if (t->len == 0) return 0;

    for (size_t i = 0; i < t->len; ++i)
        if (t->text[i].c == '\n')
        {
            templen = ( int(i) - 1 - int(old) ) + 1;

            text_height +=  ((templen - 1) / freecols + 1);
            old = i + 1;
        }

    templen = ( int(t->len) - 1 - int(old) ) + 1;

    text_height += (templen - 1) / freecols;

    return text_height;
}

int waddtext(WINDOW *w, const Text *t)
{
    int rc = OK;
    for (size_t i = 0; i < t->len; ++i)
        rc = waddcchar(w, t->text + i);
    return rc;
}

int waddcchar(WINDOW *w, const Text::cchar *t)
{
    int rc = OK;
    wattron(w, t->attr);
    rc = waddch(w, chtype(t->c));
    wattroff(w, t->attr);
    return rc;
}
