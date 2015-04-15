#include "MeshLoader.h"


MeshLoader::MeshLoader()
{
}


MeshLoader::~MeshLoader()
{
}

//	Loads .OBJ files. 
//	Parameters:
//		filepath: Location of the File
//		verts	: vector of type (Vertex) Objects
//		indices	: vector of type (unsigned int) 
bool MeshLoader::loadOBJfile(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices)
{
	//clear the vertex and indices array
	verts.clear();
	indices.clear();

	// File input object
	std::ifstream obj(filePath); // <-- Replace filename with your parameter

	// Check for successful open
	if (!obj.is_open())
	{
		return false;
	}

	// Variables used while reading the file
	std::vector<XMFLOAT3> positions;     // Positions from the file
	std::vector<XMFLOAT3> normals;       // Normals from the file
	std::vector<XMFLOAT2> uvs;           // UVs from the file
	unsigned int triangleCounter = 0;    // Count of triangles/indices
	char chars[100];                     // String for line reading

	// Still good?
	while (obj.good())
	{
		// Get the line (100 characters should be more than enough)
		obj.getline(chars, 100);

		// Check the type of line
		if (chars[0] == 'v' && chars[1] == 'n')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 norm;
			sscanf_s(
				chars,
				"vn %f %f %f",
				&norm.x, &norm.y, &norm.z);

			// Add to the list of normals
			normals.push_back(norm);
		}
		else if (chars[0] == 'v' && chars[1] == 't')
		{
			// Read the 2 numbers directly into an XMFLOAT2
			XMFLOAT2 uv;
			sscanf_s(
				chars,
				"vt %f %f",
				&uv.x, &uv.y);

			// Add to the list of uv's
			uvs.push_back(uv);
		}
		else if (chars[0] == 'v')
		{
			// Read the 3 numbers directly into an XMFLOAT3
			XMFLOAT3 pos;
			sscanf_s(
				chars,
				"v %f %f %f",
				&pos.x, &pos.y, &pos.z);

			// Add to the positions
			positions.push_back(pos);
		}
		else if (chars[0] == 'f')
		{
			// Read the 9 face indices into an array
			unsigned int i[9];
			sscanf_s(
				chars,
				"f %d/%d/%d %d/%d/%d %d/%d/%d",
				&i[0], &i[1], &i[2],
				&i[3], &i[4], &i[5],
				&i[6], &i[7], &i[8]);

			// - Create the verts by looking up
			//    corresponding data from vectors
			// - OBJ File indices are 1-based, so
			//    they need to be adusted
			Vertex v1;
			v1.Position = positions[i[0] - 1];
			v1.UV = uvs[i[1] - 1];
			v1.Normal = normals[i[2] - 1];

			Vertex v2;
			v2.Position = positions[i[3] - 1];
			v2.UV = uvs[i[4] - 1];
			v2.Normal = normals[i[5] - 1];

			Vertex v3;
			v3.Position = positions[i[6] - 1];
			v3.UV = uvs[i[7] - 1];
			v3.Normal = normals[i[8] - 1];

			// Add the verts to the vector
			verts.push_back(v1);
			verts.push_back(v2);
			verts.push_back(v3);

			// Add three more indices
			indices.push_back(triangleCounter++);
			indices.push_back(triangleCounter++);
			indices.push_back(triangleCounter++);
		}
	}

	// Close
	obj.close();

	return true;
}
bool MeshLoader::loadFBXfile(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices)
{
	static uint16_t index = 0;
	FbxManager *fbxManager = FbxManager::Create();

	FbxIOSettings *ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);

	FbxImporter *importer = FbxImporter::Create(fbxManager, "");
	importer->Initialize(filePath, -1, fbxManager->GetIOSettings());

	FbxScene *scene = FbxScene::Create(fbxManager, "tempName");

	importer->Import(scene);
	importer->Destroy();

	FbxNode *rootNode = scene->GetRootNode();									// Get the Root node
	
	if (rootNode)
	{	
		for (int i = 0; i < rootNode->GetChildCount(); i++)						// Traverse all the child nodes and read the Data
		{
			FbxNode* pFbxChildNode = rootNode->GetChild(i);						

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

			FbxVector4* pVertices = pMesh->GetControlPoints();
			
			FbxGeometryElementNormal* pNormals = pMesh->GetElementNormal();


			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
			{
				int iNumVertices = pMesh->GetPolygonSize(j);
				if (iNumVertices != 3)										// If the model is not triangulated, Bail. Dont load anything.
					return false;

				for (int k = 0; k < iNumVertices; k++)
				{
					//Reading the Vertex Position Data
					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
					FbxVector4 pos = pVertices[iControlPointIndex];
					Vertex vertex;
					vertex.Position.x = (float)pos.mData[0];
					vertex.Position.y = (float)pos.mData[1];
					vertex.Position.z = (float)pos.mData[2];

					//Reading the Vertex Normal Data
					FbxVector4 normal;
					pMesh->GetPolygonVertexNormal(j, k, normal);

					vertex.Normal.x = normal.mData[0];
					vertex.Normal.y = normal.mData[1];
					vertex.Normal.z = normal.mData[2];

					//Reading the Vertex UV data
					FbxStringList UVSetNameList;
					pMesh->GetUVSetNames(UVSetNameList);

					//Get lUVSetIndex-th uv set
					const char* UVSetName = UVSetNameList.GetStringAt(0);
					const FbxGeometryElementUV* UVElement = pMesh->GetElementUV(UVSetName);

					if (!UVElement)
					{
						continue;				// UV element not found in the object model
					}

					// only support mapping mode eByPolygonVertex and eByControlPoint
					if (UVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
						UVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
					{
						return false;			// Bail because the mapping mode in the object model is not by polygonVertex and ControlPoint
					}

					//index array, where holds the index referenced to the uv data
					const bool UseIndex = UVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
					const int IndexCount = (UseIndex) ? UVElement->GetIndexArray().GetCount() : 0;

					FbxVector2 UV;

					//the UV index depends on the reference mode
					//int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					int UVIndex = pMesh->GetTextureUVIndex(i, j);
					UV = UVElement->GetDirectArray().GetAt(UVIndex);

					vertex.UV.x = UV.mData[0];
					vertex.UV.y = UV.mData[1];

					verts.push_back(vertex);
					indices.push_back(index++);
				}
			}
		}

	}

	return true;
}

bool MeshLoader::loadModel(const char* filePath, std::vector<Vertex>& verts, std::vector<UINT>& indices)
{
	std::string FileLoc = filePath;
	char ch = FileLoc.back();

	//Check the last character of the file location to determine the type of File
	if (ch == 'j')	
		return loadOBJfile(filePath, verts, indices);					// OBJ File

	if (ch == 'x')
		return loadFBXfile(filePath, verts, indices);					// FBX File

	return false;														// If the model is is some other Type, Bail. Don't load anything. 
}