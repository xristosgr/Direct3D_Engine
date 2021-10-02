#pragma once
#include "Mesh.h"
#include "Animation.h"
#include <map>
#include<string>
#include "AppTimer.h"
#include "Camera.h"

using namespace DirectX;
class Animation
{
public:

	void Update();
	bool SetAnimIndex(unsigned int index, bool updateBoth = true, float blendDuration = 0.5f, bool temporary = false, float time = 0.f);

	int currAnim = 0;
	int modelAnimID = 1;

	float animSpeed = 1000.0f;

	bool bonesLoaded = false;
protected:
	bool LoadBones( aiMesh* mesh, std::vector<VertexBoneData>& bones, std::vector<Vertex>& vertices);
	void BoneTransform(std::vector<XMMATRIX>& Transforms);
	void ReadNodeHierarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode, const XMMATRIX ParentTransform, int StopAnimLevel);
	void ReadNodeHierarchy(const aiScene* scene1, const aiScene* scene2, float AnimationTime1, float AnimationTime2, const aiNode* pNode1, const aiNode* pNode2, const XMMATRIX& ParentTransform, int StopAnimLevel);
	aiQuaternion CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	aiVector3D CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	aiVector3D CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* p_animation, const std::string pNodeName);
	aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend);


public:

	// bones data
//aiMatrix4x4 m_GlobalInverseTransform;
	XMMATRIX m_GlobalInverseTransform;
	static const unsigned int MAX_BONES = 100;
	unsigned int m_bone_location[MAX_BONES];
	unsigned int m_NumBones = 0;
	std::vector<BoneMatrix> m_BoneInfo;
	float ticks_per_second = 0.0f;
	std::map<std::string, unsigned int> BoneMapping;
	std::map<std::string,const aiNodeAnim*> node_anims;
	//const aiScene* pScene;
	float m_lastTime;
	float _animationTime0;
	float _animationTime1;
	float _blendingTime;
	float _blendingTimeMul;
	unsigned int _prevAnimIndex;
	bool _updateBoth;
	bool _temporary;
	float _playTime;
	unsigned int _curScene;
	std::vector<const aiScene*> scenes;
	std::vector<Assimp::Importer*> importers;

	AppTimer timer;

	float mAnimationTime = 0.0f;
	float currentAnimTime = 0.0f;
	float animDuration = 0.0f;
	bool stepAnimation = false;

	XMFLOAT3 mRot;
	XMFLOAT3 mScale;
	XMFLOAT3 mPos;

	int animLevel = 0;

	XMFLOAT4 finalTrans;
	XMMATRIX FinalMatrixTrans;
private:

};

