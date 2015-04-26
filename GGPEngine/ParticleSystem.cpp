#include "ParticleSystem.h"
#include "RenderEngine.h"
#include "GameObject.h"
#include "Camera.h"

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
	InitializeParticleSystem();

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

	KillParticles();

	EmitParticles(gameTime);

	UpdateParticles(dt);

	UpdateBuffers(e->deviceContext);

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	int i;
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

void ParticleSystem::EmitParticles(float dt)
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
		- 1)))
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

void ParticleSystem::UpdateParticles(float dt)
{

}

void ParticleSystem::KillParticles()
{
	int i, j;

	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i < particles.size(); i++)
	{
		if ((particles[i].active == true) && (particles[i].position.y < -3.0f))
		{
			particles[i].active = false;
			//m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j < particles.size(); j++)
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

void ParticleSystem::InitializeParticleSystem()
{
	// Set the random deviation of where the particles can be located when emitted.
	m_particleDeviationX = 0.5f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// Set the speed and speed variation of particles.
	m_particleVelocity = 1.0f;
	m_particleVelocityVariation = 0.2f;

	// Set the physical size of the particles.
	m_particleSize = 0.2f;

	// Set the number of particles to emit per second.
	m_particlesPerSecond = 250.0f;

	// Set the maximum number of particles allowed in the particle system.
	mMaxParticles = 5000;
	// Create the particle list.

	ParticleVertex first;
	particles.push_back(first);

	particles[0].position = { 0, 0, 0 };
	particles[0].size = { 1.0f, 1.0f, 1.0f };
	particles[0].color = { 1.0f, 0, 0, 1.0f };
	particles[0].age = 0;
	particles[0].velocity = { 0, 3.0f, 0 };

	//m_particleList = new ParticleType[m_maxParticles];
	//if (!m_particleList)
	//{
	//	return false;
	//}

	// Initialize the particle list.
	/*for (i = 0; i<m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}*/

	// Clear the initial accumulated time for the particle per second emission rate.
	mAccumulatedTime = 0.0f;
}

void ParticleSystem::ShutdownParticleSystem()
{
	// Release the particle list.
	//if (particles != null)
	//{
	//	delete[] particles;
	//	particles = 0;
	//}

	return;
}

bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	/*unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	m_vertexCount = mMaxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new Vertex[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(Vertex) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;
	*/
	return true;
}

void ParticleSystem::UpdateBuffers(ID3D11DeviceContext* context)
{
	/*int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Vertex* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(Vertex) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i < particles.size(); i++)
	{
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY - m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX - m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].positionX + m_particleSize, m_particleList[i].positionY + m_particleSize, m_particleList[i].positionZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;*/
}

void ParticleSystem::RenderBuffers(ID3D11DeviceContext* context)
{

}