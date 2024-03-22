// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"

#include "BreakableActor.generated.h"

class ATreasure;
class UGeometryCollectionComponent;
class UCapsuleComponent;

UCLASS()
class SLASH_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:

	ABreakableActor();

	virtual void getHit_Implementation(const FVector& hitPoint) override;

protected:

	virtual void BeginPlay() override;

	/*
	* OnChaosBreakEvent members
	*/

	UFUNCTION()
	virtual void onChaosBreakEvent(const FChaosBreakEvent& breakEvent);

	UPROPERTY(BlueprintReadWrite)
	bool onChaosBreakEvent_processed;

	/* 
	* Spawning treasure members 
	*/

	UPROPERTY(EditAnywhere, Category = ObjectSpawning)
	TArray<TSubclassOf<ATreasure>> classesToSpawn;

	UPROPERTY(EditAnywhere, Category = Sound)
	TObjectPtr<USoundBase> breakingSound;

private:

	/*
	* Components
	*/

	UPROPERTY(EditAnywhere)
	TObjectPtr<UGeometryCollectionComponent> geometryCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> collisionCapsule;

	/*
	* Spawn treasure
	*/

	ATreasure* spawnedTreasure;		// This is needed to be a class member for the delay to work properly. If it was a local variable in spawnRandomTreasure instead, we would need to pass a copy of spawnedTreasure, cause if we'd pass a reference to the lambda like [&spawnedTreasure], it will crash after the timer! Besides, it can be a raw pointer since it's not marked with UPROPERTY (not in the Unreal refletion system)

	void spawnRandomTreasure(const FVector& location);

};
