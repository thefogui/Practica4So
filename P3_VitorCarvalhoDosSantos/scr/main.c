/**
 *
 * Practica 3
 *
 * AUTHOR Vitor Carvalho
 *
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "red-black-tree.h"

#define MAXLINE      200
#define MAGIC_NUMBER 0x0133C8F9


/**
 *
 *  Menu
 *
 */


int menu()
{
    char str[5];
    int opcio;

    printf("\n\nMenu\n\n");
    printf(" 1 - Creacio de l'arbre\n");
    printf(" 2 - Emmagatzemar arbre a disc\n");
    printf(" 3 - Llegir arbre de disc\n");
    printf(" 4 - Consultar informacio d'un node\n");
    printf(" 5 - Sortir\n\n");
    printf("   Escull opcio: ");

    fgets(str, 5, stdin);
    opcio = atoi(str);

    return opcio;
}

void writeTree(Node* x, FILE *fw){
    int aux;

    if (x->right != NIL)
      writeTree(x->right, fw);

    if (x->left != NIL)
      writeTree(x->left, fw);

    if(x != NIL){ //si no es null ecribe el
        aux = strlen(x->data->key);

        fwrite(&aux, sizeof(int), 1, fw);

        fwrite(x->data->key, sizeof(char), aux, fw);

        aux =  x->data->num_vegades;
        fwrite(&aux, sizeof(int),1, fw);
    }
}

int size(Node *x){
    if (x == NIL)
        return 0;
    else
        return(size(x->left) + size(x->right) + 1); //suma +1 si no es null y hace la llamada recursiva con los nodos inferiores
}


/**
 *
 *  Main procedure
 *
 */



int main(int argc, char **argv)
{
    FILE *fp, *fpout, *fw;
    char str[MAXLINE], line[MAXLINE], *new_str, *second_str, *word, *aux_str;
    unsigned int nodesNumberToSave;
    int opcio, len_str, len, idx, id, number, i, indicator, indicator2, j;
    char **tmp = NULL;
    RBTree *tree;
    RBData *treeData;

    if (argc != 1)
        printf("Opcions de la linia de comandes ignorades\n");

    //create the tree
    srand(time(NULL));
    tree = (RBTree *) malloc(sizeof(RBTree));
    initTree(tree);

    do {
        opcio = menu();
        printf("\n\n");

       /* Feu servir aquest codi com a pantilla */
        switch (opcio) {
            case 1:
                printf("Introdueix fitxer que conte llistat fitxers PDF: ");
                fgets(str, MAXLINE, stdin);
                str[strlen(str)-1]=0;


                fp = fopen(str, "r");
                if(!fp){
                    printf( "Error llegint fitxer de dades\n");
                    return(EXIT_FAILURE);
                }
                idx = 0;
                while (fgets(str, MAXLINE, fp) != NULL){
                    if(id == 0){
                        number = atoi(str);
                        tmp = (char**)malloc(sizeof(char*)*number); //vector save all files
                        id = number;
                    }else{

                        len = strlen(str)-1;
                        tmp[idx] = malloc(len*sizeof(char));

                        if(len > 0 && str[len-1] == '\n')
                          str[--len] = '\0';
                        strncpy(tmp[idx], str, len);
                        idx = idx + 1;
                    }
                }
                fclose(fp); //close the file

                for(idx = 0; idx < id; idx++){
                    //strcpy(str, tmp[idx]);
                    strcpy(str, "pdftotext ");
                    strcat(str, tmp[idx]);
                    strcat(str, " - >&1");
                    //create the string with the str and pdftotext str - >&1
                    fpout = popen(str, "r");

                    if(!fpout){
                        printf("Error creant la canonada\n");
                        exit(EXIT_FAILURE);
                    }

                    while(fgets(line, MAXLINE, fpout) != NULL){

                        word = strtok(line, " ,.-'"); //separate the sentence in words
                        while(word != NULL){
                            len = strlen(word); //size of the string
                            j = 0;

                            new_str = (char*)malloc(sizeof(char)*len); //allocate memory fot the new string

                            indicator = 0; //indicator for the first word
                            indicator2 = 0; //indicator fot the second word
                            for (i = 0; i < len; i++){
                                if(!ispunct(word[i]) && !isdigit(word[i]) && indicator == 0){
                                    new_str[j] = tolower(word[i]); //to lower the char
                                    j++;
                                }else{
                                    if(!ispunct(word[i]) && !isdigit(word[i]) && indicator == 1){
                                        if(word[i] != '\n' && word[i] != '"'){
                                            if(word[i] != '-'){
                                                if (indicator2 == 0)
                                                    second_str = (char *)malloc(sizeof(char)*(len-i)); //allocate the memory for the second string
                                                second_str[indicator2] = tolower(word[i]);
                                                indicator2 = indicator2 + 1;
                                            }
                                        }
                                    }
                                    indicator = 1;
                                }
                            }

                            //Insertion in the tree
                            aux_str = (char*)malloc(sizeof(char)* (j+1));
                            strcpy(aux_str, new_str); //make a copy to put in the tree
                            treeData = findNode(tree, aux_str); //put the string in the tree
                            if(treeData != NULL){
                                treeData->num_vegades++;
                            }else{
                                treeData = malloc(sizeof(RBData));
                                treeData->key = aux_str;
                                treeData->num_vegades = 1;
                                insertNode(tree, treeData);
                            }

                            if(indicator2 != 0) {
                                free(aux_str);
                                aux_str = (char*)malloc(sizeof(char)*(indicator2+1));
                                strcpy(aux_str, second_str); //make a copy to put in the tree
                                if(strcmp(aux_str, "\n") != 0 && strcmp(aux_str, "-") != 0){
                                    treeData = findNode(tree, aux_str); //put the string in the tree
                                    if(treeData != NULL){
                                        treeData->num_vegades++;
                                    }else{
                                        treeData = malloc(sizeof(RBData));
                                        treeData->key = aux_str;
                                        treeData->num_vegades = 1;
                                        insertNode(tree, treeData);
                                    }
                                }
                            }

                            word = strtok(NULL, " ,.-'"); //catch the next word of the setence
                        }

                    }
                    fclose(fpout);
                }
                break;
            case 2:
                printf("Introdueix el nom de fitxer en el qual es desara l'arbre: ");
                fgets(str, MAXLINE, stdin);
                str[strlen(str)-1]=0;

                nodesNumberToSave = size(tree->root);
                printf("%d\n", nodesNumberToSave);
                fw = fopen(str, "w");
                if(!fw){
                    printf( "Error llegint fitxer de dades\n");
                    return(EXIT_FAILURE);
                }
                len = sprintf(aux_str, "%d", MAGIC_NUMBER);
                fwrite(aux_str, 4, 1, fw);
                fwrite(&nodesNumberToSave, 4, 1, fw);
                writeTree(tree->root, fw);
                fclose(fw);
                break;

            case 3:
                printf("Introdueix nom del fitxer amb l'arbre: ");
                fgets(str, MAXLINE, stdin);
                str[strlen(str)-1]=0;

                fw = fopen(str, "r");
                if(!fw){
                    printf( "Error llegint fitxer de dades\n");
                    return(EXIT_FAILURE);
                }
                aux_str = (char*)malloc(sizeof(char)*MAXLINE);
                fread(aux_str, 4, 1, fw);
                printf("%s\n", aux_str);
                fread(&nodesNumberToSave, 4, 1, fw);
                printf("%d\n", nodesNumberToSave);
                for(idx = 0; idx < nodesNumberToSave; idx++){
                    fread(&number, 4, 1, fw);
                    printf("%d\n", nodesNumberToSave);

                    aux_str = (char*)malloc(sizeof(char)*number);
                    fread(aux_str, sizeof(char), number,fw);
                    printf("%s\n", aux_str);

                    fread(&number, 4, 1, fw);
                    printf("%d\n", number);
                    treeData = findNode(tree, aux_str); //put the string in the tree
                    if(treeData != NULL){
                        treeData->num_vegades++;
                    }else{
                        treeData = malloc(sizeof(RBData));
                        treeData->key = aux_str;
                        treeData->num_vegades = number;
                        insertNode(tree, treeData);
                    }
                }
                fclose(fw);
                break;

            case 4:
                printf("Introdueix la paraula: ");
                fgets(str, MAXLINE, stdin);
                str[strlen(str)-1]=0;

                len_str = strlen(str)-1;
                aux_str = malloc(sizeof(char)*len_str);
                strcpy(aux_str, str);
                treeData = findNode(tree, aux_str);

                if(treeData){
                    printf("El string %s apareix %d cops a l'arbre.\n", treeData->key, treeData->num_vegades);
                }else{
                    printf("La parauala %s no es troba en el arbol", aux_str);
                }

                break;

            case 5:
                deleteTree(tree);
                free(tree);
                exit(1);
                break;

            default:
                printf("Opcio no valida\n");

        } /* switch */
    }
    while (opcio != 5);

    return 0;
}
