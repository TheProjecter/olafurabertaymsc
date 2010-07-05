#ifndef CUSTOM_EFFECT_H
#define CUSTOM_EFFECT_H

#include <string>
#include <map>
#include <vector>
#include <D3D10.h>
#include <D3DX10.h>
#include "Globals.h"
#include "Structs.h"

#define CUSTOM_EFFECT_TYPE_VERTEX (1 << 0)
#define CUSTOM_EFFECT_TYPE_GEOMETRY (1 << 1)
#define CUSTOM_EFFECT_TYPE_PIXEL (1 << 2)

using namespace std;

namespace Helpers{
	class CustomEffect
	{
	public:
		CustomEffect(){}
		CustomEffect(string name, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC layout[], int layoutCount);
		CustomEffect(string shaderName, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC *layout, int layoutCount, const D3D10_SHADER_MACRO *macros);
		~CustomEffect(void){};

		void PreDraw();
		void Draw(int primitiveCount);
		void Draw(int primitiveCount, int vertexSize);
		void DrawIndexed(int indexCount);
		void DrawAuto();
		void CleanUp();
		
		void AddVariable(string variable);
		void AddTexture(string variable, string texture);

		void SetTexture(string variable, ID3D10ShaderResourceView* texture);
		void SetTexture(string variable, string texture);
		void SetMatrix(string variable, D3DXMATRIX matrix);
		void SetBoolVector(string variable, BOOL *vector);
		void SetIntVector(string variable, int *vector);
		void SetFloatVector(string variable, float *vector);
		void SetBool(string variable, BOOL scalar);
		void SetInt(string variable, int scalar);
		void SetFloat(string variable, float scalar);
		
		void ChangeTechnique();

		ID3D10EffectTechnique* GetTechnique(){return Technique;}

		// creates the vertex buffer every time, since this geometry shader is considered a special worker for the cpu
		ID3D10Buffer* WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY inputTopology, ID3D10Buffer* inputBuffer, int inputVertexCount, ID3D10Buffer* outputBuffer);

	private:
		void Init(string shaderName, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC *layout, int layoutCount, const D3D10_SHADER_MACRO *macros);
		void CheckForGeometryShaderSupport(){
			if(effectType & CUSTOM_EFFECT_TYPE_GEOMETRY)
				return;

			string errorString = name + " is not defined as a Geometry shader\n Construct the CustomEffect as CUSTOM_EFFECT_TYPE_GEOMETRY";
			std::stringstream out;							
			out << "Error - file: " << __FILE__ << " - line: " << (DWORD) __LINE__;	
			MessageBox(0, errorString.c_str(), out.str().c_str(), 0);             
		}

		int effectType;
		string name;
		ID3D10Effect* effect;
		ID3D10InputLayout **pVertexLayout;
		ID3D10EffectTechnique* Technique;

		UINT vertexLayoutSize;
		bool texturesSet;

		std::map<string, ID3D10EffectVariable*, Structs::NameComparer> effectVariables;
		std::vector<string> textureFileNames;
		std::map<string, ID3D10ShaderResourceView*, Structs::NameComparer> textureSRV;
	};
}

#endif
