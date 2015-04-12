#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "ParticleSystem.h"
#include "Camera.h"

using namespace DirectX;

class Effects
{
public:
	Effects();
	~Effects();

	void AddParticleSystem(ParticleSystem* newParticleSystem);

	//Update the particle systems
	//Draw the particle systems

	//Accessor/Mutators for the systems

private:
	//Some structure to hold all the particle systems

};

