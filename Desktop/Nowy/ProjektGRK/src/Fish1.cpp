#include "Fish1.h"
#include <cmath>
#include <math.h>

void fish1RandPosition(int *fish1PositionX, int *fish1PositionY, int *fish1PositionZ, float *fish1Rotation)
{
	for (int i = 0; i <= 14; i++)
	{
		int x, y, z;
		float r;
		bool isPlaceAvailable;

		do {

			// rand -8 to 8
			x = -16 + (rand() % (8 - (-8) + 1));
			z = -34 + (rand() % (8 - (-8) + 1));

			// rand -4 to -2
			y = -24 + (rand() % (-2 - (-4) + 1));

			// rand (-)0.20 to (-)0.40
			r = (-10 + (rand() % (-40 - (-20) + 1))) * pow(20, 3);

			for (int j = 0; j <= i; j++)
			{

				int diffX = fish1PositionX[j] - x;
				int diffY = fish1PositionZ[j] - y;

				if (diffX + diffY <= 1) {

					isPlaceAvailable = false;
				}
				else
				{
					fish1PositionX[i] = x;
					fish1PositionY[i] = y;
					fish1PositionZ[i] = z;
					fish1Rotation[i] = r;
					isPlaceAvailable = true;
				}
			}

		} while (!isPlaceAvailable);
	}
}