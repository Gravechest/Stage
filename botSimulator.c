#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#define resX 480
#define resY 640
char texture[resX][resY][3];

typedef struct VEC2 {float x;float y;}VEC2;

typedef struct ENTITY{
	char texture[10][10][3];
	short x;
	short y;
}ENTITY;

typedef struct TEXTURE{
	char size;
	char r;
	char g;
	char b;
	char rRad;
	char gRad;
	char bRad;
}TEXTURE;

typedef struct ROBOT{
	char texture[40][40][3];
	float velx;
	float vely;
	float x;
	float y;
}ROBOT;

ROBOT robot;
ENTITY item;

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,
	0,0,0,0	};
HWND hwnd;
MSG Msg;
HDC wdcontext;

void setTexture(TEXTURE text,ROBOT *ent){
	for(int i = 0;i < text.size;i++){
		for(int i2 = 0;i2 < text.size;i2++){
			VEC2 norm = {i - text.size / 2,i2 - text.size / 2};
			float dist = sqrt(norm.x * norm.x + norm.y * norm.y);
			if(dist < text.rRad){
				ent->texture[i][i2][0] = text.r;
			}
			if(dist < text.gRad){
				ent->texture[i][i2][1] = text.g;
			}
			if(dist < text.bRad){
				ent->texture[i][i2][2] = text.b;
			}
		}
	}
}

void renderObj(ROBOT *bot){
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
	glPixelZoom(640 / resY,480 / resX);
	TEXTURE red = {40,255,255,255,20,10,5};
	setTexture(red,&robot);
	robot.velx = 0.1; 
	for(;;){
		renderObj(&robot);
		robot.x += robot.velx;
		robot.y += robot.vely;
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,&texture);
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
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, "class", "raycasting", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
		400, 400, 640, 480, NULL, NULL, hInstance, NULL);
	wdcontext = GetDC(hwnd);
	CreateThread(0,0,Quarter1,0,0,0);
	for(;;){
		while(PeekMessage(&Msg,hwnd,0,0,0)){
			GetMessage(&Msg, hwnd, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessageW(&Msg);
		}
		Sleep(1);
	}
	return Msg.wParam;
}
