#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <fnmatch.h>
#include <fts.h>
#include <dirent.h>

char * concat(const char * s1, const char * s2) {
    char * result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int entcmp(const FTSENT ** a, const FTSENT ** b) {
    return strcmp(( * a)->fts_name, ( * b)->fts_name);
}

void eliminador(char * dir, const char * pattern, int inverso, int papelera) {
    FTS * tree;
    FTSENT * f;
    char * argv[] = {
            dir,
            NULL
    };

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
        if (inverso == 0){
            if (papelera == 1) {
                if (fnmatch(pattern, f->fts_name, FNM_PERIOD) == 0) {
                    rename(f->fts_path, concat("./tmp/papelera/",f->fts_name));
                    puts(concat(concat("El archivo ", f->fts_path), " fue movido a la papelera."));
                }
            }else {
                if (fnmatch(pattern, f->fts_name, FNM_PERIOD) == 0) {
                    remove(f->fts_path);
                    puts(concat(concat("El archivo ", f->fts_path), " fue eliminado."));
                }
            }
        }else if (inverso == 1){
            if (papelera == 1) {
                if (fnmatch(pattern, f->fts_name, FNM_PERIOD) == 0) {
                } else {
                    rename(f->fts_path, concat("./tmp/papelera/",f->fts_name));
                    puts(concat(concat("El archivo ", f->fts_path), " fue movido a la papelera."));
                }
            }else {
                if (fnmatch(pattern, f->fts_name, FNM_PERIOD) == 0) {
                } else {
                    remove(f->fts_path);
                    puts(concat(concat("El archivo ", f->fts_path), " fue eliminado."));
                }
            }
        }
        if (f->fts_info == FTS_DC)
            warnx("DEBUG: %s", f->fts_path);
    }

    if (errno != 0)
        err(1, "fts_read");

    if (fts_close(tree) < 0)
        err(1, "fts_close");
}

int main(int argc, char * argv[]) {
    printf("Se usa carpeta 'safe' para probar el programa. (../safe)\n");
    DIR *dr = opendir("./safe");
    printf("Bienvenido a Bulldozer!\n");

    if (argc == 1) {
        printf("DEBUG: %d\nError: Hmmm, Creo que no ingresaste ningun archivo a Eliminar", argc);
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (i + 1 <= argc) {
            if (strcmp(argv[i], "-h") == 0) {
                printf("Uso de programa\n./eliminador.c <archivo> | Utilizar '' cuando se quiere buscar por extension\n -p | Mover los archivos a una papelera (/tmp/papelera) en vez de borrar IMPORTANTE: se tiene que colocar esta flag primero\n  -h | Mostrar este menu\n  -i | Modo inverso, elimina todo lo que no coincida\n  -f /ejemplo | indicar otro directorio");
                return 0;
            }
            if (strcmp(argv[i], "-r") == 0) {
                if (strcmp(argv[2], "-f") == 0) {
                    for (int n = 4; n < argc; ++n) {
                        eliminador(concat("./safe/", argv[3]), argv[n], 0, 0);
                    }
                    return 0;
                }else{
                    for (int n = 2; n < argc; ++n) {
                        eliminador("./safe", argv[n], 0, 0);
                    }
                    return 0;
                }
            }
            if (strcmp(argv[i], "-i") == 0) {
                printf("EJECUTANDO MODO INVERSO!");
                if (strcmp(argv[2], "-f") == 0) {
                    for (int n = 4; n < argc; ++n) {
                        eliminador(concat("./safe/", argv[3]), argv[n], 1, 0);
                    }
                    return 0;
                }else{
                    for (int n = 2; n < argc; ++n) {
                        eliminador("./safe", argv[n], 1, 0);
                    }
                    return 0;
                }
            }


            if (strcmp(argv[i], "-p") == 0) {
                printf("EJECUTANDO MODO papelera!\n");
                mkdir("./tmp/papelera",0777);
                if (strcmp(argv[2], "-r") == 0) {
                    if (strcmp(argv[3], "-f") == 0) {
                        for (int n = 5; n < argc; ++n) {
                            eliminador(concat("./safe/", argv[4]), argv[n], 0, 1);
                        }
                        return 0;
                    }else{
                        for (int n = 3; n < argc; ++n) {
                            eliminador("./safe", argv[n], 0, 1);
                        }
                        return 0;
                    }
                }else if (strcmp(argv[2], "-i") == 0) {
                    printf("EJECUTANDO MODO INVERSO!");
                    if (strcmp(argv[3], "-f") == 0) {
                        for (int n = 5; n < argc; ++n) {
                            eliminador(concat("./safe/", argv[4]), argv[n], 1, 1);
                        }
                        return 0;
                    }else{
                        for (int n = 3; n < argc; ++n) {
                            eliminador("./safe", argv[n], 1, 1);
                        }
                        return 0;
                    }
                }else {
                    if (strcmp(argv[2], "-f") == 0) {
                        for (int n = 4; n < argc; ++n) {
                            eliminador(concat("./safe/", argv[3]), argv[n], 0, 1);
                        }
                        return 0;
                    }else{
                        for (int n = 2; n < argc; ++n) {
                            eliminador("./safe", argv[n], 0, 1);
                        }
                        return 0;
                    }
                }
            }
            if (strcmp(argv[i], "-f") == 0) {
                closedir(dr);
                dr = opendir(concat("./safe/", argv[2]));
            }
            i++;
        }
    }
    char * searching = argv[1];
    struct dirent *de;
    if (dr == NULL)
    {
        printf("No se pudo entrar al directorio :(\n");
        return 0;
    }

    while ((de = readdir(dr)) != NULL) {
        if (strstr(de->d_name,searching) != NULL){
            int status = remove(concat("./safe/",de->d_name));
            if (status == 0)
                printf("%s fue eliminado.\n", de->d_name);
            else
            {
                printf("Error al tratar de eliminar archivo\n%s", de->d_name);
            }
        }
    }

    closedir(dr);
    return 0;
}