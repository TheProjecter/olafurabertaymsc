#ifndef PROJECTILES_H
#define PROJECTILES_H

#include <D3DX10.h>
#include "Sphere.h"
#include <list>

class Projectiles : public Drawable
{
public:
	Projectiles(void){};
	~Projectiles(void){};

	void Init();
	void Draw();
	void Update(float dt);
	void CleanUp();

	void Add();

private:
	Sphere projectileSphere;
	Helpers::CustomEffect projectileEffect;
	std::list<Structs::PROJECTILE> projectiles;

	float deltaTime, lastShotTime;
	float initialVelocity, deterioration;
	D3DXMATRIX invWorldView;
};

#endif