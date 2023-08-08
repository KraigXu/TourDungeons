// Copyright © 2021-2022 GDI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TD_MacroLibrary.generated.h"

class UFunctionLibraryTArray;
typedef UFunctionLibraryTArray FLTArray;

/**
 * TArray扩展函数库
 */
UCLASS()
class UTD_MacroLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	* 插入元素到指定位置，如果在使用TArray自带的插入时，发生了未知bug，可使用此替代
	*/
	template <typename TValue>
	static void Insert(TArray<TValue>& TargetArray, const TValue& Value, size_t Index);

	/**
	* 获取正序数组中第一个大于给定值的元素索引，如果不存在会返回size_t的-1
	*/
	template <typename TValue>
	static size_t FirstGT(TArray<TValue>& TargetArray, const TValue& Value);

	/**
	* 获取正序数组中最后一个小于给定值的元素索引，如果不存在会返回size_t的-1
	*/
	template <typename TValue>
	static size_t LastLT(TArray<TValue>& TargetArray, const TValue& Value);

	template <typename TValue, typename Predicate>
	static bool AllMatchByPredicate(TArray<TValue>& TargetArray, Predicate Pred);
};

template <typename TValue>
void UTD_MacroLibrary::Insert(TArray<TValue>& TargetArray, const TValue& Value, size_t Index)
{
	TArray<TValue> NewArray;
	auto Length = TargetArray.Num();

	if (Index >= Length)
	{
		TargetArray.Emplace(Value);
		return;
	}

	for (size_t i = 0; i < Length + 1; ++i)
	{
		TValue NewValue;
		if (i < Index)
		{
			NewValue = TargetArray[i];
		}
		else if (i == Index)
		{
			NewValue = Value;
		}
		else
		{
			NewValue = TargetArray[i - 1];
		}
		NewArray.Emplace(NewValue);
	}
	TargetArray = NewArray;
}

template <typename TValue>
size_t UTD_MacroLibrary::FirstGT(TArray<TValue>& TargetArray, const TValue& Value)
{
	size_t Low = 0, High = TargetArray.Num(), Mid;
	if (High == 0 || TargetArray[High - 1] <= Value)
		return -1;
	while (Low != High)
		if (TargetArray[Mid = (Low + High) / 2] <= Value)
			Low = Mid + 1;
		else
			High = Mid;
	return Low;
}

template <typename TValue>
size_t UTD_MacroLibrary::LastLT(TArray<TValue>& TargetArray, const TValue& Value)
{
	size_t Low = 0, High = TargetArray.Num(), Mid;
	if (High == 0 || TargetArray[0] >= Value)
		return -1;
	if (High == 1)
		return 0;
	if (High == 2)
		if (TargetArray[1] < Value)
			return 1;
		else
			return 0;
	while (Low != High)
		if (TargetArray[Mid = (Low + High + 1) / 2] >= Value)
			High = Mid - 1;
		else
			Low = Mid;
	return Low;
}

template <typename TValue, typename Predicate>
bool UTD_MacroLibrary::AllMatchByPredicate(TArray<TValue>& TargetArray, Predicate Pred)
{
	for (TValue Value : TargetArray)
		if (Pred(Value) == false)
			return false;
	return true;
}
