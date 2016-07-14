#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define DEFAULT_TICKS 	10
#define SLEEP_TIME 	100000

#define WIN_WIDTH	30

#define MAGENTA	1
#define RED	2
#define GREEN	3
#define YELLOW	4
#define BLUE	5
#define CYAN	6
#define WHITE	7

/**
 * move(row,col)		move cursor
 * mvprintw(row,col,string)	move and printw
 * getmaxyx(stdscr,row,col)	get number of rows and columns
 * getyx(stdscr,y,x)		get current cursor position
 * scanw()			get formatted input
 * getstr()			get strings
 * chgat()			[Check online] changes attributes without moving cursor
 */ 

/**
 * A_NORMAL
 * A_STANDOUT	Best highlighting mode of the terminal
 * A_UNDERLINE
 * A_REVERSE	Reverse video = Invert colors
 * A_BLINK	
 * A_DIM	Half bright
 * A_BOLD	Extra bright/bold
 * A_PROTECT	Protected mode
 * A_INVIS	Invisible mode
 * A_ALTCHARSET	Alternate character set
 * A_CHARTEXT	Bit-mask to extract a character
 * COLOR_PAIR(n)	Color-pair number n;
 */

/**
 * RULES OF THE UNIVERSE
 * ---------------------
 *  1. Any cell with less than 2 neighbors dies
 *  2. Any cell with 2 or 3 neighbors survives
 *  3. Any cell with more than 3 neighbors dies
 *  4. Any dead cell with exactly 3 neighbors becomes alive
 */

int ch = ' ';
int rows, cols;
int xs, ys;
int row, col;
int x,y;
int area;
int ticks;
int totalTicks = 0;
int color;
WINDOW *win = NULL;
WINDOW *game = NULL;

void drawCell(int x,int y);
void eraseCell(int x, int y);
int checkNeighbors(int x, int y, int cells[]);
void update(int cells[], int size);
void updateWin(int cells[], int size);

void initCurses();
void showSplash();

void render(int cells[], int size)
{
	int c;
	for(c = 0; c < size; c++)
	{
		if(cells[c] == 0) eraseCell(c % ys, c / ys);
		else drawCell(c % ys, c / ys);
	}
}

WINDOW *createWindow(int width, int height, int starty, int startx);
void destroyWindow(WINDOW *win);

int main(int argc, char *argv[])
{
	if(argc >= 2)
	{
		if(strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"--help") == 0)
		{
			printf("\nConway\n");
			printf("Copyright 2015 Harley Wiltzer\n\n");
			printf("conway:\t\tOpen conway with a random color\n");
			printf("conway 1:\tOpen conway blue theme\n");
			printf("conway 2:\tOpen conway red theme\n");
			printf("conway 3:\tOpen conway green theme\n");
			printf("conway 4:\tOpen conway yellow theme\n");
			printf("conway 5:\tOpen conway light blue theme\n");
			printf("conway 6:\tOpen conway orange theme\n");
			printf("conway 7:\tOpen conway white theme\n");
			printf("\n");
			printf("While running conway...\n");
			printf("=======================\n");
			printf("q:\t\tClose conway\n");
			printf("space:\t\tActivate/Kill cell\n");
			printf("enter:\t\tStart ticking\n");
			printf("h:\t\tMove cursor left\n");
			printf("j:\t\tMove cursor down\n");
			printf("k:\t\tMove cursor up\n");
			printf("l:\t\tMove cursor right\n");
			printf("d:\t\tClear all cells\n");
			printf("=:\t\tIncrease the amount of ticks by 10\n");
			printf("-:\t\tDecrease the amount of ticks by 10\n");
			printf("+:\t\tIncrease the amount of ticks by 1\n");
			printf("_:\t\tDecrease the amount of ticks by 1\n");
			return 0;
		}
	}
	int i, c;
	
	initCurses();
	showSplash();
	totalTicks = 0;	
	refresh();
	getch();

	win = createWindow(WIN_WIDTH,rows,0,cols - WIN_WIDTH - 1);
	cols = cols - WIN_WIDTH - 1;
	xs = rows;
	ys = cols / 2;
	area = rows * cols;
	game = createWindow(cols - 1,rows,0,0);
	ticks = DEFAULT_TICKS;
	
	int cells[area / 2];
	for(i = 0; i < area/2; i++)
	{
		cells[i] = 0;
	}

	x = y = row = col = 0;
	if(has_colors() == 0) wprintw(game,"NO COLORS\n");

	if(argc < 2)
	{
		srand(time(NULL));
		color = rand()%7 + 1;
	}
	else
	{
		if(atoi(argv[1]) > 7 || atoi(argv[1]) < 1)
		{
			srand(time(NULL));
			color = rand()%7 + 1;
		}
		else 
		{
			color = atoi(argv[1]);
		}
	}

	x = ys/2;
	y = xs/2;
	wmove(game,y, x * 2);
	x = 2;
	y = 2;
	wrefresh(game);
	updateWin(cells,area/2);
	wmove(game,y,x * 2);

	x = ys/2;
	y = xs/2;

	wmove(game,y,x * 2);

	while((ch = getch()) != 'q')
	{
		wclear(game);
		wclear(win);
		int r, c;
		
		render(cells,area/2);	
		wmove(game,y, x * 2);

		switch(ch)
		{
			case 'h':
				if (x % ys == 0) break;
				x -= 1;
				wmove(game,y, x * 2);
				wrefresh(game);
				break;
			case 'j':
				if ((y + 1) % xs == 0) break;
				y += 1;
				wmove(game,y, x * 2);
				wrefresh(game);
				break;
			case 'k':
				if (y % xs == 0) break;
				y -= 1;
				wmove(game,y, x * 2);
				wrefresh(game);
				break;
			case 'l':
				if((x+1)%ys == 0) break;
				x += 1;
				wmove(game,y, x * 2);
				wrefresh(game);
				break;
			case ' ':
				cells[y * ys + x] = !cells[y * ys + x];
				if(cells[y * ys + x]) drawCell(x,y);
				else eraseCell(x,y);
				updateWin(cells,area/2);
				break;
			case '\n':
				update(cells, area/2);
				break;
			case 'd':
				memset(cells,0,area/2);
				int q;
				for(q = 0; q < area/2; q++)
				{
					cells[q] = 0;
				}
				totalTicks = 0;
				render(cells, area/2);
				updateWin(cells,area/2);
				break;
			case '=':
				ticks += 10;
				updateWin(cells,area/2);
				break;
			case '-':
				if(ticks >= 20) ticks -= 10;
				updateWin(cells,area/2);
				break;
			case '+':
				ticks++;
				updateWin(cells,area/2);
				break;
			case '_':
				if(ticks > 2) ticks--;
				updateWin(cells, area/2);
				break;
			default:
				break;
		}
		updateWin(cells,area/2);
		wrefresh(game);
	}
	destroyWindow(game);
	destroyWindow(win);
	endwin();
	return 0;
}

WINDOW *createWindow(int width, int height, int starty, int startx)
{
	WINDOW *local_win;
	local_win = newwin(height,width,starty,startx);
//	wmove(local_win,1,1);
//	x = y = 1;
	wrefresh(local_win);
	return local_win;
}

void destroyWindow(WINDOW *win)
{
	wborder(win,' ', ' ', ' ',' ', ' ', ' ', ' ', ' ');
	wrefresh(win);
	delwin(win);
}

void drawCell(int x, int y)
{
	wattron(game,A_REVERSE|COLOR_PAIR(color));
	mvwprintw(game,y,x*2," ");
	mvwprintw(game,y,x * 2 + 1," ");
	wattroff(game,A_REVERSE|COLOR_PAIR(color));
	wmove(game,y,x * 2);
}

void eraseCell(int x, int y)
{
	wattron(game,COLOR_PAIR(color));
	mvwprintw(game,y,x*2," ");
	mvwprintw(game,y, x * 2 + 1, " ");
	wattroff(game,COLOR_PAIR(color));
	wmove(game,y,x * 2);
}

int checkNeighbors(int x, int y, int cells[])
{
	int n = 0;
	/*LEFT*/
	if(x != 0)
	{
		if(cells[x - 1 + y * ys] == 1) ++n;
		/*TOP LEFT*/
		if(y != 0)
		{
			if(cells[x - 1 + (y-1) * ys] == 1) ++n;
		}
		/*BOTTOM LEFT*/
		if(y < xs)
		{
			if(cells[x - 1 + (y+1) * ys] == 1) ++n;
		}
	}
	/*RIGHT*/
	if(x < ys - 1)
	{
		if(cells[x + 1 + y * ys] == 1) ++n;
		/*TOP RIGHT*/
		if(y != 0)
		{
			if(cells[x + 1 + (y-1) * ys] == 1) ++n;
		}
		/*BOTTOM RIGHT*/
		if(y < xs)
		{
			if(cells[x + 1 + (y+1) * ys] == 1) ++n;
		}
	}
	/*UP*/
	if(y != 0)
	{
		if(cells[x + (y-1) * ys] == 1) ++n;
	}
	/*DOWN*/
	if(y < xs)
	{
		if(cells[x + (y+1) * ys] == 1) ++n;
	}

	return n;
}

void update(int cells[], int size)
{
	int numticks = ticks;
	while(ticks > 0)
	{
		int c;
//		wclear(game);
//		wclear(win);
		render(cells,size);
		int oldcells[size];
		for(c = 0; c < size; c++)
		{
			oldcells[c] = cells[c];
		}
		for(c = 0; c < size; c++)
		{
			if(oldcells[c] != 0) //Check if we're alive...
			{
				/*RULE 1*/
				if(checkNeighbors(c % ys, c / ys, oldcells) < 2) cells[c] = 0;
				/*RULE 3*/
				if(checkNeighbors(c % ys, c / ys, oldcells) > 3) cells[c] = 0;
				/*We don't need to check for rule 2, it's implicit*/
			}
			else //We're dead
			{
				if(checkNeighbors(c % ys, c / ys, oldcells) == 3) cells[c] = 1;
			}
		}
		updateWin(cells,size);
		wrefresh(game);
		usleep(SLEEP_TIME);
		ticks--;
		totalTicks++;
	}
	ticks = numticks;
}

void updateWin(int cells[], int size)
{
	int c;
	int cellsAlive = 0;
	wclear(win);
	wattron(win,COLOR_PAIR(color));
	box(win,0,0);
	for (c = 0; c < size; c++)
	{
		if(cells[c] == 1) ++cellsAlive;
	}
	wattroff(win,COLOR_PAIR(color));
	mvwprintw(win,rows/2 - 2,2,"Board Size: [%d, %d]",xs,ys);
	mvwprintw(win,rows/2 - 1,2,"Cells Alive: %d",cellsAlive);
	mvwprintw(win,rows/2,2,"Ticks Remaining: %d",ticks);
	mvwprintw(win,rows/2 + 2,2,"Total ticks: %d",totalTicks);
	wrefresh(win);
}

void initCurses()
{
	initscr();
	start_color();
	init_color(COLOR_MAGENTA,266,266,465);
	init_color(COLOR_RED,1000,156,156);
	init_color(COLOR_GREEN,332,730,332);
	init_color(COLOR_YELLOW,742,742,0);
	init_color(COLOR_BLUE,500,500,1000);
	init_color(COLOR_CYAN,1000,500,0);
	init_pair(1,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(2,COLOR_RED,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_BLUE,COLOR_BLACK);
	init_pair(6,COLOR_CYAN,COLOR_BLACK);
	init_pair(7,COLOR_WHITE,COLOR_BLACK);

	raw(); 			//Disable line buffering
	keypad(stdscr, TRUE);	//Get access to all keys
	cbreak();
	noecho();		//No 'echo' on wgetch(game)
	getmaxyx(stdscr,rows,cols);
}

void showSplash()
{
	int i;
	char logoPath[256];
	FILE *fp;
	logoPath[0] = '\0';
	strcat(logoPath,getenv("HOME"));
	strcat(logoPath,"/.conway/conwaylogo\0");
//	char *logoPath = "/home/harwiltz/.conway/conwaylogo";
	fp = fopen(logoPath,"r");
	char *line = NULL;
	size_t length = 0;
	ssize_t read;
	int curcol = rows/2 - 10;
	int size = 0;

	while((read = getline(&line,&length,fp)) != -1)
	{
		char *stroke = line;
		if(size == 0) for(size = 0; stroke[size] != '\0' && stroke[size] != '\n'; size++);
		mvprintw(curcol++,cols/2 - size/2,stroke);
	}
	free(line);
	fclose(fp);

	attron(COLOR_PAIR(YELLOW)|A_BOLD);
	mvprintw(0,0,"+");
	mvprintw(rows -1, 0, "+");
	mvprintw(0,cols - 1, "+");
	mvprintw(rows - 1, cols - 1, "+");
	for(i = 1; i < cols - 1; i++)
	{
		mvprintw(0,i,"-");
		mvprintw(rows - 1, i, "-");
	}
	for(i = 1; i < rows - 1; i++)
	{
		mvprintw(i,0,"|");
		mvprintw(i,cols - 1, "|");
	}

	mvprintw(curcol + 1,cols/2 - 3, "CONWAY");
	attroff(A_BOLD);
	mvprintw(curcol + 2, cols/2 - 13, "Developed by Harley Wiltzer");
	mvprintw(curcol + 3,cols/2 - 16, "Powered by Destructive Reasoning");
	move(0,0);
	attroff(COLOR_PAIR(YELLOW));
}
