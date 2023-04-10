// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_BlendGloveMoCap.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_BlendGloveMoCap::FAnimNode_BlendGloveMoCap()
{
}

void FAnimNode_BlendGloveMoCap::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);

	Glove.Initialize(Context);
	MoCap.Initialize(Context);
	UAnimInstance* anim = Context.AnimInstanceProxy->GetSkelMeshComponent()->GetAnimInstance();
	if (anim == nullptr || anim->GetOwningActor() == nullptr) {
		//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] AnimInstance is NULL."));
		return;
	}
	TArray<UActorComponent*> Comps;
	anim->GetOwningActor()->GetComponents(UGloveComponent::StaticClass(), Comps);
	for (auto& component : Comps)
	{
		UGloveComponent* glove = dynamic_cast<UGloveComponent*> (component);
		if (glove->HandType == Hand::Left) LHGloveComponent = glove;
		else if (glove->HandType == Hand::Right) RHGloveComponent = glove;
		//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] Find Glove Component %d."), (int)glove->HandType);
	}
}

void FAnimNode_BlendGloveMoCap::CacheBones_AnyThread(const FAnimationCacheBonesContext & Context)
{
	Glove.CacheBones(Context);
	MoCap.CacheBones(Context);
}

void FAnimNode_BlendGloveMoCap::Update_AnyThread(const FAnimationUpdateContext & Context)
{
	Glove.Update(Context);
	MoCap.Update(Context);
	GetEvaluateGraphExposedInputs().Execute(Context);

	if (!LHGloveComponent.IsValid() || !RHGloveComponent.IsValid()) {
		UAnimInstance* anim = Context.AnimInstanceProxy->GetSkelMeshComponent()->GetAnimInstance();
		if (anim == nullptr || anim->GetOwningActor() == nullptr) {
			//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] AnimInstance is NULL."));
			return;
		}
		TArray<UActorComponent*> Comps;
		anim->GetOwningActor()->GetComponents(UGloveComponent::StaticClass(), Comps);
		for (auto& component : Comps)
		{
			UGloveComponent* glove = dynamic_cast<UGloveComponent*> (component);
			if (glove->HandType == Hand::Left) LHGloveComponent = glove;
			else if (glove->HandType == Hand::Right) RHGloveComponent = glove;
			//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] Find Glove Component %d."), (int)glove->HandType);
		}
	}
}

void FAnimNode_BlendGloveMoCap::Evaluate_AnyThread(FPoseContext & Output)
{
	MoCap.Evaluate(Output);

	if (!LHGloveComponent.IsValid() || !RHGloveComponent.IsValid()) return;
	UGloveComponent* LeftGlove = LHGloveComponent.Get();
	UGloveComponent* RightGlove = RHGloveComponent.Get();

	if (!LeftGlove->bIsDataGloveConnected || !RightGlove->bIsDataGloveConnected) return;

	FCSPose<FCompactPose> FinalPose;
	FinalPose.InitPose(&Output.Pose.GetBoneContainer());
	const FBoneContainer Container = FinalPose.GetPose().GetBoneContainer();

	FPoseContext GloveContext(Output);
	Glove.Evaluate(GloveContext);

	int startIndex = bIsAlignWrist ? 1 : 0;
	TArray<int32> handBoneIndex;
	for (int i = startIndex; i < LeftGlove->BoneIndexToBoneNameMap.Num(); i++)
	{
		int32 MeshBoneIndex = Container.GetPoseBoneIndexForBoneName(LeftGlove->BoneIndexToBoneNameMap[i]);
		if (MeshBoneIndex != INDEX_NONE)
		{
			handBoneIndex.Push(MeshBoneIndex);
		}
	}

	for (int i = startIndex; i < RightGlove->BoneIndexToBoneNameMap.Num(); i++)
	{
		int32 MeshBoneIndex = Container.GetPoseBoneIndexForBoneName(RightGlove->BoneIndexToBoneNameMap[i]);
		if (MeshBoneIndex != INDEX_NONE)
		{
			handBoneIndex.Push(MeshBoneIndex);
		}
	}

	//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] Total Glove Bone: %d"), handBoneIndex.Num());

	for (FCompactPoseBoneIndex BoneIndex : Output.Pose.ForEachBoneIndex())
	{
		if (handBoneIndex.Find(BoneIndex.GetInt()) != INDEX_NONE) {
			Output.Pose[BoneIndex] = GloveContext.Pose[BoneIndex];
		}
	}
}
