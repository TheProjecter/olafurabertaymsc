#ifndef INFO_TEXT_H
#define INFO_TEXT_H

#include <list>
#include <string>
#include <D3DX10.h>
#include "Drawable.h"

namespace Drawables{
	
	struct INFO_TEXT{
		std::string text;
		D3DXCOLOR color;
		float life;
	};

	class InfoText
	{
	public:
		InfoText();
		InfoText(int x, int y);
		~InfoText(void);

		void AddText(const char *strText, ...);
		void AddText(D3DXCOLOR color, const char *strText, ...);
		
		void AddText(D3DXCOLOR color, std::string text);
		void AddText(std::string text);
		
		void Draw();
		void Update(float dt);
		void CleanUp();
		void SetRect(int x, int y);

	private:
		void Init(int x, int y);
		std::list<INFO_TEXT> texts;
		ID3DX10Font* font;
		RECT textRect, deltaRect;
		float deterioration;
		static D3DXCOLOR defaultColor;
	};
}

#endif