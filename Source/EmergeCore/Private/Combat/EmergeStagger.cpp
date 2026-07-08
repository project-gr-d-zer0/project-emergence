#include "Combat/EmergeStagger.h"

float UEmergeStagger::HitStaggerAdd(float Damage, EEmergeBodyPart Part, EEmergeHitDir Dir)
{
	float PartWeight = 1.0f;
	switch (Part)
	{
	case EEmergeBodyPart::Head: PartWeight = 1.5f; break;
	case EEmergeBodyPart::Torso: PartWeight = 1.0f; break;
	case EEmergeBodyPart::Leg: PartWeight = 0.6f; break;
	case EEmergeBodyPart::Arm: PartWeight = 0.4f; break;
	default: break;
	}

	float DirFactor = 1.0f;
	switch (Dir)
	{
	case EEmergeHitDir::Front: DirFactor = 1.0f; break;
	case EEmergeHitDir::Opposing: DirFactor = 1.25f; break;
	case EEmergeHitDir::Behind: DirFactor = 0.75f; break;
	default: break;
	}

	return Damage * PartWeight * DirFactor;
}

EEmergeStaggerState UEmergeStagger::StateForMeter(float Meter)
{
	if (Meter < 40.0f)
	{
		return EEmergeStaggerState::Moving;
	}
	if (Meter < 80.0f)
	{
		return EEmergeStaggerState::Stumble;
	}
	if (Meter < 120.0f)
	{
		return EEmergeStaggerState::Stagger;
	}
	return EEmergeStaggerState::Knockdown;
}

float UEmergeStagger::LocomotionSpeedMultiplier(EEmergeStaggerState State)
{
	switch (State)
	{
	case EEmergeStaggerState::Moving: return 1.0f;
	case EEmergeStaggerState::Crawl: return 0.35f;
	case EEmergeStaggerState::Stumble:
	case EEmergeStaggerState::Stagger:
	case EEmergeStaggerState::Knockdown:
	case EEmergeStaggerState::Dead:
	default:
		return 0.0f;
	}
}
