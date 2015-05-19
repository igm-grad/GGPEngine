#pragma once
#pragma warning( disable: 4251 )
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

struct Joint
{
	std::wstring name;
	int parentID;

	XMFLOAT3 pos;
	XMFLOAT4 orientation;
};

struct Weight
{
	int jointID;
	float bias;
	XMFLOAT3 pos;
	XMFLOAT3 normal;
};

struct BoundingBox
{
	XMFLOAT3 min;
	XMFLOAT3 max;
};

struct FrameData
{
	int frameID;
	std::vector<float> frameData;
};
struct AnimJointInfo
{
	std::wstring name;
	int parentID;

	int flags;
	int startIndex;
};

struct ModelAnimation
{
	int numFrames;
	int numJoints;
	int frameRate;
	int numAnimatedComponents;

	float frameTime;
	float totalAnimTime;
	float currAnimTime;

	std::vector<AnimJointInfo> jointInfo;
	std::vector<BoundingBox> frameBounds;
	std::vector<Joint>	baseFrameJoints;
	std::vector<FrameData>	frameData;
	std::vector<std::vector<Joint>> frameSkeleton;
};

class Mesh;

class Model
{
public:
	int numJoints, numSubsets;

	std::vector<Mesh*> meshes;
	std::vector<Joint> joints;

	std::vector<ModelAnimation> animations;

	Model() {};
	Model(const char* filename, ID3D11Device* device);
	~Model();
};

