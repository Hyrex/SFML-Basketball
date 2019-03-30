#include "TextRenderer.h"
#include "Application.h"

bool FTextRenderer::BindApplication(Application* Object)
{
	ContextObject = Object;
	return ContextObject != nullptr;
}

void FTextRenderer::Tick()
{
	for (auto& i : Texts)
	{
		// Lerps
		if (i->FadeTime > 0.0f && i->bIsActive)
		{
			const float LerpDelta = i->LifeTime / i->FadeTime;
			const SFML::Vector2f PositionDiff = i->EndLocation - i->StartLocation;
			const SFML::Vector2f ResultPosition = i->StartLocation + (PositionDiff * LerpDelta);
			i->Text.setPosition(ResultPosition);
			LOG_CMD(ResultPosition.x);
		

			i->LifeTime += DELTA_TIME_STEP;

			if (i->LifeTime >= i->FadeTime)
			{
				i->bIsActive = false;
			}
		}
	}
}
