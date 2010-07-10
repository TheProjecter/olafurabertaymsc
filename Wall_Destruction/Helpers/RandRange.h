#ifndef RAND_RANGE_H
#define RAND_RANGE_H

class RandRange{
public:
	static inline float Range(float bottom , float range)
	{
		return bottom + range * rand() / ((float) RAND_MAX);
	}

	static inline float Rand(float bottom , float top)
	{
		return Range(bottom, top-bottom);
	}
};
#endif 