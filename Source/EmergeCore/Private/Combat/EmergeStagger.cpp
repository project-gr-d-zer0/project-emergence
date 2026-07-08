#include "Combat/EmergeStagger.h"

float UEmergeStagger::HitStaggerAdd(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	return HitStaggerAddTuned(FEmergeStaggerTuning(), Damage, Part, Dir);
}

EEmergeStaggerState UEmergeStagger::StateForMeter(float Meter)
{
	return StateForMeterTuned(FEmergeStaggerTuning(), Meter);
}

float UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState State)
{
	return LocomotionSpeedMultiplierTuned(FEmergeStaggerTuning(), State);
}

float UEmergeStagger::MeterAfterRecovery(float Meter, float RecoveryPerSec, float DeltaSeconds)
{
	const float Rec = FMath::Max(0.0f, RecoveryPerSec) * FMath::Max(0.0f, DeltaSeconds);
	return FMath::Max(0.0f, Meter - Rec);
}

float UEmergeStagger::HitStaggerAddTuned(const FEmergeStaggerTuning& Tuning, float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	float PartWeight = Tuning.TorsoWeight;
	switch (Part)
	{
	case EEmergeBodyPart::Head: PartWeight = Tuning.HeadWeight; break;
	case EEmergeBodyPart::Torso: PartWeight = Tuning.TorsoWeight; break;
	case EEmergeBodyPart::Leg: PartWeight = Tuning.LegWeight; break;
	case EEmergeBodyPart::Arm: PartWeight = Tuning.ArmWeight; break;
	default: break;
	}

	float DirFactor = Tuning.FrontFactor;
	switch (Dir)
	{
	case EEmergeHitDir::Front: DirFactor = Tuning.FrontFactor; break;
	case EEmergeHitDir::Opposing: DirFactor = Tuning.OpposingFactor; break;
	case EEmergeHitDir::Behind: DirFactor = Tuning.BehindFactor; break;
	default: break;
	}

	return Damage * PartWeight * DirFactor;
}

EEmergeStaggerState UEmergeStagger::StateForMeterTuned(const FEmergeStaggerTuning& Tuning, float Meter)
{
	if (Meter < Tuning.StumbleThreshold)
	{
		return EEmergeStaggerState::Moving;
	}
	if (Meter < Tuning.StaggerThreshold)
	{
		return EEmergeStaggerState::Stumble;
	}
	if (Meter < Tuning.KnockdownThreshold)
	{
		return EEmergeStaggerState::Stagger;
	}
	return EEmergeStaggerState::Knockdown;
}

float UEmergeStagger::LocomotionSpeedMultiplierTuned(const FEmergeStaggerTuning& Tuning, EEmergeStaggerState State)
{
	switch (State)
	{
	case EEmergeStaggerState::Moving: return 1.0f;
	case EEmergeStaggerState::Crawl: return Tuning.CrawlSpeedMultiplier;
	case EEmergeStaggerState::Stumble:
	case EEmergeStaggerState::Stagger:
	case EEmergeStaggerState::Knockdown:
	case EEmergeStaggerState::Dead:
	default:
		return 0.0f;
	}
}
