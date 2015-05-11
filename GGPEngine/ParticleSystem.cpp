#include "ParticleSystem.h"
#include "RenderEngine.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"

ParticleSystem::ParticleSystem(RenderEngine* renderer) : mTexArraySRV(0), mRandomTexSRV(0)
{
	e = renderer;

	SetVertexShader(e->device, e->deviceContext, L"ParticleVS.cso");
	SetPixelShader(e->device, e->deviceContext, L"ParticlePS.cso");

	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.0f;

	m_particlesPerSecond = 25;

	mMaxParticles = 500;

	mEyePosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEmitDirW = XMFLOAT3(0.0f, 1.0f, 0.0f);
	
	mVertexBuffer = 0;
	mIndexBuffer = 0;

	InitializeBuffers(e->device);
}

ParticleSystem::~ParticleSystem()
{
	mVertexBuffer->Release();
	mIndexBuffer->Release();

	// Release the buffers.
	//ShutdownBuffers();

	// Release the particle system.
	//ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge = 0.0f;
}

float ParticleSystem::GetAge()const
{
	return mAge;
}

#pragma region Set Eye & Emit Pos/Dir
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
#pragma endregion

#pragma region Setting Vertex & Pixel Shaders
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
#pragma endregion

void ParticleSystem::Init(ID3D11Device* device, /*ParticleEffect* fx,*/ ID3D11ShaderResourceView* texArraySRV,
	ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	//InitializeParticleSystem();

	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;

	//BuildVB(device);
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	float velX, velY, velZ;
	
	for (int i = 0; i < 1; i++)
	{
		particles.push_back(ParticleVertex());
		particles[i].position = XMFLOAT3(0, 0, 0);

		velX = 0;
		velY = 0;
		velZ = 0;

		particles[i].size = XMFLOAT3(1.0f, 1.0f, 1.0f);
		particles[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		particles[i].age = .5f;
		particles[i].velocity = XMFLOAT3(velX, velY, velZ);
	}

	for (int i = 0; i < 5; i++)
	{
		/*particles.push_back(ParticleVertex());
		float spare = 0.25f * i;
		particles[i + 1].position = XMFLOAT3( spare, 0.36f, 0.33f );

		//velX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		//velY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		//velZ = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		velX = 0;
		velY = 0;
		velZ = 0;

		particles[i + 1].size = XMFLOAT3( 1.0f, 1.0f, 1.0f );
		particles[i + 1].color = XMFLOAT4( 0.0f, 1.0f, 0.0f, 1.0f );
		particles[i+1].age = 2;
		particles[i + 1].velocity = XMFLOAT3( velX, velY, velZ);*/
	}

	#pragma region Buffer Setup
	unsigned long indices[] = {
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, };
	//unsigned long* indices = new unsigned long[mMaxParticles];
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(ParticleVertex) * mMaxParticles;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = &particles[0];
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mMaxParticles;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	//delete[] indices;
	//indices = 0;

	return true;

	#pragma endregion
}

void ParticleSystem::Update(float dt, float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	mAge += dt;

	KillParticles();

	EmitParticles(dt);

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	int i;
	for (i = 0; i < particles.size(); i++)
	{
		particles[i].age -= 1 / dt;
		//particles[i].position.y = particles[i].position.y - (0.0001f);

		particles[i].position.x = particles[i].position.x + particles[i].velocity.x / dt;
		particles[i].position.y = particles[i].position.y + particles[i].velocity.y / dt;
		particles[i].position.z = particles[i].position.z + particles[i].velocity.z / dt;
	}

	UpdateBuffers(e->deviceContext);
}

void ParticleSystem::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ParticleVertex* verticesPtr;

	if (particles.size() > 0)
	{
		// Initialize vertex array to zeros at first.
		//	memset(m_vertices, 0, (sizeof(Vertex) * m_vertexCount));

		// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
		index = 0;

		// Lock the vertex buffer.
		result = deviceContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			printf("fuck\n");
		}

		// Get a pointer to the data in the vertex buffer.
		verticesPtr = (ParticleVertex*)mappedResource.pData;

		// Copy the data into the vertex buffer.
		memcpy(verticesPtr, &particles[0], (sizeof(ParticleVertex) * particles.size()));

		// Unlock the vertex buffer.
		deviceContext->Unmap(mVertexBuffer, 0);
	}
}

void ParticleSystem::EmitParticles(float dt)
{
	bool emitParticle;
	float positionX, positionY, positionZ, velX, velY, velZ, red, green, blue;

	// Increment the frame time.
	mAccumulatedTime += 1 / dt;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (mAccumulatedTime > (.10f / m_particlesPerSecond))
	{
		mAccumulatedTime = 0.0f;

		if (particles.size() < mMaxParticles)
		{
			emitParticle = true;
		}
		else
		{
			emitParticle = false;
		}
	}

	// If there are particles to emit then emit one per frame.
	if ((emitParticle == true) && (particles.size() < (mMaxParticles)))
	{
		//m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 5;//m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 5;//m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * 5;//m_particleDeviationZ;

		velX = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 5;
		velY = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 5;
		velZ = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 5;

		red = ((float)rand()) / RAND_MAX;
		green = ((float)rand()) / RAND_MAX;
		blue = ((float)rand()) / RAND_MAX;

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

		//Create a new particle
		ParticleVertex newParticle;

		//int newEnd = particles.size() - 1;

		//Set information about the new particle.
		newParticle.position = XMFLOAT3(positionX, positionY, positionZ);
		newParticle.color = XMFLOAT4(red, green, blue, 1.0f);
		newParticle.velocity = XMFLOAT3(velX, velY, velZ);
		newParticle.age = .5f;
		newParticle.size = XMFLOAT3(1.0f, 1.0f, 1.0f);

		//Push it into the vector of particles (which is handed to the GPU)
		particles.push_back(newParticle);
	}
}

void ParticleSystem::KillParticles()
{
	for (int i = 0; i < particles.size(); i++)
	{
		if (particles[i].age <= 0)
		{
			int i = 0;
			particles.erase(particles.begin(), particles.begin() + 1);
		}
	}


	//	int i, j;
	//
	//	 Kill all the particles that have gone below a certain height range.
	//	for (i = 0; i < particles.size(); i++)
	//	{
	//		if ((particles[i].active == true) && (particles[i].position.y < -3.0f))
	//		{
	//			particles[i].active = false;
	//			m_currentParticleCount--;
	//
	//			 Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
	//			for (j = i; j < particles.size(); j++)
	//			{
	//				particles[j].position.x = particles[j + 1].position.x;
	//				particles[j].position.y = particles[j + 1].position.y;
	//				particles[j].position.z = particles[j + 1].position.z;
	//				particles[j].color.x = particles[j + 1].color.x;
	//				particles[j].color.y = particles[j + 1].color.y;
	//				particles[j].color.z = particles[j + 1].color.z;
	//				particles[j].velocity = particles[j + 1].velocity;
	//				particles[j].active = particles[j + 1].active;
	//			}
	//		}
	//	}
	//}
	//
	//void ParticleSystem::InitializeParticleSystem()
	//{
	//	 Set the random deviation of where the particles can be located when emitted.
	//	m_particleDeviationX = 0.5f;
	//	m_particleDeviationY = 0.1f;
	//	m_particleDeviationZ = 2.0f;
	//
	//	 Set the speed and speed variation of particles.
	//	m_particleVelocity = 1.0f;
	//	m_particleVelocityVariation = 0.2f;
	//
	//	 Set the physical size of the particles.
	//	m_particleSize = 0.2f;
	//
	//	 Set the number of particles to emit per second.
	//	m_particlesPerSecond = 250.0f;
	//
	//	 Set the maximum number of particles allowed in the particle system.
	//	mMaxParticles = 5000;
	//	 Create the particle list.
	//
	//	ParticleVertex first;
	//	particles.push_back(first);
	//
	//	particles[0].position = { 0, 0, 0 };
	//	particles[0].size = { 1.0f, 1.0f, 1.0f };
	//	particles[0].color = { 1.0f, 0, 0, 1.0f };
	//	particles[0].age = 0;
	//	particles[0].velocity = { 0, 3.0f, 0 };
	//
	//	m_particleList = new ParticleType[m_maxParticles];
	//	if (!m_particleList)
	//	{
	//		return false;
	//	}
	//
	//	 Initialize the particle list.
	//	/*for (i = 0; i<m_maxParticles; i++)
	//	{
	//		m_particleList[i].active = false;
	//	}*/
	//
	//	 Clear the initial accumulated time for the particle per second emission rate.
	//	mAccumulatedTime = 0.0f;
}