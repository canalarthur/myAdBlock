#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//renvoit l'index de la premiere occurence d'un caractere dans un char*
int strchrIndex(char c, char* array);

//renvoit l'index de la premiere occurence d'un caractere dans un char*
int strstrIndex(char* str, char* array);

//extrait une chaine de caractere entre deux index
char* substring(char* buff,int index, int indexLast);

//compte le nombre d'occurence d'une chaine de caractere dans une autre chaine de caractere
int countOccurences(char* strInitial,char* strRecherche);

//remplace les occurences d'une chaine de caractere dans une autre chaine de caractere
char* replaceSTR(char* strInitial, char* strRecherche, char* strRemplace);

//remplace les occurences d'une chaine de caractere dans une autre chaine de caractere
void str_replace(char *target, const char *toChange, const char *replacement);
