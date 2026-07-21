#include "Combat/EmergeBulletDrop.h"

float UEmergeBulletDrop::FlightTime(float Distance, float Velocity)
{
    return Distance / Velocity;
}

float UEmergeBulletDrop::Drop(float Distance, float Velocity, float Gravity)
{
    const float t = Distance / Velocity;
    return 0.5f * Gravity * t * t;
}