#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <dirent.h>
#include "frozen.h"
#include <ctype.h>

#define MAXDATASIZE 100000 // max number of bytes we can get at once

const char NExemplaresFolder[50] = "n_exemplares/";
const char folder[30] = "db_movies/";

char *readJson(char *filename) {
    char *buffer = 0;
    long length;
    FILE *f = fopen(filename, "rb");

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = (char *)malloc(length);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        fclose(f);
    }
    return buffer;
}

//receives json struct and fields to print
//return the buffer with the output
void print_fields(struct json_token *arr, char fields[][30], int n_fields, char *buf) {
    char string[MAXDATASIZE] = "";
    char string2send[MAXDATASIZE] = "";
    struct json_token *tok;
    int i;

    for (i = 0; i < n_fields; ++i) {
        tok = find_json_token(arr, fields[i]);
        sprintf(string, "%s: %.*s \n", fields[i], tok->len, tok->ptr);
        strcat(string2send, string);
    }
    strcat(buf, string2send);
}

void append_count_to_buf(char *file, char *buf) {
    FILE *fd;
    char exemplaresFileString[60];
    char str[30];
    char finalStr[30];

    strcpy(exemplaresFileString, NExemplaresFolder);
    strcat(exemplaresFileString, file);

    fd = fopen(exemplaresFileString, "r");

    if (fd) {
        fgets(str, sizeof(str), fd);

        sprintf(finalStr, "ID: %s Exemplares: %s\n", file, str);

        strcat(buf, finalStr);

        fclose(fd);
    } else {
        printf("Erro abertura: %s\n", exemplaresFileString);
    }
}

int movie_count (char *file) {
    FILE *fd;
    char exemplaresFileString[60];
    char str[30];

    strcpy(exemplaresFileString, NExemplaresFolder);
    strcat(exemplaresFileString, file);

    fd = fopen(exemplaresFileString, "r");

    if (fd) {
        fgets(str, sizeof(str), fd);
        fclose(fd);
        return atoi(str);
    } else {
        printf("Erro abertura: %s\n", exemplaresFileString);
    }

    return -1;
}

int set_movie_count (char *file, int new_count) {
    FILE *fd;
    char exemplaresFileString[60];
    char str[30];

    strcpy(exemplaresFileString, NExemplaresFolder);
    strcat(exemplaresFileString, file);

    fd = fopen(exemplaresFileString, "w");

    if (fd) {
        sprintf(str, "%d", new_count);
        fputs(str,fd);
        fclose(fd);
        return 0;
    } else {
        printf("Erro abertura: %s\n", exemplaresFileString);
    }

    return -1;

}

//1- listar todos os títulos dos filmes e o ano de lançamento
int listar_filme(char *buf) {
    struct dirent *ent;
    DIR *dir;
    char *json;
    char cur_folder[100] = "";
    char path[200] = "";
    strcpy(path, folder);

    char fields[10][30];

    struct json_token *arr;
    if ((dir = opendir(path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;

            strcpy(cur_folder, path);
            strcat(cur_folder, ent->d_name);
            json = readJson(cur_folder);
            // Tokenize json string, fill in tokens array
            arr = parse_json2(json, strlen(json));

            int n_fields = 2;
            strcpy(fields[0], "Title");
            strcpy(fields[1], "Year");
            print_fields(arr, fields, n_fields, buf);

            // Do not forget to free allocated tokens array
            free(arr);
        }
        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("Erro abrir diretorio");
        return 1;
    }

    return 0;
}

//2- listar todos os títulos dos filmes e o ano de lançamento de um gênero determinado
int listar_filme_genero(char *genre, char *buf) {
    struct dirent *ent;
    DIR *dir;
    char *json;

    char cur_folder[100] = "";
    char path[200] = "";
    strcpy(path, folder);

    char fields[10][30];

    struct json_token *arr, *tok;
    if ((dir = opendir(path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;

            strcpy(cur_folder, path);
            json = readJson(strcat(cur_folder, ent->d_name));
            // Tokenize json string, fill in tokens array
            arr = parse_json2(json, strlen(json));
            tok = find_json_token(arr, "Genre");

            if (strcasestr(tok->ptr, genre) != NULL) {
                // Search for parameter "bar" and print it's value
                int n_fields = 2;
                strcpy(fields[0], "Title");
                strcpy(fields[1], "Year");
                print_fields(arr, fields, n_fields, buf);
            }
            // Do not forget to free allocated tokens array
            free(arr);
        }

        if (strlen(buf) == 0) {
            strcpy(buf, "Erro: Gênero não encontrado\n");
        }

        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("");
        return 1;
    }

    return 0;
}

// 3- listar todas as informações de todos os filmes;
int listar_toda_info_filme(char *buf) {
    struct dirent *ent;
    DIR *dir;
    char *json;
    char path[200] = "";
    strcpy(path, folder);
    char cur_folder[100] = "";

    char fields[10][30];

    struct json_token *arr;
    if ((dir = opendir(path)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;

            strcpy(cur_folder, path);
            json = readJson(strcat(cur_folder, ent->d_name));
            // Tokenize json string, fill in tokens array
            arr = parse_json2(json, strlen(json));

            int n_fields = 8;
            strcpy(fields[0], "Title");
            strcpy(fields[1], "id");
            strcpy(fields[2], "Year");
            strcpy(fields[3], "Genre");
            strcpy(fields[4], "Runtime");
            strcpy(fields[5], "Plot");
            strcpy(fields[6], "Director");
            strcpy(fields[7], "Actors");
            print_fields(arr, fields, n_fields, buf);

            append_count_to_buf(ent->d_name, buf);
            // Do not forget to free allocated tokens array
            free(arr);
        }

        if (strlen(buf) == 0) {
            strcpy(buf, "Erro\n");
        }

        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("Erro abertura diretorio");
        return 1;
    }

    return 0;
}

//4- dado o identificador de um filme, retornar a sinopse do filme;
int sinopse_filme(char *c, char *buf) {
    struct dirent *ent;
    DIR *dir;
    char *json;
    char path[200] = "";

    char fields[10][30];

    struct json_token *arr;
    if ((dir = opendir(folder)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.' || strcmp(ent->d_name, c) != 0) continue;

            strcpy(path, folder);
            strcat(path, ent->d_name);
            json = readJson(path);
            // Tokenize json string, fill in tokens array
            arr = parse_json2(json, strlen(json));

            int n_fields = 1;

            strcpy(fields[0], "Plot");
            print_fields(arr, fields, n_fields, buf);

            // Do not forget to free allocated tokens array
            free(arr);

            return 1;
        }

        if (strlen(buf) == 0) {
            strcpy(buf, "Erro\n");
        }

        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("");
        return 1;
    }

    //nao encontrou
    return 0;
}

//5- dado o identificador de um filme, retornar todas as informações deste filme;
int info_filme(char *c, char *buf) {
    struct dirent *ent;
    DIR *dir;
    char *json;
    char path[200] = "";
    char fields[10][30];
    struct json_token *arr;

    if ((dir = opendir(folder)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.' || strcmp(ent->d_name,c) != 0) continue;

            strcpy(path, folder);
            json = readJson(strcat(path, ent->d_name));
            // Tokenize json string, fill in tokens array
            arr = parse_json2(json, strlen(json));

            int n_fields = 8;
            strcpy(fields[0], "Title");
            strcpy(fields[1], "id");
            strcpy(fields[2], "Year");
            strcpy(fields[3], "Genre");
            strcpy(fields[4], "Runtime");
            strcpy(fields[5], "Plot");
            strcpy(fields[6], "Director");
            strcpy(fields[7], "Actors");
            print_fields(arr, fields, n_fields, buf);

            append_count_to_buf(ent->d_name, buf);

            // Do not forget to free allocated tokens array
            free(arr);

            return 1;
        }

        if (strlen(buf) == 0) {
            strcpy(buf, "Erro: Id não encontrado\n");
        }

        closedir(dir);
    }
    else {
        /* could not open directory */
        perror("");
        return 1;
    }

    //nao encontrou
    return 0;
}

//6 - Mostra Num exemplares disponiveis para cada filme
void contagem_exemplares(char *buf) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(NExemplaresFolder)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] == '.') continue;
            append_count_to_buf(ent->d_name, buf);
        }
    } else {
        perror("Erro abertura diretorio");
    }
}

//8 - Remove Filme
void remove_movie_with_id(char *id, char *buf) {
    int current_count = movie_count(id);

    if (current_count > 0) {
        if (set_movie_count(id, current_count - 1) == 0) {
            strcpy(buf, "Exemplar locado\n");
        } else {
            strcpy(buf, "Erro do sistema\n");
        }
    } else {
        strcpy(buf, "Nenhum Exemplar Disponível.\n");
    }
}

void return_movie_with_id(char *id, char *buf) {
    int current_count = movie_count(id);

    if (set_movie_count(id, current_count + 1) == 0) {
        strcpy(buf, "Exemplar devolvido\n");
    } else {
        strcpy(buf, "Erro do sistema\n");
    }
}

void set_num_exemplares(char *id, char *novoTotal, char *buf) {
    if (set_movie_count(id, atoi(novoTotal)) == 0) {
        strcpy(buf, "Num Exemplares Alterado\n");
    } else {
        strcpy(buf, "Erro do sistema\n");
    }
}

void client_menu(char *string) {
    strcpy(string, "Bem vindo ao servidor da Loucadora\n Opções:\n1-Listar todos os títulos dos filmes e o ano de lançamento\n2-Listar todos os títulos dos filmes e o ano de lançamento de um gênero determinado\n3-Listar todas as informações de todos os filmes\n4-Sinopse de um filme\n5-Toda informação de um filme\n6-Número de exemplares em estoque\n7-Devolver Filme\n8-Alugar filme\n9-admin\nQ-Sair\n");
}
