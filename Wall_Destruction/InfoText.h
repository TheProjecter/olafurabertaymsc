#ifndef INFO_TEXT_H
#define INFO_TEXT_H

#include <list>
#include <string>
#include <D3DX10.h>

struct INFO_TEXT{
	std::string text;
	D3DXCOLOR color;
	float life;
};

class InfoText
{
public:
	InfoText();
	InfoText(float x, float y);
	~InfoText(void);

	void Update(float dt);
	void Draw();
	void AddText(std::string text, D3DXCOLOR color);
	void AddText(std::string text);
	void CleanUp();

private:
	void Init(float x, float y);
	std::list<INFO_TEXT> texts;
	ID3DX10Font* font;
	RECT textRect, deltaRect;
	float deterioration;
};

#endif