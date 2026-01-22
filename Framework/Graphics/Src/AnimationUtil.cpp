#include "Precompiled.h"
#include "AnimationUtil.h"

#include "Colors.h"
#include "SimpleDraw.h"

using namespace VEngine;
using namespace VEngine::Graphics;

//empty namespace for global functions isolated to the cpp file
namespace
{
	void ComputeBoneTransformsRecursive(const Bone* bone, AnimationUtil::BoneTransforms& boneTransforms)
	{
		if (bone != nullptr)
		{
			//Set the bone transform to the array of matrices
			boneTransforms[bone->index] = bone->toParentTransform;
			//If there is a parent, apply the parent's transform as well
			if (bone->parent != nullptr)
			{
				boneTransforms[bone->index] = boneTransforms[bone->index] * boneTransforms[bone->parentIndex];
			}
			//Go through the children and apply their transforms
			for (const Bone* child : bone->children)
			{
				ComputeBoneTransformsRecursive(child, boneTransforms);
			}
		}
	}
}

void VEngine::Graphics::AnimationUtil::ComputeBoneTransforms(ModelId modelId, BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		//resize to sync the number of bones with the matrices
		boneTransforms.resize(model->skeleton->bones.size());
		//Generate the matrices
		ComputeBoneTransformsRecursive(model->skeleton->root, boneTransforms);
	}
}

void VEngine::Graphics::AnimationUtil::DrawSkeleton(ModelId modelId, const BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		//Iterate through the unique bone pointers
		for (const auto& bone : model->skeleton->bones)
		{
			if (bone->parent != nullptr)
			{
				//Gets the bone and parent bone positions
				const Math::Vector3 bonePos = Math::GetTranslation(boneTransforms[bone->index]);
				const Math::Vector3 parentPos = Math::GetTranslation(boneTransforms[bone->parentIndex]);
				//Draws a line from the bone to its parent
				SimpleDraw::AddLine(bonePos, parentPos, Colors::HotPink);
				//Adds a sphere
				SimpleDraw::AddSphere(10, 10, 0.03f, bonePos, Colors::Blue);
			}
		}
	}
}

void VEngine::Graphics::AnimationUtil::ApplyBoneOffset(ModelId modelId, BoneTransforms& boneTransforms)
{
	const Model* model = ModelManager::Get()->GetModel(modelId);
	if (model != nullptr && model->skeleton != nullptr)
	{
		for (auto& bone : model->skeleton->bones)
		{
			boneTransforms[bone->index] = bone->offsettTransform * boneTransforms[bone->index];
		}
	}
}
