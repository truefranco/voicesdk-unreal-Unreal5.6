/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "Wit/Request/HTTP/WitHttpRequest.h"
#include "Misc/App.h"
#include "Misc/Guid.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "HAL/PlatformMisc.h"
#include "Interfaces/IPluginManager.h"
#include "HttpModule.h"
#include "WitModule.h"
#include "Wit/Utilities/WitHelperUtilities.h"
#include "Wit/Utilities/WitLog.h"
#include "Misc/EngineVersion.h"

 /**
  * Constructor
  */
FWitHttpRequest::FWitHttpRequest()
{
	// Create the real Http request that we are wrapping
	RealRequest = FHttpModule::Get().CreateRequest();
}

/**
 * Destructor
 */
FWitHttpRequest::~FWitHttpRequest()
{
	// No longer need to manually clean up curl handles
}

/**
 * User agent for Wit
 */
FString FWitHttpRequest::GetUserAgent()
{
	// OS

	const FString PlatformName = FString(FPlatformProperties::IniPlatformName());
	const FString OSVersion = FPlatformMisc::GetOSVersion();
	const FString OperatingSystem = FString::Printf(TEXT("%s-%s"), *PlatformName, *OSVersion);

	// Device
	const FString DeviceModel = FPlatformMisc::GetDeviceMakeAndModel();

	// Unique GUID
	const FString SessionId = FApp::GetSessionId().ToString(EGuidFormats::Digits);

	// Additional Front User Data

	const FString AdditionalFrontUserData = FWitHelperUtilities::AdditionalFrontUserData;

	// SDK version
	const FString WitSdkVersion = FWitModule::Get().SdkVersion;
	const FString WitPlugin = FString::Printf(TEXT("wit-unreal-%s"), *WitSdkVersion);

	// Project
	const FString ProjectSettingsPath = TEXT("/Script/EngineSettings.GeneralProjectSettings");
	FString Company = TEXT("YourCompany");

	GConfig->GetString(
		*ProjectSettingsPath,
		TEXT("CompanyName"),
		Company,
		GGameIni
	);

	const FString Project = FApp::GetProjectName();
	const FString ProjectId = FString::Printf(TEXT("com.%s.%s"), *Company, *Project);

	// Editor
#if WITH_EDITOR
	const FString UserEnv = TEXT("Editor");
#else
	const FString UserEnv = TEXT("Runtime");
#endif

	const FString EditorVersion = ENGINE_VERSION_STRING;

	// Additional End User Data

	const FString AdditionalEndUserData = FWitHelperUtilities::AdditionalEndUserData;

	FString UserAgent = FString::Printf(TEXT("%s%s,\"%s\",\"%s\",%s,%s,%s,%s%s"),
		*FGenericPlatformHttp::EscapeUserAgentString(AdditionalFrontUserData),
		*FGenericPlatformHttp::EscapeUserAgentString(WitPlugin),
		*FGenericPlatformHttp::EscapeUserAgentString(OperatingSystem),
		*FGenericPlatformHttp::EscapeUserAgentString(DeviceModel),
		*FGenericPlatformHttp::EscapeUserAgentString(SessionId),
		*FGenericPlatformHttp::EscapeUserAgentString(ProjectId),
		*FGenericPlatformHttp::EscapeUserAgentString(UserEnv),
		*FGenericPlatformHttp::EscapeUserAgentString(EditorVersion),
		*FGenericPlatformHttp::EscapeUserAgentString(AdditionalEndUserData)
	);

	UE_LOG(LogWit, Verbose, TEXT("UserAgent: %s"), *UserAgent);

	return UserAgent;
}

//~ Begin IHttpBase Interface
const FString& FWitHttpRequest::GetURL() const
{
	return RealRequest->GetURL();
}

FString FWitHttpRequest::GetURLParameter(const FString& ParameterName) const
{
	return RealRequest->GetURLParameter(ParameterName);
}

FString FWitHttpRequest::GetHeader(const FString& HeaderName) const
{
	return RealRequest->GetHeader(HeaderName);
}

TArray<FString> FWitHttpRequest::GetAllHeaders() const
{
	return RealRequest->GetAllHeaders();
}

FString FWitHttpRequest::GetContentType() const
{
	return RealRequest->GetContentType();
}

uint64 FWitHttpRequest::GetContentLength() const
{
	return RealRequest->GetContentLength();
}

const TArray<uint8>& FWitHttpRequest::GetContent() const
{
	return RealRequest->GetContent();
}

const FString& FWitHttpRequest::GetEffectiveURL() const
{
	return RealRequest->GetEffectiveURL();
}

EHttpFailureReason FWitHttpRequest::GetFailureReason() const
{
	return RealRequest->GetFailureReason();
}
//~ End IHttpBase Interface

//~ Begin IHttpRequest Interface
FString FWitHttpRequest::GetVerb() const
{
	return RealRequest->GetVerb();
}

void FWitHttpRequest::SetVerb(const FString& Verb)
{
	RealRequest->SetVerb(Verb);
}

void FWitHttpRequest::SetURL(const FString& URL)
{
	RealRequest->SetURL(URL);
}

void FWitHttpRequest::SetContent(const TArray<uint8>& ContentPayload)
{
	RealRequest->SetContent(ContentPayload);
}

void FWitHttpRequest::SetContent(TArray<uint8>&& ContentPayload)
{
	RealRequest->SetContent(MoveTemp(ContentPayload));
}

void FWitHttpRequest::SetContentAsString(const FString& ContentString)
{
	RealRequest->SetContentAsString(ContentString);
}

bool FWitHttpRequest::SetContentAsStreamedFile(const FString& Filename)
{
	return false;
}

bool FWitHttpRequest::SetResponseBodyReceiveStream(TSharedRef<FArchive> Stream)
{
	return RealRequest->SetResponseBodyReceiveStream(Stream);
}

void FWitHttpRequest::SetHeader(const FString& HeaderName, const FString& HeaderValue)
{
	RealRequest->SetHeader(HeaderName, HeaderValue);
}

void FWitHttpRequest::AppendToHeader(const FString& HeaderName, const FString& AdditionalHeaderValue)
{
	RealRequest->AppendToHeader(HeaderName, AdditionalHeaderValue);
}

void FWitHttpRequest::SetOption(const FName Option, const FString& OptionValue)
{
	RealRequest->SetOption(Option, OptionValue);
}

void FWitHttpRequest::SetTimeout(float InTimeoutSecs)
{
	RealRequest->SetTimeout(InTimeoutSecs);
}

void FWitHttpRequest::ClearTimeout()
{
	RealRequest->ClearTimeout();
}

TOptional<float> FWitHttpRequest::GetTimeout() const
{
	return RealRequest->GetTimeout();
}

void FWitHttpRequest::SetActivityTimeout(float InTimeoutSecs)
{
	RealRequest->SetActivityTimeout(InTimeoutSecs);
}

void FWitHttpRequest::ResetTimeoutStatus()
{
	RealRequest->ResetTimeoutStatus();
}

bool FWitHttpRequest::ProcessRequest()
{
	// The original implementation for streaming relied on custom cURL callbacks.
	// Since that is no longer possible, we must change the approach.
	// Non-streaming requests can just pass through.
	// Streaming requests will need to be handled differently by the calling code (UWitRequestSubsystem)
	// For now, we just pass through to the real request.

	return RealRequest->ProcessRequest();
}

FHttpRequestCompleteDelegate& FWitHttpRequest::OnProcessRequestComplete()
{
	return RealRequest->OnProcessRequestComplete();
}

FHttpRequestProgressDelegate64& FWitHttpRequest::OnRequestProgress64()
{
	return RealRequest->OnRequestProgress64();
}

FHttpRequestHeaderReceivedDelegate& FWitHttpRequest::OnHeaderReceived()
{
	return RealRequest->OnHeaderReceived();
}

FHttpRequestWillRetryDelegate& FWitHttpRequest::OnRequestWillRetry()
{
	return RealRequest->OnRequestWillRetry();
}

FHttpRequestStatusCodeReceivedDelegate& FWitHttpRequest::OnStatusCodeReceived()
{
	return RealRequest->OnStatusCodeReceived();
}

void FWitHttpRequest::CancelRequest()
{
	RealRequest->CancelRequest();
}

EHttpRequestStatus::Type FWitHttpRequest::GetStatus() const
{
	return RealRequest->GetStatus();
}

const FHttpResponsePtr FWitHttpRequest::GetResponse() const
{
	return RealRequest->GetResponse();
}

void FWitHttpRequest::Tick(float DeltaSeconds)
{
	// We now just tick the real request. The custom pause/resume logic that was here
	// is no longer possible as we cannot access the internal curl handle.
	RealRequest->Tick(DeltaSeconds);
}

float FWitHttpRequest::GetElapsedTime() const
{
	return RealRequest->GetElapsedTime();
}

FString FWitHttpRequest::GetOption(const FName Option) const
{
	return RealRequest->GetOption(Option);
}

void FWitHttpRequest::SetDelegateThreadPolicy(EHttpRequestDelegateThreadPolicy InThreadPolicy)
{
	RealRequest->SetDelegateThreadPolicy(InThreadPolicy);
}

EHttpRequestDelegateThreadPolicy FWitHttpRequest::GetDelegateThreadPolicy() const
{
	return RealRequest->GetDelegateThreadPolicy();
}

void FWitHttpRequest::ProcessRequestUntilComplete()
{
	RealRequest->ProcessRequestUntilComplete();
}
//~ End IHttpRequest Interface

bool FWitHttpRequest::SetContentFromStream(TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	if (GetStatus() == EHttpRequestStatus::Processing)
	{
		return false;
	}

	UE_LOG(LogWit, Verbose, TEXT("FWitHttpRequest::SetContentFromStream: reading stream"));

	// Read the entire stream into a buffer
	TArray<uint8> RequestBody;
	const int64 StreamSize = Stream->TotalSize();
	RequestBody.SetNum(StreamSize);

	Stream->Seek(0);
	Stream->Serialize(RequestBody.GetData(), StreamSize);

	if (Stream->GetError())
	{
		UE_LOG(LogWit, Error, TEXT("FWitHttpRequest::SetContentFromStream: Error reading from stream."));
		return false;
	}

	// Use the standard SetContent to send the buffer
	SetContent(RequestBody);

	return true;
}