#include "IWindow.h"
#include "Time.hpp"
#include <float.h>
double ClampElapsed(double elapsed)
{
	if (elapsed > 1.0)
		return 1.0;
	if (elapsed < 0.0)
		return 0.0;
	return elapsed;
}

#include <chrono>
std::chrono::high_resolution_clock::time_point applicationstartown = std::chrono::high_resolution_clock::now();
double __timer_of_start_point()
{
	return double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - applicationstartown).count()) / 1000000.0;
}

void IWindow::RunLoops(int tickrate, int framerate)
{
	double updateRate = 1.0 / 60.0;
	double renderRate = 1.0 / 60.0;

	double lastFrameRender = __timer_of_start_point();
	double lastFrameUpdate = __timer_of_start_point();

	while (OnProcess())
	{
		const double curr = __timer_of_start_point() - lastFrameUpdate;
		if(curr >= updateRate)
		{
			lastFrameUpdate = __timer_of_start_point();
			perSecondPx = 1.0 / curr;
			OnUpdate(curr);
			pxTime = __timer_of_start_point() - lastFrameRender;
		}
		
		const double curr2 = __timer_of_start_point() - lastFrameRender;
		if (curr2 >= renderRate)
		{
			lastFrameRender = __timer_of_start_point();
			perSecondRd = 1.0 / curr;
			OnDraw();
			rdTime = __timer_of_start_point() - lastFrameRender;
		}
	
	}

	/*

	double update_time = 0.0;
	double render_time = 0.0;
	double update_timestamp = 0.0;
	double render_timestamp = 0.0;
	double update_epsilon = 0.0;
	float target_update_period = 0.0;
	float target_render_period = 0.0;

	if (tickrate == 0)
		target_update_period = 0.0;
	else
		target_update_period = 1.0f / float(tickrate);

	if (framerate == 0)
		target_render_period = 0.0;
	else
		target_render_period = 1.0f / float(framerate);

	bool is_running_slowly = false;

	while (OnProcess())
	{
		int is_running_slowly_retries = 4;
		double timestamp = __timer_of_start_point();
		double elapsed = 0;

		elapsed = ClampElapsed(timestamp - update_timestamp);

		while ((elapsed > 0.0) && ((elapsed + update_epsilon) >= target_update_period))
		{
			
			(void)elapsed; //delta time
			OnUpdate(elapsed);

			update_timestamp = timestamp;
			timestamp = __timer_of_start_point();
			update_time = timestamp - update_timestamp;


			update_epsilon += elapsed - target_update_period;

			elapsed = ClampElapsed(timestamp - update_timestamp);

			if (target_update_period <= DBL_EPSILON)
				break;

			is_running_slowly = update_epsilon >= target_update_period;
			if (is_running_slowly && --is_running_slowly_retries == 0)
				break;


		}

		elapsed = ClampElapsed(timestamp - render_timestamp);
		if (elapsed > 0 && elapsed >= target_render_period)
		{
			OnDraw();

			render_timestamp = timestamp;
			timestamp = __timer_of_start_point();
			render_time = timestamp - render_timestamp;
		}
	}	

	*/
}
