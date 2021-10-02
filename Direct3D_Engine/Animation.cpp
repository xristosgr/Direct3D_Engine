﻿#include "Animation.h"



void Animation::Update()
{
	if (m_lastTime == -1)
	{
		m_lastTime = timer.GetMilliseconds();
	}
	float newTime = timer.GetMilliseconds();
	if (animSpeed <= 0.0f)
	{
		animSpeed = 1.0f;
	}
	float dt = (newTime - m_lastTime) / animSpeed;
	m_lastTime = newTime;


	_animationTime0 += dt;
	if (_blendingTime > 0.f)
	{
		_blendingTime -= dt * _blendingTimeMul;
		if (_blendingTime <= 0.f)
		{
			_animationTime0 = _animationTime1;
		}
		if (_updateBoth)
		{
			_animationTime1 += dt;
		}
	}
	else
	{
		_animationTime1 += dt;
	}

	if (_temporary)
	{
		_playTime -= dt;
		if (_playTime <= 0.f)
		{
			_temporary = false;
			SetAnimIndex(_prevAnimIndex);
		}
	}
}

bool Animation::SetAnimIndex(unsigned int index, bool updateBoth, float blendDuration, bool temporary, float time)
{
	if (index == _curScene || index >= scenes.size())
	{
		return false;
	}
	_prevAnimIndex = _curScene;
	_curScene = index;
	_blendingTime = 1.f;
	_blendingTimeMul = 1.f / blendDuration;
	_animationTime1 = 0.f;
	_updateBoth = updateBoth;
	_temporary = temporary;
	_playTime = time;
	return true;
}



bool Animation::LoadBones(aiMesh* mesh, std::vector<VertexBoneData>& bones, std::vector<Vertex>& vertices)
{
	//OutputDebugStringA("BONE START...\n");
	//Generate bones
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{

		unsigned int BoneIndex = 0;
		std::string BoneName(mesh->mBones[i]->mName.data);

		if (BoneMapping.find(BoneName) == BoneMapping.end())
		{
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneMatrix bi;
			m_BoneInfo.push_back(bi);

			aiMatrix4x4 offset = mesh->mBones[i]->mOffsetMatrix;

			m_BoneInfo[BoneIndex].BoneOffset = XMMatrixTranspose(XMMATRIX(&offset.a1));

			BoneMapping[BoneName] = BoneIndex;

		}
		else
		{
			BoneIndex = BoneMapping[BoneName];
		}
		for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;



			bones[VertexID].addBoneData(BoneIndex, weight);

		}
	}
	return true;
}

void Animation::BoneTransform(std::vector<XMMATRIX>& Transforms)
{
	XMMATRIX identity_matrix;
	identity_matrix = XMMatrixIdentity();

	if (_blendingTime > 0.0f)
	{
		float TicksPerSecond = scenes[_prevAnimIndex]->mAnimations[0]->mTicksPerSecond != 0 ?
			scenes[_prevAnimIndex]->mAnimations[0]->mTicksPerSecond : 25.0f;
		float TimeInTicks = _animationTime0 * TicksPerSecond;
		float AnimationTime0 = fmod(TimeInTicks, scenes[_prevAnimIndex]->mAnimations[0]->mDuration);

		TicksPerSecond = scenes[_curScene]->mAnimations[0]->mTicksPerSecond != 0 ?
			scenes[_curScene]->mAnimations[0]->mTicksPerSecond : 25.0f;
		TimeInTicks = _animationTime1 * TicksPerSecond;
		float AnimationTime1 = fmod(TimeInTicks, scenes[_curScene]->mAnimations[0]->mDuration);

		ReadNodeHierarchy(scenes[_prevAnimIndex], scenes[_curScene], AnimationTime0, AnimationTime1, scenes[_prevAnimIndex]->mRootNode, scenes[_curScene]->mRootNode, identity_matrix, animLevel);
	}
	else
	{
		
		float TicksPerSecond = scenes[_curScene]->mAnimations[0]->mTicksPerSecond != 0 ? scenes[_curScene]->mAnimations[0]->mTicksPerSecond : 25.0f;
		float TimeInTicks = _animationTime0 * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, scenes[_curScene]->mAnimations[0]->mDuration);
		mAnimationTime = AnimationTime;

		animDuration = scenes[_curScene]->mAnimations[0]->mDuration;
		if(!stepAnimation)
			ReadNodeHierarchy(scenes[_curScene], AnimationTime, scenes[_curScene]->mRootNode, identity_matrix, animLevel);
		else
			ReadNodeHierarchy(scenes[_curScene], currentAnimTime, scenes[_curScene]->mRootNode, identity_matrix, animLevel);
	}

	Transforms.resize(m_NumBones);

	for (unsigned int i = 0; i < m_NumBones; i++) {
		Transforms[i] = m_BoneInfo[i].FinalTransformation;
		Transforms[i] = XMMatrixTranspose(Transforms[i]);
	}
}

void Animation::ReadNodeHierarchy(const aiScene* scene, float AnimationTime, const aiNode* pNode, const XMMATRIX ParentTransform, int StopAnimLevel)
{

	std::string NodeName(pNode->mName.data);
	const aiAnimation* pAnimation = scene->mAnimations[0];
	XMMATRIX NodeTransformation = XMMATRIX(&pNode->mTransformation.a1);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	
	//OutputDebugStringA(("DURATION = " + std::to_string(pAnimation->mDuration) + "\n").c_str());

	if (pNodeAnim)
	{
		aiVector3D Scaling;
		Scaling = CalcInterpolatedScaling(AnimationTime, pNodeAnim);
		

		aiMatrix4x4 scale_mat;
		aiMatrix4x4::Scaling(Scaling, scale_mat);

		XMFLOAT4X4 tempScale = XMFLOAT4X4(&scale_mat.a1);
		XMMATRIX ScalingM = XMMATRIX(&scale_mat.a1);

		aiQuaternion RotationQ;
		RotationQ = CalcInterpolatedRotation(AnimationTime, pNodeAnim);
		//if (NodeName == "Head")
		//{
		//	RotationQ.x += headRot.x;
		//	RotationQ.y += headRot.y;
		//	RotationQ.z += headRot.z;
		//}
		//OutputDebugStringA("NAME = ");
		//OutputDebugStringA(NodeName.c_str());
		//OutputDebugStringA("\n");
		//if (NodeName == "mixamorig_LeftHand")
		//{
		//	RotationQ.x += mRot.x;
		//	RotationQ.y += mRot.y;
		//	RotationQ.z += mRot.z;
		//}

		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));
		RotationM = XMMatrixTranspose(RotationM);

		aiVector3D Translation;
		{
			float time(StopAnimLevel <= 0 ? AnimationTime : 0.0f);
			Translation = CalcInterpolatedPosition(time, pNodeAnim);
		}
		//if (NodeName == "mixamorig_Hips")
		//{
		//	Translation.x += mPos.x;
		//	Translation.y += mPos.y;
		//	Translation.z += mPos.z;
		//
		//	//OutputDebugStringA(("X="+std::to_string(Translation.x)).c_str());
		//	//OutputDebugStringA((" Y=" + std::to_string(Translation.y)).c_str());
		//	//OutputDebugStringA((" Z=" + std::to_string(Translation.z)).c_str());
		//	//OutputDebugStringA("\n");
		//
		//	
		//}
		
		

		aiMatrix4x4 translate_mat;
		aiMatrix4x4::Translation(Translation, translate_mat);
		XMMATRIX TranslationM = XMMATRIX(&translate_mat.a1);


		NodeTransformation = (ScalingM * TranslationM) * RotationM;
		

		NodeTransformation = XMMatrixTranspose(NodeTransformation);
	
		
	}
	StopAnimLevel--;
	XMMATRIX GlobalTransformation = NodeTransformation * ParentTransform;


	if (BoneMapping.find(NodeName) != BoneMapping.end())
	{
		unsigned int BoneIndex = BoneMapping[NodeName];
	
			
		m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;

		//if (NodeName == "mixamorig_LeftHand")
		//{
		//	FinalMatrixTrans = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;
		//	
		//	XMStoreFloat4(&finalTrans, FinalMatrixTrans.r[0]);
		//
		//	OutputDebugStringA(("X=" + std::to_string(finalTrans.x)).c_str());
		//	OutputDebugStringA((" Y=" + std::to_string(finalTrans.y)).c_str());
		//	OutputDebugStringA((" Z=" + std::to_string(finalTrans.z)).c_str());
		//	OutputDebugStringA("\n");
		//}
	
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++)
	{
		ReadNodeHierarchy(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, StopAnimLevel);
	}
	
}

void Animation::ReadNodeHierarchy(const aiScene* scene1, const aiScene* scene2, float AnimationTime1, float AnimationTime2, const aiNode* pNode1, const aiNode* pNode2, const XMMATRIX& ParentTransform, int StopAnimLevel)
{

	std::string NodeName1(pNode1->mName.data);
	std::string NodeName2(pNode2->mName.data);

	const aiAnimation* pAnimation1 = scene1->mAnimations[0];

	XMMATRIX NodeTransformation1 = XMMATRIX(&pNode1->mTransformation.a1);
	const aiNodeAnim* pNodeAnim1 = FindNodeAnim(pAnimation1, NodeName1);

	const aiAnimation* pAnimation2 = scene2->mAnimations[0];
	XMMATRIX NodeTransformation2 = XMMATRIX(&pNode2->mTransformation.a1);
	const aiNodeAnim* pNodeAnim2 = FindNodeAnim(pAnimation2, NodeName2);

	if (pNodeAnim1 && pNodeAnim2)
	{

		aiVector3D Scaling1;
		Scaling1 = CalcInterpolatedScaling(AnimationTime1, pNodeAnim1);
		aiVector3D Scaling2;
		Scaling2 = CalcInterpolatedScaling(AnimationTime2, pNodeAnim2);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling1 * _blendingTime + Scaling2 * (1.0f - _blendingTime), ScalingM);
		XMMATRIX ScalingMat = XMMATRIX(&ScalingM.a1);



		aiQuaternion RotationQ1;
		RotationQ1 = CalcInterpolatedRotation(AnimationTime1, pNodeAnim1);
		if (NodeName1 == "mixamorig_LeftUpLeg" || NodeName1 == "mixamorig_RightUpLeg")
		{
			RotationQ1.x += mRot.x;
			RotationQ1.y += mRot.y;
			RotationQ1.z += mRot.z;
		}
		aiQuaternion RotationQ2;
		RotationQ2 = CalcInterpolatedRotation(AnimationTime2, pNodeAnim2);
		if (NodeName2 == "mixamorig_LeftUpLeg" || NodeName2 == "mixamorig_RightUpLeg")
		{
			RotationQ2.x += mRot.x;
			RotationQ2.y += mRot.y;
			RotationQ2.z += mRot.z;
		}

		aiQuaternion RotationQ;
		aiQuaternion::Interpolate(RotationQ, RotationQ2, RotationQ1, _blendingTime);
		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));
		RotationM = XMMatrixTranspose(RotationM);

		aiVector3D Translation1;
		{
			float time(StopAnimLevel <= 0 ? AnimationTime1 : 0.0f);
			Translation1 = CalcInterpolatedPosition(time, pNodeAnim1);
		}
		if (NodeName1 == "mixamorig_LeftUpLeg" || NodeName1 == "mixamorig_RightUpLeg")
		{
			Translation1.x += mPos.x;
			Translation1.y += mPos.y;
			Translation1.z += mPos.z;
		}
 
		aiVector3D Translation2;
		{
			float time(StopAnimLevel <= 0 ? AnimationTime2 : 0.0f);
			Translation2 = CalcInterpolatedPosition(time, pNodeAnim2);
		}
		if (NodeName2 == "mixamorig_LeftUpLeg" || NodeName2 == "mixamorig_RightUpLeg" )
		{
			Translation2.x += mPos.x;
			Translation2.y += mPos.y;
			Translation2.z += mPos.z;
		}


		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation1 * _blendingTime + Translation2 * (1.0f - _blendingTime), TranslationM);

		XMMATRIX TranslationMat = XMMATRIX(&TranslationM.a1);


		NodeTransformation1 = (ScalingMat * TranslationMat) * RotationM;

		NodeTransformation1 = XMMatrixTranspose(NodeTransformation1);
	}
	StopAnimLevel--;
	XMMATRIX GlobalTransformation = NodeTransformation1 * ParentTransform;

	if (BoneMapping.find(NodeName1) != BoneMapping.end())
	{
		unsigned int BoneIndex = BoneMapping[NodeName1];
		m_BoneInfo[BoneIndex].FinalTransformation = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;
	
		if (NodeName1 == "mixamorig_LeftHand")
		{
			FinalMatrixTrans = m_BoneInfo[BoneIndex].BoneOffset * GlobalTransformation * m_GlobalInverseTransform;
		}
	}

	unsigned int n = std::min(pNode1->mNumChildren, pNode2->mNumChildren);
	for (unsigned int i = 0; i < n; i++)
	{
		ReadNodeHierarchy(scene1, scene2, AnimationTime1, AnimationTime2, pNode1->mChildren[i], pNode2->mChildren[i], GlobalTransformation, StopAnimLevel);
	}
}

aiQuaternion Animation::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		return pNodeAnim->mRotationKeys[0].mValue;
	}
	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = RotationIndex + 1;

	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

	aiQuaternion StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	aiQuaternion EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion OutRotation;
	aiQuaternion::Interpolate(OutRotation, StartRotationQ, EndRotationQ, Factor);
	OutRotation = OutRotation.Normalize();
	return OutRotation;
	//return  nlerp(StartRotationQ, EndRotationQ, Factor);

}

aiVector3D Animation::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		return pNodeAnim->mScalingKeys[0].mValue;

	}
	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);

	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

	aiVector3D StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	aiVector3D EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D delta = EndScale - StartScale;


	return StartScale + Factor * delta;

	//return aiVector3D(1, 1, 1);
}

aiVector3D Animation::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		return pNodeAnim->mPositionKeys[0].mValue;

	}
	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);

	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	
	aiVector3D StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	aiVector3D EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D delta = EndPosition - StartPosition;


	return StartPosition + Factor * delta;
}

unsigned int Animation::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	return 0;
}

unsigned int Animation::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return 0;
}

unsigned int Animation::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
		{
			return i;
		}
	}
	return 0;
}

const aiNodeAnim* Animation::FindNodeAnim(const aiAnimation* p_animation, const std::string pNodeName)
{
	for (unsigned int i = 0; i < p_animation->mNumChannels; i++)
	{
		const aiNodeAnim* node_anim = p_animation->mChannels[i];
		if (std::string(node_anim->mNodeName.data) == pNodeName)
		{
			std::cout << std::endl;
			return node_anim;
		}
		std::cout << std::endl;
	}
	return nullptr;
}

aiQuaternion Animation::nlerp(aiQuaternion a, aiQuaternion b, float blend)
{
	a.Normalize();
	b.Normalize();

	aiQuaternion result;
	float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	float one_minus_blend = 1.0f - blend;

	if (dot_product < 0.0f)
	{
		result.x = a.x * one_minus_blend + blend * -b.x;
		result.y = a.y * one_minus_blend + blend * -b.y;
		result.z = a.z * one_minus_blend + blend * -b.z;
		result.w = a.w * one_minus_blend + blend * -b.w;
	}
	else
	{
		result.x = a.x * one_minus_blend + blend * b.x;
		result.y = a.y * one_minus_blend + blend * b.y;
		result.z = a.z * one_minus_blend + blend * b.z;
		result.w = a.w * one_minus_blend + blend * b.w;
	}

	return result.Normalize();
}