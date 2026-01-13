// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "SteamSessionsSubsystem.generated.h"

//
// Declaring our own custom delegates for the Menu class to bind callbacks to
//

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamOnCreateSessionComplete, bool, bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_TwoParams(FSteamOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FSteamOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamOnDestroySessionComplete, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSteamOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class STEAMMULTIPLAYER_API USteamSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	USteamSessionsSubsystem();
	
	// To handle session functionality. Menu Class will call these
	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void DestroySession();
	void StartSession();
	
	//
	// Our own custom delegates for the Menu Class to bind callbacks to
	//
	FSteamOnCreateSessionComplete SteamOnCreateSessionComplete;
	FSteamOnFindSessionsComplete SteamOnFindSessionsComplete;
	FSteamOnJoinSessionComplete SteamOnJoinSessionComplete;
	FSteamOnDestroySessionComplete SteamOnDestroySessionComplete;
	FSteamOnStartSessionComplete SteamOnStartSessionComplete;

protected:
	// Internal callbacks for the delegates we'll add to the Online Session Interface delegate list.
	// These don't need to be called outside this class.
	
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	
private:
	IOnlineSessionPtr SessionInterface;
	TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;
	
	// To add to the Online Session Interface delegate list. 
	// Will bind our SteamSessionsSubsystems internal callbacks to these.
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegateHandle;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteDelegateHandle;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteDelegateHandle;
	
	bool bCreateSessionOnDestroy {false};
	int32 LastNumPublicConnections {0};
	FString LastMatchType {TEXT("")};
};














