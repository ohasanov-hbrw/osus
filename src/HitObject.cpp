#include <HitObject.h>
#include "GameManager.h"

Vector2 getBezierPoint( Vector2* points, int numPoints, float t ) {
    Vector2* tmp = new Vector2[numPoints];
    memcpy(tmp, points, numPoints * sizeof(Vector2));
    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = Vector2{tmp[k].x + t *(tmp[k+1].x - tmp[k].x),tmp[k].y + t *(tmp[k+1].y - tmp[k].y)};
        i--;
    }
    Vector2 answer = tmp[0];
    delete[] tmp;
    return answer;
}

Circle::Circle(HitObjectData data){
    this->data = data;
    init();
}

void Circle::init(){
    GameManager* gm = GameManager::getInstance();

}

void Circle::update(){
    GameManager* gm = GameManager::getInstance();

    if(gm->currentTime*1000 > data.time + gm->gameFile.p50Final){
        data.time = gm->currentTime*1000;
        data.point = 0;
        gm->clickCombo = 0;

        gm->destroyHitObject();
    }
}

void Circle::render(){
    GameManager* gm = GameManager::getInstance();
    float approachScale = 3*(1-(gm->currentTime*1000 - data.time + gm->gameFile.preempt)/gm->gameFile.preempt)+1;
    if (approachScale <= 1) approachScale = 1;
    float clampedFade = (gm->currentTime*1000 - data.time  + gm->gameFile.fade_in) / gm->gameFile.fade_in;
    if(data.colour.size() > 2) DrawTextureEx(gm->hitCircle, Vector2{data.x*gm->windowScale-gm->hitCircle.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircle.height*0.5f*gm->windowScale/2},0,gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, clampedFade));
    else DrawTextureEx(gm->hitCircle, Vector2{data.x*gm->windowScale-gm->hitCircle.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircle.height*0.5f*gm->windowScale/2},0,gm->windowScale/2, Fade(WHITE, clampedFade));
    //DrawTextEx(GetFontDefault(),(std::to_string(data.comboNumber)).c_str(), Vector2{data.x*2,data.y*2},20,3,Fade(WHITE, clampedFade));
    render_combo();
    DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, clampedFade));
    if(data.colour.size() > 2) DrawTextureEx(gm->approachCircle, Vector2{data.x*gm->windowScale-gm->approachCircle.width*approachScale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->approachCircle.height*approachScale*0.5f*gm->windowScale/2},0,approachScale*gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, clampedFade));
    else DrawTextureEx(gm->approachCircle, Vector2{data.x*gm->windowScale-gm->approachCircle.width*approachScale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->approachCircle.height*approachScale*0.5f*gm->windowScale/2},0,approachScale*gm->windowScale/2, Fade(WHITE, clampedFade));
}

void Circle::dead_render(){
    //DrawRectangle(data.x*2, data.y*2, 5,5, WHITE);
    GameManager* gm = GameManager::getInstance();
    float scale = (gm->currentTime*1000 + 400 - data.time )/400;
    float fadeAnimation = 0.3*(1-((gm->currentTime*1000 + 200 - data.time )/200-1));
    float fadePoint = (1-((gm->currentTime*1000 + 400 - data.time )/400-1));
    float movePoint = (((gm->currentTime*1000 + 400 - data.time )/400-1))*20;
    if(data.colour.size() > 2) DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*scale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*scale*0.5f*gm->windowScale/2},0,scale*gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, fadeAnimation));
    else DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*scale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*scale*0.5f*gm->windowScale/2},0,scale*gm->windowScale/2, Fade(WHITE, fadeAnimation));
    if(data.point == 0) DrawTextureEx(gm->hit0, Vector2{data.x*gm->windowScale-gm->hit0.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit0.height*1*0.5f*gm->windowScale/2},(1-fadePoint)*15,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 1) DrawTextureEx(gm->hit50, Vector2{data.x*gm->windowScale-gm->hit50.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit50.height*1*0.5f*gm->windowScale/2 },0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 2) DrawTextureEx(gm->hit100, Vector2{data.x*gm->windowScale-gm->hit100.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit100.height*1*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 3) DrawTextureEx(gm->hit300, Vector2{data.x*gm->windowScale-gm->hit300.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit300.height*1*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
}

void Circle::dead_update(){
    GameManager* gm = GameManager::getInstance();
    if (data.time+400 < gm->currentTime*1000){
        gm->destroyDeadHitObject();
    }
}

void Circle::render_combo(){
    GameManager* gm = GameManager::getInstance();
    float clampedFade = (gm->currentTime*1000 - data.time  + gm->gameFile.fade_in) / gm->gameFile.fade_in;
    int digits = 1;
    if(data.comboNumber >= 1000) digits = 4;
    else if(data.comboNumber >= 100) digits = 3;
    else if(data.comboNumber >= 10) digits = 2;
    int origin = (gm->numbers[0].width + (digits - 3) * (gm->numbers[0].width - 150)) / 2;

    for(int i = digits; i >= 1 ; i--){
        int number = data.comboNumber;
        if(i == 1) number = number % 10;
        else if(i == 2) number = (number % 100 - number % 10)/10;
        else if(i == 3) number = (number % 1000 - number % 100)/100;
        else if(i == 4) number = (number % 10000 - number % 1000)/1000;
        DrawTextureEx(gm->numbers[number], Vector2{(float)data.x*gm->windowScale - origin*gm->windowScale/2 + (digits - i - 1) * (gm->numbers[0].width - 150)*gm->windowScale/2, (float)data.y*gm->windowScale - gm->numbers[0].width*gm->windowScale/2 / 2 },0,gm->windowScale / 2, Fade(WHITE, clampedFade));
    }
}

Slider::Slider(HitObjectData data){
    this->data = data;
    init();
}

void Slider::init(){
    GameManager* gm = GameManager::getInstance();

    edgePoints.push_back(Vector2{data.x, data.y});

    for(int i = 0; i < data.curvePoints.size(); i++){
        edgePoints.push_back(Vector2{data.curvePoints[i].first, data.curvePoints[i].second});
    }

    if(data.curveType == 'L'){
        for(int i = 0; i < edgePoints.size(); i++){
            renderPoints.push_back(edgePoints[i]);
        }
    }else if(data.curveType == 'B'){
        Vector2 edges[edgePoints.size()];

        for(int i = 0; i < edgePoints.size(); i++){
            edges[i] = edgePoints[i];
        }
        for(float i = 0; i <= 1; i+=0.05f){
            Vector2 tmp = getBezierPoint(edges, edgePoints.size(), i);
            renderPoints.push_back(tmp);
            std::cout<<tmp.x<<" "<<tmp.y<<std::endl;
        }
        
    }else if(data.curveType == 'P'){

    }else if(data.curveType == 'C'){

    }else{
        std::__throw_invalid_argument("What The FUck? Invalid Slider Type");
    }
}

void Slider::update(){
    GameManager* gm = GameManager::getInstance();

    if(gm->currentTime*1000 > data.time + gm->gameFile.p50Final){
        data.time = gm->currentTime*1000;
        data.point = 0;
        gm->clickCombo = 0;

        gm->destroyHitObject();
    }
}

void Slider::render(){
    GameManager* gm = GameManager::getInstance();
    //salak omer debug detected
    if(data.curveType == 'L'){
        for(int i = 0; i < renderPoints.size()-1; i++){
            DrawLineEx(renderPoints[i],renderPoints[i+1], 3, WHITE);
        }
    }else if(data.curveType == 'B'){
        for(int i = 0; i < renderPoints.size()-1; i++){
            DrawLineEx(renderPoints[i],renderPoints[i+1], 3, ORANGE);
        }
    }else{
        float approachScale = 3*(1-(gm->currentTime*1000 - data.time + gm->gameFile.preempt)/gm->gameFile.preempt)+1;
        if (approachScale <= 1) approachScale = 1;
        float clampedFade = (gm->currentTime*1000 - data.time  + gm->gameFile.fade_in) / gm->gameFile.fade_in;
        if(data.colour.size() > 2) DrawTextureEx(gm->hitCircle, Vector2{data.x*gm->windowScale-gm->hitCircle.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircle.height*0.5f*gm->windowScale/2},0,gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, clampedFade));
        else DrawTextureEx(gm->hitCircle, Vector2{data.x*gm->windowScale-gm->hitCircle.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircle.height*0.5f*gm->windowScale/2},0,gm->windowScale/2, Fade(WHITE, clampedFade));
        //DrawTextEx(GetFontDefault(),(std::to_string(data.comboNumber)).c_str(), Vector2{data.x*2,data.y*2},20,3,Fade(WHITE, clampedFade));
        render_combo();
        DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, clampedFade));
        if(data.colour.size() > 2) DrawTextureEx(gm->approachCircle, Vector2{data.x*gm->windowScale-gm->approachCircle.width*approachScale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->approachCircle.height*approachScale*0.5f*gm->windowScale/2},0,approachScale*gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, clampedFade));
        else DrawTextureEx(gm->approachCircle, Vector2{data.x*gm->windowScale-gm->approachCircle.width*approachScale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->approachCircle.height*approachScale*0.5f*gm->windowScale/2},0,approachScale*gm->windowScale/2, Fade(WHITE, clampedFade));
    }
    
}

void Slider::dead_render(){
    //DrawRectangle(data.x*2, data.y*2, 5,5, WHITE);
    GameManager* gm = GameManager::getInstance();
    float scale = (gm->currentTime*1000 + 400 - data.time )/400;
    float fadeAnimation = 0.3*(1-((gm->currentTime*1000 + 200 - data.time )/200-1));
    float fadePoint = (1-((gm->currentTime*1000 + 400 - data.time )/400-1));
    float movePoint = (((gm->currentTime*1000 + 400 - data.time )/400-1))*20;
    if(data.colour.size() > 2) DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*scale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*scale*0.5f*gm->windowScale/2},0,scale*gm->windowScale/2, Fade(Color{data.colour[0],data.colour[1],data.colour[2]}, fadeAnimation));
    else DrawTextureEx(gm->hitCircleOverlay, Vector2{data.x*gm->windowScale-gm->hitCircleOverlay.width*scale*0.5f*gm->windowScale/2,data.y*gm->windowScale-gm->hitCircleOverlay.height*scale*0.5f*gm->windowScale/2},0,scale*gm->windowScale/2, Fade(WHITE, fadeAnimation));
    if(data.point == 0) DrawTextureEx(gm->hit0, Vector2{data.x*gm->windowScale-gm->hit0.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit0.height*1*0.5f*gm->windowScale/2},(1-fadePoint)*15,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 1) DrawTextureEx(gm->hit50, Vector2{data.x*gm->windowScale-gm->hit50.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit50.height*1*0.5f*gm->windowScale/2 },0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 2) DrawTextureEx(gm->hit100, Vector2{data.x*gm->windowScale-gm->hit100.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit100.height*1*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
    else if(data.point == 3) DrawTextureEx(gm->hit300, Vector2{data.x*gm->windowScale-gm->hit300.width*1*0.5f*gm->windowScale/2 ,data.y*gm->windowScale-gm->hit300.height*1*0.5f*gm->windowScale/2},0,1*gm->windowScale/2, Fade(WHITE, fadePoint));
}

void Slider::dead_update(){
    GameManager* gm = GameManager::getInstance();
    if (data.time+400 < gm->currentTime*1000){
        gm->destroyDeadHitObject();
    }
}

void Slider::render_combo(){
    GameManager* gm = GameManager::getInstance();
    float clampedFade = (gm->currentTime*1000 - data.time  + gm->gameFile.fade_in) / gm->gameFile.fade_in;
    int digits = 1;
    if(data.comboNumber >= 1000) digits = 4;
    else if(data.comboNumber >= 100) digits = 3;
    else if(data.comboNumber >= 10) digits = 2;
    int origin = (gm->numbers[0].width + (digits - 3) * (gm->numbers[0].width - 150)) / 2;

    for(int i = digits; i >= 1 ; i--){
        int number = data.comboNumber;
        if(i == 1) number = number % 10;
        else if(i == 2) number = (number % 100 - number % 10)/10;
        else if(i == 3) number = (number % 1000 - number % 100)/100;
        else if(i == 4) number = (number % 10000 - number % 1000)/1000;
        DrawTextureEx(gm->numbers[number], Vector2{(float)data.x*gm->windowScale - origin*gm->windowScale/2 + (digits - i - 1) * (gm->numbers[0].width - 150)*gm->windowScale/2, (float)data.y*gm->windowScale - gm->numbers[0].width*gm->windowScale/2 / 2 },0,gm->windowScale / 2, Fade(WHITE, clampedFade));
    }
}