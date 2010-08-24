
#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <D3DX10.h>

namespace Drawables{
	class Drawable
	{
	public :
// 		void SetLightInfo(D3DXVECTOR3 DiffuseColor, D3DXVECTOR3 SpecularColor, float SpecularPower){
// 			this->DiffuseColor = DiffuseColor;
// 			this->SpecularColor = SpecularColor;
// 			this->SpecularPower = SpecularPower;
// 		}

		void SetMaterialInfo(float sigma, float rho){
			A = 1.0f - 0.5f * (sigma * sigma)/(sigma*sigma + 0.33f);
			B = 0.45f * (sigma * sigma)/(sigma * sigma + 0.09f);
			rhoOverPi = rho/(float)D3DX_PI;
		}

	protected:
// 		D3DXVECTOR3 DiffuseColor;
// 		D3DXVECTOR3 SpecularColor;
// 		float SpecularPower;

		float A, B, rhoOverPi;
	};
}

#endif


