// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Connection/BackendPathGetter.h"

FString UBackendPathGetter::GetBackendPathUrl_Implementation() const
{
	return FString("http://127.0.0.1/");
}
