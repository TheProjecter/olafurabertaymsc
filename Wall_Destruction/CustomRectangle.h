#ifndef CUSTOM_RECTANGLE
#define CUSTOM_RECTANGLE

#include <D3DX10.h>
#include "CustomEffect.h"

struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXCOLOR Color;
};


namespace Drawables{
	class CustomRectangle
	{
	public:
		CustomRectangle(void){
			height = 10.0f;
			width = 10.0f;
			color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}

		CustomRectangle(float height, float width, D3DXCOLOR color){
			this->height = height;
			this->width = height;
			this->color = color;
		}

		~CustomRectangle(void){}

		void Draw();
		void Init();

		void CleanUp(){
			effect.CleanUp();
			mVB->Release();
		}

	private:
		float height, width;
		D3DXCOLOR color;
		D3DXVECTOR3 position;
		D3DXVECTOR3 normal;

		ID3D10Buffer* mVB;
		Helpers::CustomEffect effect;
	};
};


#endif