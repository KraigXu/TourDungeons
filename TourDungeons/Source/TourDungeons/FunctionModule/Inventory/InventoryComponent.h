// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TourDungeons/Classes/PropItem.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

/**
 *背包系统  管控使用者当前所有物的组件
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

 DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventorySetItem,int32, InInventorySlot, FPropItem ,InInventoryItem);
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventoryNumberUpdate,int32, CurrentNumber, int32, MaxNumber);
 DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryClearItem, int32, InInventorySlot);

public:
    UInventoryComponent();

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Activation")
    FInventorySetItem OnInventorySetItem;
 
    UPROPERTY(BlueprintAssignable, Category = "Inventory|Activation")
    FInventoryNumberUpdate OnInventoryNumberUpdate;

    UPROPERTY(BlueprintAssignable, Category = "Inventory|Activation")
    FInventoryClearItem OnInventoryClearItem;
    
    /**
     * 格子数量
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 SlotsNumber = 20;

    /**
     * 背包上限
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 UpperLimit = 20;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    TArray<FPropItem> Inventory;

    UPROPERTY()
    bool IsOpen;

public:
    UFUNCTION(Server, Reliable)
    virtual void ServerInitInventory(int32 InInventorySize);

    /**
     * 初始化库存数组到指定的大小
     * @param InInventorySize
     */
    UFUNCTION()
    virtual bool InitInventory(int32 InInventorySize);

    /**
     * 返回库存物品的数组
     * @return
     */
    UFUNCTION(BlueprintCallable)
    TArray<FPropItem> GetInventoryItems();

    /**
     * 返回库存指定的物体
     * @param InInventorySlot
     * @return
     */
    UFUNCTION(BlueprintCallable)
    FPropItem GetInventoryItem(int InInventorySlot);

    /**
     * 在目录中设置目录项目
     * @param InInventorySlot
     * @param InInventoryItem
     * @return
     */
    UFUNCTION(BlueprintCallable)
    virtual bool SetInventoryItem(int32 InInventorySlot, FPropItem InInventoryItem);

    /**
     * 从目录中清除目录项
     * @param InInventorySlot
     * @return
     */
    UFUNCTION(BlueprintCallable)
    virtual bool ClearInventoryItem(int32 InInventorySlot);

    /**
     * 返回第一个可用的库存空间
     * @return
     */
    UFUNCTION(BlueprintCallable)
    virtual void GetEmptyInventorySpace(bool& Success, int32& Index);

    /**
     * 返回有效库存项目的计数
     * @return
     */
    UFUNCTION(BlueprintCallable)
    virtual int32 GetInventoryItemCount() const;

    /**
    * 清空库存并将其填满装载的物品
    * @param InInventorySize
    * @param InInventoryItems
    */
    UFUNCTION(BlueprintCallable)
    virtual void LoadInventoryItems(int32 InInventorySize, TArray<FPropItem> InInventoryItems);
 
    /**
     * 返回库存中指定Actor类的有效物品的计数
     * @param InClass 指定的Actor类
     * @param bExactlyEqual 是否完全相等（如果为True，则判断ItemClass是否和InClass完全一样，否则就是判断ItemClass是否是InClass的同类或子类）
     * @return 指定Actor类的有效物品的计数
     */
    UFUNCTION(BlueprintCallable)
    virtual int32 GetInventoryItemCountByClass(TSubclassOf<AActor> InClass, const bool bExactlyEqual = false) const;

    /**
     * 向现有库存添加新的数量
     * @param InAmount
     * @return
     */
    UFUNCTION(BlueprintCallable)
    void IncreaseInventorySize(int32 InAmount);

    /**
     * 从阵列的尾部移除数量
     * @param InAmount
     */
    UFUNCTION(BlueprintCallable)
    void DecreaseInventorySize(int32 InAmount);
 
    /**
     * 服务器:添加金额到项目金额(金额验证应该在调用函数之前完成)
     * @param InventoryItem
     * @param AmountToAdd
     */
    UFUNCTION(BlueprintCallable)
    FPropItem AddToItemAmount(FPropItem InventoryItem, int32 AmountToAdd);

    /**
     * 发送数量变动事件
     */
    UFUNCTION()
    virtual void SendNumberChangeEvent();
 
};
