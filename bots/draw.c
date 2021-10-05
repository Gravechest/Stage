#include <font.h>
#include <math.h>

void drawSquare(int x, int y, int size,RGB col){
	x *= resY * 3;
	y *= 3;
	for(int i = x;i < x + size * resY * 3;i+=resY * 3){
		for(int i2 = y;i2 < y + size * 3;i2+=3){
			background[i + i2] = col.r;
			background[i + i2 + 1] = col.g;
			background[i + i2 + 2] = col.b;
		}
	}
}

void drawSquareF(int x, int y, int size,RGB col){
	x *= resY * 3;
	y *= 3;
	for(int i = x;i < x + size * resY * 3;i+=resY * 3){
		for(int i2 = y;i2 < y + size * 3;i2+=3){
			texture[i + i2] = col.r;
			texture[i + i2 + 1] = col.g;
			texture[i + i2 + 2] = col.b;
		}
	}
}

void drawCircle(int x,int y,int size,RGB col){
	for(int i = x;i < x + size;i++){
		for(int i2 = y;i2 < y + size;i2++){
			VEC2 norm = {i - x - size / 2,i2 - y - size / 2};
			if(sqrtf(norm.x * norm.x + norm.y * norm.y) < size / 2){
				texture[i * resY * 3 + i2 * 3] = col.r;
				texture[i * resY * 3 + i2 * 3 + 1] = col.g;
				texture[i * resY * 3 + i2 * 3 + 2] = col.b;
			}
		}
	}
}

void drawRect(int x,int y,int sx,int sy,RGB col){
	x *= resY * 3;
	y *= 3;
	for(int i = x;i < x + sx * resY * 3;i+=resY * 3){
		for(int i2 = y;i2 < y + sy * 3;i2+=3){
			background[i + i2] = col.r;
			background[i + i2 + 1] = col.g;
			background[i + i2 + 2] = col.b;
		}
	}
}

void drawRectF(int x,int y,int sx,int sy,RGB col){
	x *= resY * 3;
	y *= 3;
	for(int i = x;i < x + sx * resY * 3;i+=resY * 3){
		for(int i2 = y;i2 < y + sy * 3;i2+=3){
			texture[i + i2] = col.r;
			texture[i + i2 + 1] = col.g;
			texture[i + i2 + 2] = col.b;
		}
	}
}
