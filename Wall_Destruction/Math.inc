
// method taken from  Surface Splatting: Theory, Extensions and Implementation by Jussi Räsänen
float3 Perpendicular( float3 v ) {
	float3 t;
	// select the shortest of projections of axes on v
	// (the closest to perpendicular to v),
	// and project it to the plane defined by v
	if( abs(v.x) < abs(v.y) ) { // x < y
		if( abs(v.x) < abs(v.z) ) { // x < y && x < z
			t.x = 1.0f - v.x * v.x;
			t.y = -v.x * v.y;
			t.z = -v.x * v.z;
			return t;
		}
	}
	else{ // y <= x
		if( abs(v.y) < abs(v.z) ) { // y <= x && y < z
			t.x = -v.y * v.x;
			t.y = 1.0f - v.y * v.y;
			t.z = -v.y * v.z;
			return t;
		}
	}

	// z <= x && z <= y
	t.x = -v.z * v.x;
	t.y = -v.z * v.y;
	t.z = 1.0f - v.z * v.z;
	return t;
}