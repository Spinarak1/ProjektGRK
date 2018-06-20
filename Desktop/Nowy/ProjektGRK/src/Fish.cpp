#include "Fish.h"
#include <cmath>
#include <math.h>

void fishRandPosition(int *fishPositionX, int *fishPositionY, int *fishPositionZ, float *fishRotation)
{
	for (int i = 0; i <= 14; i++)
	{
		int x, y, z;
		float r;
		bool isPlaceAvailable;

		do {

			x = -8 + (rand() % (8 - (-8) + 1));
			z = -8 + (rand() % (8 - (-8) + 1));

			y = -4 + (rand() % (-2 - (-4) + 1));

			r = (-20 + (rand() % (-40 - (-20) + 1))) * pow(10, -2);

			for (int j = 0; j <= i; j++)
			{
			
				int diffX = fishPositionX[j] - x;
				int diffY = fishPositionZ[j] - y;

				if (diffX + diffY <= 1) {

					isPlaceAvailable = false;
				}
				else
				{
					fishPositionX[i] = x;
					fishPositionY[i] = y;
					fishPositionZ[i] = z;
					fishRotation[i] = r;
					isPlaceAvailable = true;
				}
			}

		} while (!isPlaceAvailable);
	}
}