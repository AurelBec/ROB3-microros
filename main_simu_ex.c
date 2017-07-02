#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "fastsim_c.h"

int main(int argc, char**argv)
{
	robot_t *r = new_robot(300, 300);
	map_t *m = new_map("fastsim/envs/cuisine.pbm");
	display_t*d = new_display(m, r);

	float vr; // Right wheel speed command
	float vl; // Left wheel speed command
	int bumper_r; // Right bumper (1 = active, 0 = inactive)
	int bumper_l; // Left bumper (1 = active, 0 = inactive)

	int i;
	for( i = 0; i < 3000 ; i++) // 60s
	{
	// Update buffer
		bumper_r =  get_right_bumper(r);
		bumper_l =  get_left_bumper(r);


		/* Very simple reactive behavior:
		- If one (or both) bumper is activated, go backwards
		and turn left
		- If no bumper is activated, go forward and a bit to
		the right */

		if(bumper_r == 1 || bumper_l ==1)
		{
			vr = -2;
			vl = -1;
		} else {
			vr = 1;
			vl = 1.2;
		}

		// Move robot
		move_robot(r, m, vr, vl);

		// Update the visualization
		update_display(d);
		// Print debug info
		printf("timestep %d : left bump = %d   right bump = %d  vr = %f vl = %f\n", i, bumper_l, bumper_r, vr, vl);
		// Sleep to make the simulation slow enough for humans
		usleep(500);
	}

	return 0;
}
