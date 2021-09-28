#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#define resX 720
#define resY 1080
char texture[resX][resY][3];
char background[resX][resY][3];

typedef struct RGB {char r;char g;char b;} RGB;
typedef struct VEC2 {float x;float y;}VEC2;

typedef struct TEXTURE{
	char size;
	char * color;
	char * rSize;
}TEXTURE;

typedef struct ENTITY{
	char texture[40][40][3];
	float velx;
	float vely;
	float x;
	float y;
}ENTITY;

typedef struct ROBOTDAT{
	int movement;
	float battery;
	float rotation;
}ROBOTDAT;

ENTITY robot;
ROBOTDAT robotdat = {100};
ENTITY item;

RGB colGreen = {34,180,3};
char * font;

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;

char * loadImage(const char * file){
	FILE * imageF = fopen(file,"rb+");
	fseek(imageF,0,SEEK_END);
	int size = ftell(imageF);
	fseek(imageF,0,SEEK_SET);
	char * data = malloc(14);
	fread(data,1,14,imageF);
	char * returndata = malloc(size);
	fseek(imageF,data[10] - 14,SEEK_CUR);
	fread(returndata,1,size - ftell(imageF),imageF);
	free(data);
	return returndata;
}

inline void drawSquare(int x, int y, int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			background[i][i2][0] = col.r;
			background[i][i2][1] = col.g;
			background[i][i2][2] = col.b;
		}
	}
}

void fontDrawing(int x,int y,int offset,int size){
	font += offset + 5320;
	for(int i = 0;i < size * 5;i+=size){
		for(int i2 = 0;i2 < size * 5;i2+=size){
			RGB color = {font[2],font[1],font[0]};
			drawSquare(x + i,y + i2,size,color);
			font+=4;
		}
		font+=180;
	}
	font -= offset + 6320;
}

void drawWord(const char * word,int x,int y,int size){
	for(int i = 0;i < strlen(word);i++){
		int offset = 3800;
		int car = 0;
		if(word[i] > 0x60){
			car = word[i] - 0x61;
		}
		offset -= car / 10 * 1200;
		offset += car % 10 * 20;
		fontDrawing(x,y + i * size * 5,offset,size);
	}
}

void setTexture(TEXTURE text,ENTITY *ent){
	for(int i = 0;i < text.size;i++){
		for(int i2 = 0;i2 < text.size;i2++){
			VEC2 norm = {i - text.size / 2,i2 - text.size / 2};
			float dist = sqrt(norm.x * norm.x + norm.y * norm.y);
			for(int i3 = 0;text.color[i3];i3++){
				if(text.rSize[i3] > dist){
					ent->texture[i][i2][0] = text.color[i3] << 5;
					ent->texture[i][i2][1] = text.color[i3] << 3 & 192;
					ent->texture[i][i2][2] = text.color[i3] & 224;
				}
			}
		}
	}
}

void drawRect(int x,int y,int sx,int sy,RGB col){
	for(int i = x;i < x + sx;i++){
		for(int i2 = y;i2 < y + sy;i2++){
			texture[i][i2][0] = col.r;
			texture[i][i2][1] = col.g;
			texture[i][i2][2] = col.b;
		}
	}
}

void renderObj(ENTITY *bot){
	for(int i = bot->x;i < bot->x + 40;i++){
		for(int i2 = bot->y;i2 < bot->y + 40;i2++){
			texture[i][i2][0] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][0];
			texture[i][i2][1] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][1];
			texture[i][i2][2] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][2];
		}
	}
}

void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	for(int i = 0;i < resX;i++){
		for(int i2 = 720;i2 < 730;i2++){
			background[i][i2][0] = 125;
			background[i][i2][1] = 67;
			background[i][i2][2] = 45;
		}
	}
	TEXTURE red;
	red.size = 40;
	red.color = calloc(4,1);
	red.color[0] = 0x61;
	red.color[1] = 0x04;
	red.color[2] = 0x07;
	red.rSize = malloc(3);
	red.rSize[0] = 20;
	red.rSize[1] = 10;
	red.rSize[2] = 5;
	setTexture(red,&robot);
	font = loadImage("font.bmp");
	drawWord("battery",15,780,2);
	int itt = 0;
	for(;;){
		drawRect(0,740,robotdat.battery,30,colGreen);
		renderObj(&robot);
		robot.x += robot.velx;
		robot.y += robot.vely;
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		memcpy(texture,background,sizeof(texture));
		SwapBuffers(wdcontext);  
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_CLOSE:
		exit(0);
	case WM_DESTROY:
		exit(0);
	default:	
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),0,WndProc,0,0,hInstance,0,0,0,0,"class",0 };
	RegisterClassEx(&wc);
	hwnd = CreateWindowEx(0, "class", "raycasting", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		400, 400, resY, resX, NULL, NULL, hInstance, NULL);
	wdcontext = GetDC(hwnd);
	CreateThread(0,0,Quarter1,0,0,0);
	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(30);
	}
	return Msg.wParam;
}
