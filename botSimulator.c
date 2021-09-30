#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>

#define resX 720
#define resY 1080
#define PI 3.14159265359

const char * commands[] = {"ROT","MOV","END","SLP"};
const char * rot = "ROT";
const char * parameters[4] = {"LEFT","RIGHT","FORWARD","BACKWARD"};

char texture[resX][resY][3];
char background[resX][resY][3];
typedef struct RGB {char r;char g;char b;} RGB;
typedef struct VEC2 {float x;float y;}VEC2;

typedef struct BATTERY{
	float life;
	float temp;
}BATTERY;

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
	BATTERY battery;
	float rotation;
}ROBOTDAT; 

typedef struct SCRIPT{
	int dataSz;
	char * data;
	char * fileEnd;
	int comDuration;
	int comType;
}SCRIPT;

SCRIPT botScript;
ENTITY robot;
ROBOTDAT robotdat = {100,20};
ENTITY item;

RGB colGreen = {34,180,3};
RGB colBrown = {125,67,45};
RGB colDarkBrown = {75,57,35};

char * font;

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;

int numberLng(char * val){
	for(int i = 0;;i++){
		if(val[i] < 0x30 || val[i] > 0x39){
			return i;
		}
	}
}

int exp10(int val,int times){
	for(int i = 1;i < times;i++){
		val *= 10;
	}
	return val;
}

int asciiToInt(char * val){
	int size = numberLng(val);
	int result = 0;
	for(int i = 0;i < size;i++){
		result += exp10(val[i] - 0x30,size - i);
	}
	return result;
}

char * floatToAscii(float val){
	int intval = (int)val;
	char * result = calloc(4,1);
	int size = -1;
	for(;intval;size++){
		intval /= 10;
	}
	intval = (int)val;
	for(;intval;size--){
		result[size] = intval % 10 + 0x30;
		intval /= 10;
	}
	return result;
}

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

inline void drawSquareF(int x, int y, int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			texture[i][i2][0] = col.r;
			texture[i][i2][1] = col.g;
			texture[i][i2][2] = col.b;
		}
	}
}

void drawCircle(int x,int y,int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			VEC2 norm = {i - x - size / 2,i2 - y - size / 2};
			if(sqrtf(norm.x * norm.x + norm.y * norm.y) < size / 2){
				texture[i][i2][0] = col.r;
				texture[i][i2][1] = col.g;
				texture[i][i2][2] = col.b;
			}
		}
	}
}

void fontDrawing(int x,int y,int offset,int size,char flags){
	font += offset + 5324;
	for(int i = 0;i < size * 5;i+=size){
		for(int i2 = 0;i2 < size * 5;i2+=size){
			RGB color = {font[2],font[1],font[0]};
			if(flags & 1){
				drawSquareF(x + i,y + i2,size,color);
			}
			else{
				drawSquare(x + i,y + i2,size,color);
			}
			font+=4;
		}
		font+=180;
	}
	font -= offset + 6324;
}

void drawWord(const char * word,int x,int y,int size,char flags){
	for(int i = 0;i < strlen(word);i++){
		int offset = 3800;
		int car = 0;
		if(word[i] > 0x60 && word[i] < 0x7b){
			car = word[i] - 0x61;
		}
		else if(word[i] > 0x40 && word[i] < 0x5b){
			car = word[i] - 0x41;
		}
		else if(word[i] > 0x2f && word[i] < 0x3a){
			car = word[i] - 0x16;
		}
		else{
			continue;
		}
		offset -= car / 10 * 1200;
		offset += car % 10 * 20;
		fontDrawing(x,y + i * size * 5,offset,size,flags);
	}
}

void setTexture(TEXTURE text,ENTITY *ent){
	for(int i = 0;i < text.size;i++){
		for(int i2 = 0;i2 < text.size;i2++){
			VEC2 norm = {i - text.size / 2,i2 - text.size / 2};
			float dist = sqrtf(norm.x * norm.x + norm.y * norm.y);
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
			background[i][i2][0] = col.r;
			background[i][i2][1] = col.g;
			background[i][i2][2] = col.b;
		}
	}
}

void drawRectF(int x,int y,int sx,int sy,RGB col){
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
	drawCircle(robot.x + 16 + cos(robotdat.rotation) * 7.5,robot.y + 16 + sin(robotdat.rotation) * 7.5,10,colGreen);
}

void * loadScript(SCRIPT *script){
	FILE * tempscript = fopen("script.txt","rb+");
	fseek(tempscript,0,SEEK_END);
	int size = ftell(tempscript);
	fseek(tempscript,0,SEEK_SET);
	script->data = calloc(size,1);
	fread(script->data,1,size,tempscript);
	script->data += size;
	script->fileEnd = script->data;
	script->data -= size;
	fclose(tempscript);
}

void WINAPI Quarter1(){
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	drawRect(0,720,resX,10,colDarkBrown);
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
	int itt = 0;
	loadScript(&botScript);
	drawRect(0,825,100,5,colBrown);
	drawRect(100,730,5,350,colBrown);
	drawRect(0,760,100,5,colBrown);
	drawWord("battery",120,738,4,0);
	drawWord("temp",65,775,2,0);
	for(;;){
		if(!botScript.comDuration){
			for(int loop = 0;!loop;){
				for(int i = 0;i < 4;i++){
					if(!memcmp(botScript.data,commands[i],strlen(commands[i]))){
						if(memcmp(botScript.data,"END",3)){
							while(botScript.data[0] < 0x30 || botScript.data[0] > 0x39){
								botScript.data++;
							}
							botScript.comDuration = asciiToInt(botScript.data) * 4;
						}
						else{
							botScript.comDuration = -1;
						}
						botScript.comType = i;
						loop = 1;
						break;
					}
				}
				if(!loop){
					botScript.data++;
				}
			}
		}
		else{
			botScript.comDuration -= 1;
			switch(botScript.comType & 3){
			case 0:
				robotdat.battery.life -= 0.001;
				robotdat.battery.temp += 0.01;
			 	robotdat.rotation += PI / 720;
				break;
			case 1:
				robotdat.battery.life -= 0.002;
				robotdat.battery.temp += 0.01;
				robot.x += cos(robotdat.rotation) / 4;
				robot.y += sin(robotdat.rotation) / 4;
				break;
			}
		}
		drawWord(floatToAscii(robotdat.battery.temp),25,775,4,0);
		drawRectF(0,730,robotdat.battery.life,30,colGreen);
		renderObj(&robot);
		robot.x += robot.velx;
		robot.y += robot.vely;
		robotdat.battery.temp -= (robotdat.battery.temp - 20) / 1000; 
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
		SwapBuffers(wdcontext);  
		memcpy(texture,background,sizeof(texture));;
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
		400, 400, resY, resX + 20, NULL, NULL, hInstance, NULL);
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
