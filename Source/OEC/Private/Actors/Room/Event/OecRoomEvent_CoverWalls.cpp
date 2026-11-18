// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Room/Event/OecRoomEvent_CoverWalls.h"
#include "UObject/ConstructorHelpers.h"

AOecRoomEvent_CoverWalls::AOecRoomEvent_CoverWalls()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> cubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (cubeMesh.Succeeded())
	{
		// 폭 300, 두께 40, 높이 200
		AddStructureMesh(TEXT("Wall1"), cubeMesh.Object, FVector(-450.0f, -150.0f, 100.0f), FVector(3.0f, 0.4f, 2.0f));
		AddStructureMesh(TEXT("Wall2"), cubeMesh.Object, FVector(0.0f, 350.0f, 100.0f), FVector(0.4f, 3.0f, 2.0f));
		AddStructureMesh(TEXT("Wall3"), cubeMesh.Object, FVector(450.0f, -250.0f, 100.0f), FVector(3.0f, 0.4f, 2.0f));
	}
}
