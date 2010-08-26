#include "FractureManager.h"
#include "MathHelper.h"
#include "EigenProblemSolver.h"

D3DXVECTOR3 FractureManager::u, FractureManager::v, FractureManager::w;
D3DXMATRIX FractureManager::displacementGradientTranspose;
D3DXMATRIX FractureManager::B ;
float FractureManager::threshold = 30.0f;

float FractureManager::CalculateWeight(D3DXVECTOR3 x1, D3DXVECTOR3 x2, float supportRadius){
	float r = D3DXVec3Length(&(x1- x2));
	float weight = 0.0f;

	if(r < supportRadius){
		weight = (315.0f / (64.0f * (float)D3DX_PI * (float)pow(supportRadius, 9))) * (float)pow(supportRadius * supportRadius - r * r, 3);
	}

	return weight;
}

float FractureManager::CalculateWeight(ProjectStructs::PHYXEL_NODE *xi, ProjectStructs::PHYXEL_NODE *xj){
	return CalculateWeight(xi->pos, xj->pos, xi->supportRadius);
}

void FractureManager::CalculateAndInitiateFractures( Volume *volume, float dt )
{
/*
	send the info to the GPU and calculate everything there... later
*/
	std::vector<ProjectStructs::CRACK*> cracks;
	bool cracked = false;

	// add external forces
	for(unsigned int i = 0; i<ImpactList::GetImpactCount(); i++){
		ProjectStructs::PHYXEL_NODE* node = ImpactList::GetImpact(i)->phyxel;
		node->bodyForce = node->force / (dt * 0.001f);

		node->displacement += dt * dt * node->bodyForce / node->mass;

		node->bodyForce.x = 0.0f;
		node->bodyForce.y = 0.0f;
		node->bodyForce.z = 0.0f;	
//		node->force = node->bodyForce;
	}

	for(unsigned int i = 0; i<ImpactList::GetImpactCount(); i++){	
		ProjectStructs::PHYXEL_NODE* node = ImpactList::GetImpact(i)->phyxel;

		CalculateStressAndStrain(node, volume);

		float maximumEigenValue;
		D3DXVECTOR3 maximumEigenVector;

		EigenProblemSolver::GetMaxEigenVectorAndValue(node->stress, maximumEigenValue, maximumEigenVector);
		if(maximumEigenValue > volume->GetMaterialProperties().toughness ){
			// add the node to a fracturing list			
/*
			cracked = true;

			ProjectStructs::CRACK* crack = new ProjectStructs::CRACK;
			crack->eigenVector = maximumEigenVector;
			crack->impact = volume->GetImpact(i);
			cracks.push_back(crack);

			node->stress = D3DXMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 
										0.0f, 0.0f, 0.0f, 0.0f, 
										0.0f, 0.0f, 0.0f, 0.0f, 
										0.0f, 0.0f, 0.0f, 0.0f);
			node->strain = node->stress;
			node->bodyForce = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			continue;*/
		}

		D3DXMatrixMultiply(&B, &(node->displacementGradient + MathHelper::Get3x3Identity()), &node->stress);
		D3DXMatrixMultiply(&B, &B, &node->momentMatrix);
		B *= -2.0f * node->volume;
		
		for(unsigned int j = 0; j<node->neighbours.GetSize(); j++){
			if(!node->neighbours[j])
				continue;

			// reuse the U vector
			u = (node->neighbours[j]->pos - node->pos) * node->neighbourWeight[j];
			v = u;

			u.x = v.x * B._11 + v.y * B._12 + v.z * B._13;
			u.y = v.x * B._21 + v.y * B._22 + v.z * B._23;
			u.z = v.x * B._31 + v.y * B._32 + v.z * B._33;

			node->bodyForce += u;
			node->neighbours[j]->bodyForce -= u;
		}
	}

	for(unsigned int i = 0; i<ImpactList::GetImpactCount(); i++){
		ProjectStructs::PHYXEL_NODE* node = ImpactList::GetImpact(i)->phyxel;

		node->dotDisplacement = dt * node->bodyForce / node->mass;

		node->displacement += dt * node->dotDisplacement;
		
		node->dotDisplacement *= 0;

		node->isChanged = node->dotDisplacement.x != 0.0f || node->dotDisplacement.y != 0.0f || node->dotDisplacement.z != 0.0f;
/*
		node->displacementGradient = D3DXMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 
												0.0f, 0.0f, 0.0f, 0.0f, 
												0.0f, 0.0f, 0.0f, 0.0f,
												0.0f, 0.0f, 0.0f, 0.0f);

		node->force = MathHelper::GetZeroVector();
		node->strain = node->displacementGradient;
		node->stress = node->displacementGradient;
		node->bodyForce = MathHelper::GetZeroVector();
*/
	}

	if(cracked)
		StepFractureAlgorithm(volume, cracks);
}

void FractureManager::StepFractureAlgorithm(Volume *volume, std::vector<ProjectStructs::CRACK*> cracks){

	Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, RED, "Added %d cracks", cracks.size());

	for(unsigned int crackIndex = 0; crackIndex<cracks.size(); crackIndex++){
		ProjectStructs::CRACK* crack = cracks[crackIndex];

//		AddSamples(crack, volume);


		delete crack;
	}

	cracks.clear();
}

void FractureManager::AddSamples( ProjectStructs::CRACK* crack, Volume* volume )
{
	float radius = D3DXVec3Length(&crack->eigenVector);

	// create a sphere of samplepoints into the phyxel node		
	float phiStep = PI/3;

	// do not count the poles as rings
	UINT numRings = 2;

	ProjectStructs::PHYXEL_NODE* northPole = ProjectStructs::StructHelper::CreatePhyxelNodePointer();

	// spherical to cartesian
	northPole->pos.x = crack->impact->phyxel->pos.x;
	northPole->pos.y = crack->impact->phyxel->pos.y + radius;
	northPole->pos.z = crack->impact->phyxel->pos.z;
	D3DXVECTOR3 tmp;
	D3DXVec3TransformCoord(&tmp, &northPole->pos, &volume->GetPhyxelGrid()->GetInvWorld());

	crack->impact->phyxel->addedNodes.push_back(northPole);

	ProjectStructs::PHYXEL_NODE* southPole = ProjectStructs::StructHelper::CreatePhyxelNodePointer();
	
	// spherical to cartesian
	southPole->pos.x = crack->impact->phyxel->pos.x;
	southPole->pos.y = crack->impact->phyxel->pos.y - radius;
	southPole->pos.z = crack->impact->phyxel->pos.z;
	D3DXVec3TransformCoord(&tmp, &southPole->pos, &volume->GetPhyxelGrid()->GetInvWorld());
	crack->impact->phyxel->addedNodes.push_back(southPole);

	// Compute vertices for each stack ring.
	for(UINT i = 1; i <= numRings; ++i)
	{
		float phi = i*phiStep;

		// vertices of ring
		float thetaStep = 2.0f*PI/3;
		for(UINT j = 0; j <= 3; ++j)
		{
			float theta = j*thetaStep;

			ProjectStructs::PHYXEL_NODE* node = ProjectStructs::StructHelper::CreatePhyxelNodePointer();

			// spherical to cartesian
			node->pos.x = crack->impact->phyxel->pos.x + radius*sinf(phi)*cosf(theta);
			node->pos.y = crack->impact->phyxel->pos.y + radius*cosf(phi);
			node->pos.z = crack->impact->phyxel->pos.z + radius*sinf(phi)*sinf(theta);

			// check if the node is inside of a filled phyxel
			D3DXVECTOR3 index = volume->GetPhyxelGrid()->GetIndexOfPosition(tmp);

			ProjectStructs::Phyxel_Grid_Cell* cell = volume->GetPhyxelGrid()->GetCells()(index);
			if(cell != NULL && cell->phyxel != NULL){
				cell->phyxel->addedNodes.push_back(node);
			}
		}
	}
}

void FractureManager::CalculateStressAndStrain( ProjectStructs::PHYXEL_NODE* node, Volume * volume)
{
	u = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	v = u;
	w = u;

	for(unsigned int j = 0; j<node->neighbours.GetSize(); j++){
		ProjectStructs::PHYXEL_NODE* neighbour = node->neighbours[j];
		if(!node->neighbours[j])
			continue;

		float weight = CalculateWeight(node->neighbours[j], node);

		u += (node->neighbours[j]->displacement[0] - node->displacement[0]) * (node->neighbours[j]->pos - node->pos) * weight;
		v += (node->neighbours[j]->displacement[1] - node->displacement[1]) * (node->neighbours[j]->pos - node->pos) * weight;
		w += (node->neighbours[j]->displacement[2] - node->displacement[2]) * (node->neighbours[j]->pos - node->pos) * weight;
	}

	D3DXVECTOR3 tmp = u;
	u.x = tmp.x * node->momentMatrix._11 + tmp.y * node->momentMatrix._12 + tmp.z * node->momentMatrix._13;
	u.y = tmp.x * node->momentMatrix._21 + tmp.y * node->momentMatrix._22 + tmp.z * node->momentMatrix._23;
	u.z = tmp.x * node->momentMatrix._31 + tmp.y * node->momentMatrix._32 + tmp.z * node->momentMatrix._33;

	tmp = v;
	v.x = tmp.x * node->momentMatrix._11 + tmp.y * node->momentMatrix._12 + tmp.z * node->momentMatrix._13;
	v.y = tmp.x * node->momentMatrix._21 + tmp.y * node->momentMatrix._22 + tmp.z * node->momentMatrix._23;
	v.z = tmp.x * node->momentMatrix._31 + tmp.y * node->momentMatrix._32 + tmp.z * node->momentMatrix._33;

	tmp = w;
	w.x = tmp.x * node->momentMatrix._11 + tmp.y * node->momentMatrix._12 + tmp.z * node->momentMatrix._13;
	w.y = tmp.x * node->momentMatrix._21 + tmp.y * node->momentMatrix._22 + tmp.z * node->momentMatrix._23;
	w.z = tmp.x * node->momentMatrix._31 + tmp.y * node->momentMatrix._32 + tmp.z * node->momentMatrix._33;

	node->displacementGradient(0, 0) = u[0];
	node->displacementGradient(0, 1) = u[1]; 
	node->displacementGradient(0, 2) = u[2]; 
	node->displacementGradient(1, 0) = v[0]; 
	node->displacementGradient(1, 1) = v[1]; 
	node->displacementGradient(1, 2) = v[2]; 
	node->displacementGradient(2, 0) = w[0]; 
	node->displacementGradient(2, 1) = w[1]; 
	node->displacementGradient(2, 2) = w[2]; 

	D3DXMatrixTranspose(&displacementGradientTranspose, &node->displacementGradient);

	D3DXMatrixMultiply(&node->strain, &displacementGradientTranspose, &node->displacementGradient);
	node->strain += node->displacementGradient + displacementGradientTranspose;
	node->strain *= 0.5f;

	// stress
	float vp = volume->GetMaterialProperties().poissonRatio;
	float preMultiplier = volume->GetMaterialProperties().youngsModulus / ((1.0f + vp) * (1.0f - 2.0f*vp));

	// stress is calculated in eq 7.8 p 371 in point based graphics
	node->stress(0, 0) = preMultiplier * ((1 - vp) * node->strain(0, 0) + vp * node->strain(1, 1) + vp * node->strain(2, 2));
	node->stress(1, 1) = preMultiplier * (vp * node->strain(0, 0) + (1 - vp) * node->strain(1, 1) + vp * node->strain(2, 2));
	node->stress(2, 2) = preMultiplier * (vp * node->strain(0, 0) + vp * node->strain(1, 1) + (1 - vp) * node->strain(2, 2));
	node->stress(0, 1) = preMultiplier * (1 - 2*vp) * node->strain(0, 1);
	node->stress(1, 0) = preMultiplier * (1 - 2*vp) * node->strain(1, 0);
	node->stress(1, 2) = preMultiplier * (1 - 2*vp) * node->strain(1, 2);
	node->stress(2, 1) = preMultiplier * (1 - 2*vp) * node->strain(2, 1);
	node->stress(2, 0) = preMultiplier * (1 - 2*vp) * node->strain(2, 0);
	node->stress(0, 2) = preMultiplier * (1 - 2*vp) * node->strain(0, 2);
}