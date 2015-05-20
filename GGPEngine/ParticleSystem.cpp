#include "ParticleSystem.h"
#include "RenderEngine.h"
#include "GameObject.h"
#include "Camera.h"
#include "Transform.h"

ParticleSystem::ParticleSystem(RenderEngine* renderer, Material* mat) : mTexArraySRV(0), mRandomTexSRV(0)
{
	e = renderer;
	material = mat;

	//SetVertexShader(e->device, e->deviceContext, L"ParticleVertexShader.cso");
	//SetPixelShader(e->device, e->deviceContext, L"ParticlePixelShader.cso");

	//Transform t = Transform();
	//transform->position = { 0, 0, 3 };
	//t.position = XMFLOAT3(0, 0, 0);
	//t.rotation = XMFLOAT3(0, 0, 0);
	//t.scale = XMFLOAT3(1, 1, 1);
	//t.movementSpeed = -.05f;
	//transform = &t;

	mFirstRun = true;
	mGameTime = 0.0f;
	mTimeStep = 0.0f;
	mAge = 0.1f;

	m_particlesPerSecond = 1;

	mMaxParticles = 50;
	
	mVertexBuffer = 0;
	mIndexBuffer = 0;

	InitializeBuffers(e->device);
}

ParticleSystem::~ParticleSystem()
{
	mVertexBuffer->Release();
	mIndexBuffer->Release();
	delete(transform);
	//delete(material);

	//delete simpleVS;
	//delete simplePS;

	// Release the buffers.
	//ShutdownBuffers();

	// Release the particle system.
	//ShutdownParticleSystem();
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

void ParticleSystem::Init(ID3D11Device* device, ID3D11ShaderResourceView* texArraySRV,
	ID3D11ShaderResourceView* randomTexSRV, UINT maxParticles)
{
	mTexArraySRV = texArraySRV;
	mRandomTexSRV = randomTexSRV;
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

		particles[i].size = XMFLOAT3(8.0f, 8.0f, 8.0f);
		particles[i].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		particles[i].age = .5f;
		particles[i].velocity = XMFLOAT3(velX, velY, velZ);
		particles[i].acceleration = XMFLOAT3(0, 0, 0);
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

	return true;

	#pragma endregion
}

void ParticleSystem::Update(float dt, float gameTime)
{
	mGameTime = gameTime;
	mTimeStep = dt;

	//transform->MoveForward();

	mAge += dt;

	KillParticles();

	EmitParticles(dt);

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	int i;
	for (i = 0; i < particles.size(); i++)
	{
		particles[i].age -= 1 * dt;
		//particles[i].position.y = particles[i].position.y - (0.0001f);

		//particles[i].velocity.x = particles[i].acceleration.x * dt;
		//particles[i].velocity.y = particles[i].acceleration.y * dt;
		//particles[i].velocity.z = particles[i].acceleration.z * dt;

		//particles[i].position.x = particles[i].acceleration.x * dt * dt + particles[i].velocity.x * dt + particles[i].position.x;
		//particles[i].position.y = particles[i].acceleration.y * dt * dt + particles[i].velocity.y * dt + particles[i].position.y;
		//particles[i].position.z = particles[i].acceleration.z * dt * dt + particles[i].velocity.z * dt + particles[i].position.z;

		particles[i].position.x = particles[i].position.x + particles[i].velocity.x * dt;
		particles[i].position.y = particles[i].position.y + particles[i].velocity.y * dt;
		particles[i].position.z = particles[i].position.z + particles[i].velocity.z * dt;
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
	float positionX, positionY, positionZ, velX, velY, velZ, accelX, accelY, accelZ, red, green, blue, alpha;

	// Increment the frame time.
	mAccumulatedTime += dt;

	// Set emit particle to false for now.
	emitParticle = false;

	// Check if it is time to emit a new particle or not.
	if (mAccumulatedTime > (1 / (m_particlesPerSecond)))
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
		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 1;//m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 1;//m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * 5;//m_particleDeviationZ;

		velX = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 0.5f;
		velY = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 0.5f;
		velZ = 0;//m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 5;

		accelX = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 1500.0f;
		accelY = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 1500.0f;
		accelZ = 0;//m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * 5;

		red = ((float)rand()) / RAND_MAX;
		green = ((float)rand()) / RAND_MAX;
		blue = ((float)rand()) / RAND_MAX;
		alpha = 0.4f;// ((float)rand()) / RAND_MAX;

		//Create a new particle
		ParticleVertex newParticle;

		//Set information about the new particle.
		newParticle.position = XMFLOAT3(positionX, positionY, positionZ);
		newParticle.color = XMFLOAT4(red, green, blue, alpha);
		newParticle.velocity = XMFLOAT3(velX, velY, velZ);
		newParticle.acceleration = XMFLOAT3(accelX, accelY, accelZ);
		newParticle.age = 10.0f;
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
}