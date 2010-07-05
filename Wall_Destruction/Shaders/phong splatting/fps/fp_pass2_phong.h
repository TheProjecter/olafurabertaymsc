!!FP1.0

# PHONG SPLATTING
# M. Botsch, M. Spernat, L. Kobbelt

# Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen          


# Fragment program for the second pass of our phongsplat renderer




# Program parameters:

# vp.x = w_n/w_vp
# vp.y = -near
# vp.z = -0.5*w_n
# vp.w = -0.5*h_n

# zb.x = far/(far-near)
# zb.y = far*near/(far-near)
# zb.z = -near
# zb.w = alpha_factor             // used for prescaling colors before blending



# Input from vertex program:

# f[WPOS] = (x,y)

# f[TEX1] = (c_x, c_y, c_z, 0.0)  // splat center in eye coordinates
# f[TEX2] = (n_x, n_y, n_z)       // scaled splat normal in eye coordinates
#         = n/(c^T*n)             // cf. Eq. 6

# f[TEX0] = (U_x, U_y, U_z)       // tangent u in eye coordinates
# f[TEX3] = (V_x, V_y, V_z)       // tangent v in eye coordinates

# f[TEX6] = (n0_x, n0_y, n0_z)    // denotes the center normal of normal field
# f[TEX7] = alpha*(U_x, U_y, U_z)
# f[TEX5] = beta*(V_x, V_y, V_z)

DECLARE vp;
DECLARE zb;



# Calculate q_n
MAD R0.xyz, f[WPOS], vp.xxxx, vp.zwyy;


# R1.w = (c^T*n)/(q_n*n)          // cf. Eq. 6
DP3  R1.w, f[TEX2], R0;
RCP  R1.w, R1.w;


# q    = q_n * (c^T*n)/(q_n*n)    // cf. Eq. 6
# R1   = q - c
# R1.w = q_z
MAD R1, R0.xyzz, R1.w, -f[TEX1];


# Per pixel depth correction
RCP R0.z, R1.w;
MAD o[DEPR], zb.y, R0.z, zb.x;


# Orthogonal projection on splat wrt. local tangent frame, yielding (u,v)
DP3  R0.x, R1, f[TEX0];
DP3  R0.y, R1, f[TEX3];


# Calculate normal vector N = n0 + u*alpha*U + v*beta*V
MOV  R1, f[TEX6];
MAD  R1, f[TEX7], R0.x, R1;
MAD  R1, f[TEX5], R0.y, R1;


# Assign color
TEX  o[COLR], R1, TEX0, CUBE;

# For textured models replace the previous line by the following
#TEX R1, R1, TEX0, CUBE;
#MAD o[COLR], f[COL0], R1.x, R1.y;


# Get squared distance. Actually this is a DP2, therefor R0.z has to be 0.
# Since R0.w is not yet used it is still initialized with 0 -> use it as R0.z
DP3  R1.x, R0.xyww, R0.xyww;

# In order to assign a radially decreasing weight to the pixel,
# (1 - R1.x) * zb.w is assigned as alpha value.
MAD  o[COLR].w, R1.x, -zb.w, zb.w;


END