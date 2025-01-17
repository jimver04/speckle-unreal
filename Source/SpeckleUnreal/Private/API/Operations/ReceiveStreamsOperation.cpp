﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "API/Operations/ReceiveStreamsOperation.h"

#include "Dom/JsonObject.h"
#include "Transports/Transport.h"
#include "API/SpeckleSerializer.h"
#include "Mixpanel.h"
#include "LogSpeckle.h"


// ReceiveOperation
UReceiveStreamsOperation* UReceiveStreamsOperation::ReceiveStreamsOperation(UObject* WorldContextObject,
													 
							                           TScriptInterface<ITransport> RemoteTransport,
							                           TScriptInterface<ITransport> LocalTransport)
{

	FString ObjectId = "Streams";
	
    UReceiveStreamsOperation* Node = NewObject<UReceiveStreamsOperation>();
    Node->ObjectId = ObjectId;
    Node->RemoteTransport = RemoteTransport;
    Node->LocalTransport = LocalTransport;

	Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

// Activate
void UReceiveStreamsOperation::Activate()
{
	FAnalytics::TrackEvent("unknown",
		"unknown", "NodeRun", TMap<FString, FString> { {"name", StaticClass()->GetName() }});

	//Async(EAsyncExecution::Thread, [this]{Receive();});
	Receive();
}

void UReceiveStreamsOperation::Receive()
{
	check(LocalTransport != nullptr);
	
	// 1. Try and get object from local transport
	auto Obj = LocalTransport->GetSpeckleObject(ObjectId);

	if (Obj != nullptr )
	{
		HandleReceive(Obj);
		return;
	}

	// 2. Try and get object from remote transport
	if(RemoteTransport == nullptr)
	{
		FString ErrorMessage = TEXT(
			"Could not find specified object using the local transport, and you didn't provide a fallback remote from which to pull it.");

		HandleError(ErrorMessage);
		return;
	}

	FTransportCopyObjectCompleteDelegate CompleteDelegate;
	CompleteDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleReceive);

	FTransportErrorDelegate ErrorDelegate;
	ErrorDelegate.BindUObject(this, &UReceiveStreamsOperation::HandleError);
	UE_LOG(LogSpeckle, Log, TEXT("----------->PJSON RECEIVE 1"));
	
	RemoteTransport->CopyListOfStreams(ObjectId, LocalTransport, CompleteDelegate, ErrorDelegate);
}

void UReceiveStreamsOperation::HandleReceive(TSharedPtr<FJsonObject> Object)
{
	check(IsInGameThread())
		
	FEditorScriptExecutionGuard ScriptGuard;
	if(Object == nullptr)
	{
		TArray<FSpeckleStream> EmptyListOfStreams;
		OnError.Broadcast(EmptyListOfStreams, FString::Printf(TEXT("Failed to get Stream object %s from transport"), *ObjectId));
	}
	else
	{
		// --- Here Deserialize the list of Streams ----
		// It is not Deserializing the Streams well
		TArray<FSpeckleStream> FullListOfStreams = USpeckleSerializer::DeserializeListOfStreams(Object, LocalTransport);

		
		if(FullListOfStreams.Num()>0)
		{
			OnReceiveSuccessfully.Broadcast(FullListOfStreams, "");
		}else
		{
			TArray<FSpeckleStream> EmptyListOfStreams;
			OnError.Broadcast(EmptyListOfStreams,
				FString::Printf(TEXT("Root Speckle Stream Object %s failed to deserialize"), *ObjectId));
		}
	}
		
	
	SetReadyToDestroy();
}

void UReceiveStreamsOperation::HandleError(FString& Message)
{
	FEditorScriptExecutionGuard ScriptGuard;
	TArray<FSpeckleStream> EmptyListOfStreams;
	OnError.Broadcast(EmptyListOfStreams, Message);
	SetReadyToDestroy();
}

