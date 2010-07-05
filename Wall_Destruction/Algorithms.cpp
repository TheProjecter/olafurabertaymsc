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
/*
Structs::SURFEL_VERTEX* Algorithms::RIMLS(Structs::POINT_CLOUD_VERTEX *points){


	return NULL;
}*/

/*
	for each point P[k] {
		project P[k] to screen space;
		determine the resampling kernel rho[k];
		splat rho[k];
	}
	for each pixel x in the frame buffer {
		shade x;
	}
*/
// algorithm taken from the Surface Splatting
// by Matthias Zwicker, Hanspeter Pfister, Heroen van Baar and Markus Gross
/*void Algorithms::SurfaceSplatting(Structs::POINT_CLOUD_VERTEX *points){
	
}*/

Structs::SURFEL_VERTEX* APSS(Structs::POINT_CLOUD_VERTEX *points){
	



	return NULL;
}
/*
template<typename _MeshType>
typename APSS<_MeshType>::MatrixType APSS<_MeshType>::hessian(const VectorType& x, int* errorMask) const
{
	if ((!mCachedQueryPointIsOK) || mCachedQueryPoint!=x)
	{
		if (!fit(x))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			return MatrixType();
		}
	}

	MatrixType hessian;
	if (Base::mHessianHint==MLS_DERIVATIVE_ACCURATE)
	{
		mlsHessian(x, hessian);
	}
	else
	{
		// this is very approximate !!
		Scalar c = Scalar(2) * uQuad;
		for (int i=0; i<3; ++i)
			for (int j=0; j<3; ++j)
			{
				if (i==j)
					hessian[i][j] = c;
				else
					hessian[i][j] = 0;
			}
	}
	return hessian;
}

template<typename _MeshType>
typename APSS<_MeshType>::VectorType APSS<_MeshType>::project(const VectorType& x, VectorType* pNormal, int* errorMask) const
{
	int iterationCount = 0;
	LVector lx(x.X(), x.Y(), x.Z());
	LVector position = lx;
	LVector normal;
	LVector previousPosition;
	LScalar delta2;
	LScalar epsilon2 = mAveragePointSpacing * mProjectionAccuracy;
	epsilon2 = epsilon2 * epsilon2;
	do {
		if (!fit(VectorType(position.X(), position.Y(), position.Z())))
		{
			if (errorMask)
				*errorMask = MLS_TOO_FAR;
			//std::cout << " proj failed\n";
			return x;
		}

		previousPosition = position;
		// local projection
		if (mStatus==ASS_SPHERE)
		{
			normal = lx - mCenter;
			normal.Normalize();
			position = mCenter + normal * mRadius;

			normal = uLinear + position * (LScalar(2) * uQuad);
			normal.Normalize();
		}
		else if (mStatus==ASS_PLANE)
		{
			normal = uLinear;
			position = lx - uLinear * (vcg::Dot(lx,uLinear) + uConstant);
		}
		else
		{
			// Newton iterations
			LVector grad;
			LVector dir = uLinear+lx*(2.*uQuad);
			LScalar ilg = 1./vcg::Norm(dir);
			dir *= ilg;
			LScalar ad = uConstant + vcg::Dot(uLinear,lx) + uQuad * vcg::SquaredNorm(lx);
			LScalar delta = -ad*std::min<Scalar>(ilg,1.);
			LVector p = lx + dir*delta;
			for (int i=0 ; i<2 ; ++i)
			{
				grad = uLinear+p*(2.*uQuad);
				ilg = 1./vcg::Norm(grad);
				delta = -(uConstant + vcg::Dot(uLinear,p) + uQuad * vcg::SquaredNorm(p))*std::min<Scalar>(ilg,1.);
				p += dir*delta;
			}
			position = p;

			normal = uLinear + position * (Scalar(2) * uQuad);
			normal.Normalize();
		}

		delta2 = vcg::SquaredNorm(previousPosition - position);
	} while ( delta2>epsilon2 && ++iterationCount<mMaxNofProjectionIterations);

	if (pNormal)
	{
		if (mGradientHint==MLS_DERIVATIVE_ACCURATE)
		{
			VectorType grad;
			mlsGradient(vcg::Point3Cast<Scalar>(position), grad);
			grad.Normalize();
			*pNormal = grad;
		}
		else
		{
			for (int k=0; k<3; ++k)
				(*pNormal)[k] = normal[k];
		}
	}

	if (iterationCount>=mMaxNofProjectionIterations && errorMask)
		*errorMask = MLS_TOO_MANY_ITERS;

	return VectorType(position.X(), position.Y(), position.Z());
}

template<typename _MeshType>
bool APSS<_MeshType>::fit(const VectorType& x) const
{
	Base::computeNeighborhood(x, true);
	unsigned int nofSamples = mNeighborhood.size();

	if (nofSamples==0)
	{
		mCachedQueryPointIsOK = false;
		return false;
	}
	else if (nofSamples==1)
	{
		int id = mNeighborhood.index(0);
		LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
		LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());

		uLinear = n;
		uConstant = -vcg::Dot(p, uLinear);
		uQuad = 0;
		mStatus = ASS_PLANE;
		return true;
	}

	LVector sumP; sumP.SetZero();
	LVector sumN; sumN.SetZero();
	LScalar sumDotPN = 0.;
	LScalar sumDotPP = 0.;
	LScalar sumW = 0.;
	for (unsigned int i=0; i<nofSamples; i++)
	{
		int id = mNeighborhood.index(i);
		LVector p = vcg::Point3Cast<LScalar>(mPoints[id].cP());
		LVector n = vcg::Point3Cast<LScalar>(mPoints[id].cN());
		LScalar w = mCachedWeights.at(i);

		sumP += p * w;
		sumN += n * w;
		sumDotPN += w * vcg::Dot(n,p);
		sumDotPP += w * vcg::SquaredNorm(p);
		sumW += w;
	}

	LScalar invSumW = Scalar(1)/sumW;
	LScalar aux4 = mSphericalParameter * LScalar(0.5) *
		(sumDotPN - invSumW*vcg::Dot(sumP,sumN))
		/(sumDotPP - invSumW*vcg::SquaredNorm(sumP));
	uLinear = (sumN-sumP*(Scalar(2)*aux4))*invSumW;
	uConstant = -invSumW*(Dot(uLinear,sumP) + sumDotPP*aux4);
	uQuad = aux4;

	// finalize
	if (fabs(uQuad)>1e-7)
	{
		mStatus = ASS_SPHERE;
		LScalar b = 1./uQuad;
		mCenter = uLinear*(-0.5*b);
		mRadius = sqrt( vcg::SquaredNorm(mCenter) - b*uConstant );
	}
	else if (uQuad==0.)
	{
		mStatus = ASS_PLANE;
		LScalar s = LScalar(1)/vcg::Norm(uLinear);
		assert(!vcg::math::IsNAN(s) && "normal should not have zero len!");
		uLinear *= s;
		uConstant *= s;
	}
	else
	{
		mStatus = ASS_UNDETERMINED;
		// normalize the gradient
		LScalar f = 1./sqrt(vcg::SquaredNorm(uLinear) - Scalar(4)*uConstant*uQuad);
		uConstant *= f;
		uLinear *= f;
		uQuad *= f;
	}

	// cache some values to be used by the mls gradient
	mCachedSumP = sumP;
	mCachedSumN = sumN;
	mCachedSumW = sumW;
	mCachedSumDotPP = sumDotPP;
	mCachedSumDotPN = sumDotPN;

	mCachedQueryPoint = x;
	mCachedQueryPointIsOK = true;
	return true;
}

*/