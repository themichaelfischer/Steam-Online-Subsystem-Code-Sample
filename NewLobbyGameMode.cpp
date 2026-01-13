// Fill out your copyright notice in the Description page of Project Settings.


#include "NewLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ANewLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green,
		FString::Printf(TEXT("POST LOGIN Number of Players: %i"), NumberOfPlayers));
	} 
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Number of Players: %i"), NumberOfPlayers);
	}
	
	if (NumberOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			bUseSeamlessTravel = true;
			World->ServerTravel(FString("/Game/Maps/Lvl_MainMap?listen"));
		}
	}
}

void ANewLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay | Auth=%d NetMode=%d"),
		HasAuthority(),
		(int32)GetWorld()->GetNetMode());

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("First PC = %s"), *GetNameSafe(PC));
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Green,
		FString::Printf(TEXT("GAME MODE RAN")));
	} else
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME MODE FAILED"));
	}

}
