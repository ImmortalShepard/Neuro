#include "NeuroGameMode.h"

#include "NeuroGameInstance.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"

ANeuroGameMode::ANeuroGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	EegChannels = BoardShim::get_eeg_channels(BoardId);
	CachedBrainFlowData.SetNumZeroed(EegChannels.size());
	RawCachedBrainFlowData.SetNumZeroed(EegChannels.size());
}

void ANeuroGameMode::Tick(float DeltaSeconds)
{
	LatestCachedBrainFlowData = false;
	
	Super::Tick(DeltaSeconds);
}

void ANeuroGameMode::BeginDestroy()
{
	Super::BeginDestroy();
	
	StopBoard();
}

bool ANeuroGameMode::GetBrainFlowData(TArray<float>& BrainFlowData)
{
	LatestBrainFlowData();
	BrainFlowData = CachedBrainFlowData;
	return BoardRunning;
}

bool ANeuroGameMode::GetRawBrainFlowData(TArray<float>& RawBrainFlowData)
{
	LatestBrainFlowData();
	RawBrainFlowData = RawCachedBrainFlowData;
	return BoardRunning;
}

int ANeuroGameMode::GetBrainFlowSize()
{
	return EegChannels.size();
}

void ANeuroGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateBoard();
	ConnectBoard();
}

void ANeuroGameMode::CreateBoard()
{
	BoardShim::set_log_file("brainflow.log");
	BoardShim::enable_dev_board_logger();

	BrainFlowInputParams params;
	UGameInstance* gameInstance = GetGameInstance();
	UNeuroGameInstance* neuroGameInstance = Cast<UNeuroGameInstance>(gameInstance);
	if (neuroGameInstance)
	{
		params.mac_address = TCHAR_TO_UTF8(*neuroGameInstance->GetMacAddress());
	}
	else
	{
		params.mac_address = TCHAR_TO_UTF8(*MacAddress);
	}

	BoardPtr = MakeShared<BoardShim>(BoardId, params);

	std::vector<int> channels = BoardShim::get_eeg_channels(BoardId);
	std::vector<std::string> names = BoardShim::get_eeg_names(BoardId);
	FString eegString(TEXT("EEG: channels: "));
	for (int i = 0; i < channels.size(); ++i)
	{
		eegString.AppendInt(channels[i]);
		eegString.Append(FString::Printf(TEXT(":%hs "), names[i].data()));
	}
	eegString.Appendf(TEXT(", sampling rate:%d"), BoardPtr->get_sampling_rate(BoardId));
	UE_LOG(LogTemp, Display, TEXT("%s"), *eegString)
	GEngine->AddOnScreenDebugMessage(0, 20.f, FColor::Yellow, eegString);
}

void ANeuroGameMode::ConnectBoard()
{
	try
	{
		BoardPtr->prepare_session();
		if (BoardPtr->is_prepared())
		{
			GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Green, TEXT("BrainFlow prepared"));
		}
		else
		{
			return;
		}
		BoardPtr->start_stream(BufferSize);
		BoardRunning = true;
		UE_LOG(LogTemp, Display, TEXT("BrainFlow connected"));
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, TEXT("BrainFlow connected"));
	}
	catch (const BrainFlowException& Err)
	{
		UE_LOG(LogTemp, Warning, TEXT("BrainFlow: %hs"), Err.what());
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, FString::Printf(TEXT("BrainFlow: %hs"), Err.what()));

		if (BoardPtr->is_prepared())
		{
			BoardPtr->release_session();
		}
	}
	catch (...)
	{
		UE_LOG(LogTemp, Warning, TEXT("BrainFlow: Unexpected"));
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("BrainFlow: Unexpected"));
	}
}

void ANeuroGameMode::StopBoard()
{
	if (BoardRunning)
	{
		BoardPtr->stop_stream();
		BoardPtr->release_session();
		BoardRunning = false;
	}
}

void ANeuroGameMode::LatestBrainFlowData()
{
	if (BoardRunning)
	{
		if (!LatestCachedBrainFlowData && BoardPtr->get_board_data_count() > 0)
		{
			BrainFlowArray<double, 2> data = BoardPtr->get_board_data();
			int filteredSize;
			for (int index = 0; index < EegChannels.size(); ++index)
			{
				double* downSampledData = DataFilter::perform_downsampling(data.get_address(EegChannels[index]), data.get_size(1), data.get_size(1), static_cast<int>(AggOperations::MEAN), &filteredSize);

				switch (BrainFlowDataFormat)
				{
				case EBrainFlowFormat::Absolute:
					CachedBrainFlowData[index] = FMath::Clamp(static_cast<const float>(downSampledData[0]), -MaxBrainFlowValue, MaxBrainFlowValue) / MaxBrainFlowValue;
					break;
				case EBrainFlowFormat::Relative:
				{
					const float oldData = FMath::Clamp(static_cast<const float>(RawCachedBrainFlowData[index]), -MaxBrainFlowValue, MaxBrainFlowValue) / MaxBrainFlowValue;
					const float newData = FMath::Clamp(static_cast<const float>(downSampledData[0]), -MaxBrainFlowValue, MaxBrainFlowValue) / MaxBrainFlowValue;
					CachedBrainFlowData[index] = FMath::Abs(newData - oldData) / 2;
					break;
				}
				default:
					CachedBrainFlowData[index] = downSampledData[0];
				}
				RawCachedBrainFlowData[index] = downSampledData[0];
				
				delete[] downSampledData;
			}
			LatestCachedBrainFlowData = true;
		}
	}

	
}
