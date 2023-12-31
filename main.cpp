#include "stat.hpp"

#define GETTEXT_DOMAIN "lst_en"
#define GETTEXT_TARGET "gettext-target"
#define GETTEXT_OUTPUT_DIR "/mnt/c/Users/helio/Documents/Github/Long-Stairs-Traveller_EN/locales"

const int GEN_LOC_NUM = 4;

char* my_env(const char* var) {
    char* rv = NULL;
    char* ptr = getenv(var);
    if (ptr) {
        rv = (char*)malloc(strlen(ptr) + 1);
        if (rv) {
            strcpy(rv, ptr);
        }
    }
    return rv;
}

static void setup_i18n(const char* locale){
    /*
    char* pwd = my_env("PWD");
    char* language = getenv("LANG");
    char* pwd_locales = (char*)malloc(strlen(pwd) + 9);
    strcpy(pwd_locales, pwd);
    strcat(pwd_locales, "/locales");
    printf("getenv(LANG): %s\n", language ? language : "NULL");
    */

    //i18n: initializes the entire current locale of the program as per environment variables set by the user
    char* set = setlocale(LC_MESSAGES, locale);
    //printf("setlocale(): %s\n", set ? set : "NULL");
    
    //i18n: sets the message domain (expected name of the catalog file)
    char* text = textdomain("lst-en");
    //printf("textdomain(): %s\n", text ? text : "NULL");
    
    //i18n: Indicate the path of the i18n catalog file
    char* bind = bindtextdomain("lst-en", GETTEXT_OUTPUT_DIR);
    //printf("bindtextdomain(): %s\n", bind ? bind : "NULL");
    
    bind_textdomain_codeset("lst-en", "UTF-8");
}

void show_loot(const ConcreteLocation &l, int level, int luck) {
    if (l.loc_id >= 0) {
        static int current_magic_chance = MAGIC_LOOT_CHANCE;

        double power = l.power / 10.0 + int(level / LANDING_DIST) * 0.25 + 1;
        double max_cost_real = 9.0 * (int(level / LANDING_DIST) * 2 + (int(level / LANDING_DIST) == 0)) * (5.0 * (power >= 0 ? power * (l.enemy >= 3) * l.enemy : power) + 15.0 * LOC[l.loc_id].skull_level); // some magic happens here
        double luck_modifier = (luck - PERCENT(50)) / MAX_PROBABILITY;
        if (luck_modifier > 0.1) luck_modifier = 0.1;
        else if (luck_modifier < -0.1) luck_modifier = -0.1;
        max_cost_real *= 1 + luck_modifier;
        /*if (luck > MAX_PROBABILITY / 10 && luck < 3 * MAX_PROBABILITY / 2) {
            max_cost_real = max_cost_real * 2 * luck / MAX_PROBABILITY;
        } else if (luck <= MAX_PROBABILITY / 10) {
            max_cost_real /= 5;
        } else if (luck >= 3 * MAX_PROBABILITY / 2) {
            max_cost_real *= 3;
        }*/
        max_cost_real /= 50;
        int max_cost = int(max_cost_real) + (max_cost_real - int(max_cost_real) >= 0.5);
        int cost = (rand() % MAX_PROBABILITY + rand() % MAX_PROBABILITY + rand() % MAX_PROBABILITY) / 3 * max_cost / MAX_PROBABILITY;
        printf(_("Улов на %d из %d%s"), cost, max_cost, (cost >= MAGIC_LOOT_LIMIT ? "" : "\n"));
        if (cost >= MAGIC_LOOT_LIMIT) {
            int chance = MAX_PROBABILITY;
            int cost_left = cost;
            int magic_met = 0;
            do {
                chance = rand() % MAX_PROBABILITY;
                cost_left -= MAGIC_LOOT_LIMIT;
                printf(".");
                if (chance < current_magic_chance * int(level / LANDING_DIST)) {
                        if (rand() % MAX_PROBABILITY < (CURSED_MAGIC_CHANCE - (luck - PERCENT(50)) / 150)) { // shift <= 10%
                            printf(_("\nЧто-то магическое!!! Правда проклятое :( "));
                        }
                        else {
                            printf(_("\nЧто-то магическое!!!"));
                        }
                    chance = MAX_PROBABILITY;
                    magic_met += 1;
                    current_magic_chance = MAGIC_LOOT_CHANCE;
                } else {
                    current_magic_chance += PERCENT(2);
                }
            } while (cost_left >= MAGIC_LOOT_LIMIT);
            printf("\n");
            if (magic_met) {
                return;
            }
        }
        if (cost >= MONEY_LOOT_LIMIT) {
            int cost_left = cost - MONEY_TOP_LIMIT;
            if (cost > MONEY_TOP_LIMIT) cost = MONEY_TOP_LIMIT - rand() % 5; // we don't want to give folks TONS of money
            int weight_chance = rand() % MAX_PROBABILITY;
            if (weight_chance <= VERY_HEAVY_LOOT_CHANCE) {
                printf(_("Тяжеленный (веса 2+)... "));
            } else if (weight_chance <= HEAVY_LOOT_CHANCE) {
                printf(_("Увесистый (веса 1)... "));
            }
            printf(_("%s (%d зм)!\n"), TREASURE[rand() % TREASURE_NUM], cost);
            if (cost_left > 3 * MONEY_TOP_LIMIT) cost_left = 3 * MONEY_TOP_LIMIT; // too many trinkets is not good as well
            while (cost_left > 0) {
                printf(_("Безделушка: %s\n"), TRINKET[rand() % TRINKET_NUM]);
                cost_left -= MONEY_TOP_LIMIT;
            }
        } else if (cost > 0) {
            printf(_("Безделушка: %s\n"), TRINKET[rand() % TRINKET_NUM]);
        } else {
            printf(_("Полный облом :(\n"));
        }
    } else {
        printf(_("В якорных точках не бывает лёгких денег!\n"));
    }
}

void show_info(const ConcreteLocation &l, int level, int map_quality, int goal, int luck) {
    printf(_("Уровень - %d\tЦель - %d\n"), level, goal);
    const char *luck_level;
    if (luck >= MAX_PROBABILITY) {
        luck_level = "Умопомрачительное";
    } else if (luck < 0) {
        luck_level = "Антиумопомрачительное";
    } else {
        luck_level = LUCK_LEVELS[luck / (MAX_PROBABILITY / (LUCK_LEVEL_NUM))];
    }
    printf(_("Качество карты - ±%.1lf%%\tВезение - %s\n"), map_quality / 2.0, luck_level);
    if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
        printf(_("Добро пожаловать в якорную точку: %s!\n"), LANDING_NAME[level / LANDING_DIST]);
    }
    print_loc(l, map_quality, level);
}

int main(void) {
    srand(time(NULL));

    char lang[2] = {};
    printf("Пожалуйста, выберите язык / Please select language: (1) Русский (2) English\n");
    printf(">");
    scanf("%s", lang);
    if (!strcmp(lang, "1")) {
        setup_i18n(NULL);
        printf("\nВы входите на Длинную лестницу...\n\n");
    } else {
        setup_i18n("en_US.UTF-8");
        printf("\nYou are entering the Long Stairs...\n\n");
    }

    ConcreteLocation l = LANDING[0];
    int level = 0, goal = 30;
    int map_quality = 10;
    int picture_id = 0;
    int luck = BASIC_LUCK;

    gen_doors(l, GEN_LOC_NUM, level, goal, luck);
    show_info(l, level, map_quality, goal, luck);

    char cmd[256] = {};
    printf(">");
    scanf("%s", cmd);
    while (cmd[0] != 'q') {
        if (!strcmp(cmd, "info")) {
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "go")) {
            int door = 0;
            // printf("Дверь (0-%d): ", l.door_num - 1);
            scanf("%d", &door);
            if (door >= 0 && door < l.door_num) {
                int new_level = level;
                int new_luck = luck;
                int new_loc_id = use_door(l, door, new_level, goal, new_luck);
                if (new_loc_id == -1)
                    return 1;
                ConcreteLocation new_l = make_loc(new_loc_id);
                if (new_level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                    new_l = LANDING[new_level / LANDING_DIST];
                    gen_doors(new_l, GEN_LOC_NUM, new_level, goal, new_luck);
                    printf(_("Добро пожаловать в якорную точку: %s!\n"), LANDING_NAME[new_level / LANDING_DIST]);
                } else {
                    gen_doors(new_l, GEN_LOC_NUM, new_level, goal, new_luck);
                    gen_troubles(new_l);
                    if (new_l.troubles.empty()) {
                        // greater chances for at least one trouble
                        gen_troubles(new_l);
                    }
                }
                show_info(new_l, new_level, map_quality, goal, new_luck);
                show_loot(new_l, new_level, new_luck);
                printf(_("Захлопнуть дверь?(-1 - да): "));
                scanf("%d", &door);
                if (door < 0) {
                    printf("RDP breach detected!\n");
                    map_quality += 4;
                } else {
                    printf(_("Удачи!\n"));
                    l = new_l;
                    level = new_level;
                    luck = new_luck;
                    if (level == goal && goal > 0) {
                        printf(_("Ура!!! Цель достигнута!!! Новая цель - Hellmouth:)\n"));
                        goal = 0;
                    }
                }
            }
        } else if (!strcmp(cmd, "loc")) {
            if (level % LANDING_DIST == 0 && level < int(LANDING_DIST * LANDING_NUM)) {
                ConcreteLocation new_l = LANDING[level / LANDING_DIST];
                gen_doors(new_l, GEN_LOC_NUM, level, goal, luck);
                gen_troubles(new_l);
                if (new_l.troubles.empty()) {
                    // greater chances for at least one trouble
                    gen_troubles(new_l);
                }
                printf(_("Добро пожаловать в якорную точку: %s!\n"), LANDING_NAME[level / LANDING_DIST]);
                print_loc(new_l, map_quality, level);
                show_loot(new_l, level, luck);
                int ans = -1;
                printf(_("Отменить?(-1 - да): "));
                scanf("%d", &ans);
                if (ans >= 0) {
                    printf(_("Удачи!\n"));
                    l = new_l;
                }
            } else {
                int loc_id = 0;
                for (unsigned i = 0; i < LOC_NUM; i++) {
                    printf("%2d - %-40s%c", i, LOC[i].name, (i % 3 == 2 ? '\n' : '\t'));
                }
                printf("ID (0-%d): ", LOC_NUM - 1);
                scanf("%d", &loc_id);
                if (loc_id >= 0 && loc_id < int(LOC_NUM)) {
                    ConcreteLocation new_l = make_loc(loc_id);
                    gen_doors(new_l, GEN_LOC_NUM, level, goal, luck);
                    gen_troubles(new_l);
                    if (new_l.troubles.empty()) {
                        // greater chances for at least one trouble
                        gen_troubles(new_l);
                    }
                    print_loc(new_l, map_quality, level);
                    show_loot(new_l, level, luck);
                    printf(_("Отменить?(-1 - да): "));
                    scanf("%d", &loc_id);
                    if (loc_id >= 0) {
                        printf(_("Удачи!\n"));
                        l = new_l;
                    }
                }
            }
        } else if (!strcmp(cmd, "trouble")) {
            gen_troubles(l);
            if (l.troubles.empty()) {
                // greater chances for at least one trouble
                gen_troubles(l);
            }
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "door")) {
            gen_doors(l, GEN_LOC_NUM, level, goal, luck);
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "focus")) {
            printf(_("Какой меняем (0 - 1ый, 1 - 2ой, 2 - оба)? "));
            int ans = -1;
            scanf("%d", &ans);
            if (ans == 2) {
                l.focus[0] = rand() % FOCUS_NUM;
                do {
                    l.focus[1] = rand() % FOCUS_NUM;
                } while (l.focus[1] == l.focus[0]);
            } else if (ans == 0 || ans == 1) {
                do {
                    l.focus[ans] = rand() % FOCUS_NUM;
                } while (l.focus[ans] == l.focus[!ans]);
            } else {
                printf(_("Неверный ввод!\n"));
            }
            show_info(l, level, map_quality, goal, luck);
        } else if (!strcmp(cmd, "show")) {
            FILE *out = fopen("showed_map.json", "w");
            MapSettings data = {};
            if (l.loc_id >= 0) {
                int use_crates = 0;
                printf(_("Враги используют ящики? (1 - да, 0 - нет): "));
                scanf("%d", &use_crates);
                data = LOC[l.loc_id].map_data;
                data.width = l.x; data.height = l.y;
                data.door_num = l.door_num + 1;
                data.plant_chance = l.plants; data.stone_chance = l.stones;
                data.crate_chance = l.enemy * use_crates;
                if (out) {
                    make_map(out, data);
                    fclose(out);
                    printf("DONE\n");
                    printf(_("Сохраняйте по ссылке: https://dnd.alex2000.ru/alex/Data/Pictures/Карты/LongStairs/%d.jpg\n"), picture_id);
                    picture_id++;
                }
            } else {
                printf(_("Рисуй сам, ленивый ты ДМ!!!\n"));
            }
        } else if (!strcmp(cmd, "loot")) {
            show_loot(l, level, luck);
        } else if (!strcmp(cmd, "magic")) {
            int imb = 4, cool = 1;
            printf(_("Имбовость: 4 - косметика, 3 - небольшой немеханический бонус, 2 - большой немеханический или небольшой механический бонус, 1 - имба, бан - лютейшая имба\n"));
            printf(_("Необычность: 1 - тупой обычный предмет, 2 - потуги сделать нечто, 3 - \"а это прикольно\", 4 - поражает до глубины души\n"));
            printf(_("Класс имбовости (4-1) и необычность (1-4): "));
            scanf("%d%d", &imb, &cool);
            int rating = imb + cool;
            switch (rating) {
                case 4: printf(_("%d использований (2-4) или %d минут (30-60)\n"), gen_range(2, 4), gen_range(30, 60)); break;
                case 5: printf(_("%d использований (5-8) или %d минут (60-120)\n"), gen_range(5, 8), gen_range(60, 120)); break;
                case 6: printf(_("%d использований (9-12) или %d минут (120-180)\n"), gen_range(9, 12), gen_range(120, 180)); break;
                case 7: case 8: printf(_("Ваше навсегда:)\n")); break;
                default: printf(_("Иди лесом!\n"));
            }
        } else if (!strcmp(cmd, "goal")) {
            printf(_("Текущая цель - %d\nЗадайте новую: "), goal);
            scanf("%d", &goal);
        } else if (!strcmp(cmd, "luck")) {
            printf(_("Текущая удача - %d\nЗадайте новую (%d<=>100%%): "), luck, MAX_PROBABILITY);
            scanf("%d", &luck);
        } else if (!strcmp(cmd, "map")) {
            printf(_("Текущее качество - %d\nЗадайте новое: "), map_quality);
            scanf("%d", &map_quality);
        } else if (!strcmp(cmd, "level")) {
            printf(_("Текущая глубина - %d\nЗадайте новую: "), level);
            scanf("%d", &level);
        } else if (!strcmp(cmd, "pic")) {
            printf(_("Текущий id карты - %d\nЗадайте новый: "), picture_id);
            scanf("%d", &picture_id);
        } else if (!strcmp(cmd, "save")) {
            save(l, level, map_quality, picture_id, goal, luck);
        } else if (!strcmp(cmd, "load")) {
            load(l, level, map_quality, picture_id, goal, luck);
            show_info(l, level, map_quality, goal, luck);
        } else {
            printf(_("Хелп:\n\tinfo - о локации, go - идти,\n\tПерегенерировать: loc - локацию по id, trouble - особенности, door - двери, focus - фокус монстров\n\tshow - сгенерить json, loot - потребовать с ДМа лут, magic - потребовать с ДМа магический предмет\n\tУправление: goal - целью, luck - удачей, map - качеством карты, level - глубиной, pic - id карты для отображения\n\tsave/load - сохранить/загрузить игру, quit - сдаться\n"));
        }
        printf(">");
        scanf("%s", cmd);
    }
    
    printf(_("Прощай, слабак!\n"));
    return 0;
}
