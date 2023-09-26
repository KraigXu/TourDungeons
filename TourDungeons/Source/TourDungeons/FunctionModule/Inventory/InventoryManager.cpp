// Copyright © 2021-2022 GDI. All Rights Reserved.

#include "InventoryManager.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UInventoryManager::UInventoryManager()
{
    PrimaryComponentTick.bCanEverTick = true;
}

#pragma region  通用函数 有些表示在常在某些场合下调用


void UInventoryManager::InitInventory()
{
    if (PlayerInventory)
    {
        //初始化Player的背包大小
        PlayerInventory->InitInventory(InventorySlotMax);

        //初始化提供给UI的数据
        InventorySlots.Empty();
        for (auto Val : PlayerInventory->GetInventoryItems())
        {
            UInventorySlot* LocalSlot = NewObject<UInventorySlot>();
            LocalSlot->SetContent(Val);
            InventorySlots.Add(LocalSlot);
        }
    }
    else
    {
        //
        UE_LOG(LogTemp, Error, TEXT("初始化背包大小时出现错误"));
    }
}

EEquipmentSlotType UInventoryManager::GetEquipmentTypeBySlot(int32 EquipmentSlot)
{
    if (EquipmentSlot == 0)
    {
        return EEquipmentSlotType::Head;
    }
    if (EquipmentSlot == 1)
    {
        return EEquipmentSlotType::Chest;
    }
    if (EquipmentSlot == 2)
    {
        return EEquipmentSlotType::Hands;
    }
    if (EquipmentSlot == 3)
    {
        return EEquipmentSlotType::Feet;
    }
    if (EquipmentSlot == 4)
    {
        return EEquipmentSlotType::MainHand;
    }
    if (EquipmentSlot == 5)
    {
        return EEquipmentSlotType::OffHand;
    }
    return EEquipmentSlotType::Head;
}

int32 UInventoryManager::GetEquipmentSlotByType(EEquipmentSlotType EquipmentSlot)
{
    EEquipmentSlotType LocalSlotType = EquipmentSlot;

    int32 LocalInventorySlot = 0;

    for (int i = 0; i < EquipmentSlots; i++)
    {
        LocalInventorySlot = i;
        if (LocalSlotType == GetEquipmentTypeBySlot(LocalInventorySlot))
        {
            break;
        }
    }
    return LocalInventorySlot;
}

TArray<UInventorySlot*> UInventoryManager::GetSlotDataArray()
{
    return InventorySlots;
}

UInventorySlot* UInventoryManager::GetSlotData(int32 Index)
{
    return InventorySlots[Index];
}

FTransform UInventoryManager::RandomizeDropLocation()
{
    AController* LocalController = Cast<AController>(GetOwner());

    FVector LocalDir = (LocalController->GetCharacter()->GetActorForwardVector() * 50 + FVector(0, 0, 100)) + LocalController->GetCharacter()->GetActorLocation();
    FHitResult HitResults;
    TArray<AActor*> IgnoreCheckTarget;
    IgnoreCheckTarget.Add(LocalController->GetCharacter());
    UKismetSystemLibrary::LineTraceSingle(this,
                                          LocalDir, LocalDir - FVector(0, 0, 1000),
                                          TraceTypeQuery1, false,
                                          IgnoreCheckTarget, EDrawDebugTrace::Type::None,
                                          HitResults, true
    );

    return FTransform(FRotator(0, 0, 0), HitResults.Location, FVector::OneVector);
}


void UInventoryManager::ClearInventorySlotItem(int32 InventorySlot)
{
    PlayerInventory->SetInventoryItem(InventorySlot, FPropItem());
    PlayerInventory->SendNumberChangeEvent();

    if (InventorySlots.Num() > InventorySlot)
    {
        if (InventorySlots[InventorySlot])
        {
            //赋值
            InventorySlots[InventorySlot]->SetContent(FPropItem());
        }
    }
}


void UInventoryManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryManager, PlayerInventory);
    DOREPLIFETIME(UInventoryManager, ContainerInventory);
}

void UInventoryManager::InitializeInventoryManager(UEquipmentInventoryComponent* InPlayerInventory)
{
    PlayerInventory = InPlayerInventory;
}

bool UInventoryManager::TryToAddItemToInventory(UInventoryComponent* InInventoryComponent, FPropItem InventoryItem)
{
    if (InventoryItem.bIsStackable)
    {
        //如果是堆叠物品
        int32 LocalItemAmount = FindandAddAmounttoStacks(InInventoryComponent, InventoryItem.ID, InventoryItem.Amount);

        //当LocalItemAmount不等于0的时候，不存在或已经有堆叠上限的物品或溢出数据
        if (LocalItemAmount > 0)
        {
            FPropItem LocalInventoryItem = InventoryItem;
            LocalInventoryItem.Amount = LocalItemAmount;
            bool bSuccess;
            int32 LocalIndex;
            InInventoryComponent->GetEmptyInventorySpace(bSuccess, LocalIndex);
            if (bSuccess)
            {
                AddItem(InInventoryComponent, LocalIndex, LocalInventoryItem);
                return true;
            }
            //TODO:包裹满了
            GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, FString::Printf(TEXT("无法拾取，背包已满")));
        }
        return false;
    }
    FPropItem LocalInventoryItem = InventoryItem;
    bool bSuccess;
    int32 LocalIndex;
    InInventoryComponent->GetEmptyInventorySpace(bSuccess, LocalIndex);
    if (bSuccess)
    {
        AddItem(InInventoryComponent, LocalIndex, LocalInventoryItem);
        return true;
    }
    //TODO:包裹满了
    GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, FString::Printf(TEXT("无法拾取，背包已满")));
    return true;
}

bool UInventoryManager::TryToAddEquipToInventory(UInventoryComponent* InInventoryComponent, FPropItem InventoryItem)
{
    if (InventoryItem.ItemType == EInventoryItemType::Equipment)
    {
        FPropItem LocalInventoryItem = InventoryItem;

        //判断指定装备槽是不是空的
        bool bLocalHasEmptySlot = false;
        int32 LocalEquipmentSlot = 0;
        GetEmptyEquipmentSlotByType(LocalInventoryItem.EquipSlot, bLocalHasEmptySlot, LocalEquipmentSlot);

        if (bLocalHasEmptySlot)
        {
            //如果是空的就直接装备
            AddItem(InInventoryComponent, LocalEquipmentSlot, InventoryItem);
            CallEquipEvent(InInventoryComponent, InventoryItem);
        }
        else
        {
            //如果不是空的就将装备卸下转移装备
            //获取空余位置
            bool bLocalSuccess;
            int32 LocalIndex;
            InInventoryComponent->GetEmptyInventorySpace(bLocalSuccess, LocalIndex);

            if (bLocalSuccess)
            {
                LocalEquipmentSlot = GetEquipmentSlotByType(LocalInventoryItem.EquipSlot);

                UnEquipItem(PlayerInventory, LocalEquipmentSlot, PlayerInventory, LocalIndex);

                //装备物体
                AddItem(InInventoryComponent, LocalEquipmentSlot, InventoryItem);
                CallEquipEvent(InInventoryComponent, InventoryItem);
            }
            else
            {
                //TODO:包裹满了
                GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Black, FString::Printf(TEXT("无法拾取，背包已满")));
                return false;
            }
        }
        return true;
    }
    return false;
}

bool UInventoryManager::CanContainerStoreItems(UInventoryComponent* Inventory)
{
    return true;
}

int32 UInventoryManager::FindandAddAmounttoStacks(UInventoryComponent* InInventoryComponent, int32 ItemId, int32 Amount)
{
    int32 LocalAmount = Amount;
    TArray<FPropItem> LocalInventoryItems = InInventoryComponent->GetInventoryItems();

    for (int i = 0; i < LocalInventoryItems.Num(); i++)
    {
        if (LocalInventoryItems[i].ID == ItemId && LocalInventoryItems[i].bIsStackable)
        {
            //TODO:暂无堆叠物品

            return true;
        }
    }
    return LocalAmount;
}

void UInventoryManager::AddItem(UInventoryComponent* Inventory, int32 InventorySlot, FPropItem InventoryItem)
{
    Inventory->SetInventoryItem(InventorySlot, InventoryItem);

    //是不是在更新玩家库存
    if (PlayerInventory == Inventory)
    {
        //TODO:未更新客户端背包？
        //客户端:更新HUD库存槽位信息
        SetInventorySlotItem(InventorySlot, InventoryItem);
    }
    else
    {
        //服务器:更新所有查看器的HUD容器槽位信息
       // FPropSlot LocalItemInfo = InventoryItemCastSlot(InventoryItem);
        //SetViewersContainerSlot(InventorySlot, LocalItemInfo);
    }
}

void UInventoryManager::MoveItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory, int32 ToInventorySlot)
{
    if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
    {
        return;
    }
    if (CanContainerStoreItems(FromInventory))
    {
        FPropItem FromItem = FromInventory->GetInventoryItem(FromInventorySlot);
        if (FromItem.ItemType == EInventoryItemType::Currency)
        {
            //TODO:暂无数量道具相关
        }
        else
        {
            FPropItem ToItem = ToInventory->GetInventoryItem(ToInventorySlot);
            if (ToItem.IsValid())
            {
                if (ToItem.bIsStackable && ToItem.ItemFreshStackSpace() > 0 && FromItem.ItemIsSame(ToItem))
                {
                    //堆叠
                    const int LocalResult = AddItemtoStack(ToInventory, ToInventorySlot, FromItem.Amount);
                    if (LocalResult > 0)
                    {
                        FPropItem NewItem = SetItemAmount(FromItem, LocalResult);
                        AddItem(FromInventory, FromInventorySlot, NewItem);
                    }
                    else
                    {
                        //清除旧的物品槽
                        RemoveItem(FromInventory, FromInventorySlot);
                    }
                }
                else
                {
                    //交换物品
                    if (CanContainerStoreItems(FromInventory))
                    {
                        AddItem(ToInventory, ToInventorySlot, FromItem);
                        AddItem(FromInventory, FromInventorySlot, ToItem);
                    }
                    else
                    {
                        //无法交换
                    }
                }
            }
            else
            {
                //向空位置转移道具
                AddItem(ToInventory, ToInventorySlot, FromItem);
                RemoveItem(FromInventory, FromInventorySlot);
            }
        }
    }
}

void UInventoryManager::EquipItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory, int32 ToInventorySlot)
{
    if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
    {
        return;
    }
    FPropItem InventoryItem = FromInventory->GetInventoryItem(FromInventorySlot);
    if (InventoryItem.ItemType == EInventoryItemType::Equipment)
    {
        if (GetEquipmentTypeBySlot(ToInventorySlot) == InventoryItem.EquipSlot)
        {
            FPropItem LocalToInventoryItem = ToInventory->GetInventoryItem(ToInventorySlot);
            if (LocalToInventoryItem.IsValid())
            {
                //交换物品
                if (CanContainerStoreItems(FromInventory))
                {
                    AddItem(FromInventory, FromInventorySlot, LocalToInventoryItem);
                    CallUnEquipEvent(FromInventory, LocalToInventoryItem);

                    AddItem(ToInventory, ToInventorySlot, InventoryItem);
                    CallEquipEvent(ToInventory, InventoryItem);

                    UpdateEquippedStats();
                }
            }
            else
            {
                AddItem(ToInventory, ToInventorySlot, InventoryItem);
                CallEquipEvent(ToInventory, InventoryItem);

                RemoveItem(FromInventory, FromInventorySlot);
                UpdateEquippedStats();
            }
        }
        else
        {
            //无法放入 槽位类型不符
        }
    }
    else
    {
        //非装备项进入不做处理
    }
}

void UInventoryManager::UnEquipItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory, int32 ToInventorySlot)
{
    if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
    {
        return;
    }
    FPropItem FromItem = FromInventory->GetInventoryItem(FromInventorySlot);
    EEquipmentSlotType LocalEquipmentSlotType = GetEquipmentTypeBySlot(ToInventorySlot);
    FPropItem ToItem = ToInventory->GetInventoryItem(ToInventorySlot);
    if (ToItem.IsValid())
    {
        //交换物品
        if (CanContainerStoreItems(ToInventory))
        {
            if (ToItem.ItemType == EInventoryItemType::Equipment)
            {
                if (ToItem.EquipmentType == FromItem.EquipmentType)
                {
                    AddItem(ToInventory, ToInventorySlot, FromItem);
                    CallUnEquipEvent(ToInventory, FromItem);
                    AddItem(FromInventory, FromInventorySlot, ToItem);
                    CallEquipEvent(FromInventory, ToItem);
                    UpdateEquippedStats();
                }
                else
                {
                    //不是一样的装备无法交换
                }
            }
            else
            {
                //不是装备无法交换
            }
        }
        else
        {
            //无法交换
        }
    }
    else
    {
        //是否是向另外一个装备库转移装备
        if (Cast<UEquipmentInventoryComponent>(ToInventory))
        {
            if (ToInventorySlot < EquipmentSlots)
            {
                if (GetEquipmentTypeBySlot(ToInventorySlot) != GetEquipmentTypeBySlot(FromInventorySlot))
                {
                    //装备类型不符
                    return;
                }
            }
        }

        AddItem(ToInventory, ToInventorySlot, FromItem);
        CallUnEquipEvent(ToInventory, FromItem);
        RemoveItem(FromInventory, FromInventorySlot);
        UpdateEquippedStats();
    }
}

void UInventoryManager::RemoveItem(UInventoryComponent* Inventory, int InventorySlot)
{
    Inventory->ClearInventoryItem(InventorySlot);

    //是否是在更新玩家信息
    if (PlayerInventory == Inventory)
    {
        //清理客户端指定库存信息
        ClientClearInventorySlotItem(InventorySlot);
    }
    else
    {
    }
}


int32 UInventoryManager::AddItemtoStack(UInventoryComponent* Inventory, int32 InventorySlot, int32 AmountToAdd)
{
    int32 LocalAmountToAdd = AmountToAdd;
    int32 LocalRemainingAmount = AmountToAdd;

    FPropItem LocalInventoryItem = Inventory->GetInventoryItem(InventorySlot);

    if (LocalInventoryItem.Amount < LocalInventoryItem.MaxStackSize)
    {
        int32 LocalResult = (LocalInventoryItem.MaxStackSize - LocalInventoryItem.Amount);
        if (LocalResult <= LocalAmountToAdd)
        {
            LocalRemainingAmount = 0;
            FPropItem LocalItem = Inventory->AddToItemAmount(LocalInventoryItem, LocalAmountToAdd);
            AddItem(Inventory, InventorySlot, LocalItem);
            return LocalRemainingAmount;
        }
        LocalRemainingAmount = LocalAmountToAdd - LocalResult;
        FPropItem LocalItem = Inventory->AddToItemAmount(LocalInventoryItem, LocalResult);
        AddItem(Inventory, InventorySlot, LocalItem);
        return LocalRemainingAmount;
    }
    return LocalRemainingAmount;
}

FPropItem UInventoryManager::SetItemAmount(FPropItem InventoryItem, int32 Amount)
{
    InventoryItem.Amount = Amount;
    return InventoryItem;
}

void UInventoryManager::SplitItem(UInventoryComponent* FromInventory, int32 FromInventorySlot, UInventoryComponent* ToInventory, int32 ToInventorySlot, int32 Amount)
{
    if (FromInventory == ToInventory && FromInventorySlot == ToInventorySlot)
    {
        return;
    }
    int32 LocalSplitAmount = Amount;
    //目标库存是否能够存储道具?
    if (CanContainerStoreItems(ToInventory))
    {
        FPropItem LocalFromItem = FromInventory->GetInventoryItem(FromInventorySlot);
        if (LocalFromItem.Amount == LocalSplitAmount || LocalFromItem.bIsStackable == false)
        {
            //不能堆叠的情况下进行移动道具
            MoveItem(FromInventory, FromInventorySlot, ToInventory, ToInventorySlot);
        }
        else
        {
            FPropItem LocalToItem = ToInventory->GetInventoryItem(ToInventorySlot);
            //我们是否正在尝试将分裂添加到一个项目?
            if (LocalToItem.IsValid())
            {
                //检查To Item是否可堆叠
                if (LocalToItem.ItemIsSame(LocalFromItem) && LocalToItem.bIsStackable && LocalToItem.ItemFreshStackSpace() > 0)
                {
                    //如果堆叠数量没有满
                    int32 LocalResult = AddItemtoStack(ToInventory, ToInventorySlot, LocalSplitAmount);
                    bool LocalIsFullRemoved;
                    FPropItem LocalResultItem;
                    RemoveFromItemAmount(LocalFromItem, LocalSplitAmount - LocalResult, LocalIsFullRemoved, LocalResultItem);

                    if (LocalIsFullRemoved)
                    {
                        //删除旧项目
                        RemoveItem(FromInventory, FromInventorySlot);
                    }
                    else
                    {
                        //更新旧的数量
                        AddItem(FromInventory, FromInventorySlot, LocalResultItem);
                    }
                }
                else
                {
                    //不该出现
                    GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("拆分堆栈时不能交换项目")));
                }
            }
            else
            {
                bool LocalIsFullRemoved;
                FPropItem LocalResultItem;
                int32 LocalAmountRemoved = RemoveFromItemAmount(LocalFromItem, LocalSplitAmount, LocalIsFullRemoved, LocalResultItem);

                FPropItem LocalNewItem = SetItemAmount(LocalFromItem, LocalAmountRemoved);

                AddItem(ToInventory, ToInventorySlot, LocalNewItem);
                //拆分并移动了整个项目?
                if (LocalIsFullRemoved)
                {
                    // 删除旧项目
                    RemoveItem(FromInventory, FromInventorySlot);
                }
                else
                {
                    //更新旧的数量
                    AddItem(FromInventory, FromInventorySlot, LocalResultItem);
                }
            }
        }
    }
}

int32 UInventoryManager::RemoveFromItemAmount(FPropItem InventoryItem, int32 AmountToRemove, bool& WasFullAmountRemoverd, FPropItem& OutInventoryItem)
{
    if (InventoryItem.Amount >= AmountToRemove)
    {
        WasFullAmountRemoverd = true;
        OutInventoryItem = FPropItem();
        return InventoryItem.Amount;
    }
    OutInventoryItem = SetItemAmount(InventoryItem, InventoryItem.Amount - AmountToRemove);
    WasFullAmountRemoverd = false;
    return AmountToRemove;
}

void UInventoryManager::UseInventoryItem(int32 InInventorySlot)
{
    int32 LocalInventorySlot = InInventorySlot;
    FPropItem LocalInventoryItem = PlayerInventory->GetInventoryItem(LocalInventorySlot);

    //判断是否是容器
    if (CurrentContainer)
    {
        // 判断存储容器是否是打开的 TODO：暂时没有转移操作
        if (CurrentContainer->GetCanStoreItems())
        {
            //TODO:所有的物体类型目前皆为0，只做转移位置操作
            if (LocalInventoryItem.ItemType == EInventoryItemType::Miscellaneous)
            {
                // ContainerInventory
            }
            else if (LocalInventoryItem.ItemType == EInventoryItemType::Equipment)
            {
            }
        }
    }
    else
    {
        //如果不是容器，则从库存中使用道具
        switch (LocalInventoryItem.ItemType)
        {
            case EInventoryItemType::Miscellaneous:

                break;
            case EInventoryItemType::Equipment:
                //使用装备型道具
                UseEquipmentItem(LocalInventorySlot, LocalInventoryItem, PlayerInventory);
                break;
            case EInventoryItemType::Consumable:
                //使用消耗型道具
                UseConsumableItem(LocalInventorySlot, LocalInventoryItem);
                break;
            case EInventoryItemType::Currency:
                break;
            case EInventoryItemType::Place:
                //使用放置类道具
                UsePlaceItem(LocalInventorySlot, LocalInventoryItem);
                break;
            default: ;
        }
    }
}


void UInventoryManager::UseConsumableItem(int32 InInventorySlot, FPropItem InInventoryItem)
{
    //TODO:暂无消耗类型道具逻辑
}

void UInventoryManager::UseEquipmentItem(int32 InInventorySlot, FPropItem InInventoryItem, UInventoryComponent* ToInventory)
{
    int32 LocalInventorySlot = InInventorySlot;
    FPropItem LocalInventoryItem = InInventoryItem;
    UInventoryComponent* LocalInventoryReference = ToInventory;

    //是否在卸下装备
    if (InInventorySlot < EquipmentSlots)
    {
        //当在卸下装备时
        //获取空余位置
        bool bLocalSuccess;
        int32 LocalIndex;
        LocalInventoryReference->GetEmptyInventorySpace(bLocalSuccess, LocalIndex);
        if (bLocalSuccess)
        {
            //卸下装备
            UnEquipItem(PlayerInventory, LocalInventorySlot, LocalInventoryReference, LocalIndex);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("背包没有多余位置了"));
        }
    }
    else
    {
        //当在装备物体是时
        bool bLocalHasEmptySlot = false;
        int32 LocalEquipmentSlot = 0;
        GetEmptyEquipmentSlotByType(LocalInventoryItem.EquipSlot, bLocalHasEmptySlot, LocalEquipmentSlot);
        if (bLocalHasEmptySlot)
        {
            ServerEquipFromInventory(LocalInventorySlot, LocalEquipmentSlot);
        }
        else
        {
            LocalEquipmentSlot = GetEquipmentSlotByType(LocalInventoryItem.EquipSlot);
            ServerEquipFromInventory(LocalInventorySlot, LocalEquipmentSlot);
        }
    }
}

void UInventoryManager::UsePlaceItem(int32 InInventorySlot, FPropItem InInventoryItem)
{
    FPropItem LocalInventoryItem = InInventoryItem;
    //放置道具
    if (IInventoryHUDInterface* InventoryUI = Cast<IInventoryHUDInterface>(GetOwner()))
    {
        //放置道具
        InventoryUI->UIPlaceConstructionItem(LocalInventoryItem.ItemClass);
    }
}

void UInventoryManager::DestroyItem(UInventoryComponent* InPlayerInventory, int32 InventorySlot)
{
    UInventoryComponent* LocalInventory = InPlayerInventory;
    int32 LocalInventorySlot = InventorySlot;

    FPropItem LocalInventoryItem = LocalInventory->GetInventoryItem(LocalInventorySlot);

    if (LocalInventoryItem.IsDroppable())
    {
        if (LocalInventorySlot < EquipmentSlots)
        {
            UpdateEquippedStats();
            CallUnEquipEvent(LocalInventory, LocalInventoryItem);
        }
        RemoveItem(LocalInventory, LocalInventorySlot);
    }
    else
    {
        RemoveItem(LocalInventory, LocalInventorySlot);
    }
}


void UInventoryManager::GetEmptyEquipmentSlotByType(EEquipmentSlotType EquipmentSlot, bool& HasEmptySlot, int32& InventorySlot)
{
    EEquipmentSlotType LocalSlotType = EquipmentSlot;
    int32 LocalLength = EquipmentSlots;
    bool bLocalHasEmptySlot = false;
    int32 LocalInventorySlot = 0;
    for (int i = 0; i < LocalLength; i++)
    {
        LocalInventorySlot = i;
        FPropItem LocalItem = PlayerInventory->GetInventoryItem(LocalInventorySlot);
        //这个类型的装备槽是不是空的
        if (!LocalItem.IsValid() && LocalSlotType == GetEquipmentTypeBySlot(LocalInventorySlot))
        {
            bLocalHasEmptySlot = true;
            break;
        }
    }

    HasEmptySlot = bLocalHasEmptySlot;
    InventorySlot = LocalInventorySlot;
}

void UInventoryManager::DropItem(UInventoryComponent* InPlayerInventory, int32 InventorySlot)
{
    UInventoryComponent* LocalInventory = InPlayerInventory;
    int32 LocalInventorySlot = InventorySlot;
    FPropItem LocalInventoryItem = LocalInventory->GetInventoryItem(LocalInventorySlot);
    if (LocalInventoryItem.IsDroppable())
    {
        if (LocalInventorySlot < EquipmentSlots)
        {
            UpdateEquippedStats();
            CallUnEquipEvent(LocalInventory, LocalInventoryItem);
        }
        RemoveItem(LocalInventory, LocalInventorySlot);
    }
    else
    {
        RemoveItem(LocalInventory, LocalInventorySlot);
    }
}


void UInventoryManager::CallUnEquipEvent_Implementation(UInventoryComponent* FromInventory,
                                                        FPropItem InventoryItem)
{
    APlayerController* LocalPlayerController = Cast<APlayerController>(FromInventory->GetOwner());
    // if (APickableBase* LocalPickable = APickableBase::GetPickableById(LocalPlayerController, InventoryItem.ID))
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("%s (ID:%d) 被 %s 卸下了"), *LocalPickable->GetItemName().ToString(), InventoryItem.ID, *LocalPlayerController->GetName());
    //     LocalPickable->MulticastUnequip(LocalPlayerController->GetPawn());
    // }
}

void UInventoryManager::CallEquipEvent_Implementation(UInventoryComponent* FromInventory, FPropItem InventoryItem)
{
    APlayerController* LocalPlayerController = Cast<APlayerController>(FromInventory->GetOwner());
    //
    // if (APickableBase* LocalPickable = APickableBase::GetPickableById(LocalPlayerController, InventoryItem.ID))
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("%s (ID:%d) 被 %s 装备了"), *LocalPickable->GetItemName().ToString(), InventoryItem.ID, *LocalPlayerController->GetName());
    //     LocalPickable->MulticastEquip(LocalPlayerController->GetPawn());
    // }
}

// void UInventoryManager::SetViewersContainerSlot(int32 ContainerSlot, FPropSlot ItemInfo)
// {
//     //TODO：暂无容器多人查看功能 如：两名玩家同时看一个容器内的逻辑
// }
//
// void UInventoryManager::SetContainerSlotItem(int32 ContainerSlot, FPropSlot InItemInfomation)
// {
// }


#pragma endregion

#pragma region  客户端调用函数

void UInventoryManager::LoadInventory_Implementation()
{
}


void UInventoryManager::ClientClearInventorySlotItem_Implementation(int32 InventorySlot)
{
    ClearInventorySlotItem(InventorySlot);
}
//
// void UInventoryManager::ClientSetContainerSlotItem_Implementation(int32 ContainerSlot, FPropSlot InItemInfomation)
// {
//     SetContainerSlotItem(ContainerSlot, InItemInfomation);
// }

void UInventoryManager::ClearInventorySlotItems_Implementation()
{
    for (auto item : InventorySlots)
    {
        //清空背包
        item->SetContent(FPropItem());
    }
    PlayerInventory->SendNumberChangeEvent();
}

void UInventoryManager::SetInventorySlotItem_Implementation(int32 InInventorySlot, FPropItem InInventoryItem)
{
    //当库存UI格子数量小于要增加时不给UI增加 但依然给库存容量变动
    PlayerInventory->SetInventoryItem(InInventorySlot, InInventoryItem);
    PlayerInventory->SendNumberChangeEvent();

    if (InventorySlots.Num() > InInventorySlot)
    {
        if (InventorySlots[InInventorySlot])
        {
            //赋值
            InventorySlots[InInventorySlot]->SetContent(InInventoryItem);
        }
    }
}


#pragma endregion

#pragma region  服务器调用函数

void UInventoryManager::ServerInitInventory_Implementation()
{
    InitInventory();
}


void UInventoryManager::RefreshInventorySlots_Implementation()
{
    ClearInventorySlotItems();

    int i = 0;
    for (auto item : PlayerInventory->GetInventoryItems())
    {
        if (item.IsValid())
        {
            //转换数据并传入
            SetInventorySlotItem(i, item);
        }
        i++;
    }
    UpdateEquippedStats();
}


void UInventoryManager::UpdateEquippedStats_Implementation()
{
    //TODO：暂不实现装备属性的复制与更新
    //更新玩家装备属性并复制更改
}


void UInventoryManager::ServerEquipFromInventory_Implementation(int32 FromInventorySlot, int32 ToInventorySlot)
{
    EquipItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManager::ServerUnEquipInventoryItem_Implementation(int32 FromInventorySlot, int32 ToInventorySlot)
{
    UnEquipItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManager::ServerMoveItem_Implementation(int32 FromInventorySlot, int32 ToInventorySlot)
{
    MoveItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot);
}

void UInventoryManager::ServerSplitInventoryItem_Implementation(int32 FromInventorySlot, int32 ToInventorySlot, int32 Amount)
{
    SplitItem(PlayerInventory, FromInventorySlot, PlayerInventory, ToInventorySlot, Amount);
}

void UInventoryManager::ServerUseInventoryItem_Implementation(int32 InInventorySlot)
{
    UseInventoryItem(InInventorySlot);
}

void UInventoryManager::ServerDropItemFromInventory_Implementation(int32 InInventorySlot)
{
    DropItem(PlayerInventory, InInventorySlot);
}

void UInventoryManager::ServerDestroyItemFromInventory_Implementation(int32 InInventorySlot)
{
    DestroyItem(PlayerInventory, InInventorySlot);
}

#pragma endregion
