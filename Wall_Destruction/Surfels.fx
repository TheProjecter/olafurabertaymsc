
float4x4 World;
float4x4 View;
float4x4 Projection;

struct VS_OUTPUT{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

//
// Vertex Shader
//
VS_OUTPUT VS( float4 Pos : POSITION , float4 Color : COLOR) 
{
	VS_OUTPUT output = (VS_OUTPUT) 0;
	output.Pos = Pos;
	output.Color = Color;

    return output;
}

struct GS_OUTPUT
{
	float4 Pos  : SV_Position;
	float4 Color : COLOR;
};

//we take one triangle but output two, which are 6 vertices then.
[maxvertexcount(6)]
//we get triangles (3 vertices) as input.
//Because we have a quad we could set here 6 and maxvertexcount to 12.
void GS( triangle VS_OUTPUT In[3], inout TriangleStream<GS_OUTPUT> TriStream )
{
    GS_OUTPUT Out;
    //First loop through the first, 'normal' triangle
    //transform its 3 vertices and add them to the output stream
	int v;
    for( v = 0; v < 3; v++ )
    {
        Out.Pos = mul( mul( mul(In[v].Pos, World), View), Projection);//Now transform to screen
        Out.Color = In[v].Color;
      //  Out.TexCoords = In[v].TexCoords;
		TriStream.Append( Out );
    }  
    TriStream.RestartStrip();//to end the first triangle
    
    //Now create 3 new vertices 
    for( v = 0; v < 3; v++ )
    {
	  //add new triangle to the right of the previous triangle
	  float4 VertexPos = In[v].Pos + float4(2.0, 0.0, 0.0, 0.0);
	  Out.Pos = mul( mul( mul(VertexPos, World), View), Projection);
	  Out.Color = float4(1.0, 0.0, 0.0, 1.0);//white
	  //we add 1 in x (or u) so that we get TexCoords from 1 to 2 (in u)
	  //so that you can play with AddressU in the SamplerState above.
	//  Out.TexCoords = float2(In[v].TexCoords.x+1.0, In[v].TexCoords.y);
	  TriStream.Append( Out );
    }
    TriStream.RestartStrip();//to end the 2nd triangle
}

//
// Pixel Shader
//
float4 PS( VS_OUTPUT input) : SV_Target
{
    return input.Color;
}


technique10 SimpleSurfelTechnique
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( CompileShader(gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
