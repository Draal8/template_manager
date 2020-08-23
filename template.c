#ifndef _XOPEN_SOURCE
	#define _XOPEN_SOURCE	//pour compiler sur ma machine, turing ignorera ceci
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdnoreturn.h> //compiler en C11
#include <errno.h>

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string.h>

#define SIZE 4096
#define CHECK(op) do { if (op == -1) rerror(#op);} while(0)

noreturn void rerror(char *str) {
	if (errno != 0) {
		perror(str);
	} else {
		write(STDOUT_FILENO, str, strlen(str));
		write(STDOUT_FILENO, "\n", 1);
	}
    exit(EXIT_FAILURE);
}

void affichage ();
void selection ();
void fetch (char *number);
void copie (char *to_copy, char *filename);

int main () {
	affichage();
	selection();
    return 0;
}

void affichage () {
	DIR *dp;
	struct dirent *d;
	chdir(getenv("HOME"));
	dp = opendir ("Templates");
	int i = 0;
	while ((d = readdir(dp)) != NULL) {
		if (strcmp (d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
			printf("%i) %s\n", i, d->d_name);
			i++;
		}
	}
	printf("\nVeuillez entrer les numero des fichiers templates a copier ici :\n");
}

void selection () {
	char str[SIZE], num[5];
	if (fgets(str, SIZE, stdin) == NULL) rerror ("Le string entre na pas le bon format\n");
	int i = 0, j = 0, taille;
	taille = strlen (str);
	num[0] = '\0';
	while (i < taille && str[i] != '\n') {
		if (str[i] > 0x2F && str[i] < 0x3A) {	//entre '0' et '9' compris
			if (j <= 4) {
				num[j] = str[i];
				num[j+1] = '\0';
				j++;
			} else { rerror ("la taille du numero de template est trop grande\n"); }
		} else {
			if (j != 0) fetch(num);
			j = 0;
			num[0] = '\0';
		}
		i++;
	}
	if (j != 0) fetch(num);
}

void fetch (char *number) {
	int valeur = strtol(number, NULL, 10);
	char str[SIZE];
	
	DIR *dp;
	struct dirent *d;
	chdir(getenv("HOME"));
	dp = opendir ("Templates");
	int i = 0;
	while ((d = readdir(dp)) != NULL) {
		if (strcmp (d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0) {
			if (i == valeur) {
				if (strncat(str, getenv("HOME"), SIZE) == NULL) rerror("strncat error");
				if (strncat(str, "/Templates/", SIZE-strlen(str)) == NULL) rerror("strncat error");
				if (strncat(str, d->d_name, SIZE-strlen(str)) == NULL) rerror("strncat error");
				copie(str, d->d_name);
				str[0] = '\0';
				i++;
			} else {
				i++;
			}
		}
	}
}

void copie (char *to_copy, char *filename) {
	
	char str[SIZE], str2[SIZE];
	int d, e;
	str2[0] = '\0';
	if (strcat(str2, getenv("PWD")) == NULL) rerror("strncat error");
	d = strlen(str2);
	str2[d] = '/';
	str2[d+1] = '\0';
	if (strncat(str2, filename, SIZE-strlen(str2)) == NULL) rerror("strncat error");
	//write(STDOUT_FILENO, str2, strlen(str2));
	CHECK((d = open(to_copy, O_RDONLY)));
	CHECK((e = open(str2, O_WRONLY | O_CREAT | O_TRUNC, 0666)));
	while (read(d, str, SIZE) > 0) {
		CHECK(write(e, str, strlen(str)));
	}
	CHECK(close(d));
	CHECK(close(e));
}

