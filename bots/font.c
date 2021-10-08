#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <global.h>

char * font;

void loadImage(const char * file){
	FILE * imageF = fopen(file,"rb+");
	fseek(imageF,0,SEEK_END);
	int size = ftell(imageF);
	fseek(imageF,0,SEEK_SET);
	char * data = malloc(14);
	fread(data,1,14,imageF);
	font = malloc(size);
	fseek(imageF,data[10] - 14,SEEK_CUR);
	fread(font,1,size - ftell(imageF),imageF);
	free(data);
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
