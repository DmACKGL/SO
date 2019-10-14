#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fnmatch.h>
#include <fts.h>

char * concat(const char * s1, const char * s2) {
    char * result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int entcmp(const FTSENT **a, const FTSENT **b)
{
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

void pmatch(char *dir, const char *pattern)
{
    FTS *tree;
    FTSENT *f;
    char *argv[] = { dir, NULL };

    tree = fts_open(argv, FTS_LOGICAL | FTS_NOSTAT, entcmp);
    if (tree == NULL)
        err(1, "fts_open");

    while ((f = fts_read(tree))) {
        switch (f->fts_info) {
            case FTS_DNR:
            case FTS_ERR:
            case FTS_NS:
                warn("%s", f->fts_path);
                continue;
            case FTS_DP:
                continue;
        }

        if (fnmatch(pattern, f->fts_name, FNM_PERIOD) == 0)
            puts(concat("Archivo encontrado en: ",f->fts_path));

        if (f->fts_info == FTS_DC)
            warnx("DEBUG: %s", f->fts_path);
    }

    if (errno != 0)
        err(1, "fts_read");

    if (fts_close(tree) < 0)
        err(1, "fts_close");
}

int main(int argc, char *argv[]) {

    printf("Bienvenido a Scanner!\n");

    if (argc == 1){
        printf("DEBUG: %d\nError: Hmmm, Creo que no ingresaste ningun archivo a buscar", argc);
        return 0;
    }

    if (strcasestr(argv[1], "help") != NULL){
        printf("Uso del programa: ./buscador '*.pdf' archivo.c\n Utilizar '' cuando se quiere buscar por extension");
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        printf("Se usa carpeta 'safe' para probar el programa. (../safe)\n");
        pmatch("./safe", argv[i]);
    }

    return 0;
}