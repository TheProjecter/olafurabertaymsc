!!FP1.0

# PHONG SPLATTING
# M. Botsch, M. Spernat, L. Kobbelt

# Copyright (C) 2004 by Computer Graphics Group, RWTH Aachen


# Fragment program for normalization



# Get color from texture
TEXC  R0, f[TEX0], TEX2, RECT;

# kill fragments with alpha = 0, so that background is not overwritten
KIL  EQ.w;

# Divide rgb by alpha
RCP  R0.w, R0.w;
MUL  o[COLR].xyz, R0, R0.w;

END
