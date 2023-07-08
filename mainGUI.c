#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include<stdio.h>
#include<stdlib.h>
//#include<string.h>
#define MAXN 100
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TEXTBOX_WIDTH 200
#define TEXTBOX_HEIGHT 30
#define BG_COLOR SDL_Color { 200, 200, 200, 255 }

int quit = 0, i_state = 0, textLength = 0;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
char input[10][MAXN], userpass[MAXN][2][MAXN], rankings[MAXN][2][MAXN];
int gametable[MAXN][MAXN], rankbyid[MAXN][2];
int MODE = 1, LOGEDINID = 0, SCORE = 0, CHANGED = 0, SBP = 0;

void cleanup() {
    if (font) {
        TTF_CloseFont(font);
        font = NULL;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}
SDL_Keycode get_key() {
    	SDL_Event event;
	while (true) {
		SDL_PollEvent(&event);
        	if (event.type == SDL_KEYDOWN) {
        		return event.key.keysym.sym;
        	}
        	if (event.type == SDL_QUIT) {
            		SDL_Quit();
        	}
    	}
    	return SDLK_UNKNOWN;
}
typedef struct {
    SDL_Rect rect;
    const char* label;
} Button;

void renderButton(const Button* button, SDL_Color color, SDL_Color textColor) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &button->rect);
    SDL_Surface* surface = TTF_RenderText_Solid(font, button->label, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect;
    textRect.x = button->rect.x + (button->rect.w - surface->w) / 2;
    textRect.y = button->rect.y + (button->rect.h - surface->h) / 2;
    textRect.w = surface->w;
    textRect.h = surface->h;
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
/*void renderText(const char* text, int x, int y, int h, int w) {
    SDL_Color textColor = { 0, 0, 0 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textWidth = surface->w;
    int textHeight = surface->h;

    SDL_Rect textRect = { x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}*/
void text_box(int i) {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
                	SDL_Quit();
            	}
            	else if (e.type == SDL_KEYDOWN) {
                	if (e.key.keysym.sym == SDLK_BACKSPACE && textLength > 0) {
                    		textLength--;
                    		input[i][textLength] = '\0';
                	}
			else if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
				i_state ++;
				break;
			}
            	}
           	else if (e.type == SDL_TEXTINPUT) {
                	if (textLength < sizeof(input[i]) - 1) {
                    		strcat(input[i], e.text.text);
                    		textLength = strlen(input[i]);
                	}
            	}
        }
	return;
}
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
			printf("user exist! please login.\n");
			input[1][i] = '\0';
			i_state = 2;
			for (int i = 0; i < MAXN; i ++) {
				input[2][i] = '\0';
			}
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
				i_state = 0;
				for (int i = 0; i < MAXN; i ++) {
					input[2][i] = '\0';
					input[1][i] = '\0';
				}
				return;
			}
			else {
				printf("password incorrecnt!\n");
				i_state = 2;
				for (int i = 0; i < MAXN; i ++) {
					input[2][i] = '\0';
				}
				return;
			}
		}
			
	}
	printf("username not found!\n");
	i_state = 1;
	for (int i = 0; i < MAXN; i ++) {
		input[1][i] = '\0';
		input[2][i] = '\0';
	}
	return;
}
void guest() {
	MODE = 2;
	LOGEDINID = -1;
}
Button gtb[MAXN][MAXN];
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
		SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    		SDL_RenderClear(renderer);
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				char s[MAXN];
				if (gametable[i][j] == 0) {
					printf("X");
					gtb[i][j] = { { 40 * j + 100, 40 * i + 150, 35, 35 }, " "};
				}
				else { 
					printf("%d", gametable[i][j]);
					gtb[i][j] = { { 40 * j + 100, 40 * i + 150, 35, 35 }, itoa(gametable[i][j], s)};

				}
				renderButton(&gtb[i][j], (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
			}
			printf("\n");
		}
		char s1[MAXN];
		Button scr1 = {{100, 90, 90, 50}, "Score:"};
		Button scr = { {200, 90, 40 * size - 200, 50}, itoa(SCORE, s1)};
		renderButton(&scr, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
		renderButton(&scr1, (SDL_Color){ 0, 200, 0, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
		SDL_RenderPresent(renderer);	
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (!WON && gametable[i][j] == 16 && CHANGED) {
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
		SDL_Keycode key = get_key();
		if (key == SDLK_RIGHT){
			CHANGED = right(size);
		}
		else if (key == SDLK_LEFT) {
			CHANGED = left(size);
		}
		else if (key == SDLK_UP) {
			CHANGED = up(size);
		}
		else if (key == SDLK_DOWN) {
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
		for (int i = 0; i < size; i ++) {
			for (int j = 0; j < size; j ++) {
				if (gametable[i][j] != 0)
					empty[i * size + j] = 1;
			}
		} 
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
				gametable[i / size][i % size] = vall[rvall];
				break;
			}
		}

	}
}
int rank() {
	MODE = 3;
	if (LOGEDINID == -1) {
		printf("you are guest!\n");
		return -1;
	}
	for (int i = 0; i < MAXN; i ++) {
		if (rankbyid[i][0] == LOGEDINID) {
			printf("%d- %s %d\n", i + 1, userpass[rankbyid[i][0]], rankbyid[i][1]);
			return i;
		}
	}
}
Button sBoard[20][3];
void scoreboard() {
	MODE = 3;
	if (!SBP) {
		SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    		SDL_RenderClear(renderer);
	}
	for (int i = 0; i < 10; i ++) {
		char s[MAXN], s1[MAXN];
		if (rankbyid[i][1] == '\0')
			break;
		if (!SBP) {
			printf("%d- %s %d\n", i + 1, userpass[rankbyid[i][0]], rankbyid[i][1]);
			sBoard[i][0] = { { 30, 35 * i + 30, 50, 30 }, itoa(i + 1, s) };
		    	sBoard[i][1] = { { 105 + 5, 35 * i + 30, 250 + 30, 30 }, userpass[rankbyid[i][0]][0]};
		    	sBoard[i][2] = { { 365 + 5, 35 * i + 30, 250, 30 }, itoa(rankbyid[i][1], s1)};
			renderButton(&sBoard[i][0], (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
			renderButton(&sBoard[i][1], (SDL_Color){ 0, 200, 0, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
			renderButton(&sBoard[i][2], (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray colo	
		}
	}		
	int r = rank();
	if (r > 10) {
		char s[MAXN], s1[MAXN];
		sBoard[10][0] = { { 30, 380, 50, 50 }, itoa(r + 1, s) };
		sBoard[10][1] = { { 95 + 5, 380, 250, 50 }, userpass[rankbyid[r][0]][0]};
	    	sBoard[10][2] = { { 355 + 5, 380, 250, 50 }, itoa(rankbyid[r][1], s1)};
		renderButton(&sBoard[10][0], (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
		renderButton(&sBoard[10][1], (SDL_Color){ 0, 200, 0, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
		renderButton(&sBoard[10][2], (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray colo	
	}
	if (!SBP) {
		SDL_RenderPresent(renderer);	
	}
	SBP = 1;
	SDL_Keycode key = get_key();
	if (key == SDLK_b) {
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
	printf("%d\n", i_state);
	Button usernameButton;
    	Button passwordButton; 
	if (i_state == 0) {
		usernameButton = { { 100, 200, 150, 50 }, "Username"};
    		passwordButton = { { 100, 300, 150, 50 }, "Password"};
	}
	else if (i_state == 1) {
		text_box(1);
		if (input[1][0] != '\0') {
			usernameButton = { { 100, 200, 150, 50 }, input[1]};
    			passwordButton = { { 100, 300, 150, 50 }, "Password"};
		}
		else {
			usernameButton = { { 100, 200, 150, 50 }, "Username"};
    			passwordButton = { { 100, 300, 150, 50 }, "Password"};
		}

	}
	else if (i_state == 2) {
		text_box(2);
		if (input[2][0] != '\0') {
			usernameButton = { { 100, 200, 150, 50 }, input[1]};
    			passwordButton = { { 100, 300, 150, 50 }, input[2]};
		}
		else {
			usernameButton = { { 100, 200, 150, 50 }, input[1]};
    			passwordButton = { { 100, 300, 150, 50 }, "Password"};
		}
		if (i_state == 3)
			return;
	}
	else {
		usernameButton = { { 100, 200, 150, 50 }, input[1]};
    		passwordButton = { { 100, 300, 150, 50 }, input[2]};
	}
    	Button loginButton = { { 300, 200, 100, 50 }, "Login" };
    	Button signupButton = { { 300, 300, 100, 50 }, "Signup" };
    	Button guestButton = { { 300, 400, 100, 50 }, "Guest" };
	SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    	SDL_RenderClear(renderer);
	renderButton(&usernameButton, (SDL_Color){ 255, 255, 255, 255 }, (SDL_Color){200, 200, 200, 255}); // White color
	renderButton(&passwordButton, (SDL_Color){ 255, 255, 255, 255 }, (SDL_Color){200, 200, 200, 255}); // White color
	renderButton(&loginButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&signupButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&guestButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	SDL_RenderPresent(renderer);
	//cleanup();
	if (i_state == 3) { 
		SDL_Keycode key = get_key();
		if (key == SDLK_s)
			signup(input[1], input[2]);
		else if (key == SDLK_l)
			login(input[1], input[2]);
		else if (key == SDLK_g)
			guest();
		else
			printf("invalid command\n");
		return;
	}
	if (i_state == 0) {
		SDL_Keycode key = get_key();
		if (key == SDLK_SPACE || key == SDLK_RETURN) {
			i_state ++;
			return;
		}
		if (key == SDLK_g) {
			guest();
			return;
		}
	}
	return;
}
void main_menu() {
	Button usernameButton = {{100, 200, 75, 50}, "size"};
	if (i_state != 0 && input[1][0] != '\0')
		usernameButton = {{100, 200, 75, 50}, input[1]};
	Button loginButton = { { 300, 200, 150, 50 }, "Start" };
    	Button signupButton = { { 300, 300, 150, 50 }, "Scoreboard" };
    	Button guestButton = { { 300, 400, 150, 50 }, "Logout" };
	SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
    	SDL_RenderClear(renderer);
	renderButton(&usernameButton, (SDL_Color){ 255, 255, 255, 255 }, (SDL_Color){200, 200, 200, 255}); // White color
	renderButton(&loginButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&signupButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&guestButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	SDL_RenderPresent(renderer);
	//printf("%d", i_state);
	if (i_state != 1) {
		SDL_Keycode key = get_key();
		if (key == SDLK_s) {
			start(atoi(input[1]));
		}
		else if (key == SDLK_c)
			MODE = 3;
		else if (key == SDLK_l)
			logout();
		else if (key == SDLK_RETURN)
			i_state ++;
		else 
			printf("invalid command\n");
	}
	else if (i_state == 1) {
		text_box(1);
		if (input[1][0] == '\0') {
			usernameButton = { { 100, 200, 75, 50 }, "size"};
		}
		else {
			usernameButton = { { 100, 200, 75, 50 }, input[1]};
		}
		if (i_state == 3)
			return;
	}
	renderButton(&usernameButton, (SDL_Color){ 255, 255, 255, 255 }, (SDL_Color){200, 200, 200, 255}); // White color
	renderButton(&loginButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&signupButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	renderButton(&guestButton, (SDL_Color){ 128, 128, 128, 255 }, (SDL_Color){255, 255, 255, 255}); // Gray color
	SDL_RenderPresent(renderer);
	return;
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
       		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        	return 1;
    	}
    	window = SDL_CreateWindow("2048", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    	if (window == NULL) {
        	printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        	return 1;
    	}
    	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    	if (renderer == NULL) {
        	printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        	return 1;
    	}
    	if (TTF_Init() < 0) {
        	printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
        	return 1;
    	}
        font = TTF_OpenFont("font.ttf", 28);
    	if (font == NULL) {
        	printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
        	return 1;
    	}
	//SDL_SetRenderDrawColor(renderer, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, BG_COLOR.a);
	//SDL_RenderClear(renderer);
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
