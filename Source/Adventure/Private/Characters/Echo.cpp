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
};
void AEcho::BeginPlay()
{
    Super::BeginPlay();
    SpawnDefaultWeapon();

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
    if(CharacterState != ECharacterState::ECS_EquippedGun || ActionState == EActionState::EAS_Reloading) return;
    //if (!EquippedWeapon->CanShoot()) return;

    if(EquippedWeapon->TryFire())
    {
        PlayFireMontage(FName("Fire"));
    }

    //PlayFireMontage(FName("Fire"));
    //EquippedWeapon->Shoot();
    //if(ActionState == EActionState::EAS_Aiming)
    //{
    //    
    //}

}
void AEcho::Aim()
{
    ActionState = EActionState::EAS_Aiming;
    IsAiming = true;
    if(AmmoWidget && CharacterState != ECharacterState::ECS_Unequipped)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEcho::StopAiming()
{
    ActionState = EActionState::EAS_Unoccupied;
    IsAiming = false;
    if(AmmoWidget)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
void AEcho::Sprint()
{
    if(ActionState == EActionState::EAS_Aiming) return;

    GetCharacterMovement()->MaxWalkSpeed = 900.f;
}
void AEcho::StopSprint()
{
    GetCharacterMovement()->MaxWalkSpeed = 150.f;
}
void AEcho::Reload()
{
    if (CharacterState != ECharacterState::ECS_EquippedGun) return;
    ActionState = EActionState::EAS_Reloading;
    PlayReloadMontage(FName("Reload"));
}
void AEcho::EquipWeapon(AWeapon* Weapon)
{
    Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
    CharacterState = ECharacterState::ECS_EquippedGun;
    OverlappingItem = nullptr;
    EquippedWeapon = Weapon;
}
void AEcho::PlayEquipMontage(const FName& SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && EquipMontage)
    {
        AnimInstance->Montage_Play(EquipMontage);
        AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
    }
}
void AEcho::PlayFireMontage(const FName& SectionName)
{
    if(ActionState != EActionState::EAS_Aiming) return;

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && FireMontage)
    {
        AnimInstance->Montage_Play(FireMontage);
        AnimInstance->Montage_JumpToSection(SectionName, FireMontage);
    }
}
void AEcho::PlayReloadMontage(const FName & SectionName)
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if(AnimInstance && ReloadMontage)
    {
        AnimInstance->Montage_Play(ReloadMontage);
        AnimInstance->Montage_JumpToSection(SectionName, ReloadMontage);
    }
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
void AEcho::Disarm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("PistolSocket"));
    }
}
void AEcho::Arm()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
    }
}
bool AEcho::CanDisarm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;
}
bool AEcho::CanArm()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState == ECharacterState::ECS_Unequipped && EquippedWeapon;
}
void AEcho::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}
void AEcho::FinishReloading()
{
    IsAiming ? ActionState = EActionState::EAS_Aiming : ActionState = EActionState::EAS_Unoccupied;
    EquippedWeapon->ReloadAmmo();
}
void AEcho::SpawnDefaultWeapon()
{
    UWorld* World = GetWorld();
    if(World)
    {
        AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
        EquippedWeapon = DefaultWeapon;
        CharacterState = ECharacterState::ECS_EquippedGun;
    }
}
