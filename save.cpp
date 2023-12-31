#include "stat.hpp"

// RU
/* Формат сохранения:
<глубина> <качество карты (в процентах * 2)> <id картинки для генерации> <цель> <удача>
<тип локации> <XxYxZ в клетках> <число дверей (кроме входной)> <#растений> <#жидкости> <#камней> <#врагов> <модификатор силы врагов * 10>
<первый фокус> <второй фокус>
Door-<число описанных дверей>
<описания дверей по 1 в строке: вероятности вверх, на уровне, вниз, число локаций и вероятности для каждой из них>
Trouble-<число особенностей>
<id особенностей по 1 в строке: id группы и номер в ней>
*/

// EN
/* Save format:
<depth> <map quality (in per cent * 2)> <id of picture to generate> <target> <success>
<location type> <XxYxZ in squares> <number of doors (except entrance door)> <#plants> <#liquid> <#stones> <#enemies> <enemy strength modifier * 10>
<first focus> <second focus>
Door-<number of described doors>
<descriptions of doors 1 per row: probabilities up, level, down, number of locations and probabilities for each location>
Trouble-<number of features>
<id of features 1 per line: id of the group and number in it>
*/

void save(const ConcreteLocation &l, int level, int map_quality, int picture_id, int goal, int luck) {
    char filepath[256];
    printf(_("Куда: "));
    scanf("%s", filepath);
    FILE *out = fopen(filepath, "w");
    if (!out) {
        printf(_("Не удалось открыть файл на запись:(\n"));
        return;
    }

    fprintf(out, "%d %d %d %d %d\n", level, map_quality, picture_id, goal, luck);
    fprintf(out, "%d %d %d %d %d %d %d %d %d %d\n", l.loc_id, l.x, l.y, l.z, l.door_num, l.plants, l.fluid, l.stones, l.enemy, l.power);
    fprintf(out, "%d %d\n", l.focus[0], l.focus[1]);
    fprintf(out, "Door-%lu\n", l.doors.size());
    for (Door d : l.doors) {
        fprintf(out, "%d %d %d %lu:", d.up, d.same, d.down, d.chances.size());
        for (auto el = d.chances.begin(); el != d.chances.end(); el++) {
            fprintf(out, " %d-%d", el->first, el->second);
        }
        fprintf(out, "\n");
    }
    fprintf(out, "Trouble-%lu\n", l.troubles.size());
    for (TroubleId trouble : l.troubles) {
        fprintf(out, "%d %d\n", trouble.group, trouble.ingroup_id);
    }

    fclose(out);
    printf("DONE\n");
}

void load(ConcreteLocation &l, int &level, int &map_quality, int &picture_id, int &goal, int &luck) {
    char filepath[256];
    printf(_("Откуда: "));
    scanf("%s", filepath);
    FILE *out = fopen(filepath, "r");
    if (!out) {
        printf(_("Не удалось открыть файл на чтение:(\n"));
        return;
    }

    fscanf(out, "%d %d %d %d %d\n", &level, &map_quality, &picture_id, &goal, &luck);
    fscanf(out, "%d %d %d %d %d %d %d %d %d %d\n", &l.loc_id, &l.x, &l.y, &l.z, &l.door_num, &l.plants, &l.fluid, &l.stones, &l.enemy, &l.power);
    fscanf(out, "%d %d\n", &l.focus[0], &l.focus[1]);
    unsigned long door_num = 0;
    fscanf(out, "Door-%lu\n", &door_num);
    l.doors.clear();
    for (unsigned long i = 0; i < door_num; i++) {
        Door d = {};
        unsigned long chances_num = 0;
        fscanf(out, "%d %d %d %lu:", &d.up, &d.same, &d.down, &chances_num);
        for (unsigned long j = 0; j < chances_num; j++) {
            int first = 0, second = 0;
            fscanf(out, " %d-%d", &first, &second);
            d.chances[first] = second;
        }
        fscanf(out, "\n");
        l.doors.push_back(d);
    }
    unsigned long trouble_num = 0;
    fscanf(out, "Trouble-%lu\n", &trouble_num);
    l.troubles.clear();
    for (unsigned long i = 0; i < trouble_num; i++) {
        TroubleId trouble = {};
        fscanf(out, "%d %d\n", &trouble.group, &trouble.ingroup_id);
        l.troubles.push_back(trouble);
    }

    fclose(out);
    printf("DONE\n");
}