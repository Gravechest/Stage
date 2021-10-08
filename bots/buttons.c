#include <buttons.h>
#include <math.h>
#include <global.h>
#include <stdlib.h>

void manualButton(){
	createButton(500,500,1);
	botScript.comDuration = -1;
	botScript.comType = -1;
}
void upButton(){
	robotdat.velx += cos(robotdat.rotation);
	robotdat.vely += sin(robotdat.rotation);
}
void rightButton(){
	
}
void leftButton(){
	
}
void downButton(){
	
}
