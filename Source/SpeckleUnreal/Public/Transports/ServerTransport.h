﻿// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "Transport.h"
#include "Objects/HighLevel/SpeckleStream.h"

#include "ServerTransport.generated.h"


class FHttpModule;

// Data for graphQL request for object ids.
USTRUCT()
struct FObjectIdRequest
{
	GENERATED_BODY()
		
	UPROPERTY()
	TArray<FString> Ids;
};

/**
 *  Transport for receiving objects from a Speckle Server
 */
UCLASS(BlueprintType)
class SPECKLEUNREAL_API UServerTransport : public UObject, public ITransport
{
 GENERATED_BODY()
	
protected:
	
	UPROPERTY()
	FString ServerUrl;

	UPROPERTY()
	FString StreamId;

	UPROPERTY(meta=(PasswordField))
	FString AuthToken;

	UPROPERTY()
	int32 MaxNumberOfObjectsPerRequest = 20000;

	UPROPERTY()
	FString ResponseListOfStreamsSerialized = "";

	UPROPERTY()
	FString ResponseListOfBranchesSerialized = "";

	UPROPERTY()
	FString ResponseListOfCommitsSerialized = "";

	UPROPERTY()
	FString ResponseGlobalsSerialized = "";
	
	UPROPERTY()
	TArray<FSpeckleStream> ArrayOfStreams;

	UPROPERTY()
	TArray<FSpeckleStream> ArrayOfBranches;
	
	FTransportCopyObjectCompleteDelegate OnComplete;
	FTransportErrorDelegate OnError;
	
	
public:



	UFUNCTION(BlueprintPure, Category = "Speckle|Transports")
	static UServerTransport* CreateServerTransport(UPARAM(ref) FString& _ServerUrl,
												   UPARAM(ref)  FString& _StreamId,
												   UPARAM(ref) FString& _AuthToken)
	{
		UServerTransport* Transport = NewObject<UServerTransport>();
		Transport->ServerUrl = _ServerUrl;
		Transport->StreamId = _StreamId;
		Transport->AuthToken = _AuthToken;
		return Transport;
	}
	
	virtual TSharedPtr<FJsonObject> GetSpeckleObject(const FString& ObjectId) const override;
	virtual void SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject) override;
	
	virtual bool HasObject(const FString& ObjectId) const override;
	
 	virtual void CopyObjectAndChildren(const FString& ObjectId,
 										TScriptInterface<ITransport> TargetTransport,
 										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
 										const FTransportErrorDelegate& OnErrorAction) override;
	
	virtual void CopyListOfStreams(const FString& ObjectId,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) override;

	virtual void CopyListOfBranches(
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) override;

	virtual void CopyListOfCommits( 
									const FString& BranchName,
									TScriptInterface<ITransport> TargetTransport,
									const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
									const FTransportErrorDelegate& OnErrorAction) override;

	virtual void CopyMyUserData( 
									TScriptInterface<ITransport> TargetTransport,
									const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
									const FTransportErrorDelegate& OnErrorAction) override;

	virtual void CopyGlobals(       const FString& ReferencedObjectId,
									TScriptInterface<ITransport> TargetTransport,
									const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
									const FTransportErrorDelegate& OnErrorAction) override;
	
protected:
	virtual void HandleRootObjectResponse(const FString& RootObjSerialized,
										  TScriptInterface<ITransport> TargetTransport,
										  const FString& ObjectId) const;
	//
	// virtual void HandleListOfStreamsResponse(const FString& RootObjSerialized,
	// 										 TScriptInterface<ITransport> TargetTransport
	// 										);

    /**
	 * Iteratively fetches chunks of children
	 * @param TargetTransport the transport to store the fetched objects
	 * @param RootObjectId the id of the root object
	 * @param ChildrenIds array of all children to be fetched
	 * @param CStart the index in ChildrenIds of the start point of the current chunk
	 */
    virtual void FetchChildren(TScriptInterface<ITransport> TargetTransport,
	                            const FString& RootObjectId,
	                            const TArray<FString>& ChildrenIds,
	                            int32 CStart = 0) const;
	
	virtual void InvokeOnError(FString& Message) const;

	static bool LoadJson(const FString& ObjectJson, TSharedPtr<FJsonObject>& OutJsonObject);
	
	static int32 SplitLines(const FString& Content, TArray<FString>& OutLines);
};


