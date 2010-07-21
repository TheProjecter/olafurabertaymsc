#include "InfoText.h"
#include "Globals.h"
#include <iterator>

using namespace Helpers;
namespace Drawables{
	D3DXCOLOR InfoText::defaultColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	InfoText::InfoText()
	{
		Init(50, 5);
	}

	InfoText::InfoText(int x, int y)
	{
		Init(x, y);
	}

	InfoText::~InfoText(void)
	{
	}

	void InfoText::Init(int x, int y){

		D3DX10CreateFont(Globals::Device, 16, 0, 0, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman", &font);
		
		deterioration = 0.05f;

		deltaRect.left = 0;
		deltaRect.right = 0;
		deltaRect.top = 20;
		deltaRect.bottom = 0;

		SetRect(x, y);
	}

	void InfoText::SetRect(int x, int y){
		textRect.left = x;
		textRect.right = 0;
		textRect.top = y;
		textRect.bottom = 0;
	}

	void InfoText::StartTimer(){
		startTime.push_back(clock());
	}

	void InfoText::EndTimer(TEXT_TYPE type, D3DXCOLOR color, const char *strText, ...){
		va_list args;
		char strBuffer[4096];

		if (!strText)
			return;

		va_start(args, strText);
		vsprintf_s(strBuffer, strText, args);
		va_end(args);		

		std::string str = strBuffer;
		str.append(" in %.3f sec");

		double time = (double)(clock() - startTime.front())/ (double)CLOCKS_PER_SEC;
		startTime.pop_front();

		AddText(type, color, str.c_str(), time);
	}

	void InfoText::EndTimer(TEXT_TYPE type, const char *strText, ...){
		va_list args;
		char strBuffer[4096];

		if (!strText)
			return;
		
		va_start(args, strText);
		vsprintf_s(strBuffer, strText, args);
		va_end(args);		
	
		std::string str = strBuffer;
		str.append(" in %.3f sec");

		double time = (double)(clock() - startTime.front())/ (double)CLOCKS_PER_SEC;
		startTime.pop_front();

		AddText(type, str.c_str(), time);
	}

	// printf wrapper idea taken from Henry Fortunas PS2 Framework (http://www.hsfortuna.pwp.blueyonder.co.uk/)
	void InfoText::AddText(TEXT_TYPE type, const char *strText, ...){
		va_list args;
		char strBuffer[4096];

		if (!strText)
			return;

		va_start(args, strText);
		vsprintf_s(strBuffer, strText, args);
		va_end(args);

		AddText(type, defaultColor, (std::string)strBuffer);
	}

	// printf wrapper idea taken from Henry Fortunas PS2 Framework (http://www.hsfortuna.pwp.blueyonder.co.uk/)
	void InfoText::AddText(TEXT_TYPE type, D3DXCOLOR color, const char *strText, ...){
		va_list args;
		char strBuffer[4096];

		if (!strText)
			return;

		va_start(args, strText);
		vsprintf_s(strBuffer, strText, args);
		va_end(args);

		AddText(type, color, (std::string)strBuffer);
	}

	void InfoText::AddText( TEXT_TYPE type,std::string text){
		AddText(type, defaultColor, text);
	}

	void InfoText::AddText(TEXT_TYPE type, D3DXCOLOR color, std::string text){
		// put a cap on the texts
		if(texts.size() == 30){
			texts.pop_front();
		}

		INFO_TEXT it;
		if(type == DEBUG_TYPE)
			it.text = "Debug - " +text;
		else if(type == INFO_TYPE)
			it.text = "Info - " +text;
		else if(type == ERROR_TYPE)
			it.text = "Error - " +text;
		else
			it.text = text;

		it.color = color;
		it.life = 1.0f;
		it.type = type;
		texts.push_back(it);
	}

	void  InfoText::CleanUp(){
		
		texts.clear();
		texts.swap( std::list<INFO_TEXT>() );

		startTime.clear();
		startTime.swap(std::list<clock_t>());

		font->Release();
		font = NULL;
	}

	void InfoText::Update(float dt){	
		std::list<INFO_TEXT>::iterator it;

		for(it = texts.begin(); it != texts.end(); it++){
			it->life -= dt*deterioration;
			it->color.a = it->life;
		}

		// take out the "dead" texts
		while(texts.size() > 0 && texts.front().life <= 0.0f){
			texts.erase(texts.begin());
		}	
	}

	void InfoText::Draw(){

		if(texts.size() > 0){

			std::list<INFO_TEXT>::iterator it = texts.end();
			it--;
			int pos = 0;
			RECT dR = textRect;

			for(; it != texts.begin(); it--){		
				if((it->type == DEBUG_TYPE && Helpers::Globals::SHOW_DEBUG) || 
					(it->type == INFO_TYPE && Helpers::Globals::SHOW_INFO) ||
					(it->type == ERROR_TYPE && Helpers::Globals::SHOW_ERRORS) || it->type == ALWAYS){
					dR.top = textRect.top + (pos++) * deltaRect.top;
					font->DrawTextA(0, it->text.c_str(), -1, &(dR), DT_NOCLIP, it->color);
				}
			}
			if((it->type == DEBUG_TYPE && Helpers::Globals::SHOW_DEBUG) || 
				(it->type == INFO_TYPE && Helpers::Globals::SHOW_INFO) ||
				(it->type == ERROR_TYPE && Helpers::Globals::SHOW_ERRORS) || it->type == ALWAYS){
				dR.top = textRect.top + (pos++) * deltaRect.top;
				font->DrawTextA(0, it->text.c_str(), -1, &(dR), DT_NOCLIP, it->color);
			}
		}
	}
}