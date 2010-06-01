#ifndef CUSTOM_EFFECT_H
#define CUSTOM_EFFECT_H

#include <string>
#include <map>
#include <D3D10.h>
#include <D3DX10.h>

#define CUSTOM_EFFECT_TYPE_VERTEX (1 << 0)
#define CUSTOM_EFFECT_TYPE_GEOMETRY (1 << 1)
#define CUSTOM_EFFECT_TYPE_PIXEL (1 << 2)

using namespace std;

namespace Helpers{
	struct EffectNameComparer {
		bool operator()( string s1, string s2 ) const {
			return s1 < s2;
		}
	};

	class CustomEffect
	{
	public:
		CustomEffect(){}
		CustomEffect(string name, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC layout[], int layoutCount);
		~CustomEffect(void){};

		void PreDraw();
		void Draw(int primitiveCount);
		void CleanUp();
		
		void AddVariable(string variable);
		void SetMatrix(string variable, D3DXMATRIX matrix);
		void SetBoolVector(string variable, BOOL *vector);
		void SetIntVector(string variable, int *vector);
		void SetFloatVector(string variable, float *vector);
		void SetBool(string variable, BOOL scalar);
		void SetInt(string variable, int scalar);
		void SetFloat(string variable, float scalar);

	private:
		int effectType;
		ID3D10Effect* effect;
		ID3D10InputLayout **pVertexLayout;
		ID3D10EffectTechnique* pTechnique;

		UINT vertexLayoutSize;

		std::map<string, ID3D10EffectVariable*, EffectNameComparer> effectVariables;
	};
}

#endif