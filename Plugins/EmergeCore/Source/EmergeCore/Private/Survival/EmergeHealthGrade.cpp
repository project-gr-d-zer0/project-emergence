#include "Survival/EmergeHealthGrade.h"

FString UEmergeHealthGrade::GradeFor(float Health)
{
	if (Health >= 95.0f) return TEXT("OK");
	else if (Health >= 80.0f) return TEXT("Minor");
	else if (Health >= 60.0f) return TEXT("Moderate");
	else if (Health >= 35.0f) return TEXT("Severe");
	else if (Health > 0.0f) return TEXT("Critical");
	else return TEXT("Terminal");
}