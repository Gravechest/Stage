#include <buttons.h>
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/gl.h>
#include <global.h>

const char * commands[] = {"ROT","MOV","END","SLP"};
const char * rot = "ROT";
const char * parameters[4] = {"LEFT","RIGHT","FORWARD","BACKWARD"};
const char * objectNames[] = {"CRATE","CHARGE"};

char texture[resX][resY][3];
char background[resX][resY][3];

typedef struct TEXTURE{
	char size;
	char * color;
	char * rSize;
}TEXTURE;

typedef struct ENTITY{
	char texture[40][40][3];
	float x;
	float y;
}ENTITY;

typedef struct OBJECTDAT{
	char data;
}OBJECTDATA;

typedef struct BUTTON{
	short x;
	short y;
	short id;
}BUTTON;

ENTITY robot;

short staticObjectAmm;
ENTITY *staticObject;
OBJECTDATA *objectdat;

short buttonsAmm;
BUTTON *buttons;

ROBOTDAT robotdat = {100,20};

const RGB colGreen = {34,180,3};
const RGB colBrown = {125,67,45};
const RGB colDarkBrown = {75,57,35};
const RGB colRed = {234,34,10};
const RGB colGrey = {126,128,122};

char *objects;
SCRIPT botScript;

void (*buttonFunctions[])(void) = {manualButton,upButton,rightButton,downButton,leftButton};

PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,PFD_TYPE_RGBA,
	24,0, 0, 0, 0, 0, 0,0,0,0,
	0,0,0,0,32,0,0,PFD_MAIN_PLANE,0,0,0,0	
	};
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

char * loadFile(const char * file){
	FILE * FILE = fopen(file,"rb+");
	fseek(FILE,0,SEEK_END);
	int size = ftell(FILE);
	fseek(FILE,0,SEEK_SET);
	char * returndata = malloc(size);
	fread(returndata,1,size,FILE);
	return returndata;
}

void loadScript(SCRIPT *script){
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

void renderObj(ENTITY *bot){
	for(int i = bot->x;i < bot->x + 40;i++){
		for(int i2 = bot->y;i2 < bot->y + 40;i2++){
			texture[i][i2][0] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][0];
			texture[i][i2][1] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][1];
			texture[i][i2][2] = bot->texture[i - (int)bot->x][i2 - (int)bot->y][2];
		}
	}
	drawCircleF(robot.x + 16 + cos(robotdat.rotation) * 7.5,robot.y + 16 + sin(robotdat.rotation) * 7.5,10,colGreen);
}

void renderRotObj(ENTITY *bot, float rot){
	for(int i = 0;i < 40;i++){
		for(int i2 = 0;i2 < 40;i2++){
			VEC2 norm = {i - 20,i2 - 20};
			int px = cosf(rot) * norm.x - sinf(rot) * norm.y + 20;
			int py = sinf(rot) * norm.x + cosf(rot) * norm.y + 20;
			texture[((int)bot->x+px)][((int)bot->y+py)][0] = bot->texture[i][i2][0];
			texture[((int)bot->x+px)][((int)bot->y+py)][1] = bot->texture[i][i2][1];
			texture[((int)bot->x+px)][((int)bot->y+py)][2] = bot->texture[i][i2][2];
		}
	}
	drawCircleF(robot.x + 16 + cos(robotdat.rotation) * 7.5,robot.y + 16 + sin(robotdat.rotation) * 7.5,10,colGreen);
}

char * jumpToNumber(char * val){
	while(val[0] > 0x29 && val[0] < 0x40){
		val++;
	}
	while(val[0] < 0x30 || val[0] > 0x39){
		val++;
	}
	return val;
}

char hitboxCheck(float x1,float x2,float y1,float y2){
	float dist = 0;
	dist += fabsf(x1 - x2) + fabsf(y1 - y2);
	dist += fabsf(x1 + 40 - x2) + fabsf(y1 - y2);
	dist += fabsf(x1 - x2) + fabsf(y1 + 40 - y2);
	dist += fabsf(x1 + 40 - x2) + fabsf(y1 + 40 - y2);
	if(dist == 160){
		return 1;
	}
	return 0;
}

char globalHitboxCheck(float x1,float x2,float y1,float y2){
	if(hitboxCheck(x1,x2,y1,y2)){
		return 1;
	}
	else if(hitboxCheck(x1 - 40,x2,y1,y2)){
		return 1;
	}
	else if(hitboxCheck(x1,x2,y1 - 40,y2)){
		return 1;
	}
	else if(hitboxCheck(x1 - 40,x2,y1 - 40,y2)){
		return 1;
	}
	return 0;
}

char checkChargePos(float x1,float x2,float y1,float y2){
	if(fabsf(x1 - x2) + fabsf(y1 - y2) < 3){
		return 1;
	}
	return 0;
}

void createButton(short x,short y,short id){
	buttonsAmm++;
	buttons = realloc(buttons,sizeof(BUTTON) * buttonsAmm);
	int b = buttonsAmm - 1;
	buttons[b].id = id;
	buttons[b].x = x;
	buttons[b].y = y;
	drawCircle(x - 3,y - 3,30,colGrey);
	drawCircle(x,y,25,colRed);
}

void WINAPI Quarter1(){
	objects = loadFile("objects.txt");
	robot.x = 0;
	robot.y = 100;
	SetPixelFormat(wdcontext, ChoosePixelFormat(wdcontext, &pfd), &pfd);
	wglMakeCurrent(wdcontext, wglCreateContext(wdcontext));
	drawRect(0,720,resX,10,colDarkBrown);
	drawRect(0,1069,resX,10,colDarkBrown);

	TEXTURE red = {40,malloc(3),malloc(3)};
	red.color[0] = 0x61;
	red.color[1] = 0x04;	
	red.color[2] = 0x07;
	red.rSize[0] = 26;
	red.rSize[1] = 10;
	red.rSize[2] = 5;
	setTexture(red,&robot);

	TEXTURE chargeText = {40,malloc(2),malloc(2)};
	chargeText.color = malloc(2);
	chargeText.color[0] = 0x74;
	chargeText.color[1] = 0x54;
	chargeText.rSize = malloc(2);
	chargeText.rSize[0] = 28;
	chargeText.rSize[1] = 17;

	TEXTURE obstakelText = {40,malloc(2),malloc(2)};
	obstakelText.color = malloc(2);
	obstakelText.color[0] = 0x74;
	obstakelText.color[1] = 0x75;
	obstakelText.rSize = malloc(2);
	obstakelText.rSize[0] = 28;
	obstakelText.rSize[1] = 17;

	loadImage("font.bmp");
	loadScript(&botScript);
	drawRect(204,730,5,339,colBrown);
	drawRect(152,730,5,339,colBrown);
	drawRect(100,730,5,339,colBrown);
	drawRect(152,780,52,5,colBrown);
	drawRect(0,760,100,5,colBrown);
	drawRect(0,825,100,5,colBrown);
	drawRect(0,890,152,5,colBrown);
	drawWord("battery",120,738,4,0);
	drawWord("temp",65,775,2,0);
	drawWord("char",65,840,2,0);
	drawWord("logging",120,912,4,0);
	drawWord("manual mode",170,790,5,0);
	createButton(167,740,0);
	for(;memcmp(objects,"END",3);objects++){
		for(int i = 0;i < 2;i++){
			if(!memcmp(objects,objectNames[i],5)){
				if(!staticObjectAmm){
					staticObjectAmm++;
					objectdat = malloc(sizeof(OBJECTDATA));
					staticObject = malloc(sizeof(ENTITY));
				}
				else{
					staticObjectAmm++;
					objectdat = realloc(objectdat,sizeof(OBJECTDATA) * staticObjectAmm);
					staticObject = realloc(staticObject,sizeof(ENTITY) * staticObjectAmm);
				}
				objects = jumpToNumber(objects);
				staticObject[staticObjectAmm-1].x = asciiToInt(objects);
				objects = jumpToNumber(objects);
				staticObject[staticObjectAmm-1].y = asciiToInt(objects);
				objectdat[staticObjectAmm-1].data = i;
			}
		}
	}
	for(int i = 0;i < staticObjectAmm;i++){
		switch(objectdat[i].data){
		case 0:
			setTexture(obstakelText,&staticObject[i]);	
			break;
		case 1:
			setTexture(chargeText,&staticObject[i]);	
			break;
		}
	}
	for(;;){
		if(robotdat.battery.data & 1){
			drawCircleF(23,848,23,colGreen);
		}
		else{
			drawCircleF(23,848,23,colRed);
		}
		for(int i = 0;i < staticObjectAmm;i++){
			renderObj(&staticObject[i]);
		}
		if(!botScript.comDuration){
			for(int loop = 0;!loop;){
				for(int i = 0;i < 4;i++){
					if(!memcmp(botScript.data,commands[i],strlen(commands[i]))){
						if(memcmp(botScript.data,"END",3)){
							botScript.data = jumpToNumber(botScript.data);
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
			robot.x += cosf(robotdat.rotation) / 4;
			robot.y += sinf(robotdat.rotation) / 4;
			break;
		}
		for(int i = 0;i < staticObjectAmm;i++){
			switch(objectdat[i].data){
			case 1:
				if(checkChargePos(robot.x,staticObject[i].x,robot.y,staticObject[i].y)){
					robotdat.battery.data = 1;
				}
				else{
					robotdat.battery.data = 0;
				}
				break;
			default:
				if(globalHitboxCheck(robot.x,staticObject[i].x,robot.y,staticObject[i].y)){
					botScript.comDuration = 0;
					drawWord("lowpad gecrashed",10,900,2,0);
				}
			}
		}
		robot.x += robotdat.velx;
		robot.y += robotdat.vely;
		robotdat.velx /= 1.03;
		robotdat.vely /= 1.03;
		if(robot.x < 0){
			robot.x = 0;
		}
		if(robot.y < 0){
			robot.y = 0;
		}
		if(robot.x > resX - 40){
			robot.x = resX - 40;
		}
		if(robot.y > resX - 40){
			robot.y = resX - 40;
		}
		renderRotObj(&robot,robotdat.rotation);
		drawWord(floatToAscii(robotdat.battery.temp),25,775,4,0);
		drawRectF(0,730,robotdat.battery.life,30,colGreen);
		robotdat.battery.temp -= (robotdat.battery.temp - 20) / 1000; 
		glDrawPixels(resY,resX,GL_RGB,GL_UNSIGNED_BYTE,texture);
		SwapBuffers(wdcontext);  
		memcpy(texture,background,resX * resY * 3);;
	}
}

char checkPixelCol(RGB col,int x,int y){
	if((texture[x][y][0] == col.r && texture[x][y][1] == col.g && texture[x][y][2] == col.b) || 
		(background[x][y][0] == col.r && background[x][y][1] == col.g && background[x][y][2] == col.b)){
		return 1;
	}
	return 0;
}

POINT mouse;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch (msg){
	case WM_LBUTTONDOWN:
		GetCursorPos(&mouse);
		ScreenToClient(hwnd,&mouse);
		mouse.y = resX - 20 - mouse.y;
		int tempx = mouse.x;
		mouse.x = mouse.y;
		mouse.y = tempx;
		for(int i = 0;i < buttonsAmm;i++){
			if(mouse.y > buttons[i].x && mouse.y > buttons[i].y && mouse.x < buttons[i].x + 25 && mouse.x < buttons[i].y + 25){
				buttonFunctions[buttons[i].id]();
				if(checkPixelCol(colGreen,mouse.x,mouse.y)){
					drawCircle(buttons[i].x,buttons[i].y,25,colRed);
				}
				else if(checkPixelCol(colRed,mouse.x,mouse.y)){
					drawCircle(buttons[i].x,buttons[i].y,25,colGreen);
				}
				break;
			}
		}
		break;
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
	hwnd = CreateWindowEx(0, "class", "raycasting", WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, resY + 20, resX + 20, NULL, NULL, hInstance, NULL);
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
