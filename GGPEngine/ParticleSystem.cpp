#include "ParticleSystem.h"
#include "RenderEngine.h"
#include "GameObject.h"

ParticleSystem::ParticleSystem(RenderEngine* renderer) : mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	e = renderer;

	SetVertexShader(e->device, e->deviceContext, L"ParticleVS.cso");
	SetPixelShader(e->device, e->deviceContext, L"ParticlePS.cso");

	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mMaxParticles = 3;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}

/*ParticleSystem::ParticleSystem() : mInitVB(0), mDrawVB(0), mStreamOutVB(0), mTexArraySRV(0), mRandomTexSRV(0)
{
	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
}*/

ParticleSystem::~ParticleSystem()
{
	if (mInitVB){ mInitVB->Release(); mInitVB = 0; }
	if (mDrawVB){ mDrawVB->Release(); mDrawVB = 0; }
	if (mStreamOutVB){ mStreamOutVB->Release(); mStreamOutVB = 0; }
	//ReleaseCOM(mInitVB);
	//ReleaseCOM(mDrawVB);
	//ReleaseCOM(mStreamOutVB);

	// Release the buffers.
	//ShutdownBuffers();

	// Release the particle system.
	//ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();
}

float ParticleSystem::GetAge()const
{
	return mAge;
}

void ParticleSystem::SetEyePos(const XMFLOAT3& eyePosW)
{
	mEyePosW = eyePosW;
}

void ParticleSystem::SetEmitPos(const XMFLOAT3& emitPosW)
{
	mEmitPosW = emitPosW;
}

void ParticleSystem::SetEmitDir(const XMFLOAT3& emitDirW)
{
	mEmitDirW = emitDirW;
}

void ParticleSystem::SetVertexShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR vertexShaderFile)
{
	simpleVS = new SimpleVertexShader(device, deviceContext);
	simpleVS->LoadShaderFile(vertexShaderFile);
}

void ParticleSystem::SetVertexShader(SimpleVertexShader* simpleVertexShader)
{
	simpleVS = simpleVertexShader;
}

void ParticleSystem::SetPixelShader(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LPCWSTR pixelShaderFile)
{
	simplePS = new SimplePixelShader(device, deviceContext);
	simplePS->LoadShaderFile(pixelShaderFile);
}

void ParticleSystem::SetPixelShader(SimplePixelShader* simplePixelShader)
{
	simplePS = simplePixelShader;
}

void ParticleSystem::Init(ID3D11Device* device, /*ParticleEffect* fx,*/ ID3D11ShaderResourceView* texArraySRV,
	ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	ParticleVertex first;
	particles.push_back(first);

	particles[0].position = { 0, 0, 0 };
	particles[0].size = { 1.0f, 1.0f, 1.0f };
	particles[0].color = { 1.0f, 0, 0, 1.0f };
	particles[0].age = 0;
	particles[0].velocity = { 0, 3.0f, 0 };

	mMaxParticles = maxParticles;

	//mFX = fx;

	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;

	BuildVB(device);
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

void ParticleSystem::Update(float dt, float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	mAge += dt;

	int i;

	EmitParticles(gameTime);

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i < particles.size(); i++)
	{
		particles[i].age += dt;
		particles[i].position.y = particles[i].position.y - (particles[i].velocity.y * mGameTime * 0.001f);
	}
}

void ParticleSystem::Draw(ID3D11DeviceContext* dc, const Camera* cam)
{
	XMMATRIX VP = XMLoadFloat4x4(&cam->view);


	/*XMFLOAT4X4 view = cam.view();
	XMMATRIX VP = XMLoadFloat4x4(&view);

	//
	// Set constants.
	//
	mFX->SetViewProj(VP);
	mFX->SetGameTime(mGameTime);
	mFX->SetTimeStep(mTimeStep);
	mFX->SetEyePosW(mEyePosW);
	mFX->SetEmitPosW(mEmitPosW);
	mFX->SetEmitDirW(mEmitDirW);
	mFX->SetTexArray(mTexArraySRV);
	mFX->SetRandomTex(mRandomTexSRV);

	//
	// Set IA stage.
	//
	dc->IASetInputLayout(InputLayouts::Particle);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(Vertex::Particle);
	UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if (mFirstRun)
	dc->IASetVertexBuffers(0, 1, &mInitVB, &stride, &offset);
	else
	dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	//
	// Draw the current particle list using stream-out only to update them.
	// The updated vertices are streamed-out to the target VB.
	//
	dc->SOSetTargets(1, &mStreamOutVB, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	mFX->StreamOutTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
	mFX->StreamOutTech->GetPassByIndex(p)->Apply(0, dc);

	if (mFirstRun)
	{
	dc->Draw(1, 0);
	mFirstRun = false;
	}
	else
	{
	dc->DrawAuto();
	}
	}

	// done streaming-out--unbind the vertex buffer
	ID3D11Buffer* bufferArray[1] = { 0 };
	dc->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVB, mStreamOutVB);

	//
	// Draw the updated particle system we just streamed-out.
	//
	dc->IASetVertexBuffers(0, 1, &mDrawVB, &stride, &offset);

	mFX->DrawTech->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
	mFX->DrawTech->GetPassByIndex(p)->Apply(0, dc);

	dc->DrawAuto();
	}*/
}

void ParticleSystem::EmitParticles(float gameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	// Increment the frame time.
	//m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	//if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	//{
		//m_accumulatedTime = 0.0f;
		emitParticle = true;
	//}

	// If there are particles to emit then emit one per frame.
		if ((emitParticle == true) && (particles.size() < (mMaxParticles
		-1)))
	{
		//m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		// Now since the particles need to be rendered from back to front for blending we have to sort the particle array.
		// We will sort using Z depth so we need to find where in the list the particle should be inserted.
		/*index = 0;
		found = false;
		while (!found)
		{
			if ((particles[index].active == false) || (particles[index].position.z < position.z))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}*/

		// Now that we know the location to insert into we need to copy the array over by one position from the index to make room for the new particle.
		//i = m_currentParticleCount;
		//j = i - 1;

		ParticleVertex first;
		particles.push_back(first);

		int newEnd = particles.size() - 1;

		particles[newEnd].position = { positionX, positionY, positionZ };
		particles[newEnd].size = { 1.0f, 1.0f, 1.0f };
		particles[newEnd].color = { red, green, blue, 1.0f };
		particles[newEnd].age = 0;
		particles[newEnd].velocity = { 0, velocity, 0 };
		particles[newEnd].active = true;
	}
}

void ParticleSystem::KillParticles()
{
	int i, j;

	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < mMaxParticles; i++)
	{
		if ((particles[i].active == true) && (particles[i].position.y < -3.0f))
		{
			particles[i].active = false;
			//m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j<mMaxParticles - 1; j++)
			{
				particles[j].position.x = particles[j + 1].position.x;
				particles[j].position.y = particles[j + 1].position.y;
				particles[j].position.z = particles[j + 1].position.z;
				particles[j].color.x = particles[j + 1].color.x;
				particles[j].color.y = particles[j + 1].color.y;
				particles[j].color.z = particles[j + 1].color.z;
				particles[j].velocity = particles[j + 1].velocity;
				particles[j].active = particles[j + 1].active;
			}
		}
	}
}

void ParticleSystem::BuildVB(ID3D11Device* device)
{
	/*
	//
	// Create the buffer to kick-off the particle system.
	//

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex::Particle) * 1;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	Vertex::Particle p;
	ZeroMemory(&p, sizeof(Vertex::Particle));
	p.Age = 0.0f;
	p.Type = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &p;

	HR(device->CreateBuffer(&vbd, &vinitData, &mInitVB));

	//
	// Create the ping-pong buffers for stream-out and drawing.
	//
	vbd.ByteWidth = sizeof(Vertex::Particle) * mMaxParticles;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;

	HR(device->CreateBuffer(&vbd, 0, &mDrawVB));
	HR(device->CreateBuffer(&vbd, 0, &mStreamOutVB));
	*/
}

bool ParticleSystem::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	/*bool result;

	// Create the texture object.
	mTexArraySRV = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}*/

	return true;
}

void ParticleSystem::ReleaseTexture()
{
	/*// Release the texture object.
	if (mTexArraySRV)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}*/

	return;
}