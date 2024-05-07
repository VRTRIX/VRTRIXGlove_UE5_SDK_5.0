// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VRTRIXDataGloveClient.h"
#include <Core.h>
#include <Engine.h>
#include "EngineUtils.h"
#include "CoreMinimal.h"


#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Components/PoseableMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GloveComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogVRTRIXGlovePlugin, Log, All);

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class Hand : uint8
{
	Right 	UMETA(DisplayName = "RightHand"),
	Left 	UMETA(DisplayName = "LeftHand")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class HardwareVersion : uint8
{
	DK1 	UMETA(DisplayName = "DK1"),
	DK2 	UMETA(DisplayName = "DK2"),
	PRO		UMETA(DisplayName = "PRO"),
	PRO7	UMETA(DisplayName = "PRO7"),
	PRO11	UMETA(DisplayName = "PRO11"),
	PRO12	UMETA(DisplayName = "PRO12")
};

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class DeviceID : uint8
{
	Device0 	UMETA(DisplayName = "Device0"),
	Device1 	UMETA(DisplayName = "Device1"),
	Device2		UMETA(DisplayName = "Device2"),
	Device3 	UMETA(DisplayName = "Device3"),
	Device4 	UMETA(DisplayName = "Device4"),
	Device5		UMETA(DisplayName = "Device5")
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGestureDetectDelegate);
//hand position cross the bound and fall into (Lower,Upper) will trigger the gesture event 
USTRUCT(BlueprintType, Blueprintable)//"BlueprintType" is essential to include
struct FMyGesture
{
	GENERATED_BODY()
    public:
	//The Lower Bound  position for this Gesture.Number indicate the angle between fingers and palm.Default O means don't care for the corresponding Finger. 
	//Sequence: Thumb,Index,Middle,Ring,Pinky
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyGesture")
	TArray <float> Lower_bound = {0,0,0,0,0};
	//The Upper Bound  position for this Gesture.Default O means don't care for the corresponding Finger.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyGesture")
	TArray <float> Upper_bound = { 0,0,0,0,0 };
	//Position State indicator,Blueprint accessable,True means postion is inside (Lower,Upper) 
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "MyGesture")
	bool Gesture_State =false;
    // function to check if current hand gesture is fall into the boundaries of this gesture
	bool TriggerPositionCheck(TArray <float> Current_Position);
};


USTRUCT(BlueprintType)
struct FHandBonesName
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Wrist = "LeftHand";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Thumb1 = "LeftHandThumb1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Thumb2 = "LeftHandThumb2";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Thumb3 = "LeftHandThumb3";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName IndexInHand = "LeftInHandIndex";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Index1 = "LeftHandIndex1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Index2 = "LeftHandIndex2";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Index3 = "LeftHandIndex3";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName MiddleInHand = "LeftInHandMiddle";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Middle1 = "LeftHandMiddle1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Middle2 = "LeftHandMiddle2";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Middle3 = "LeftHandMiddle3";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName RingInHand = "LeftInHandRing";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Ring1 = "LeftHandRing1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Ring2 = "LeftHandRing2";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Ring3 = "LeftHandRing3";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName PinkyInHand = "LeftInHandPinky";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Pinky1 = "LeftHandPinky1";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Pinky2 = "LeftHandPinky2";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FName Pinky3 = "LeftHandPinky3";
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRTRIXGLOVE_API UGloveComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGloveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EEndPlayReason)override;

public:	
	// Receive new pose from data gloves handler
	void OnReceiveNewPose(VRTRIX::Pose pose);

	// Connect data gloves
	void OnConnectGloves();
	
	// Disconnect data gloves
	void OnDisconnectGloves();

	// Initialize tracking system
	bool GetTrackingSystem();

	//Gesture detecting,update gesture state,generate gesture event
	void Calculate_Gesture_Event();

	//Gesture detecting,update gesture state
	void Calculate_Gesture_State();

	// VRTRIX Data Gloves System
	VRTRIX::IVRTRIXDataGloveClient* pDataGlove;
	


	// VRTRIX Data Gloves Port States
	bool bIsDataGlovePortOpened = false;
	
	// VRTRIX Data Gloves HandType
	VRTRIX::HandType type;

	// VRTRIX Data Gloves bone index to name map
	TMap<int, FName> BoneIndexToBoneNameMap;
	
	// Reconnect data gloves
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void ApplyHandMoCapWorldSpaceRotation(UPoseableMeshComponent *SkinMesh, FRotator alignment);

	// Reconnect data gloves
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void OnReconnect();

	//Call this function to trigger vibration for specific duration
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void OnTriggerHaptics(int duration = 100);

	//Call this function to toggle vibration (ON->OFF, OFF->ON)
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void OnToggleHaptics();

	//Allign hand's orientation  to Head Mounted Display'sorientation, only need do once after Head Mounted Display'sorientation set up changed
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void OrientationAlignment();

	//Call this function to set wrist parent joint for alignment.
	UFUNCTION(BlueprintCallable, Category = "VRTRIX_GLOVES")
	void SetWristAlignment(FRotator alignment);
	

	//Gesture Trigger Event. Hand Position Cross Into the Defined Bounds of this Gesture 
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_0;
	//Gesture Release Event. Hand Position Cross Out the Defined Bounds of this Gesture
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_0;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_1;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_1;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_2;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_2;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_3;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_3;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_4;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_4;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_5;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_5;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_6;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_6;

	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Triggered_7;
	UPROPERTY(BlueprintAssignable, Category = "Gesture_Event")
		FGestureDetectDelegate Gesture_Released_7;
	


	//Which hand you want to bond with your model,make sure the receiver for selected hand is connected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		Hand HandType = Hand::Left;
	//Which hand you want to bond with your model,make sure the receiver for selected hand is connected
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		HardwareVersion HardwareVersion = HardwareVersion::PRO11;
	//Server ip address to fetch glove data
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FString ServerIP = "127.0.0.1";
	//Device ID to identify which glove to connect.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		DeviceID GloveID = DeviceID::Device0;
	//Advanced mode to unlock finger yaw rotation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		bool AdvancedMode = false;
	//Toggle VR Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		bool bIsVREnabled = false;
	//Toggle Wrist Alignment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		bool bIsWristAlignEnabled = false;
	//Show debug info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		bool ShowDebugInfo = false;
	//Rotating Coordinate offset between model and IMU.Only if using your own hand-mesh &your hand-rotation vs mesh-rotation not match, adjust this value to re-match them.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		TArray<FVector> AxisOffset = { FVector(0, 1, 0), FVector(0, 0, -1), FVector(-1, 0, 0) };
	//The original pose global rotation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FRotator InitialPoseOffset = FRotator(90, 0, 180);

	//The model thumb offset.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		TArray<FVector> ThumbOffset = { FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0) };
	//The model thumb proximal slerp.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		float ThumbProximalSlerp = 0.4;
	//The model thumb middle slerp.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		float ThumbMiddleSlerp = 0.7;
	//The model finger spacing when advanced mode is NOT enabled.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		float FingerSpacing = 2;
	//The model final finger spacing when four fingers are fully bended.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		float FinalFingerSpacing = 2;

	//The model finger & wrist offset.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FRotator WristFingerOffset = FRotator(0, 0, 0);
	//The wrist tracker offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FVector WristTrackerOffset = FVector(-4.5f, 0, 0);
	//The wrist tracker offset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		FRotator WristTrackerRotOffset = FRotator(180.0f, -90.0f, 0);

	//Customized Gestures define, gesture state will be updated every frame and can be accessed anytime.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
		TArray<FMyGesture> Gesture_NonEvent;  
         int state_gesture_num = 0;
	//Customized Gestures define. Defined Gestures will trigger the corresponding Gesture Event in Event Category.Gesture state can be accessed anytime.Up to 8 Gestures.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Developer_Configurable")
	     TArray<FMyGesture> Gesture_Event;
     	 int event_gesture_num=0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bone_Configuration")
		FHandBonesName handBoneNames;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Glove_Data")
		TArray<FRotator> rotation;
	//Angles between fingers and palm, can be used for customized Gesture debugging. Sequence: Thumb,Index,Middle,Ring,Pinky
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Glove_Data")
		TArray<float> FingerBendingAngle;
	// VRTRIX Data Gloves States
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Transient, Category = "Glove_Data")
		bool bIsDataGloveConnected = false;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	FMatrix ml_axisoffset;
	FMatrix mr_axisoffset;
	FQuat initialPoseOffset;
	FQuat alignmentPose;
	FQuat LWristTrackerPitchOffset;
	FQuat RWristTrackerPitchOffset;
	bool bIsLOffsetCal = false;
	bool bIsROffsetCal = false;
	FVector m_LTrackerLoc;
	FVector m_RTrackerLoc;
	FRotator m_LTrackerRot;
	FRotator m_RTrackerRot;
	int m_LHTrackerIndex;
	int m_RHTrackerIndex;
	VRTRIX::Pose m_pose;

	TArray<FVector> m_ThumbOffset = { FVector(0, 0, 0), FVector(0, 0, 0), FVector(0, 0, 0) };
	float m_ThumbProximalSlerp = 0;
	float m_ThumbMiddleSlerp = 0;
	float m_FingerSpacing = 0;
	float m_FinalFingerSpacing = 0;

private:
	void CreateBoneIndexToBoneNameMap(FHandBonesName names);
	double CalculateBendAngle(const VRTRIX::VRTRIXQuaternion_t& q1, const VRTRIX::VRTRIXQuaternion_t& q2);
	double GetFingerBendAngle(VRTRIX::Joint finger);
	void PerformAlgorithmTuning();
};

class CVRTRIXIMUEventHandler :public VRTRIX::IVRTRIXIMUEventHandler
{
	/** OnReceivedNewPose event call back function implement
	*
	* @param pose: Pose struct returned by the call back function
	* @param pUserParam: user defined parameter
	* @returns void
	*/
	void OnReceivedNewPose(VRTRIX::Pose pose, void* pUserParam) {
		UGloveComponent* source = (UGloveComponent*)pUserParam;
		source->OnReceiveNewPose(pose);
	}

	/** OnReceivedNewEvent event call back function implement
	*
	* @param event: Event struct returned by the call back function
	* @param pUserParam: user defined parameter
	* @returns void
	*/
	void OnReceivedNewEvent(VRTRIX::HandEvent event, void* pUserParam) {
		UGloveComponent* source = (UGloveComponent*)pUserParam;
		FString handTypeString = (event.type == VRTRIX::Hand_Left) ? "Left Hand Glove" : "Right Hand Glove";
 		switch (event.stat) {
		case(VRTRIX::HandStatus_Connected): {
			UE_LOG(LogVRTRIXGlovePlugin, Warning, TEXT("[GLOVES PULGIN] %s Connected at address: %s: %s."), *handTypeString, *source->ServerIP, *(FString::FromInt(11002+(int)source->GloveID)));
			source->bIsDataGloveConnected = true;

			//Set radio channel limit between 65 to 99 (2465Mhz to 2499Mhz) before start data streaming if needed. (this step is optional)
			VRTRIX::EIMUError eIMUError;
			source->pDataGlove->SetRadioChannelLimit(eIMUError, 99, 65);
			if (eIMUError == VRTRIX::IMUError_DataNotValid) UE_LOG(LogVRTRIXGlovePlugin, Display, TEXT("[GLOVES PULGIN] Radio Channel Not Valid!"));
			break;
		}
		case(VRTRIX::HandStatus_Disconnected): {
			UE_LOG(LogVRTRIXGlovePlugin, Warning, TEXT("[GLOVES PULGIN] %s Disconnected."), *handTypeString);
			source->bIsDataGloveConnected = false;
			break;
		}
		}
	}
};

