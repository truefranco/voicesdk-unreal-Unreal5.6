/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

class FWitHttpResponse;

/**
 * A custom Wit.ai Http request that wraps a real engine http request to support chunked transfers
 */

 /* No longer inherit from FCurlHttpRequest , so we need to declare all virtual functions from the interface IHttpRequest
  if not we get Abstract function errors */

class FWitHttpRequest : public IHttpRequest
{
public:

	/**
	 * Constructor
	 */
	FWitHttpRequest();

	/**
	 * Destructor.
	 */
	virtual ~FWitHttpRequest() override;

	//~ Begin IHttpBase Interface
	virtual const FString& GetURL() const override;
	virtual FString GetURLParameter(const FString& ParameterName) const override;
	virtual FString GetHeader(const FString& HeaderName) const override;
	virtual TArray<FString> GetAllHeaders() const override;
	virtual FString GetContentType() const override;
	virtual uint64 GetContentLength() const override;
	virtual const TArray<uint8>& GetContent() const override;
	//~ End IHttpBase Interface

	//~ Begin IHttpRequest Interface
	virtual FString GetVerb() const override;
	virtual void SetVerb(const FString& Verb) override;
	virtual void SetURL(const FString& URL) override;
	virtual void SetContent(const TArray<uint8>& ContentPayload) override;
	virtual void SetContent(TArray<uint8>&& ContentPayload) override;
	virtual void SetContentAsString(const FString& ContentString) override;
	
	virtual bool SetContentAsStreamedFile(const FString& Filename) override;
	virtual bool SetResponseBodyReceiveStream(TSharedRef<FArchive> Stream) override;
	virtual void SetHeader(const FString& HeaderName, const FString& HeaderValue) override;
	virtual void AppendToHeader(const FString& HeaderName, const FString& AdditionalHeaderValue) override;
	virtual void SetOption(const FName Option, const FString& OptionValue) override;
	virtual void SetTimeout(float InTimeoutSecs) override;
	virtual void ClearTimeout() override;
	virtual TOptional<float> GetTimeout() const override;
	virtual void SetActivityTimeout(float InTimeoutSecs) override;
	virtual void ResetTimeoutStatus() override;
	virtual bool ProcessRequest() override;
	virtual FHttpRequestCompleteDelegate& OnProcessRequestComplete() override;
	
	virtual FHttpRequestProgressDelegate64& OnRequestProgress64() override;
	virtual FHttpRequestHeaderReceivedDelegate& OnHeaderReceived() override;
	virtual FHttpRequestWillRetryDelegate& OnRequestWillRetry() override;
	virtual FHttpRequestStatusCodeReceivedDelegate& OnStatusCodeReceived() override;
	virtual void CancelRequest() override;
	virtual EHttpRequestStatus::Type GetStatus() const override;
	virtual const FHttpResponsePtr GetResponse() const override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float GetElapsedTime() const override;
	virtual FString GetOption(const FName Option) const override;
	virtual void SetDelegateThreadPolicy(EHttpRequestDelegateThreadPolicy InThreadPolicy) override;
	virtual EHttpRequestDelegateThreadPolicy GetDelegateThreadPolicy() const override;
	virtual void ProcessRequestUntilComplete() override;
	virtual const FString& GetEffectiveURL() const override;
	virtual EHttpFailureReason GetFailureReason() const override;
	//~ End IHttpRequest Interface

	/*
	 * User agent for Wit
	 */
	static FString GetUserAgent();

	/**
	 * Sets the request content from a FArchive stream. This will read the entire stream into a buffer and send it.
	 * This is intended for use with chunked transfer encoding.
	 *
	 * @param Stream The stream to read the content from.
	 * @return True if the content was set successfully.
	 */
	bool SetContentFromStream(TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);

private:

	/** The real Http request that this class is wrapping */
	FHttpRequestPtr RealRequest;

};