#pragma once

static void GetCubeSideCaptureAngles(float& yaw, float& pitch, int i)
{
	if (i <= 1)
	{
		yaw = (-i * 2 + 1) * 90.0f;
		pitch = 0.0f;
	}
	else if (i <= 3)
	{
		yaw = 0.0f;
		pitch = ((i - 2) * 2 - 1) * 90.0f;
	}
	else
	{
		yaw = (i % 4) * 180.0f;
		pitch = 0.0f;
	}
}