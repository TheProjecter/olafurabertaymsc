#include "Projectiles.h"
#include "PhysicsWrapper.h"
#include "Globals.h"

void Projectiles::Init(){
	initialVelocity = 200.0f;

	// have the deterioration slow so the projectiles can actually do something
	deterioration = 0.3f;
	deltaTime = 0.5f;
	lastShotTime = 1.0f;
	this->projectileSphere.init(1.0f, 10, 10);

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // normal
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 2*sizeof(D3DXVECTOR3), D3D10_INPUT_PER_VERTEX_DATA, 0 }, // dimensions
	};

	this->SetMaterialInfo(0.600672f, 0.668533f);

	projectileEffect = Helpers::CustomEffect("SphereEffect.fx", "SphereTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, layout, 3);
	projectileEffect.AddVariable("World");
	projectileEffect.AddVariable("View");
	projectileEffect.AddVariable("Projection");
	// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0019_2_S.jpg?id=38615&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
	projectileEffect.AddTexture("tex", "Textures\\Projectile.jpg");

	projectileEffect.AddVariable("LightPos");
	projectileEffect.AddVariable("AmbientColor");
	projectileEffect.AddVariable("CameraPos");
	projectileEffect.AddVariable("LightDirection");
	projectileEffect.AddVariable("A");
	projectileEffect.AddVariable("B");
	projectileEffect.AddVariable("rhoOverPi");
	projectileEffect.AddVariable("LightColor");		
	projectileEffect.AddVariable("windowWidth");
	projectileEffect.AddVariable("windowHeight");

	projectileEffect.SetFloatVector("AmbientColor", Helpers::Globals::AppLight.GetAmbientColor());
	projectileEffect.SetFloatVector("LightPos", Helpers::Globals::AppLight.GetPosition());
	projectileEffect.SetFloatVector("LightDirection", Helpers::Globals::AppLight.GetDirection());
	projectileEffect.SetFloat("A", this->A);
	projectileEffect.SetFloat("B", this->B);
	projectileEffect.SetFloat("rhoOverPi", this->rhoOverPi);
	projectileEffect.SetFloatVector("LightColor", Helpers::Globals::AppLight.GetColor());


	// image taken from http://www.cgtextures.com/getfile.php/ConcreteRough0019_2_S.jpg?id=38615&s=s&PHPSESSID=fb96f672ea0d7aff54fd54fe3f539e00
	projectileEffect.SetTexture("tex", "Textures\\Projectile.jpg");

	D3D10_INPUT_ELEMENT_DESC depthLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 }, // pos
	};

	depthEffect = Helpers::CustomEffect("SphereEffect.fx", "SphereDepthTechnique", CUSTOM_EFFECT_TYPE_PIXEL | CUSTOM_EFFECT_TYPE_VERTEX, depthLayout, 1);
	depthEffect.AddVariable("World");
	depthEffect.AddVariable("View");
	depthEffect.AddVariable("Projection");
	depthEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
}

void Projectiles::Draw()
{
	projectileEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	projectileEffect.SetMatrix("Projection", Helpers::Globals::AppCamera.Projection());
	projectileEffect.SetFloatVector("CameraPos", Helpers::Globals::AppCamera.Position());
	projectileEffect.PreDraw();

	for(int i = 0; i<projectiles.size(); i++){
		projectileEffect.SetMatrix("World", projectiles[i]->world);
		projectileSphere.Draw(&projectileEffect);
	}	
}

void Projectiles::DrawDepth(){
/*
	depthEffect.SetMatrix("View", Helpers::Globals::AppCamera.View());
	depthEffect.PreDraw();

	std::list<ProjectStructs::PROJECTILE*>::iterator it;

	for(it = projectiles.begin(); it != projectiles.end(); it++){
		depthEffect.SetMatrix("World", it->world);
		projectileSphere.DrawDepth(&depthEffect);
	}	
	*/
}

void Projectiles::Update(float dt){
	for(int i = 0; i< projectiles.size(); i++){
		projectiles[i]->life -= dt*deterioration;
		projectiles[i]->position = PhysicsWrapper::GetVector(&projectiles[i]->rigidBody->getPosition());
		projectiles[i]->world = PhysicsWrapper::GetWorld(projectiles[i]->rigidBody);
		projectiles[i]->lastVelocity = PhysicsWrapper::GetVector(&projectiles[i]->rigidBody->getLinearVelocity());

		if(!projectiles[i]->rigidBody->isActive())
			projectiles[i]->life = 0.0f;
	}

	// take out the "dead" projectiles
	while(projectiles.size() > 0 && projectiles[0]->life <= 0.0f){
		if(projectiles[0]->rigidBody->isActive())
			PhysicsWrapper::RemoveRigidBody(projectiles[0]->rigidBody);

		projectiles.erase(projectiles.begin());
	}

	lastShotTime+=dt;
}

void Projectiles::CleanUp(){
	projectileSphere.CleanUp();
	projectileEffect.CleanUp();
	depthEffect.CleanUp();

	while(!projectiles.empty()){
		delete projectiles[0];
		projectiles.erase(projectiles.begin());
	}

	projectiles.clear();
	projectiles.swap( std::vector<ProjectStructs::PROJECTILE*>() );

}

void Projectiles::Add(){
	if(lastShotTime < deltaTime)
		return;

	ProjectStructs::PROJECTILE *projectile = new ProjectStructs::PROJECTILE;
	projectile->life = 1.0f;
	projectile->position = Helpers::Globals::AppCamera.Position() + 5*Helpers::Globals::AppCamera.Forward();
	projectile->velocity = initialVelocity * Helpers::Globals::AppCamera.Forward();
	projectile->lastVelocity = projectile->velocity;

	PhysicsWrapper::AddProjectile(projectile);
	projectiles.push_back(projectile);
	lastShotTime = 0.0f;
}
