#pragma once
#define resX 720
#define resY 1080
#define PI 3.14159265359

typedef struct BATTERY{
	float life;
	float temp;
	char data; 
}BATTERY;

typedef struct ROBOTDAT{
	BATTERY battery;
	float velx;
	float vely;
	float rotation;
}ROBOTDAT; 

typedef struct SCRIPT{
	int dataSz;
	char * data;
	char * fileEnd;
	int comDuration;
	int comType;
}SCRIPT;

extern char texture[resX][resY][3];
extern char background[resX][resY][3];

typedef struct VEC2 {float x;float y;}VEC2;
typedef struct RGB {char r;char g;char b;} RGB;

void drawWord(const char * word,int x,int y,int size,char flags);
void loadImage(const char * file);

void drawCircle(int x,int y,int size,RGB col);
void drawRect(int x,int y,int sx,int sy,RGB col);
void drawRectF(int x,int y,int sx,int sy,RGB col);
void drawSquareF(int x, int y, int size,RGB col);
void drawSquare(int x, int y, int size,RGB col);

extern SCRIPT botScript;
extern ROBOTDAT robotdat;




