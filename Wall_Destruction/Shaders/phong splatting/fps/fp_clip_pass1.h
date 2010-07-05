!!FP1.0

# PHONG SPLATTING
# M. Botsch, M. Spernat, L. Kobbelt

# Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen


# Fragment program for the visibility splatting pass of our phongsplat renderer
# Use this for clipped splats



# Program parameters:

# vp.x = w_n/w_vp
# vp.y = -near
# vp.z = -0.5*w_n
# vp.w = -0.5*h_n

# zb.x = far/(far-near)
# zb.y = far*near/(far-near)
# zb.z = -near
# zb.w = alpha_factor             // used for prescaling colors before blending

# do.x = depth_offset



# Input from vertex program:

# f[WPOS] = (x,y)

# f[TEX1] = (c_x, c_y, c_z, 0.0)  // splat center in eye coordinates
# f[TEX2] = (n_x, n_y, n_z)       // scaled splat normal in eye coordinates
#         = n/(c^T*n)             // cf. Eq. 6

# f[TEX0] = (U_x, U_y, U_z)       // tangent u in eye coordinates
# f[TEX3] = (V_x, V_y, V_z)       // tangent v in eye coordinates

# f[TEX4] = clip line 1
# f[TEX5] = clip line 2

DECLARE vp;
DECLARE zb;
DECLARE do;


# Calculate q_n
MAD R0.xyz, f[WPOS], vp.xxxx, vp.zwyy;


# R1.w = (c^T*n)/(q_n*n)          // cf. Eq. 6
DP3  R1.w, f[TEX2], R0;
RCP  R1.w, R1.w;


# q    = q_n * (c^T*n)/(q_n*n)    // cf. Eq. 6
# R1   = q - c
# R1.w = q_z
MAD R1, R0.xyzz, R1.w, -f[TEX1];


# Adjust depth for visibility splatting
ADD R0.z, R1.w, -do.x;


# Per pixel depth correction (including depth-offset)
RCP R0.z, R0.z;
MAD o[DEPR], zb.y, R0.z, zb.x;


# Orthogonal projection on splat wrt. local tangent frame, yielding (u,v)
DP3  R0.x, R1, f[TEX0];
DP3  R0.y, R1, f[TEX3];


# For correct evaluation of the half-space test
MOV  R0.z, 1.0;


# Get clip line 1 and kill fragments if they are outside
DP3C  R1.y, R0, f[TEX4];
KIL LT.y;


# Get clip line 2 and kill fragments if they are outside
DP3C  R1.x, R0, f[TEX5];
KIL LT.x;


# Get squared distance. Actually this is a DP2, therefor R0.z has to be 0.
# Since R0.w is not yet used it is still initialized with 0 -> use it as R0.z
DP3  R1.x, R0.xyww, R0.xyww;

# In order to assign a radially decreasing weight to the pixel,
# (1 - R1.x) * zb.w is assigned as alpha value.
MAD  o[COLR].w, R1.x, -zb.w, zb.w;


END
