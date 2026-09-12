#ifndef LANG_H
#define LANG_H

/* Language indices - new projects use this order (plan.txt §8.1) */
#define LANG_EN    0
#define LANG_ES    1
#define LANG_NL    2
#define LANG_DE    3
#define LANG_FR    4
#define LANG_IT    5
#define LANG_COUNT 6

/* String IDs */
enum {
    STR_MENU_GAME=0,
    STR_MENU_NEW,
    STR_MENU_EXIT,
    STR_MENU_OPTIONS,
    STR_MENU_LANGUAGE,
    STR_MENU_SAVEONEXIT,
    STR_MENU_FRAME,
    STR_MENU_HELP,
    STR_MENU_ABOUT,
    STR_MSG_IWON,
    STR_TITLE_IWON,
    STR_MSG_YOUWON,
    STR_TITLE_YOUWON,
    STR_COUNT
};

extern int current_lang;
extern const char *lang_strings[LANG_COUNT][STR_COUNT];
#define tr(id) ((char*)lang_strings[current_lang][(id)])

#endif
