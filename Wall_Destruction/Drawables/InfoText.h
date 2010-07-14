#ifndef INFO_TEXT_H
#define INFO_TEXT_H

#include <list>
#include <string>
#include <D3DX10.h>
#include "Drawable.h"
#include <ctime>

namespace Drawables{
	
	enum TEXT_TYPE {INFO_TYPE, DEBUG_TYPE, ERROR_TYPE, ALWAYS};

	struct INFO_TEXT{
		std::string text;
		D3DXCOLOR color;
		float life;
		TEXT_TYPE type;
	};

	class InfoText
	{
	public:
		InfoText();
		InfoText(int x, int y);
		~InfoText(void);

		void AddText(TEXT_TYPE type, const char *strText, ...);
		void AddText(TEXT_TYPE type, D3DXCOLOR color, const char *strText, ...);
		
		void AddText(TEXT_TYPE type, D3DXCOLOR color, std::string text);
		void AddText(TEXT_TYPE type, std::string text);

		void StartTimer();
		void EndTimer(TEXT_TYPE type, D3DXCOLOR color, const char *strText, ...);
		void EndTimer(TEXT_TYPE type, const char *strText, ...);
		
		void Draw();
		void Update(float dt);
		void CleanUp();
		void SetRect(int x, int y);

	private:
		void Init(int x, int y);
		std::list<INFO_TEXT> texts;
		std::list<clock_t> startTime;

		ID3DX10Font* font;
		RECT textRect, deltaRect;
		float deterioration;
		static D3DXCOLOR defaultColor;
	};
}

#endif