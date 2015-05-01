#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
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
	ParticleSystem(RenderEngine* renderer);
	//ParticleSystem();
	~ParticleSystem();

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	SimpleVertexShader* simpleVS;
	SimplePixelShader* simplePS;
	std::vector<Transform> particles;

	float GetAge()const;

	void SetEyePos(const XMFLOAT3& eyePosW);
	void SetEmitPos(const XMFLOAT3& emitPosW);
	void SetEmitDir(const XMFLOAT3& emitDirW);

	void SetVertexShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile);
	void SetVertexShader(SimpleVertexShader* simpleVertexShader);
	void SetPixelShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR pixelShaderFile);
	void SetPixelShader(SimplePixelShader* simplePixelShader);

	void Init(ID3D11Device* device, 
		ID3D11ShaderResourceView* texArraySRV, 
		ID3D11ShaderResourceView* randomTexSRV, 
		UINT maxParticles);

	void Reset();
	void Update(float dt, float gameTime);
	void Draw(ID3D11DeviceContext* dc, const Camera* cam);

	void EmitParticles(float dt);
	void UpdateParticles(float dt);
	void KillParticles();

//	void InitializeParticleSystem();
	void ShutdownParticleSystem();

	bool InitializeBuffers(ID3D11Device* device);
	void ShutdownBuffers();

	void UpdateBuffers(ID3D11DeviceContext* context);
	void RenderBuffers(ID3D11DeviceContext* context);

private:

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

	bool LoadTexture(ID3D11Device* device, WCHAR* filename);
	void ReleaseTexture();

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

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;
	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;
};

