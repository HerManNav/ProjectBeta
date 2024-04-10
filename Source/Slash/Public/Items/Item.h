// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class USoundBase;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH_API AItem : public AActor
{
	GENERATED_BODY()

public:

	AItem();
	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	/**
	* Methods
	*/

	void AttachToComponentAndSocket(USceneComponent* InParent, const FName& InSocketName);
	void PlaySound(USoundBase* Sound, FVector Location);
	void PlayPickupSound();
	void PlayPickupParticles();
	void DisableSphereCollision();
	bool CanActorPickup(AActor* Actor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** 
	* Variables
	*/

	/** State */

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere, Category = "Pickup|Sound")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditAnywhere, Category = "Pickup|Particles")
	TObjectPtr<UNiagaraComponent> PickupParticles;

	/** Components */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> Niagara;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

public:
	
	/*
	* Setters and getters
	*/

	void SetStaticMesh(UStaticMesh* Mesh) { ItemMesh->SetStaticMesh(Mesh); }

	void SetGenerateOverlapEvents(bool Enable);		// Need to define this in the .cpp because we do stuff with UBoxComponent, which is not included in this .h file!

};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}