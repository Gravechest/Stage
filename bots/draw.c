#include <global.h>
#include <math.h>

void drawSquare(int x, int y, int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			background[i][i2][0] = col.r;
			background[i][i2][1] = col.g;
			background[i][i2][2] = col.b;
		}
	}
}

void drawSquareF(int x, int y, int size,RGB col){
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
				background[i][i2][0] = col.r;
				background[i][i2][1] = col.g;
				background[i][i2][2] = col.b;
			}
		}
	}
}

void drawCircleF(int x,int y,int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			VEC2 norm = {x - size / 2,y - size / 2};
			if(sqrtf(norm.x * norm.x + norm.y * norm.y) < size / 2){
				texture[i][i2][0] = col.r;
				texture[i][i2][1] = col.g;
				texture[i][i2][2] = col.b;
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
