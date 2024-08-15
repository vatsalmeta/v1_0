#include "FileDownloader.h"

AFileDownloader::AFileDownloader()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AFileDownloader::BeginPlay()
{
    Super::BeginPlay();

    DownloadFile();
}

void AFileDownloader::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFileDownloader::DownloadFile()
{
    FString URL = "https://firebasestorage.googleapis.com/v0/b/arcane-text-412011.appspot.com/o/Sweatshirt.usd?alt=media&token=678c5db1-5fa9-4e27-bca4-2e7547630e6c";

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AFileDownloader::OnDownloadComplete);
    Request->SetURL(URL);
    Request->SetVerb("GET");
    Request->SetHeader(TEXT("Alt-Svc:"), TEXT("\"h3=\":443\"; ma=2592000,h3-29=\" : 443\"; ma=2592000"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
    Request->SetHeader(TEXT("User-Agent"), TEXT("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36 Edg/127.0.0.0"));
    Request->SetHeader(TEXT("Accept"), TEXT("*/*"));
    Request->SetHeader(TEXT("Accept-Encoding"), TEXT("gzip, deflate, br"));
    Request->SetHeader(TEXT("Accept-Language"), TEXT("en-US,en;q=0.9"));
    Request->ProcessRequest();

    UE_LOG(LogTemp, Log, TEXT("Download request sent"));
}

void AFileDownloader::OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        int32 ResponseCode = Response->GetResponseCode();
        UE_LOG(LogTemp, Log, TEXT("Response Code: %d"), ResponseCode);

        TArray<uint8> FileData = Response->GetContent();
        UE_LOG(LogTemp, Log, TEXT("Downloaded data size: %d bytes"), FileData.Num());

        FString SavePath = FPaths::ProjectDir() / TEXT("Saved/Sweatshirt.usd");

        if (FFileHelper::SaveArrayToFile(FileData, *SavePath))
        {
            UE_LOG(LogTemp, Log, TEXT("File saved successfully to %s"), *SavePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save file to %s"), *SavePath);
        }
    }
    else
    {
        if (Response.IsValid())
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to download file. Response Code: %d, Message: %s"), Response->GetResponseCode(), *Response->GetContentAsString());
            UE_LOG(LogTemp, Error, TEXT("Response Content: %s"), *Response->GetContentAsString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to download file. No response received."));
        }
    }
}