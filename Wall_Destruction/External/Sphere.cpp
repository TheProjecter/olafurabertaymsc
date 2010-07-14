#include "Sphere.h"
#include "Globals.h"
#include "PhysicsWrapper.h"

Sphere::Sphere()
: mNumVertices(0), mNumFaces(0), mVB(0), mIB(0),
  mRadius(0.0f), mNumSlices(0), mNumStacks(0)
{
}

Sphere::~Sphere()
{
}

void Sphere::CleanUp(){
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
/*
void Sphere::UpdateWorld(){
	this->World = PhysicsWrapper::GetWorld(this->rigidBody);
}*/


void Sphere::init(float radius, UINT numSlices, UINT numStacks)
{
	mRadius    = radius;
	mNumSlices = numSlices;
	mNumStacks = numStacks;

	std::vector<ProjectStructs::SPHERE_VERTEX> vertices;
	std::vector<D3DXVECTOR3> depthVertices;
	std::vector<DWORD> indices;

	buildStacks(vertices, indices);
	
	mNumVertices = (UINT)vertices.size();
	mNumFaces    = (UINT)indices.size()/3;

	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(ProjectStructs::SPHERE_VERTEX) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
	HR(Helpers::Globals::Device->CreateBuffer(&vbd, &vinitData, &mVB));

	for(DWORD i = 0; i<mNumVertices; i++){
		depthVertices.push_back(vertices[i].pos);
	}

	D3D10_BUFFER_DESC vbdDepth;
	vbdDepth.Usage = D3D10_USAGE_IMMUTABLE;
	vbdDepth.ByteWidth = sizeof(D3DXVECTOR3) * mNumVertices;
	vbdDepth.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vbdDepth.CPUAccessFlags = 0;
	vbdDepth.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitDataDepth;
	vinitDataDepth.pSysMem = &depthVertices[0];
	HR(Helpers::Globals::Device->CreateBuffer(&vbdDepth, &vinitDataDepth, &mVBDepth));

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    HR(Helpers::Globals::Device->CreateBuffer(&ibd, &iinitData, &mIB));

	D3DXMatrixTranslation(&RadiusTranslation, 0.0f, radius, 0.0f);
}

void Sphere::Draw(Helpers::CustomEffect *effect)
{
	UINT stride = sizeof(ProjectStructs::SPHERE_VERTEX);
    UINT offset = 0;
	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Helpers::Globals::Device->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	Helpers::Globals::Device->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	effect->DrawIndexed(mNumFaces*3);
}

void Sphere::DrawDepth(Helpers::CustomEffect *effect)
{
	UINT stride = sizeof(D3DXVECTOR3);
	UINT offset = 0;
	Helpers::Globals::Device->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Helpers::Globals::Device->IASetVertexBuffers(0, 1, &mVBDepth, &stride, &offset);
	Helpers::Globals::Device->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	effect->DrawIndexed(mNumFaces*3);
}

void Sphere::buildStacks(VertexList& vertices, IndexList& indices)
{
	float phiStep = PI/mNumStacks;

	// do not count the poles as rings
	UINT numRings = mNumStacks-1;

	// Compute vertices for each stack ring.
	for(UINT i = 1; i <= numRings; ++i)
	{
		float phi = i*phiStep;

		// vertices of ring
		float thetaStep = 2.0f*PI/mNumSlices;
		for(UINT j = 0; j <= mNumSlices; ++j)
		{
			float theta = j*thetaStep;

			ProjectStructs::SPHERE_VERTEX v;

			// spherical to cartesian
			v.pos.x = mRadius*sinf(phi)*cosf(theta);
			v.pos.y = mRadius*cosf(phi);
			v.pos.z = mRadius*sinf(phi)*sinf(theta);

			// partial derivative of P with respect to theta
/*			v.tangent.x = -mRadius*sinf(phi)*sinf(theta);
			v.tangent.y = 0.0f;
			v.tangent.z = mRadius*sinf(phi)*cosf(theta);
*/
			D3DXVec3Normalize(&v.normal, &v.pos);

			v.UV.x = theta / (2.0f*PI);
			v.UV.y = phi / PI;

			vertices.push_back( v );
		}
	}

	// poles: note that there will be texture coordinate distortion
	vertices.push_back( ProjectStructs::SPHERE_VERTEX(D3DXVECTOR3(0.0f, -mRadius, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f)) );
	vertices.push_back( ProjectStructs::SPHERE_VERTEX(D3DXVECTOR3(0.0f, mRadius, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f)) );

	UINT northPoleIndex = (UINT)vertices.size()-1;
	UINT southPoleIndex = (UINT)vertices.size()-2;

	UINT numRingVertices = mNumSlices+1;

	// Compute indices for inner stacks (not connected to poles).
	for(UINT i = 0; i < mNumStacks-2; ++i)
	{
		for(UINT j = 0; j < mNumSlices; ++j)
		{
			indices.push_back(i*numRingVertices + j);
			indices.push_back(i*numRingVertices + j+1);
			indices.push_back((i+1)*numRingVertices + j);

			indices.push_back((i+1)*numRingVertices + j);
			indices.push_back(i*numRingVertices + j+1);
			indices.push_back((i+1)*numRingVertices + j+1);
		}
	}

	// Compute indices for top stack.  The top stack was written 
	// first to the vertex buffer.
	for(UINT i = 0; i < mNumSlices; ++i)
	{
		indices.push_back(northPoleIndex);
		indices.push_back(i+1);
		indices.push_back(i);
	}

	// Compute indices for bottom stack.  The bottom stack was written
	// last to the vertex buffer, so we need to offset to the index
	// of first vertex in the last ring.
	UINT baseIndex = (numRings-1)*numRingVertices;
	for(UINT i = 0; i < mNumSlices; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex+i);
		indices.push_back(baseIndex+i+1);
	}
}
 