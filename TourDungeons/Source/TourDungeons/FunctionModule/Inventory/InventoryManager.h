// Copyright © 2021-2022 GDI. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentInventoryComponent.h"
#include "InventoryHUDInterface.h"
#include "InventoryInterface.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.h"
#include "InventorySlot.h"
#include "InventoryManager.generated.h"


/**
 *  库存管理器
 *  Server:管理当前所有客户端的物品交互逻辑
 *  Client:管理本地客户端物品与UI的交互逻辑
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UInventoryManager : public UActorComponent, public IInventoryHUDInterface
{
    GENERATED_BODY()

public:
    UInventoryManager();

    /**
     * 玩家本身的装备清单
     */
    UPROPERTY(Replicated)
    UInventoryComponent* PlayerInventory;

    /**
     * 玩家本身的库存清单
     */
    UPROPERTY(Replicated)
    UInventoryComponent* ContainerInventory;

protected:
    IInventoryInterface* CurrentContainer;

    /**
     * 当前界面内所有库存widget
     * InventorySlots只能初始化一次，数据的大小为界面内所有的格子
     * 0-4为装备栏，5-25为背包栏位
     */
    UPROPERTY()
    TArray<UInventorySlot*> InventorySlots;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
#pragma region  通用函数 服务器和客户端关键字表示该函数常在这些场合下调用
 
    UFUNCTION(BlueprintCallable)
    EEquipmentSlotType GetEquipmentTypeBySlot(int32 EquipmentSlot);

    UFUNCTION(BlueprintCallable)
    int32 GetEquipmentSlotByType(EEquipmentSlotType EquipmentSlot);

    UFUNCTION(BlueprintCallable)
    TArray<UInventorySlot*> GetSlotDataArray();

    UFUNCTION(BlueprintCallable)
    UInventorySlot* GetSlotData(int32 Index);

    /**
     * @brief 服务器:返回一个随机旋转的位置和与角色的距离
     * @return
     */
    UFUNCTION(BlueprintCallable)
    FTransform RandomizeDropLocation();

    /**
      *  服务器:初始化库存数组到指定的大小
      * @return
      */
    UFUNCTION(BlueprintCallable)
    void InitInventory();

    /**
     * 设置组件的引用
     * @param InPlayerInventory
     */
    UFUNCTION(BlueprintCallable)
    void InitializeInventoryManager(UEquipmentInventoryComponent* InPlayerInventory);

    /**
      * 服务器:如果有空闲空间，或可用的堆栈空间，尝试将项目添加到库存。
      * 这只能由权威机构调用。它不是一个事件，因为我们可能需要返回一个项目。
      * @param InInventoryComponent
      * @param InventoryItem
      */
    UFUNCTION(BlueprintCallable)
    bool TryToAddItemToInventory(UInventoryComponent* InInventoryComponent, FPropItem InventoryItem);

    /**
     * 服务器：如果有空闲装备空间,或可用的堆栈空间，尝试将项目添加到库存
     *  这只能由权威机构调用。它不是一个事件，因为我们可能需要返回一个项目。
     * @param InInventoryComponent
     * @param InventoryItem
     * @return
     */
    UFUNCTION()
    bool TryToAddEquipToInventory(UInventoryComponent* InInventoryComponent, FPropItem InventoryItem);

    /**
     * 服务器:检查当前容器是否可以存储项目
     * @param Inventory
     */
    UFUNCTION()
    bool CanContainerStoreItems(UInventoryComponent* Inventory);


    /**
     * 服务器:当拾取一个可堆叠的项目时，尝试将其添加到现有的堆栈
     * @param InInventoryComponent
     * @param ItemId
     * @param Amount
     */
    UFUNCTION()
    int32 FindandAddAmounttoStacks(UInventoryComponent* InInventoryComponent, int32 ItemId, int32 Amount);

    /**
     * 服务器:添加项目到库存，然后创建UI项目信息并发送给客户端
     * @param Inventory
     * @param InventorySlot
     * @param InventoryItem
     */
    UFUNCTION()
    void AddItem(UInventoryComponent* Inventory, int32 InventorySlot, FPropItem InventoryItem);

    /**
     * 服务器:处理物品在库存和插槽之间的移动
     * @param FromInventory
     * @param FromInventorySlot
     * @param ToInventory
     * @param ToInventorySlot
     */
    UFUNCTION()
    void MoveItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory,
                  int32 ToInventorySlot);


    /**
     * 服务器:尝试从指定目录中装备一件物品
     * @param FromInventory
     * @param FromInventorySlot
     * @param ToInventory
     * @param ToInventorySlot
     */
    UFUNCTION()
    void EquipItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory,
                   int32 ToInventorySlot);

    /**
     * 服务器:尝试从指定库存中移除装备的物品
     * @param FromInventory
     * @param FromInventorySlot
     * @param ToInventory
     * @param ToInventorySlot
     */
    UFUNCTION()
    void UnEquipItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory,
                     int32 ToInventorySlot);

    /**
     *
     * 服务器:删除库存槽位置
     * @param Inventory
     * @param InventorySlot
     */
    UFUNCTION()
    void RemoveItem(UInventoryComponent* Inventory, int32 InventorySlot);

    /**
     * 客户端:清除库存UI槽位项目信息
     * @param InventorySlot
     */
    UFUNCTION()
    void ClearInventorySlotItem(int32 InventorySlot);

    /**
     * 服务器:添加数量到库存堆栈和返回剩余的数量
     * @param Inventory
     * @param InventorySlot
     * @param AmountToAdd
     * @return
     */
    UFUNCTION()
    int32 AddItemtoStack(UInventoryComponent* Inventory, int32 InventorySlot, int32 AmountToAdd);

    /**
     *服务器:设置一个新的数量
     * @return
     */
    UFUNCTION()
    FPropItem SetItemAmount(FPropItem InventoryItem, int32 Amount);


    /**
     * 服务器:拆分项目移动(类似于MoveItem，但处理拆分堆栈以创建新项目的逻辑)
     * @param FromInventory
     * @param FromInventorySlot
     * @param ToInventory
     * @param ToInventorySlot
     * @param Amount
     */
    UFUNCTION()
    void SplitItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory,
                   int32 ToInventorySlot, int32 Amount);

    /**
     * 服务器:从项目数量中移除指定数量
     * @param InventoryItem
     * @param AmountToRemove
     * @param WasFullAmountRemoverd 返回是否全部移除
     * @param OutInventoryItem  返回新的项目值
     * @return
     */
    UFUNCTION()
    int32 RemoveFromItemAmount(FPropItem InventoryItem, int32 AmountToRemove, bool& WasFullAmountRemoverd,
                               FPropItem& OutInventoryItem);

    /**
     * 服务器:在库存中使用道具。装备，消耗或移动到一个容器(如果存储容器是打开的)
     * @param InventorySlot
     */
    UFUNCTION()
    void UseInventoryItem(int32 InventorySlot);

    // /**
    //  * 服务器:设置所有查看客户端的Container UI Slot Info
    //  * @param ContainerSlot
    //  * @param ItemInfo
    //  */
    // UFUNCTION()
    // void SetViewersContainerSlot(int32 ContainerSlot, FPropSlot ItemInfo);
    //
    // /**
    //  * 客户端:设置容器UI槽项信息
    //  * @param ContainerSlot
    //  * @param InItemInfomation
    //  */
    // UFUNCTION()
    // void SetContainerSlotItem(int32 ContainerSlot, FPropSlot InItemInfomation);
    //

    /**
     * 服务器:使用消耗品
     * @param InInventorySlot
     * @param InInventoryItem
     */
    UFUNCTION()
    void UseConsumableItem(int32 InInventorySlot, FPropItem InInventoryItem);

    /**
     * 服务器:使用装备项目。在使用或移动到容器时装备或拆卸库存物品(如果存储容器是打开的)
     * @param InInventorySlot
     * @param InInventoryItem
     * @param ToInventory
     */
    UFUNCTION()
    void UseEquipmentItem(int32 InInventorySlot, FPropItem InInventoryItem, UInventoryComponent* ToInventory);


    /**
     * 按装备类型返回第一个空装备槽
     * @param EquipmentSlot
     * @param HasEmptySlot
     * @param InventorySlot
     */
    UFUNCTION()
    void GetEmptyEquipmentSlotByType(EEquipmentSlotType EquipmentSlot, bool& HasEmptySlot, int32& InventorySlot);


    /**
     * 服务器:将道具扔到世界中
     * @param InPlayerInventory
     * @param InventorySlot
     */
    UFUNCTION()
    void DropItem(UInventoryComponent* InPlayerInventory, int32 InventorySlot);


    /**
     * 服务器：使用放置类道具
     * @param InInventorySlot
     * @param InInventoryItem
     */
    UFUNCTION()
    void UsePlaceItem(int32 InInventorySlot, FPropItem InInventoryItem);

    /**
     * 服务器：销毁道具
     * @param InPlayerInventory 
     * @param InventorySlot 
     */
    UFUNCTION()
    void DestroyItem(UInventoryComponent* InPlayerInventory, int32 InventorySlot);


#pragma endregion

#pragma region  客户端事件: Client: Inventory Events

    UFUNCTION(Client, Reliable)
    void ClientClearInventorySlotItem(int32 InventorySlot);

    // UFUNCTION(Client, Reliable)
    // void ClientSetContainerSlotItem(int32 ContainerSlot, FPropSlot InItemInfomation);

    /**
     * 客户端:清除所有库存UI插槽项目信息
     */
    UFUNCTION(Client, Reliable)
    void ClearInventorySlotItems();

    /**
     * 客户端:设置库存UI槽位项目信息
     * @param InInventorySize
     * @param InInventoryItem
     */
    UFUNCTION(Client, Reliable)
    void SetInventorySlotItem(int32 InInventorySize, FPropItem InInventoryItem);

    /**
     * 创建玩家库存UI(或重新构建，如果已经加载)
     */
    UFUNCTION(Client, Reliable)
    void LoadInventory();


#pragma endregion

#pragma region  服务器事件: Server: Inventory Events
    //------------------------------服务器函数

    UFUNCTION(Server, Reliable)
    void ServerInitInventory();

    UFUNCTION(Server, Reliable)
    void ServerMoveItem(int32 FromInventorySlot, int32 ToInventorySlot);

    UFUNCTION(Server, Reliable)
    void ServerEquipFromInventory(int32 FromInventorySlot, int32 ToInventorySlot);

    UFUNCTION(Server, Reliable)
    void ServerUnEquipInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot);

    UFUNCTION(Server, Reliable)
    void ServerSplitInventoryItem(int32 FromInventorySlot, int32 ToInventorySlot, int32 Amount);

    UFUNCTION(Server, Reliable)
    void ServerUseInventoryItem(int32 InInventorySlot);

    UFUNCTION(Server, Reliable)
    void ServerDropItemFromInventory(int32 InInventorySlot);

    UFUNCTION(Server,Reliable)
    void ServerDestroyItemFromInventory(int32 InInventorySlot);

    /**
     *服务器:清除客户端库存UI和刷新客户端UI从服务器库存
     */
    UFUNCTION(Server, Reliable)
    void RefreshInventorySlots();

    /**
     * 服务器:更新玩家装备的数据并复制变化
     */
    UFUNCTION(Server, Reliable)
    void UpdateEquippedStats();

    /**
     * 服务器:呼叫装备卸下事件
     */
    UFUNCTION(Server, Reliable)
    void CallUnEquipEvent(UInventoryComponent* FromInventory, FPropItem InventoryItem);

    /**
     * 服务器:呼叫装备装备事件
     */
    UFUNCTION(Server, Reliable)
    void CallEquipEvent(UInventoryComponent* FromInventory, FPropItem InventoryItem);

#pragma endregion
};
