// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_RisingPillar.h"
#include "UObject/ConstructorHelpers.h"

AOecRoomEvent_RisingPillar::AOecRoomEvent_RisingPillar()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (cylinderMesh.Succeeded())
	{
		// 지름 220, 높이 160 (점프로 올라갈 수 있는 높이)
		AddStructureMesh(TEXT("Pillar"), cylinderMesh.Object, FVector(0.0f, 0.0f, 80.0f), FVector(2.2f, 2.2f, 1.6f));
	}
}
