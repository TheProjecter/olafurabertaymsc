// parameters
float F; // the size of the reconstruction filter, usually 2.0f

float conditionThreshold; // ill-conditioning threshold, usually 100.0f
float prefiltersize; // the size of the prefilter, usually 1.0f
float focus; // the distance of the focal plane
float aperture; // the aperture of the lens, default value is 2.8f
float imagePlane = 1.0f; // fixed, do not change
int subpixels; // the number of sampler per pixel

Matrix4 objectToScreen; // mapping from object-space to screen-space
Vector2 vpScale; // the scaling factor from homogenous screen-space to the viewport
Vector2 vpMid; // the coordinates of the viewport origin
int scissorxmin,scissorxmax,scissorymin,scissorymax;

int clipPlanes = 6;
Vector4 clipOrigin[clipPlanes],clipNormal[clipPlanes];

int exptablesize; // 256 works well
float exptable[exptablesize]; // exp(-0.5f * F * float(index)/float(exptablesize))

// a set of poisson distributed samples locations
int jitterPatterns; // 64 distributions are usually enough
Vector2 prefilterJitter[jitterPatterns * subpixels];
Vector2 lensJitter[jitterPatterns * subpixels];

// a 2D array of randomly selected indices to distributions
int selSize; // 32 patterns are usually enough
int prefilterSelection[selSize * selSize];
int doffilterSelection[selSize * selSize];

// constants
float focalLength = imagePlane*focus/(imagePlane+focus);
float lensRadius = focalLength*0.5f/aperture;
float exptablescale = float(exptablesize)/F;

Vector2 oovpScale(1.0f/vpScale.x, 1.0f/vpScale.y);

Matrix2 Vp( prefiltersize*oovpScale.x*oovpScale.x, 0,
		   0, prefiltersize*oovpScale.y*oovpScale.y );
Matrix4 screenToObject = Invert(objectToScreen);
Vector4 c = screenToObject.z;
if( c.w != 0.0f ) c *= 1.0f/c.w;
else c = -c;

for(int i=0;i<numberOfSplats;++i) {
	Vector3 p0 = splats[i].position;
	Vector3 n = splats[i].normal;

	// back-face cull (in object-space)
	Vector4 pleq(n.x, n.y, n.z, -Dot(p0,n));
	int dotSign = extractSign( Dot(c, pleq) );
	if( dotSign ) continue; // negative = back-facing

	// set up the conic matrix
	Matrix2 Vr( splats[i].size, 0,
		0, splats[i].size );
	Matrix2 Q = Invert(Vr * Transpose(Vr));

	// make the tangent plane (in object-space)
	Vector3 tu = Normalize( Perpendicular( n ) );
	Vector3 tv = Cross(n, tu);

	// transform the tangents and the splat center to screen-space
	Vector4 tuh(tu.x, tu.y, tu.z, 0);
	Vector4 tvh(tv.x, tv.y, tv.z, 0);
	Vector4 p0h(p0.x, p0.y, p0.z, 1);
	tuh *= objectToScreen;
	tvh *= objectToScreen;
	p0h *= objectToScreen;

	// view frustum culling
	for(int cp=0;cp<clipPlanes;cp++) {
		float a = Dot(tuh, clipNormal[cp]);
		float b = Dot(tvh, clipNormal[cp]);
		float c = Dot(p0h, clipNormal[cp]) - Dot(clipNormal[cp], clipOrigin[cp]);
		if( a == 0.0f && b == 0.0f ) { // the planes are parallel
			if( c < 0.0f ) break; // behind the clip plane
		} else { // if behind the plane, discard
			if( c < 0.0f &&					
				F * (Q[0][0]*b*b - 2.0f*Q[0][1]*a*b + Q[1][1]*a*a) - c*c*Det(Q) < 0.0f )
				break;
		}
	}
	if( cp < clipPlanes ) continue; // the splat was behind some of the planes

	// mapping from the splat to screen-space
	Matrix3 M(tuh.x, tuh.y, tuh.w,
		tvh.x, tvh.y, tvh.w,
		p0h.x, p0h.y, p0h.w);
	float detM = Det(M);

	// mapping from screen-space to the splat
	Matrix3 Mi( M[1][1]*M[2][2] - M[2][1]*M[1][2],
		M[2][1]*M[0][2] - M[0][1]*M[2][2],
		M[0][1]*M[1][2] - M[1][1]*M[0][2],
		M[2][0]*M[1][2] - M[1][0]*M[2][2],
		M[0][0]*M[2][2] - M[2][0]*M[0][2],
		M[1][0]*M[0][2] - M[0][0]*M[1][2],
		M[1][0]*M[2][1] - M[2][0]*M[1][1],
		M[2][0]*M[0][1] - M[0][0]*M[2][1],
		M[0][0]*M[1][1] - M[1][0]*M[0][1]); // inverse of M * Det(M)

	Matrix3 Qh( Q[0][0], Q[0][1], 0,
		Q[1][0], Q[1][1], 0,
		0, 0, -F );
	Matrix3 Qhs = Mi * Qh * Transpose(Mi);

	// the bounding rectangle
	float xmin = -1, xmax = 1;
	float ymin = -1, ymax = 1;
	float delta = Qhs[0][0] * Qhs[1][1] - Qhs[0][1] * Qhs[1][0];
	if( delta > 0.0f ) { // ellipses can be bound, others must test the whole screen
		float oodelta = 1.0f/delta;
		float xt = (Qhs[0][1] * Qhs[1][2] - Qhs[1][1] * Qhs[0][2]) * oodelta;
		float yt = (Qhs[0][1] * Qhs[0][2] - Qhs[0][0] * Qhs[1][2]) * oodelta;
		float f = -Qhs[2][2] - xt * Qhs[0][2] - yt * Qhs[1][2];

		float bx = sqrt(Qhs[1][1] * f * oodelta);
		float by = sqrt(Qhs[0][0] * f * oodelta);

		xmin = xt - bx;
		xmax = xt + bx;
		ymin = yt - by;
		ymax = yt + by;
	}

	// calculate zmin and zmax
	float zmin = p0h.w,zmax = p0h.w;
	float l = Q[0][0]*tuh.w*tuh.w + 2.0f*Q[0][1]*tuh.w*tvh.w + Q[1][1]*tvh.w*tvh.w;
	if( l != 0.0f ) {
		l = sqrt(MAX(0.0f,F/l));
		zmin = p0h.w - l * (tuh.w*tuh.w + tvh.w*tvh.w);
		zmax = p0h.w + l * (tuh.w*tuh.w + tvh.w*tvh.w);
	}

	// calculate the radius of the circle of confusion for DOF
	float focusDistZMin = focalLength * zmin / (zmin - focalLength);
	float focusDistZMax = focalLength * zmax / (zmax - focalLength);
	float cocZMin = fabs(focusDistZMin - imagePlane)*focalLength/(aperture*focusDistZMin);
	float cocZMax = fabs(focusDistZMax - imagePlane)*focalLength/(aperture*focusDistZMax);
	float coc = 0.5f * MAX(cocZMin,cocZMax);

	// determine the integer bounding rectangle and do scissoring
	xmin = (xmin - coc) * vpScale.x + vpMid.x - prefiltersize;
	xmax = (xmax + coc) * vpScale.x + vpMid.x + prefiltersize;
	ymin = (ymin - coc) * vpScale.y + vpMid.y - prefiltersize;
	ymax = (ymax + coc) * vpScale.y + vpMid.y + prefiltersize;

	int sx = floor( xmin ), ex = ceil( xmax );
	if( sx >= width || ex <= 0 ) continue;
	sx = MAX(sx,scissorxmin);
	ex = MIN(ex,scissorxmax);
	if( ex - sx == 0 ) continue;

	int sy = floor( ymin ), ey = ceil( ymax );
	if( sy >= height || ey <= 0 ) continue;

	sy = MAX(sy,scissorymin);
	ey = MIN(ey,scissorymax);
	if( ey - sy == 0 ) continue;

	Vector4 color(0,0,0,1); // rgba
	// call the splat shader here
	/*
	An example splat shader
	 float alpha = 0.75f;
	
	 // diffuse
	 float dot = (n | lightDirectionInObject) * diffuseIntensity;
	 dot = MAX(ambientIntensity,dot);
	 color = splats[i].color;
	 color *= dot;
	
	 // specular
	 halfway = Normalize( Normalize(cameraInObject - splats[i].position) + lightDirectionInObject);
	 dot = Dot(halfway, n);
	 if( dot > 0.0f ) {
	 dot = pow(dot,specularExponent) * specularIntensity;
	 color.r += dot;
	 color.g += dot;
	 color.b += dot;
	 }
	 color *= alpha; // premultiplied alpha
	 color.a = alpha;

	*/

	// rasterization loop
	ABuffer::Fragment fragment;
	fragment.color = color;
	fragment.zmin = zmin;
	fragment.zmax = zmax;
	fragment.dotSign = dotSign;
	for(int y=sy;y<ey;++y)
		for(int x=sx;x<ex;++x) {
			int aapattern = prefilterSelection[(y&(selSize-1))*selSize+(x&(selSize-1))];
			int dofpattern = doffilterSelection[(y&(selSize-1))*selSize+(x&(selSize-1))];
			Vector2 sp(((float)x - vpMid.x) * oovpScale.x,
				((float)y - vpMid.y) * oovpScale.y);
			for(int j=0;j<subpixels;j++) {
				// jittered sample location around the pixel
				Vector3 f(sp.x + prefilterJitter[aapattern+j].x*oovpScale.x,
					sp.y + prefilterJitter[aapattern+j].y*oovpScale.y, 1);
				f *= focus; // the focal point
				Vector3 p(lensJitter[dofpattern+j].x,
					lensJitter[dofpattern+j].y, 0);// a point on the lens
				f -= p;
				f *= Mi;
				p *= Mi;
				Vector3 uv(detM*f.x - f.x*p.z + f.z*p.x,
					detM*f.y - f.y*p.z + f.z*p.y,
					detM*f.z); // homogenous point on the splat plane

				float radius = Q[0][0]*uv.x*uv.x + 2.0f*Q[0][1]*uv.x*uv.y + Q[1][1]*uv.y*uv.y;
				if( radius < F*uv.z*uv.z ) {
					float ooh = 1.0f / uv.z;
					float z = p0h.z + uv.x * ooh * tuh.z + uv.y * ooh * tvh.z;
					if( z > 0.0f ) {
						radius *= ooh * ooh * exptablescale;
						fragment.weight = exptable[MAX(0,(int)floor(radius))];
						fragment.z = z; // needed, if the surface reconstruction uses z
						abuffer->appendFragment(x,y,j,fragment);
					}
				}
			}
		}

		/*
		 Vector4 color(0,0,0,0); // rgba
		 for(int j=0;j<subpixels;++j) {
		 // sort fragments in the order of ascending depth according to fragment’s zmin
		 // (not shown)
		
		 // the outer loop blends surfaces
		 unsigned int coverageMask = 0;
		 int i = 0;
		while(i < fragmentCount)
		 {
		 const Fragment *f1 = sortBuffer[i];
		 i++;
		
		 Vector4 cs = f1->color * f1->weight;
		 float w = f1->weight;
		
		 // the inner loop reconstructs a surface
		 float zmax = f1->zmax;
		
		
   		 while(i < fragmentCount)
		 {
		 const Fragment *f2 = sortBuffer[i];
		 if( f2->dotSign != f1->dotSign ) break; // facing the opposite directions
		 if( f2->zmin > zmax ) break; // no depth overlap
		 if( f2->zmax > zmax ) zmax = f2->zmax;
		
		 cs += f2->color * f2->weight;
		 w += f2->weight;
		
		 i++;
		 f1 = f2;
		 }
		
		 cs *= 1.0f/w; // normalize color by the accumulated weight
		
		 if( cs.a >= 0.999f ) { // opaque
		 cs.a = 1.0f;
		 coverageMask = 1; // surface occludes the rest of the surfaces
		 }
		
		 // blend new surface to the pixel’s color
		 color += cs * (1.0f - color.a);
		
		 if( coverageMask ) break; // no more visible surfaces, so early exit
		 }
		
		 // if not full coverage, blend with the background color
		 if( !coverageMask )
		 color += background * (1.0f - color.a);
		 }
		 // write color to the frame buffer
		*/
}