!!ARBvp1.0

# PHONG SPLATTING
# M. Botsch, M. Spernat, L. Kobbelt

# Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen


# Vertex program for the visibility splatting pass of our phongsplat renderer




# Input: center, U, V
ATTRIB iPos          = vertex.position;

# U = (U_x, U_y, U_z, alpha)
ATTRIB iAxis1        = vertex.texcoord[0];

# V = (V_x, V_y, V_z, beta)
ATTRIB iAxis2        = vertex.texcoord[1];

OUTPUT oPos         = result.position;
OUTPUT oPointsize   = result.pointsize;

# Splat center in eye coordinates
# (c_x, c_y, c_z, 0.0)
OUTPUT oTex1        = result.texcoord[1];

# Scaled splat normal in eye coordinates
OUTPUT oTex2        = result.texcoord[2];

# Transformed U
OUTPUT oTex0        = result.texcoord[0];

# transformed V
OUTPUT oTex3        = result.texcoord[3];



PARAM  mv[4]        = { state.matrix.modelview };
PARAM  mvp[4]       = { state.matrix.mvp };
PARAM  mvinvtr[4]   = { state.matrix.modelview.invtrans };

# (size_fac, 1.0, 1.0/size_factor, 0.0)
# size_factor allows the user to modify splat size
# size_fac = size_factor * 2.0 * n * h_vp / (t-b)     // cf. Eq. 5
PARAM  c            = program.env[0];

TEMP   R0, R1, R2;
TEMP   Normal;


# Transform U
DP3  R0.x, mvinvtr[0], iAxis1;
DP3  R0.y, mvinvtr[1], iAxis1;
DP3  R0.z, mvinvtr[2], iAxis1;

# Allow the user to modify splat size
MUL  oTex0.xyz, R0, c.z;

# R0.w = 1.0/||U||
DP3  R0.w, iAxis1, iAxis1;
RSQ  R0.w, R0.w;


# Transform V
DP3  R1.x, mvinvtr[0], iAxis2;
DP3  R1.y, mvinvtr[1], iAxis2;
DP3  R1.z, mvinvtr[2], iAxis2;

# Allow the user to modify splat size
MUL  oTex3.xyz, R1, c.z;

# R1.w = 1.0/||V||
DP3  R1.w, iAxis2, iAxis2;
RSQ  R1.w, R1.w;


# Compute normal from tangents
XPD	 Normal, R1, R0;


# R2.y = max(1.0/||U||, 1.0/||V||) = r                // cf. Eq. 5
MAX R2.y, R0.w, R1.w;


# normalize normal
DP3 R1.w, Normal, Normal;
RSQ R1.w, R1.w;
MUL R1.xyz, R1.w, Normal;


# Transform points to eye coords and set w = 0.0
DP4  R0.x, mv[0], iPos;
DP4  R0.y, mv[1], iPos;
DP4  R0.z, mv[2], iPos;
MOV  oTex1, R0;
MOV  oTex1.w, 0.0;


# Pass (scaled) eye-space normal n/(c^T*n)            // cf. Eq. 6
DP3  R1.w, R0, R1;
RCP  R2.x, R1.w;
MUL  oTex2, R1, R2.x;


# Backface culling -> R1.w = {0,1}
SLT  R1.w, R1.w, c.w;


# Transform point to NDC
DP4  R0.x, mvp[0], iPos;
DP4  R0.y, mvp[1], iPos;
DP4  R0.z, mvp[2], iPos;
DP4  R0.w, mvp[3], iPos;
MUL  R0.w, R0.w, R1.w;
MOV  oPos, R0;


# Compute point size (integer!)                       // cf. Eq. 5
# R0.w = z_eye
# R2.y = r
# c.x  = size_factor * 2.0 * n * h_vp / (t-b)
# c.y  = 1.0
RCP  R2.x, R0.w;
MUL  R2.x, R2.x, R2.y;
MAD  R2.x, c.x, R2.x, c.y;
FLR  oPointsize, R2.x;


END
