#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Material.h"
#include "RenderEngine.h"
#include "Vertex.h"
#include <vector>

using namespace DirectX;

class Transform;
class Camera;
class RenderEngine; //Forward declaration
class GameObject; //Forward declaration
//typedef GPPEngineAPI void(*JSFunctionCallback)();


//Array of particles on CPU side
//Pass them in
//Define data if you're drawing them. ParticleVertex
//Dynamic buffer to copy

//Draw all the squares

//----------
//GPU particles geometry shaders Pass the geometry forward

//Reference:
//http://www.rastertek.com/dx11tut39.html


class ParticleSystem
{
public:
	ParticleSystem(RenderEngine* renderer, Material* mat);
	~ParticleSystem();

	Transform*					transform;
	Material*					material;
	ID3D11Buffer*				mVertexBuffer;
	ID3D11Buffer*				mIndexBuffer;
	std::vector<ParticleVertex>	particles;

	void Reset();

	float GetAge()const;

	void Init(ID3D11Device* device,
		ID3D11ShaderResourceView* texArraySRV,
		ID3D11ShaderResourceView* randomTexSRV,
		UINT maxParticles);

	bool InitializeBuffers(ID3D11Device* device);
	void Update(float dt, float gameTime);
	void UpdateBuffers(ID3D11DeviceContext* context);
	
	void EmitParticles(float dt);
	void KillParticles();

private:

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

	bool LoadTexture(ID3D11Device* device, WCHAR* filename);

	RenderEngine* e;

	UINT mMaxParticles;
	bool mFirstRun;

	float mAccumulatedTime;
	float mGameTime;
	float mTimeStep;
	float mAge;

	float m_particleDeviationX, m_particleDeviationY, m_particleDeviationZ;
	float m_particleVelocity, m_particleVelocityVariation;
	float m_particleSize, m_particlesPerSecond;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};

