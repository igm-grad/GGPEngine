#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "Camera.h"
#include "RenderEngine.h"
#include "Vertex.h"
#include <vector>

using namespace DirectX;

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

class ParticleSystem
{
public:
	ParticleSystem(RenderEngine* renderer);
	//ParticleSystem();
	~ParticleSystem();

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

private:
	void BuildVB(ID3D11Device* device);

	ParticleSystem(const ParticleSystem& rhs);
	ParticleSystem& operator=(const ParticleSystem& rhs);

private:
	RenderEngine* e;

	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mTimeStep;
	float mAge;

	XMFLOAT3 mEyePosW;
	XMFLOAT3 mEmitPosW;
	XMFLOAT3 mEmitDirW;

	//ParticleEffect* mFX;

	ID3D11Buffer* mInitVB;
	ID3D11Buffer* mDrawVB;
	ID3D11Buffer* mStreamOutVB;

	SimpleVertexShader* simpleVS;
	SimplePixelShader* simplePS;

	ID3D11ShaderResourceView* mTexArraySRV;
	ID3D11ShaderResourceView* mRandomTexSRV;

	std::vector<ParticleVertex> particles;
};

