// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "SteamSessionsSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UMenu::MenuSetup(int NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		SteamSessionsSubsystem = GameInstance->GetSubsystem<USteamSessionsSubsystem>();
	}
	
	if (SteamSessionsSubsystem)
	{
		SteamSessionsSubsystem->SteamOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		SteamSessionsSubsystem->SteamOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		SteamSessionsSubsystem->SteamOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		SteamSessionsSubsystem->SteamOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		SteamSessionsSubsystem->SteamOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
	
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
			FString::Printf(TEXT("OSS: %s"), OSS ? *OSS->GetSubsystemName().ToString() : TEXT("None")));
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize()) return false;
	
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,
				"Session Created Successfully");
		}
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	} else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
				"Session Created FAILED");
			HostButton->SetIsEnabled(true);
		}
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (SteamSessionsSubsystem == nullptr) return;
	
	for (FOnlineSessionSearchResult Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			// Valid search result matching what I am looking for (FreeForAll
			SteamSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	
	if (!Subsystem) return;
	
	IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		FString Address;
		SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
		
		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
			"HostButtonClicked");
	}
	if (SteamSessionsSubsystem)
	{
		SteamSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
		
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
			"JoinButtonClicked");
	}
	if (SteamSessionsSubsystem)
	{
		SteamSessionsSubsystem->FindSessions(1000);
	}
}

void UMenu::MenuTearDown()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,
			"PLAYER CAN MOVE");
	}
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
	
}
