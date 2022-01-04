#include "BrainFlowActor.h"
#include "NeuroGameInstance.h"

ABrainFlowActor::ABrainFlowActor()
{
	PrimaryActorTick.bCanEverTick = true;

	EegChannels = BoardShim::get_eeg_channels(BoardId);
	CachedBrainFlowData.SetNumZeroed(EegChannels.size());
}

void ABrainFlowActor::BeginPlay()
{
	Super::BeginPlay();

	CreateBoard();
	ConnectBoard();
}

void ABrainFlowActor::CreateBoard()
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

void ABrainFlowActor::ConnectBoard()
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

void ABrainFlowActor::StopBoard()
{
	if (BoardRunning)
	{
		BoardPtr->stop_stream();
		BoardPtr->release_session();
		BoardRunning = false;
	}
}

void ABrainFlowActor::Tick(float DeltaTime)
{
	LatestCachedBrainFlowData = false;
	Super::Tick(DeltaTime);
}

void ABrainFlowActor::BeginDestroy()
{
	Super::BeginDestroy();
	StopBoard();
}

bool ABrainFlowActor::GetBrainFlowData(TArray<float>& BrainFlowData)
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
				CachedBrainFlowData[index] = MaxBrainFlowValue == 0 ? 0 : FMath::Clamp(static_cast<const float>(downSampledData[0]), -MaxBrainFlowValue, MaxBrainFlowValue) / MaxBrainFlowValue;
				delete[] downSampledData;
			}
			LatestCachedBrainFlowData = true;

			std::vector<std::string> names = BoardShim::get_eeg_names(BoardId);
			FString eegString(TEXT("EEG: channels: "));
			for (int i = 0; i < names.size(); ++i)
			{
				eegString.Append(FString::Printf(TEXT("%hs:%f "), names[i].data(), CachedBrainFlowData[i]));
			}
			UE_LOG(LogTemp, Display, TEXT("%s"), *eegString)
			GEngine->AddOnScreenDebugMessage(1, 20.f, FColor::Yellow, eegString);
		}
		BrainFlowData = CachedBrainFlowData;
	}
	return BoardRunning;
}
