#include "CustomEffect.h"
//#include "Globals.h"

namespace Helpers{
	
	CustomEffect::CustomEffect(string name, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC layout[], int layoutCount){
		texturesSet = false;
		effect = NULL;
		this->name = name;

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

		Technique = effect->GetTechniqueByName(technique.c_str());
		D3D10_TECHNIQUE_DESC pDesc;

		ERR(Technique->GetDesc( &pDesc ), "Technique " +technique+ " is not available");

		pVertexLayout = new ID3D10InputLayout*[ pDesc.Passes ];
		vertexLayoutSize = pDesc.Passes;
		for( UINT idx = 0; idx < pDesc.Passes; idx ++ )
		{
			D3D10_PASS_DESC PassDesc;
			HR(Technique->GetPassByIndex( idx )->GetDesc( &PassDesc ));

			ERR(Globals::Device->CreateInputLayout( layout, layoutCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pVertexLayout[idx]), "Error creating the layout, is the layoutCount equal to the layout array?");
		}

		this->effectType = effectType;
	}

	void CustomEffect::PreDraw(){
 		Globals::Device->IASetInputLayout( *pVertexLayout );
	}

	void CustomEffect::Draw(int primitiveCount, int vertexSize){
		// render
		D3D10_TECHNIQUE_DESC techDesc;
		Technique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			for(UINT i = 0; i<=primitiveCount; i+=vertexSize)
				Globals::Device->Draw( vertexSize, i);
		}
	}

	void CustomEffect::Draw(int primitiveCount){
		// render
		D3D10_TECHNIQUE_DESC techDesc;
		Technique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			Globals::Device->Draw( primitiveCount, 0 );
		}
	}

	void CustomEffect::DrawAuto(){		
		// render
		D3D10_TECHNIQUE_DESC techDesc;
		Technique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			Globals::Device->DrawAuto();
		}
	}

	void CustomEffect::AddVariable(string variable){
		if(effectVariables.find(variable) == effectVariables.end()){
			effectVariables[variable] = effect->GetVariableByName(variable.c_str());
		}
	}

	void CustomEffect::AddTexture( string variable, string texture ){
		textureFileNames.push_back(texture);
		AddVariable(variable);
	}

	void CustomEffect::SetTexture(string variable, string texture){
		if(!texturesSet){
			texturesSet = true;

			//load textures
			for ( int i=0; i < (int) textureFileNames.size(); i++ )
			{
				textureSRV[textureFileNames[i]] = NULL;
				ERR(D3DX10CreateShaderResourceViewFromFile( Helpers::Globals::Device, textureFileNames[i].c_str(), NULL, NULL, &textureSRV[textureFileNames[i]], NULL ), "Could not load texture: " + textureFileNames[i]);
			}
		}

		effectVariables[variable]->AsShaderResource()->SetResource(textureSRV[texture]);
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

		std::map<std::string, ID3D10ShaderResourceView*, NameComparer>::const_iterator textureItr;

		for(textureItr = textureSRV.begin(); textureItr != textureSRV.begin(); textureItr++){
			textureItr->second->Release();
			delete textureItr->second;
		}

		std::map<std::string, ID3D10EffectVariable*, NameComparer>::const_iterator variableItr;

		for(variableItr = effectVariables.begin(); variableItr != effectVariables.begin(); variableItr++){
			delete variableItr->second;
		}

		effect->Release();
		delete effect;
	}

	// creates the vertex buffer every time, since this geometry shader is considered a special worker for the cpu
	ID3D10Buffer* CustomEffect::WriteToGeometryShader(D3D10_PRIMITIVE_TOPOLOGY inputTopology, ID3D10Buffer* inputBuffer, int inputVertexCount, ID3D10Buffer* outputBuffer){
		CheckForGeometryShaderSupport();

		// Set IA parameters
		ID3D10Buffer* pBuffers[1];
		pBuffers[0] = inputBuffer;

		D3D10_BUFFER_DESC desc;
		inputBuffer->GetDesc(&desc);

		UINT stride[1] = { desc.ByteWidth / inputVertexCount };
		UINT offset[1] = { 0 };

		Globals::Device->IASetVertexBuffers( 0, 1, pBuffers, stride, offset );
		Globals::Device->IASetPrimitiveTopology( inputTopology );

		// Point to the correct output buffer
		pBuffers[0] = outputBuffer;
		Globals::Device->SOSetTargets( 1, pBuffers, offset );

		PreDraw();
		Draw(inputVertexCount, 1); 

		// Get back to normal
		pBuffers[0] = NULL;
		Globals::Device->SOSetTargets( 1, pBuffers, offset );

		//std:swap(inputBuffer, outputBuffer);
		
		return outputBuffer;
	}

}


