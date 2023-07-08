#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define MAXN 100

char input[10][MAXN], userpass[MAXN][2][MAXN], rankings[MAXN][2][MAXN];
int gametable[MAXN][MAXN], rankbyid[MAXN][2];
int MODE = 1, LOGEDINID = 0, SCORE = 0, CHANGED = 0, SBP = 0;

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
void sortArray(int array[][2], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j][1] < array[j + 1][1]) {
                swap(&array[j][1], &array[j + 1][1]);
                swap(&array[j][0], &array[j + 1][0]);
            }
        }
    }
}
int istowpow(int x) {
	while (x) {
		if (x != 1 && x % 2 == 1)
			return 0;
		else 
			x /= 2;
	}
	return 1;
}
void reverse(char s[]) {
	int i, j;
	char c;
	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
char* itoa(int n, char s[]) {
	int i, sign;
	if ((sign = n) < 0)
		n = -n;
	i = 0;
	do {
		s[i++] = n % 10 + '0';
	} while ((n /= 10) > 0);
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
	return s;
 }
int compress(int size) {
	int changed = 0;
	int mat[size][size];
	for (int i = 0; i < size; i ++) {
		int pos = 0;
		for (int j = 0; j < size; j ++) {
			mat[i][j] = 0;
			if (gametable[i][j] != 0) {
				mat[i][pos] = gametable[i][j];
				if (j != pos)
					changed = 1;
				pos ++;
			}
		}
	}
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++)
			gametable[i][j] = mat[i][j];
	}
	return changed;
}
int merge(int size) {
	int changed = 0;
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size - 1; j ++) {
			if (gametable[i][j] == gametable[i][j + 1] && gametable[i][j] != 0) {
				gametable[i][j] = gametable[i][j] * 2;
				SCORE += gametable[i][j];
				gametable[i][j + 1] = 0;
				changed = true;
			}
		}
	}
	return changed;
}
void reverse(int size) {
	int mat[size][size];
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++)
			mat[i][j] = gametable[i][size - j - 1];
	}
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++)
			gametable[i][j] = mat[i][j];
	}
}
void transpose(int size) {
	int mat[size][size];
	for (int i = 0;i < size; i ++) {
		for (int j = 0; j < size; j ++)
			mat[i][j] = gametable[j][i];
	}
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++)
			gametable[i][j] = mat[i][j];
	}
}
int left(int size) {
	int changed1 = compress(size);
	int changed2 = merge(size);
	int changed = changed1 || changed2;
	compress(size);
	return changed;
}
int right(int size) {
	reverse(size);
	int changed = left(size);
	reverse(size);
	return changed;
}
int up(int size) {
	transpose(size);
	int changed = left(size);
	transpose(size);
	return changed;
}
int down(int size) {
	transpose(size);
	int changed = right(size);
	transpose(size);
	return changed;
}
void get_input() {
	for (int i = 0; i < 10; i ++) {
		for (int j = 0; j < MAXN; j ++) 
			input[i][j] = '\0';
	}
	int p = 0, q = 0;
	while (1) {
		char c;
		scanf ("%c", &c);
		if (c == '\n')
			break;
		else if (c == ' ') {
			p ++;
			q = 0;
		}
		else {
			input[p][q] = c;
			q ++;
		}
	}
}
void putfileinstring() {
	FILE *fptr, *fp;
    	char c;
	int i = 0, j = 0, k = 0;
    	fptr = fopen("userpass", "r");
	if (fptr == NULL) {
        	printf("Cannot open the file.\n");
        	return;
    	}
    	while (c != EOF) {
        	c = fgetc(fptr);
		if (c == ' ') {
			j ++;
			k = 0;
		}
		else if (c == '\n') {
			i ++;
			j = 0;
			k = 0;
		}
		else if (c == EOF) {
				break;
		}
		else {
			userpass[i][j][k] = c;
			k ++;
		}
    	}
	fclose(fptr);
	fp = fopen("rankings", "r");
	i = 0, j = 0, k = 0;
	c = '\0';
    	while (c != EOF) {
        	c = fgetc(fptr);
		if (c == ' ') {
			j ++;
			k = 0;
		}
		else if (c == '\n') {
			i ++;
			j = 0;
			k = 0;
		}
		else if (c == EOF) {
			break;
		}
		else {
			rankings[i][j][k] = c;
			k ++;
		}
	}
	fclose(fp);
    	return;
}
void insertstr(char path[], char str[]) {
	FILE *fp;
	long Fin, lSize;
	int position = 0, line_num = 0;
	char c;
	fp = fopen(path, "r+");
	if (fp == NULL) {
        	printf("Cannot open the file.\n");
        	return;
    	}
	fseek(fp, 0L, SEEK_END);
	fputs(str, fp);
	fclose(fp);
	return;
}
void signup(char username[], char password[]) {
	for (int i = 0; i < MAXN; i ++) {
		if (!strcmp(username, userpass[i][0])) {
			printf("user exist!\n");
			return;
		}
	}
	insertstr("userpass", username);
	insertstr("userpass", " ");
	insertstr("userpass", password);
	insertstr("userpass", "\n");
	printf("done! please login to continue.\n");
	return;
}
void login(char username[], char password[]) {
	for (int i = 0; i < MAXN; i ++) {
		if (!strcmp(username, userpass[i][0])) {
			if (!strcmp(password, userpass[i][1])) {
				MODE = 2;
				LOGEDINID = i;
				printf("welcome to 2048! %s\n", username);
				return;
			}
			else {
				printf("password incorrecnt!\n");
				return;
			}
		}
			
	}
	printf("username not found!\n");
	return;
}
void guest() {
	MODE = 2;
	LOGEDINID = -1;
}
int start(int size) {
	if (size < 3) {
		printf("invalid size, size must be grater than 2!\n");
		return 0;
	}
	int vall[2] = {2, 4}, empty[size * size];
	char score[MAXN], logedinid[MAXN];
	for (int i = 0; i < size; i ++) {
		for (int j = 0; j < size; j ++)
			gametable[i][j] = 0;
	}
	int rcell = rand() % int(size * size), rvall = rand() % 2, LOSE = 0, countE = 0, WON = 0;
	gametable[rcell / size][rcell % size] = 2;
	empty[rcell] = 1;
	for (int i = 0; i < size * size; i ++) {
		if (empty[i] == 0)
			countE ++;
	}
	rcell = rand() % countE, rvall = rand() % 2;
	countE = 0;
	for (int i = 0; i < size * size; i ++) {
		if (empty[i] == 0)
			countE ++;
		if (countE == rcell) {
			gametable[i / size][i % size] = 2;
			break;
		}
	}
	while (LOSE == 0) {
		printf("%d\n", SCORE);
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (gametable[i][j] == 0)
					printf("X");
				else 
					printf("%d", gametable[i][j]);
			}
			printf("\n");
		}
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (!WON && gametable[i][j] == 2048 && CHANGED) {
					printf("you won!!!!!!!\n");
					WON = 1;
				}
			}
		}
		LOSE = 1;
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (gametable[i][j] == 0) 
					LOSE = 0;
			}
		}
		for (int i = 0; i < size - 1; i ++) {
			for (int j = 0; j < size - 1; j ++) {
				if (gametable[i][j] == gametable[i + 1][j] || gametable[i][j] == gametable[i][j + 1])
					LOSE = 0;
			}	
		}
		for (int i = 0; i < size - 1; i ++) {
			if (gametable[size - 1][i] == gametable[size - 1][i + 1] || gametable[i][size - 1] == gametable[i + 1][size - 1])
				LOSE = 0;
		}
		if (LOSE == 1) {
			printf("you lose!\n");
			if (LOGEDINID != -1) {
				insertstr("rankings", itoa(LOGEDINID, logedinid));
				insertstr("rankings", " ");
				insertstr("rankings", itoa(SCORE, score));
				insertstr("rankings", "\n");
			}
			return SCORE;
		}
		//printf("do your move!\n");
		CHANGED = 0;
		get_input();
		if (!strcmp(input[0], "right")){
			CHANGED = right(size);
		}
		else if (!strcmp(input[0], "left")) {
			CHANGED = left(size);
		}
		else if (!strcmp(input[0], "up")) {
			CHANGED = up(size);
		}
		else if (!strcmp(input[0], "down")) {
			CHANGED = down(size);
		}
		else if (!strcmp(input[0], "put")) {
			if (atoi(input[1]) >= size || atoi(input[2]) >= size)
				printf("coordinate out of range!\n");
			else if (gametable[atoi(input[1])][atoi(input[2])] != 0)
				printf("cell is not empty!\n");
			else if (!istowpow(atoi(input[3])))
				printf("invalid value!\n");
			else
				gametable[atoi(input[1])][atoi(input[2])] = atoi(input[3]);
			continue;
		}
		else {
			printf("invalid command!\n");
			continue;
		}
		for (int i = 0; i < size * size; i ++) {
			empty[i] = 0;
		}
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (gametable[i][j] == 1)
					empty[i * size + j] = 1;
			}
		} 
		for (int i = 0; i < size * size; i ++) {
			if (empty[i] == 0)
				countE ++;
		}
		rcell = rand() % countE, rvall = rand() % 2;
		int cc = 0;
		for (int i = 0; i < size * size; i ++) {
			if (empty[i] == 0)
				cc ++;
			if (cc == rcell) {
				gametable[i / size][i % size] = vall[rvall];
				break;
			}
		}

	}
}
void rank() {
	MODE = 3;
	if (LOGEDINID == -1) {
		printf("you are guest!\n");
		return;
	}
	for (int i = 0; i < MAXN; i ++) {
		if (rankbyid[i][0] == LOGEDINID) {
			printf("%d- %s %d\n", i + 1, userpass[rankbyid[i][0]], rankbyid[i][1]);
			return;
		}
	}
}
void scoreboard() {
	MODE = 3;
	for (int i = 0; i < 10; i ++) {
		if (rankbyid[i][1] == '\0')
			break;
		if (!SBP)
			printf("%d- %s %d\n", i + 1, userpass[rankbyid[i][0]], rankbyid[i][1]);
	}
	SBP = 1;
	get_input();
	if (!strcmp(input[0], "myrank"))
		rank();
	else if (!strcmp(input[0], "back")) {
		SBP = 0;
		MODE = 2;
	}
	else
		printf("invalid command\n");
}
void logout() {
	printf("logedout!\n");
	MODE = 1;
	LOGEDINID = -1;
}
void registeration() {
	get_input();
	if (!strcmp(input[0], "signup"))
		signup(input[1], input[2]);
	else if (!strcmp(input[0], "login"))
		login(input[1], input[2]);
	else if (!strcmp(input[0], "guest"))
		guest();
	else
		printf("invalid command\n");
}
void main_menu() {
	get_input();
	if (!strcmp(input[0], "start"))
		start(atoi(input[1]));
	else if (!strcmp(input[0], "scoreboard"))
		scoreboard();
	else if (!strcmp(input[0], "logout"))
		logout();
	else 
		printf("invalid command\n");
}

int main() {
	while (1) {
		putfileinstring();
		for (int i = 0; i < 10; i ++) {
			rankbyid[i][0] = atoi(rankings[i][0]);
			rankbyid[i][1] = atoi(rankings[i][1]);
		}
		sortArray(rankbyid, MAXN);
		if (MODE == 1)
			registeration();
		else if (MODE == 2)
			main_menu();
		else if (MODE == 3)
			scoreboard();
		else
			break;
	}
}
