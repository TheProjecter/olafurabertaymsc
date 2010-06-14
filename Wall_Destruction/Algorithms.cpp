#include "Algorithms.h"


// Implementation of the Robust implicit Moving Least Squares algorithm

/*
	Pseudo code:
	repeat
		i = 0
		repeat
			sumW = sumGw = sumF = sumGF = sumN = 0;
			for p in neighbors(x) do
				px = x-p.position;
				fx = dot(ps, p.normal);

				if i > 0 then alpha = exp(-((fx-f)/sigma_r)^2)
									* exp(-(norm(p.normal-grad_f)/sigma_n)^2);
				else alpha = 1;

				w = alpha * phi(norm(px)^2);
				grad_w = alpha * 2 * px * dphi(norm(px)^2);

				sumW += w;
				sumGw += grad_w;
				sumF += w * fx;
				sumGF += grad_w * fx;
				sumN += w*p.normal;
			end
			f = sumF/sumW;
			grad_f = (sumGf - f * sumGw + sumN)/sumW;
		until ++i > max_iters || convergance();
		x = x - f*grad_f;
	until norm(f * grad_f) < threshold;


*/
/*Structs::SURFEL_VERTEX* Algorithms::RIMLS(Structs::POINT_CLOUD_VERTEX *points){


	return NULL;
}*/