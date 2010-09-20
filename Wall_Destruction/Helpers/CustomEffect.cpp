#include "CustomEffect.h"
#include <d3d10.h>

namespace Helpers{
	
	CustomEffect::CustomEffect(string shaderName, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC *layout, int layoutCount){
		Init(shaderName, technique, effectType, layout, layoutCount, NULL);
	}

	CustomEffect::CustomEffect(string shaderName, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC *layout, int layoutCount, const D3D10_SHADER_MACRO *macros){
		Init(shaderName, technique, effectType, layout, layoutCount, macros);
	}

	void CustomEffect::Init(string shaderName, string technique, int effectType, D3D10_INPUT_ELEMENT_DESC *layout, int layoutCount, const D3D10_SHADER_MACRO *macros){

		texturesSet = false;
		effect = NULL;
		this->name = "Shaders\\" + shaderName;

		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
		
		ID3D10Blob* compilationErrors = 0;

		HRESULT hr = D3DX10CreateEffectFromFile( name.c_str(), macros, NULL, "fx_4_0", shaderFlags, 0, Globals::Device, NULL,
			NULL, &effect, &compilationErrors, NULL ) ;

		if(FAILED(hr)){
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			return;
		}

		Technique = effect->GetTechniqueByName(technique.c_str());

		ERR(Technique->GetDesc( &techDesc ), "Technique " +technique+ " is not available");

		pVertexLayout = new ID3D10InputLayout*[ techDesc.Passes ];
		vertexLayoutSize = techDesc.Passes;
		for( UINT idx = 0; idx < techDesc.Passes; idx ++ )
		{
			D3D10_PASS_DESC PassDesc;
			HR(Technique->GetPassByIndex( idx )->GetDesc( &PassDesc ));

			ERR(Globals::Device->CreateInputLayout( layout, layoutCount, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pVertexLayout[idx]), 
				"Error creating " +shaderName + "-" +technique+ " : Does the layout match the effect layout?");
		}

		this->effectType = effectType;
	}

	void CustomEffect::PreDraw(){
 		Globals::Device->IASetInputLayout( *pVertexLayout );
	}

	void CustomEffect::Draw(int primitiveCount, int vertexSize){
		// render
		Technique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			for(int i = 0; i<=primitiveCount; i+=vertexSize)
				Globals::Device->Draw( vertexSize, i);
		}
	}

	void CustomEffect::Draw(int primitiveCount){
		// render
		Technique->GetDesc( &techDesc );
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			Globals::Device->Draw( primitiveCount, 0 );
		}
	}

	void CustomEffect::DrawIndexed(int indexCount){
		// render
		for( UINT p = 0; p < techDesc.Passes; ++p )
		{
			Technique->GetPassByIndex( p )->Apply( 0 );
			Globals::Device->DrawIndexed( indexCount, 0, 0);
		}
	}

	void CustomEffect::DrawAuto(){		
		// render
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
		texturesSet = false;
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

	void CustomEffect::SetTexture(string variable, ID3D10ShaderResourceView* texture){
		effectVariables[variable]->AsShaderResource()->SetResource(texture);
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

		if(!effect){
			return;
		}

		if(pVertexLayout){
			for(unsigned int i = 0; i<vertexLayoutSize; i++){
				if(pVertexLayout[i])
 					pVertexLayout[i]->Release();
			}
			delete [] pVertexLayout;
		}

		std::map<std::string, ID3D10ShaderResourceView*, ProjectStructs::NameComparer>::const_iterator textureItr;
		for(textureItr = textureSRV.begin(); textureItr != textureSRV.begin(); textureItr++){
			textureItr->second->Release();
			delete textureItr->second;
		}
		textureSRV.clear();

		std::map<std::string, ID3D10EffectVariable*, ProjectStructs::NameComparer>::const_iterator variableItr;
		for(variableItr = effectVariables.begin(); variableItr != effectVariables.begin(); variableItr++){
			delete variableItr->second;
		}
		effectVariables.clear();

		textureFileNames.clear();
		textureFileNames.swap(vector<string>());

		effectVariables.swap(std::map<string, ID3D10EffectVariable*, ProjectStructs::NameComparer>());
		textureSRV.swap(std::map<string, ID3D10ShaderResourceView*, ProjectStructs::NameComparer>());

		if(effect){
			effect->Release();
			effect = NULL;
		}
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
		Draw(inputVertexCount); 

		// Get back to normal
		pBuffers[0] = NULL;
		Globals::Device->SOSetTargets( 1, pBuffers, offset );

		//std:swap(inputBuffer, outputBuffer);
		
		return outputBuffer;
	}

}


