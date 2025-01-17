﻿// Copyright 2022 AEC Systems, Licensed under the Apache License, Version 2.0

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Dom/JsonObject.h"
#include "Objects/HighLevel/SpeckleStream.h"

#include "Transport.generated.h"

DECLARE_DELEGATE_OneParam(FTransportCopyObjectCompleteDelegate, TSharedPtr<FJsonObject>);
DECLARE_DELEGATE_OneParam(FTransportErrorDelegate, FString&);
DECLARE_DELEGATE_OneParam(FStreamsRequestProcessedDelegate, const TArray<FSpeckleStream>&);

//DECLARE_DELEGATE_OneParam(FTransportTotalChildrenCountKnownDelegate, int32);
//DECLARE_DELEGATE_OneParam(FTransportProgressDelegate, int32);

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UTransport : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECKLEUNREAL_API ITransport
{
	GENERATED_BODY()

public:

	
	FString ResponseListOfStreamsSerialized = "";
	
 	virtual void SaveObject(const FString& ObjectId, const TSharedPtr<FJsonObject> SerializedObject) = 0;

 	//virtual void SaveObjectFromTransport(FString& ObjectID, TScriptInterface<ITransport> SourceTransport) = 0;

 	virtual TSharedPtr<FJsonObject> GetSpeckleObject(const FString& ObjectId) const = 0;
	
 	virtual bool HasObject(const FString& ObjectId) const = 0;
	
 	virtual void CopyObjectAndChildren(const FString& ObjectId,
 									   TScriptInterface<ITransport> TargetTransport,
 									   const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
 									   const FTransportErrorDelegate& OnErrorAction) = 0;

	virtual void CopyListOfStreams(const FString& ObjectId,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) = 0;

	virtual void CopyListOfBranches(
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) = 0;


	virtual void CopyListOfCommits(		
										const FString& BranchName,
										TScriptInterface<ITransport> TargetTransport,
										const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
										const FTransportErrorDelegate& OnErrorAction) = 0;

	virtual void CopyMyUserData(		
									TScriptInterface<ITransport> TargetTransport,
									const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
									const FTransportErrorDelegate& OnErrorAction) = 0;

	virtual void CopyGlobals(		const FString& ReferencedObjectId,
									TScriptInterface<ITransport> TargetTransport,
									const FTransportCopyObjectCompleteDelegate& OnCompleteAction,
									const FTransportErrorDelegate& OnErrorAction) = 0;

};
