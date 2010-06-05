#include "InfoText.h"
#include "Globals.h"
#include <iterator>

using namespace Helpers;

InfoText::InfoText()
{
	Init(50.0f, 5.0f);
}

InfoText::InfoText(float x, float y)
{
	Init(x, y);
}

InfoText::~InfoText(void)
{
}

void InfoText::Init(float x, float y){

	D3DX10CreateFont(Globals::Device, 16, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman", &font);
	deterioration = 0.5f;
	textRect.left = x;
	textRect.right = 0;
	textRect.top = y;
	textRect.bottom = 0;

	deltaRect.left = 0;
	deltaRect.right = 0;
	deltaRect.top = 20;
	deltaRect.bottom = 0;

}

void InfoText::AddText(std::string text){
	AddText(text, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
}

void InfoText::AddText(std::string text, D3DXCOLOR color){
	INFO_TEXT it;
	it.text = text;
	it.color = color;
	it.life = 1.0f;
	texts.push_back(it);
}

void InfoText::CleanUp(){
	texts.clear();
	font->Release();
}

void InfoText::Update(float dt){	
	std::list<INFO_TEXT>::iterator it;

	for(it = texts.begin(); it != texts.end(); it++){
		it->life -= dt*deterioration;
		it->color.a = it->life;
	}

	// take out the "dead" texts
	while(texts.size() > 0 && texts.front().life <= 0.0f){
		texts.pop_front();
	}	
}

void InfoText::Draw(){
	std::list<INFO_TEXT>::iterator it;
	int pos = texts.size()-1;
	RECT dR = textRect;

	for(it = texts.begin(); it != texts.end(); it++, pos--){
		
		dR.top = textRect.top + pos * deltaRect.top;

		font->DrawTextA(0, it->text.c_str(), -1, &(dR), DT_NOCLIP, it->color);
	}
}