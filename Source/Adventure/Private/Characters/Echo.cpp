#include "Characters/Echo.h"
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
#include "Components/Combat/CombatComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

AEcho::AEcho()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->AddLocalOffset(FVector(0.f, 0.0f, 48.5f));
    CameraBoom->TargetArmLength = 130.f;
    CameraBoom->SocketOffset = FVector(0.0f, 50.f, 25.0f);
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bInheritRoll = false;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);
    ViewCamera->AddLocalRotation(FRotator(0.0f, -5.0f, 0.0f));

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

    //StimuliSourceComponent =
    //    CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(
    //        TEXT("StimuliSource")
    //    );

    //StimuliSourceComponent->RegisterForSense(
    //    UAISense_Sight::StaticClass()
    //);

    //StimuliSourceComponent->bAutoRegister = true;
};
void AEcho::BeginPlay()
{
    Super::BeginPlay();

    HealthComponent->bHasArmor = true;

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
            AmmoWidget->BindWeapon(CombatComponent->GetEquippedWeapon());
            AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
    if(CrosshairClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(GetWorld(), CrosshairClass);
        if(CrosshairWidget)
        {
            CrosshairWidget->AddToViewport();
            CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
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
    if(HealthComponent)
    {
        HealthComponent->OnDeath.AddDynamic(this, &AEcho::Die);
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
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AEcho::StartAiming);
        EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AEcho::StopAiming);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AEcho::Sprint);
        EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AEcho::StopSprint);
        EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AEcho::Reload);
    }
}
void AEcho::EKeyPressed()
{
    CombatComponent->ToggleWeapon();
}
void AEcho::StartAiming()
{
    if(!CombatComponent) return;
    CombatComponent->StartAiming();
    
    ShowAmmoWidget();
    ShowCrosshairWidget();
}
void AEcho::StopAiming()
{
    if(!CombatComponent) return;
    
    CombatComponent->StopAiming();
    
    HideAmmoWidget();
    HideCrosshairWidget();
}
void AEcho::Sprint()
{
    if(CombatComponent && CombatComponent->GetCombatState() == ECombatState::ECS_Aiming) return;

    SetMovementSpeed(600.f);
}
void AEcho::StopSprint()
{
    SetMovementSpeed(150.f);
}
void AEcho::Fire()
{
    if(CombatComponent)
    {
        CombatComponent->Fire();
    }
}
void AEcho::Reload()
{
    if(CombatComponent)
    {
        CombatComponent->Reload();
    }
}
void AEcho::ShowCrosshairWidget()
{
    if(CrosshairWidget && CombatComponent->GetCombatState() == ECombatState::ECS_Aiming)
    {
        CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEcho::HideCrosshairWidget()
{
    if(CrosshairWidget)
    {
        CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
void AEcho::ShowAmmoWidget()
{
    if(AmmoWidget && CombatComponent->GetCombatState() == ECombatState::ECS_Aiming)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Visible);
    }
}
void AEcho::HideAmmoWidget()
{
    if(AmmoWidget)
    {
        AmmoWidget->SetVisibility(ESlateVisibility::Hidden);
    }
}
void AEcho::Die()
{
    Super::Die();


}
void AEcho::DeathFinished_Implementation()
{
    if (DeathScreenClass)
    {
        DeathScreenWidget = CreateWidget<UUserWidget>(GetWorld(), DeathScreenClass);

        if (DeathScreenWidget)
        {
            DeathScreenWidget->AddToViewport();

            APlayerController* PC = GetWorld()->GetFirstPlayerController();
            if (PC)
            {
                FInputModeGameAndUI InputMode;
                InputMode.SetWidgetToFocus(DeathScreenWidget->TakeWidget());
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
                PC->SetPause(true);
            }
        }
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

    AddControllerYawInput(LookAxisVector.X * LookSensitivity);
    AddControllerPitchInput(LookAxisVector.Y * LookSensitivity);
}
void AEcho::Jump()
{
    Super::Jump();
}