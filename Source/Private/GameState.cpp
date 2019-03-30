#include "GameState.h"
#include "Application.h"

void FGameState::ResetValues()
{
	Score = 0;
	HiScore = 0;
	Level = 1;
	RequiredBallCount = (int)(Level + 4)* 1.5f;
	AdditionalTime = ((Level + 4) * 5) / 2.0f;
	CountdownTime = 1000;
	TotalSurvivedTime = 0.0f;

	bIsGameStarted = false;
	bIsGameOver = false;
	bIsLevelUp = false;
	BallVelocity = 5.0f;
	RestartTimer = 3.0f;
}

void FGameState::ChargeProjectionVelocity()
{
	if (BallVelocity <= 60.0f)
		BallVelocity += DELTA_TIME_STEP * 5.0f;
	else
		BallVelocity = 60.0f;
}

void FGameState::DischargeProjectionVelocity()
{
	if (BallVelocity >= 5.0f)
		BallVelocity -= DELTA_TIME_STEP * 5.0f;
	else
		BallVelocity = 5.0f;
}

bool FGameState::BindApplication(Application * Object)
{
	ContextObject = Object;
	return ContextObject != nullptr;
}

void FGameState::Tick()
{
	// Update scores
	if (Score >= HiScore)
		HiScore = Score;

	// Check Level up
	if(RequiredBallCount <= 0)
	{
		Level += 1;
		RequiredBallCount = (int) (Level + 4)* 1.5f;
		AdditionalTime = ((Level + 4) * 5) / 2.0f;
		CountdownTime += AdditionalTime;
		bIsLevelUp = true;
	}
	else
	{
		bIsLevelUp = false;
	}

	// Check GameOver
	bIsGameOver = (CountdownTime <= 0);
	if (bIsGameOver)
		RestartTimer -= DELTA_TIME_STEP;

	if (RestartTimer < 0.0f)
		ResetValues();

	if (bIsGameStarted)
	{
		CountdownTime -= DELTA_TIME_STEP;
		TotalSurvivedTime += DELTA_TIME_STEP;
	}
}

void FGameState::StartGame()
{
	if (!bIsGameStarted)
		bIsGameStarted = true;
}
