// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_VRTRIXGlovePose.h"
#include "Animation/AnimInstanceProxy.h"

FAnimNode_VRTRIXGlovePose::FAnimNode_VRTRIXGlovePose()
{
}

void FAnimNode_VRTRIXGlovePose::Initialize_AnyThread(const FAnimationInitializeContext & Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	if (!Context.AnimInstanceProxy || Context.AnimInstanceProxy->GetMainInstanceProxy() == nullptr)
	{
		//wait until we have a proper anim instance
		return;
	}
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
		else if(glove->HandType == Hand::Right) RHGloveComponent = glove;
		//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] Find Glove Component %d."), (int)glove->HandType);
	}
}

void FAnimNode_VRTRIXGlovePose::Update_AnyThread(const FAnimationUpdateContext & Context)
{
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

void FAnimNode_VRTRIXGlovePose::Evaluate_AnyThread(FPoseContext & Output)
{
	//Output.ResetToRefPose();
	if (!LHGloveComponent.IsValid() || !RHGloveComponent.IsValid()) return;
	UGloveComponent* LeftGlove = LHGloveComponent.Get();
	UGloveComponent* RightGlove = RHGloveComponent.Get();

	if (!LeftGlove->bIsDataGloveConnected || !RightGlove->bIsDataGloveConnected) return;

	FCSPose<FCompactPose> pose;
	pose.InitPose(&Output.Pose.GetBoneContainer());
	const FBoneContainer Container = pose.GetPose().GetBoneContainer();

	TArray<FQuat> LHFingerRotation, RHFingerRotation;
	LHFingerRotation.Init(FQuat::Identity, LeftGlove->BoneIndexToBoneNameMap.Num());
	RHFingerRotation.Init(FQuat::Identity, RightGlove->BoneIndexToBoneNameMap.Num());

	if (LeftGlove->BoneIndexToBoneNameMap.Num() == VRTRIX::Joint_MAX && RightGlove->BoneIndexToBoneNameMap.Num() == VRTRIX::Joint_MAX) {
		for (int i = 0; i < LeftGlove->BoneIndexToBoneNameMap.Num(); i++) {
			LHFingerRotation[i] = LeftGlove->rotation[i].Quaternion();
			RHFingerRotation[i] = RightGlove->rotation[i].Quaternion();
		}
	}
	else if (LeftGlove->BoneIndexToBoneNameMap.Num() == VRTRIX::Joint_MAX + 4 && RightGlove->BoneIndexToBoneNameMap.Num() == VRTRIX::Joint_MAX + 4) {
		for (int i = 0; i < LeftGlove->BoneIndexToBoneNameMap.Num(); i++) {
			int QuatIndex = 0;
			if (i == 4 || i == 8 || i == 12 || i == 16) QuatIndex = 0;
			else if (i < 4) QuatIndex = i;
			else if (i < 8) QuatIndex = i - 1;
			else if (i < 12) QuatIndex = i - 2;
			else if (i < 16) QuatIndex = i - 3;
			else QuatIndex = i - 4;
			LHFingerRotation[i] = LeftGlove->rotation[QuatIndex].Quaternion();
			RHFingerRotation[i] = RightGlove->rotation[QuatIndex].Quaternion();
		}
	}
	else return;

	for (int i = 0; i < LeftGlove->BoneIndexToBoneNameMap.Num(); i++)
	{
		int32 MeshBoneIndex = Container.GetPoseBoneIndexForBoneName(LeftGlove->BoneIndexToBoneNameMap[i]);
		//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] LH MeshBoneIndex: %d."), MeshBoneIndex);
		if (MeshBoneIndex != INDEX_NONE)
		{
			FCompactPoseBoneIndex CPIndex = Container.MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshBoneIndex));
			if (!pose.GetPose().IsValidIndex(CPIndex)) return;
			FTransform CSTransformFromSource = pose.GetComponentSpaceTransform(CPIndex);
			CSTransformFromSource.SetRotation(LHFingerRotation[i]);
			pose.SetComponentSpaceTransform(CPIndex, CSTransformFromSource);
		}
	}

	for (int i = 0; i < RightGlove->BoneIndexToBoneNameMap.Num(); i++)
	{
		int32 MeshBoneIndex = Container.GetPoseBoneIndexForBoneName(RightGlove->BoneIndexToBoneNameMap[i]);
		//UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] RH MeshBoneIndex: %d."), MeshBoneIndex);
		if (MeshBoneIndex != INDEX_NONE)
		{
			FCompactPoseBoneIndex CPIndex = Container.MakeCompactPoseIndex(FMeshPoseBoneIndex(MeshBoneIndex));
			if (!pose.GetPose().IsValidIndex(CPIndex)) return;
			FTransform CSTransformFromSource = pose.GetComponentSpaceTransform(CPIndex);
			CSTransformFromSource.SetRotation(RHFingerRotation[i]);
			pose.SetComponentSpaceTransform(CPIndex, CSTransformFromSource);
		}
	}

	FCSPose<FCompactPose>::ConvertComponentPosesToLocalPoses(pose, Output.Pose);
}
