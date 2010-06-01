#include "CustomEffect.h"
#include "Globals.h"

namespace Helpers{
	
	CustomEffect::CustomEffect(string name, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC layout[], int layoutCount){
		
		effect = NULL;

		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		
		ID3D10Blob* compilationErrors = 0;

		HRESULT hr = D3DX10CreateEffectFromFile( name.c_str(), NULL, NULL, "fx_4_0", shaderFlags, 0, Globals::Device, NULL,
			NULL, &effect, &compilationErrors, NULL ) ;

		if(FAILED(hr)){
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			return;
		}

		pTechnique = effect->GetTechniqueByName(technique.c_str());
		D3D10_TECHNIQUE_DESC pDesc;

		if(FAILED(pTechnique->GetDesc( &pDesc ))){
			string error = "Technique " +technique+ " is not available";
			MessageBoxA(0, error.c_str(), 0, 0);
			return;
		}

		pVertexLayout = new ID3D10InputLayout*[ pDesc.Passes ];
		vertexLayoutSize = pDesc.Passes;
		for( UINT idx = 0; idx < pDesc.Passes; idx ++ )
		{
			D3D10_PASS_DESC PassDesc;
			HR(pTechnique->GetPassByIndex( idx )->GetDesc( &PassDesc ));

			HR(Globals::Device->CreateInputLayout( layout, layoutCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pVertexLayout[idx]));
		}

		effectType = effectType;
	}

	void CustomEffect::PreDraw(){
 		Globals::Device->IASetInputLayout( *pVertexLayout );
	}

	void CustomEffect::Draw(int primitiveCount){

		// Render a triangle
		D3D10_TECHNIQUE_DESC techDesc;
		pTechnique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			pTechnique->GetPassByIndex( p )->Apply( 0 );
			Globals::Device->Draw( primitiveCount, 0 );
		}
	}

	void CustomEffect::AddVariable(string variable){
		if(effectVariables.find(variable) == effectVariables.end()){
			effectVariables[variable] = effect->GetVariableByName(variable.c_str());
		}
	}

	void CustomEffect::SetMatrix(string variable, D3DXMATRIX matrix){
		effectVariables[variable]->AsMatrix()->SetMatrix(matrix);
	}

	void CustomEffect::SetBoolVector(string variable, BOOL *vector){
		effectVariables[variable]->AsVector()->SetBoolVector(vector);
	}

	void CustomEffect::SetIntVector(string variable, int *vector){
		effectVariables[variable]->AsVector()->SetIntVector(vector);
	}

	void CustomEffect::SetFloatVector(string variable, float *vector){
		effectVariables[variable]->AsVector()->SetFloatVector(vector);
	}
	
	void CustomEffect::SetBool(string variable, BOOL scalar){
		effectVariables[variable]->AsScalar()->SetBool(scalar);
	}

	void CustomEffect::SetInt(string variable, int scalar){
		effectVariables[variable]->AsScalar()->SetInt(scalar);
	}

	void CustomEffect::SetFloat(string variable, float scalar){
		effectVariables[variable]->AsScalar()->SetFloat(scalar);
	}

	void CustomEffect::CleanUp(){

		for(int i = 0; i<vertexLayoutSize; i++){
			pVertexLayout[i]->Release();
		}
		delete [] pVertexLayout;

		effect->Release();
	}
}


