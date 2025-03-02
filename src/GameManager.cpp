#include "GameManager.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <vector>
#include <math.h>

float GameManager::clip(float value, float min, float max) {
  return std::min(std::max(value, min), max);
}

GameManager* GameManager::inst_ = NULL;

GameManager* GameManager::getInstance() {
   if (inst_ == NULL) {
      inst_ = new GameManager();
   }

   return(inst_);
}

GameManager::GameManager(){
	init();
}

void GameManager::init(){
	InitAudioDevice();
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(640*windowScale,480*windowScale,"osus?");
	SetTargetFPS(60);
	HideCursor();
	
	hitCircle = LoadTexture("../skin/hitcircle.png");
    hitCircleOverlay = LoadTexture("../skin/hitcircleoverlay.png");
    approachCircle = LoadTexture("../skin/approachcircle.png");
    cursor = LoadTexture("../skin/Extra Cursors/cursor.png");
    selectCircle = LoadTexture("../skin/hitcircleselect.png");
    hit0 = LoadTexture("../skin/hit0.png");
    hit50 = LoadTexture("../skin/hit50.png");
    hit100 = LoadTexture("../skin/hit100.png");
    hit300 = LoadTexture("../skin/hit300.png");
    sliderb = LoadTexture("../skin/sliderb0.png");

    for(int i = 0; i < 10; i++){
    	numbers[i] = LoadTexture(("../skin/default-" + (std::to_string(i)) + ".png").c_str());
    }
}

void GameManager::update(){
	UpdateMusicStream(backgroundMusic);
	currentTime = GetMusicTimePlayed(backgroundMusic);
	
	MousePosition = Vector2{GetMouseX(), GetMouseY()};
	pressed = IsMouseButtonPressed(0);
	down = IsMouseButtonDown(0);

	int timingSize = gameFile.timingPoints.size();
	for(int i = timingSize-1; i >= 0; i--){
		if(gameFile.timingPoints[i].time <= currentTime*1000){
			time = gameFile.timingPoints[i].time;
			beatLength = gameFile.timingPoints[i].beatLength;
			meter = gameFile.timingPoints[i].meter;
			sampleSet = gameFile.timingPoints[i].sampleSet;
			sampleIndex = gameFile.timingPoints[i].sampleIndex;
			volume = gameFile.timingPoints[i].volume;
			uninherited = gameFile.timingPoints[i].uninherited;
			effects = gameFile.timingPoints[i].effects;
			gameFile.timingPoints.pop_back();
		}
	}

	if(beatLength < 0) sliderSpeedOverride = (100 / beatLength * (-1));

	int size = gameFile.hitObjects.size();	
	for(int i = size-1; i >= 0; i--){
		if(gameFile.hitObjects[i].time - gameFile.preempt <= currentTime*1000){
			spawnHitObject(gameFile.hitObjects[i]);
			
			if(objects[objects.size()-1]->data.startingACombo){
				currentComboIndex++;
				if(gameFile.comboColours.size()) 
					currentComboIndex = (currentComboIndex + objects[objects.size()-1]->data.skipComboColours) % gameFile.comboColours.size();
				combo = 1;
			}

			if(gameFile.comboColours.size()) objects[objects.size()-1]->data.colour = gameFile.comboColours[currentComboIndex];

			objects[objects.size()-1]->data.comboNumber = combo;
			combo++;
			gameFile.hitObjects.pop_back();
		}
	}
	
	for(int i = 0; i < objects.size(); i++){
		if(i == 0){
			if (pressed){
				if (objects[i]->data.type != 2){
					//TODO
					
					if (CheckCollisionPointCircle(MousePosition, Vector2{(float)objects[i]->data.x*windowScale,(float)objects[i]->data.y*windowScale}, 56*windowScale/2) && pressed){
						if(std::abs(currentTime*1000 - objects[i]->data.time) > gameFile.p50Final){
							objects[i]->data.point = 0;
							clickCombo = 0;
						}
						else if(std::abs(currentTime*1000 - objects[i]->data.time) > gameFile.p100Final){
							objects[i]->data.point = 1;
							score+= 50 + (50 * (std::max(clickCombo-1,0) * difficultyMultiplier * 1)/25);
							clickCombo++;
						}
						else if(std::abs(currentTime*1000 - objects[i]->data.time) > gameFile.p300Final){
							objects[i]->data.point = 2;
							score+= 100 + (100 * (std::max(clickCombo-1,0) * difficultyMultiplier * 1)/25);
							clickCombo++;
						}
						else{
							objects[i]->data.point = 3;
							score+= 300 + (300 * (std::max(clickCombo-1,0) * difficultyMultiplier * 1)/25);
							clickCombo++;
						}
						objects[i]->data.time = currentTime*1000;
						destroyHitObject();
					}
					else {
						objects[i]->update();
					}
				}
				else if (objects[i]->data.type == 2){
					if(Slider* tempslider = dynamic_cast<Slider*>(objects[i])){
						//TODO
						if(CheckCollisionPointCircle(MousePosition,Vector2{tempslider->renderPoints[tempslider->position].x*windowScale, tempslider->renderPoints[tempslider->position].y*windowScale} ,128*windowScale/2 ) && pressed && currentTime*1000 < tempslider->data.time + gameFile.p100Final){
        					tempslider->is_hit_at_first = true;
    					}
    				}
    				objects[i]->update();
				}
				else{
					objects[i]->update();
				}
			}
			else{
				objects[i]->update();
			}
		}else{
			objects[i]->update();
		}
	}
	
	//Dead Animations
	for(int i = 0; i < dead_objects.size(); i++){
		dead_objects[i]->dead_update();
	}
}

void GameManager::render(){
	BeginDrawing();
	ClearBackground(BLANK);
	DrawFPS(10, 10);

	float scale = 0.6f;

	for(int i = objects.size() - 1; i >= 0; i--){
		objects[i]->render();
	}

	//Dead animations
	for(int i = dead_objects.size() - 1; i >= 0; i--){
		dead_objects[i]->dead_render();
	}

	render_points();
	render_combo();
	DrawTextureEx(cursor, Vector2{GetMouseX()-cursor.width*windowScale/2*scale*0.5f,GetMouseY()-cursor.height*scale*windowScale/2*0.5f},0,scale*windowScale/2, WHITE);
	
	EndDrawing();
}

void GameManager::run(){
	while(!WindowShouldClose()){
		update();
		render();
	}

	CloseWindow();
}

void GameManager::loadGame(std::string filename){
	Parser parser = Parser();
	gameFile = parser.parse(filename);

	std::reverse(gameFile.hitObjects.begin(),gameFile.hitObjects.end());

	if(std::stoi(gameFile.configDifficulty["ApproachRate"]) < 5){
		gameFile.preempt = 1200 + 600 * (5 - std::stoi(gameFile.configDifficulty["ApproachRate"])) / 5;
		gameFile.fade_in = 800 + 400 * (5 - std::stoi(gameFile.configDifficulty["ApproachRate"])) / 5;
	}
	else if(std::stoi(gameFile.configDifficulty["ApproachRate"]) < 5){
		gameFile.preempt = 1200 - 750 * (std::stoi(gameFile.configDifficulty["ApproachRate"]) - 5) / 5;
		gameFile.fade_in = 800 - 500 * (std::stoi(gameFile.configDifficulty["ApproachRate"]) - 5) / 5;
	}
	else{
		gameFile.preempt = 1200;
		gameFile.fade_in = 800;
	}

	gameFile.p300Final = gameFile.p300 - std::stoi(gameFile.configDifficulty["OverallDifficulty"]) * gameFile.p300Change;
	gameFile.p100Final = gameFile.p100 - std::stoi(gameFile.configDifficulty["OverallDifficulty"]) * gameFile.p100Change;
	gameFile.p50Final = gameFile.p50 - std::stoi(gameFile.configDifficulty["OverallDifficulty"]) * gameFile.p50Change;

	std::cout << ("../beatmaps/" + gameFile.configGeneral["AudioFilename"]) << std::endl;

	backgroundMusic = LoadMusicStream(("../beatmaps/" + gameFile.configGeneral["AudioFilename"]).c_str());
	PlayMusicStream(backgroundMusic);
    SetMusicVolume(backgroundMusic, 0.2f);

	float hpdrainrate = std::stof(gameFile.configDifficulty["HPDrainRate"]);
	float circlesize = std::stof(gameFile.configDifficulty["CircleSize"]);
	float overalldifficulty = std::stof(gameFile.configDifficulty["OverallDifficulty"]);

	difficultyMultiplier = ((hpdrainrate + circlesize + overalldifficulty + clip((float)gameFile.hitObjects.size() / GetMusicTimeLength(backgroundMusic) * 8.f, 0.f, 16.f)) / 38.f * 5.f);
	
	if (gameFile.configDifficulty.find("SliderMultiplier") != gameFile.configDifficulty.end()) {
		sliderSpeed = std::stof(gameFile.configDifficulty["SliderMultiplier"]);
	}
}

void GameManager::spawnHitObject(HitObjectData data){
	HitObject *temp;

	if(data.type == 1){
		temp = new Circle(data);
		objects.push_back(temp);
	}
	else if(data.type == 2){
		temp = new Slider(data);
		objects.push_back(temp);
	}
	else if(data.type == 3){
		temp = new Circle(data);
		objects.push_back(temp);
	}
	else {
		temp = new Circle(data);
		objects.push_back(temp);
	}
}

void GameManager::destroyHitObject(){
	dead_objects.push_back(objects[0]);
	objects.erase(objects.begin());
}

void GameManager::destroyDeadHitObject(){
	delete dead_objects[0];
	dead_objects.erase(dead_objects.begin());
}

void GameManager::render_points(){
    int digits = 1;
    int tempScore = score;
	
    while(true){
    	if (tempScore < 10) break;
    	digits++;
    	tempScore /= 10;
    }

    for(int i = digits; i >= 1; i--){
        int number = score;
        int mod = 10;

        for(int j = 1; j < i; j++){
        	mod *= 10;
        }

        number = (number % mod - number % (mod/10))/(mod/10);

        DrawTextureEx(numbers[number], Vector2{0 + (digits - i - 1) * (numbers[0].width - 150)*windowScale/2, 0 },0,windowScale/2, Fade(WHITE, 1));
    }
}

void GameManager::render_combo(){
    int digits = 1;
    int tempCombo = clickCombo;

    while(true){
    	if (tempCombo < 10) break;
    	digits++;
    	tempCombo /= 10;
    }

    for(int i = digits; i >= 1 ; i--){
        int number = clickCombo;
        int mod = 10;

        for(int j = 1; j < i; j++){
        	mod *= 10;
        }

        number = (number % mod - number % (mod/10))/(mod/10);

        DrawTextureEx(numbers[number], Vector2{0 + (digits - i - 1) * (numbers[0].width - 150)*windowScale/2 /2, 440*windowScale },0,0.5f*windowScale/2, Fade(WHITE, 1));
    }
}
