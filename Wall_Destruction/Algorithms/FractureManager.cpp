#include "FractureManager.h"
#include "MathHelper.h"

D3DXVECTOR3 FractureManager::u, FractureManager::v, FractureManager::w;
D3DXMATRIX FractureManager::displacementGradientTranspose;
D3DXMATRIX FractureManager::B ;
float FractureManager::threshold = 30.0f;

float FractureManager::CalculateWeight(ProjectStructs::PHYXEL_NODE *xi, ProjectStructs::PHYXEL_NODE *xj){
	float r = abs(D3DXVec3Length(&(xi->pos - xj->pos))) / xi->supportRadius;
	
	if(r <= 1.0f){
		return 1 - 6 * r * r + 8 * r * r * r - 3 * r * r * r * r;
	}
	
	return 0.0f;
}

void FractureManager::CalculateJacobian(ProjectStructs::Phyxel_Grid_Cell *cell){

}

void FractureManager::CalculateShape(ProjectStructs::Phyxel_Grid_Cell *cell){

}

void FractureManager::CalculateStrain(ProjectStructs::Phyxel_Grid_Cell *cell){

}

void FractureManager::CalculateStress(ProjectStructs::Phyxel_Grid_Cell *cell){

}

/*
E
Eelastic
Eplastic
V
E'
Stress
StressElastic
StressViscous


algorithm taken from animation of brittle fracture thesis
	1 foreach ell in elements of ( mesh )
	2		compute E using (3.1) and (3.26)
	3		compute V using (3.3), (3.26) and (3.27)
	4		compute E' using (3.4)
	5		if J(E') > k1
	6			update Eplastic according to (3.8)
	7		Eelastic := E − Eplastic
	8		compute StressElastic using (3.12)
	9		compute StressViscous using (3.13)
	10		Stress := StressElastic + StressViscous

	11 foreach nod in nodes of ( mesh )
	12		fnod := 0
	13		foreach ell in elements attached to ( nod )
	14			accumulate fnod due to ell using (3.29)
	15		accnod := fnod	/ mass( nod )

	16 compute accelerations due to external forces, i.e. gravity, collisions

	17 foreach nod in nodes of ( mesh )
	18		xnod += deltaT vnod + 1	2(deltaT)2 accnod
	19		vnod += deltaT accnod
*/
/*void FractureManager::PreStepFractureAlgorithm(Volume *volume, float dt){

	//1 foreach ell in elements of ( mesh )
	for(unsigned int i = 0; i<volume->GetChangedPhyxelsSize(); i++){
		ProjectStructs::PHYXEL_NODE* phyxel = volume->GetChangedPhyxel(i);
		// 2		compute E using (3.1) and (3.26)
		// dx / du = P*beta*delta
		// P = pos    , beta = materialPos, 

	}
}*/

void FractureManager::PreStepFractureAlgorithm( Volume *volume, float dt )
{
/*
	send the info to the GPU and calculate everything there... later
*/
	// add external forces
	for(unsigned int i = 0; i<volume->GetChangedPhyxelsSize(); i++){
		ProjectStructs::PHYXEL_NODE* node = volume->GetChangedPhyxel(i);
		node->bodyForce = node->force / (dt * 0.001f);

	}

	for(unsigned int i = 0; i<volume->GetChangedPhyxelsSize(); i++){	
		ProjectStructs::PHYXEL_NODE* node = volume->GetChangedPhyxel(i);

		u = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		v = u;
		w = u;

		for(unsigned int j = 0; j<node->neighbours.GetSize(); j++){
			ProjectStructs::PHYXEL_NODE* neighbour = node->neighbours[j];
			if(!node->neighbours[j])
				continue;

			float weight = node->neighbourWeight[j];

			u += (node->neighbours[j]->displacement[0] - node->displacement[0]) * (node->neighbours[j]->pos - node->pos) * node->neighbourWeight[j];
			v += (node->neighbours[j]->displacement[1] - node->displacement[1]) * (node->neighbours[j]->pos - node->pos) * node->neighbourWeight[j];
			w += (node->neighbours[j]->displacement[2] - node->displacement[2]) * (node->neighbours[j]->pos - node->pos) * node->neighbourWeight[j];
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


		if(abs(D3DXVec3Length(&u)) > 1.0f){
			printf("hmm...");
		}
		if(abs(D3DXVec3Length(&v)) > 1.0f){
			printf("hmm...");
		}
		if(abs(D3DXVec3Length(&w)) > 1.0f){
			printf("hmm...");
		}

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

		node->strain = 0.5f * (node->displacementGradient + displacementGradientTranspose + displacementGradientTranspose * node->displacementGradient);
		
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

		float maxEigenValue;
		D3DXVECTOR3 maxEigenVector;
		MathHelper::CalculateMaximumEigenvalues(node->stress, maxEigenValue, maxEigenVector);

		if(maxEigenValue > volume->GetMaterialProperties().toughness ){
			// add the node to a fracturing list
			Helpers::Globals::DebugInformation.AddText(DEBUG_TYPE, "CRACK!!!");
		}

		B = -2.0f * node->volume * (node->displacementGradient + MathHelper::GetIdentity()) * node->stress * node->momentMatrix;
		
		for(unsigned int j = 0; j<node->neighbours.GetSize(); j++){
			if(!node->neighbours[j])
				continue;

			// reuse the U vector
			u = (node->neighbours[j]->pos - node->pos) * node->neighbourWeight[j];
			v = u;
			
			if(abs(D3DXVec3Length(&u)) > 1.0f){
				printf("hmm...");
			}

			u.x = v.x * B._11 + v.y * B._12 + v.z * B._13;
			u.y = v.x * B._21 + v.y * B._22 + v.z * B._23;
			u.z = v.x * B._31 + v.y * B._32 + v.z * B._33;

			if(abs(D3DXVec3Length(&u)) > 1.0f){
				printf("hmm...");
			}

			node->bodyForce -= u;
			node->neighbours[j]->bodyForce += u;
		}	
	}

	for(unsigned int i = 0; i<volume->GetChangedPhyxelsSize(); i++){
		ProjectStructs::PHYXEL_NODE* node = volume->GetChangedPhyxel(i);
		
		node->dotDisplacement = node->dotDisplacement + dt * node->bodyForce / node->mass;
		node->displacement = node->displacement + dt * node->dotDisplacement;
		if(abs(D3DXVec3Length(&node->dotDisplacement)) > 1.0f){
			printf("hmm...");
		}
		if(abs(D3DXVec3Length(&node->displacement)) > 1.0f){
			printf("hmm...");
		}
	}

	if(volume->GetChangedPhyxelsSize() != 0)
		volume->CleanChangedPhyxels();
}

void FractureManager::StepFractureAlgorithm(Volume *volume){
	
}