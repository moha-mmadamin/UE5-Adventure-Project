#include "Characters/Echo.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Characters/CharacterTypes.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

AEcho::AEcho()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 130.f;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);

    Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = FString("head");

    Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = FString("head");

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetMesh()->SetGenerateOverlapEvents(false);

    HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
};
void AEcho::BeginPlay()
{
    Super::BeginPlay();

    if(APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(EchoContex, 0);
        }
    }
    if(AmmoWidgetClass)
    {
        AmmoWidget = CreateWidget<UAmmoWidget>(GetWorld(), AmmoWidgetClass);
        if(AmmoWidget)
        {
            AmmoWidget->AddToViewport();
            AmmoWidget->BindWeapon(EquippedWeapon);
            AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    if(HealthWidgetClass)
    {
        HealthWidget = CreateWidget<UHealthBarWidget>(GetWorld(), HealthWidgetClass);
        if(HealthWidget)
        {
            HealthWidget->AddToViewport();
            HealthWidget->SetHealthComponent(HealthComponent);
        }
    }
}
void AEcho::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
void AEcho::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AEcho::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEcho::Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AEcho::Jump);
        EnhancedInputComponent->BindAction(EkeyAction, ETriggerEvent::Triggered, this, &AEcho::EKeyPressed);
        EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AEcho::Fire);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AEcho::Aim);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AEcho::StopAiming);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AEcho::Sprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AEcho::StopSprint);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AEcho::Reload);
    }
}
void AEcho::EKeyPressed()
{
    AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if(OverlappingWeapon)
    {
        EquipWeapon(OverlappingWeapon);
    }
    else
    {
        if(CanDisarm())
        {
            PlayEquipMontage(FName("Unequip"));
            CharacterState = ECharacterState::ECS_Unequipped;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
        else if(CanArm())
        {
            PlayEquipMontage(FName("Equip"));
            CharacterState = ECharacterState::ECS_EquippedGun;
            ActionState = EActionState::EAS_EquippingWeapon;
        }
    }
}
void AEcho::Fire()
{
    if(!CanFire()) return;

    Super::Fire();
}
void AEcho::Aim()
{
    Super::Aim();

    ActionState = EActionState::EAS_Aiming;
    if(AmmoWidget && CharacterState != ECharacterState::ECS_Unequipped)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEcho::StopAiming()
{
    Super::StopAiming();

    ActionState = EActionState::EAS_Unoccupied;
    if(AmmoWidget)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
void AEcho::Sprint()
{
    if(ActionState == EActionState::EAS_Aiming) return;

    SetMovementSpeed(900.f);
}
void AEcho::StopSprint()
{
    SetMovementSpeed(150.f);
}
bool AEcho::CanFire() const
{
    return CharacterState == ECharacterState::ECS_EquippedGun &&
           ActionState == EActionState::EAS_Aiming &&
           Super::CanFire();
}
bool AEcho::CanReload() const
{
    return CharacterState == ECharacterState::ECS_EquippedGun &&
           Super::CanReload();
}
void AEcho::Reload()
{
    if(!CanReload()) return;
 
    ActionState = EActionState::EAS_Reloading;
    Super::Reload();
}
void AEcho::EquipWeapon(AWeapon* Weapon)
{
    Super::EquipWeapon(Weapon);

    CharacterState = ECharacterState::ECS_EquippedGun;
}
void AEcho::Move(const FInputActionValue& Value)
{
    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);

    const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(ForwardDirection, MovementVector.Y);
    const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(RightDirection, MovementVector.X);
}
void AEcho::Look(const FInputActionValue& Value)
{
    const FVector2D LookAxisVector = Value.Get<FVector2D>();

    AddControllerYawInput(LookAxisVector.X);
    AddControllerPitchInput(LookAxisVector.Y);
}
void AEcho::Jump()
{
    Super::Jump();
}
bool AEcho::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;
}
void AEcho::FinishEquipping_Implementation()
{
    Super::FinishEquipping_Implementation();

    ActionState = EActionState::EAS_Unoccupied;
}
void AEcho::FinishReloading_Implementation()
{
    Super::FinishReloading_Implementation();

    IsAiming ? ActionState = EActionState::EAS_Aiming : ActionState = EActionState::EAS_Unoccupied;
}
bool AEcho::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}