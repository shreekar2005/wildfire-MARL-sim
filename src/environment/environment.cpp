
#include <environment/environment.hpp>
#include <iostream>
#include <raylib.h>


int Environment::screenWidth = 1500;
int Environment::screenHeight= 900;
int Environment::pixelSize = 30;

void Environment::generateFire(){
		Environment::img = GenImagePerlinNoise(screenWidth/pixelSize, screenHeight/pixelSize, 0, 0, 1);
		Environment::FireArray = LoadImageColors(img);
		

}
void Environment::drawEnvironment(){
		for(int i = 0 ; i < screenHeight;i++){
				for(int j = 0;j<screenWidth;j++){
						Color thisColor = FireArray[i*screenWidth + j];
						
						
				}

		}
}
