/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//=======
// 650b1
//=======

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpStorageExtendedMeshShapeMaterial", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpMeshMaterial")
	HK_PATCH_MEMBER_ADDED("restitution", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_MEMBER_ADDED("friction", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_MEMBER_ADDED("userData", TYPE_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpFirstPersonGun", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
	HK_PATCH_MEMBER_ADDED("name", TYPE_CSTRING, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("keyboardKey", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpBallGun", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpFirstPersonGun")
	HK_PATCH_MEMBER_ADDED("bulletRadius", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bulletVelocity", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bulletMass", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("damageMultiplier", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxBulletsInWorld", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bulletOffsetFromCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpFirstPersonGun", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpGravityGun", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpFirstPersonGun")
	HK_PATCH_MEMBER_ADDED("maxNumObjectsPicked", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxMassOfObjectPicked", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxDistOfObjectPicked", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("impulseAppliedWhenObjectNotPicked", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("throwVelocity", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("capturedObjectPosition", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("capturedObjectsOffset", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpFirstPersonGun", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpProjectileGun", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpFirstPersonGun")
	HK_PATCH_MEMBER_ADDED("maxProjectiles", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("reloadTime", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpFirstPersonGun", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpMountedBallGun", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpBallGun")
	HK_PATCH_MEMBER_ADDED("position", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpBallGun", 0)
	HK_PATCH_DEPENDS("hkpFirstPersonGun", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpShapeRayBundleCastInput", 0)
	HK_PATCH_MEMBER_ADDED("from", TYPE_TUPLE_VEC_4, HK_NULL, 3)
	HK_PATCH_MEMBER_ADDED("to", TYPE_TUPLE_VEC_4, HK_NULL, 3)
	HK_PATCH_MEMBER_ADDED("filterInfo", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("rayShapeCollectionFilter", TYPE_OBJECT, "hkpRayShapeCollectionFilter", 0)
	HK_PATCH_DEPENDS("hkpRayShapeCollectionFilter", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpExtendedMeshShape", 0, "hkpExtendedMeshShape", 1)
	HK_PATCH_MEMBER_ADDED("defaultCollisionFilterInfo", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpExtendedMeshShapeTrianglesSubpart", 0, "hkpExtendedMeshShapeTrianglesSubpart", 1)
	HK_PATCH_MEMBER_ADDED("userData", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConstraintInstance", 0, "hkpConstraintInstance", 1)
	HK_PATCH_MEMBER_ADDED("destructionRemapInfo", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpConstraintInstance_0_to_1)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeShapeSubpartStorage", 0, "hkpStorageExtendedMeshShapeShapeSubpartStorage", 1)
	HK_PATCH_MEMBER_RENAMED("materials", "int_materials")
	HK_PATCH_MEMBER_ADDED("materials", TYPE_ARRAY_STRUCT, "hkpStorageExtendedMeshShapeMaterial", 0)
	HK_PATCH_FUNCTION(hkpStorageExtendedMeshShapeShapeSubpartStorage_0_to_1)
	HK_PATCH_MEMBER_REMOVED("int_materials", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpStorageExtendedMeshShapeMaterial", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeMeshSubpartStorage", 0, "hkpStorageExtendedMeshShapeMeshSubpartStorage", 1)
	HK_PATCH_MEMBER_RENAMED("materials", "int_materials")
	HK_PATCH_MEMBER_ADDED("materials", TYPE_ARRAY_STRUCT, "hkpStorageExtendedMeshShapeMaterial", 0)
	HK_PATCH_FUNCTION(hkpStorageExtendedMeshShapeMeshSubpartStorage_0_to_1)
	HK_PATCH_MEMBER_REMOVED("int_materials", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpStorageExtendedMeshShapeMaterial", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 0, "hkpWorldCinfo", 1)
	HK_PATCH_MEMBER_ADDED("useKdTree", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("autoUpdateKdTree", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpWorldCinfo_0_to_1)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpMotion", 0, "hkpMotion", 1)
	HK_PATCH_MEMBER_RENAMED("savedMotion", "max_savedMotion")
	HK_PATCH_MEMBER_ADDED("savedMotion", TYPE_OBJECT, "hkpMotion", 0)
	HK_PATCH_MEMBER_ADDED("gravityFactor", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_FUNCTION(hkpMotion_0_to_1)
	HK_PATCH_MEMBER_REMOVED("max_savedMotion", TYPE_OBJECT, "hkpMaxSizeMotion", 0)
	HK_PATCH_DEPENDS("hkpKeyframedRigidMotion", 0)
	HK_PATCH_DEPENDS("hkpMaxSizeMotion", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpEntity", 0, "hkpEntity", 1)
	HK_PATCH_MEMBER_RENAMED("motion", "max_motion")
	HK_PATCH_MEMBER_ADDED("motion", TYPE_STRUCT, "hkpMotion", 0)
	HK_PATCH_FUNCTION(hkpEntity_0_to_1)
	HK_PATCH_MEMBER_REMOVED("max_motion", TYPE_STRUCT, "hkpMaxSizeMotion", 0)
	HK_PATCH_DEPENDS("hkpMotion", 0)
	HK_PATCH_DEPENDS("hkpKeyframedRigidMotion", 0)
	HK_PATCH_DEPENDS("hkpMaxSizeMotion", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpPairwiseCollisionFilterCollisionPair", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("a", TYPE_OBJECT, "hkpEntity", 0)
	HK_PATCH_MEMBER_REMOVED("b", TYPE_OBJECT, "hkpEntity", 0)
	HK_PATCH_DEPENDS("hkpEntity", 1)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpWorldObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpPairwiseCollisionFilter", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpCollisionFilter", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("disabledPairs", TYPE_ARRAY_STRUCT, "hkpPairwiseCollisionFilterCollisionPair", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpCollisionFilter", 0)
	HK_PATCH_DEPENDS("hkpPairwiseCollisionFilterCollisionPair", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCenterOfMassChangerModifierConstraintAtom", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpModifierConstraintAtom")
	HK_PATCH_MEMBER_ADDED("displacementA", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("displacementB", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpModifierConstraintAtom", 0)
	HK_PATCH_DEPENDS("hkpConstraintAtom", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpMassChangerModifierConstraintAtom", 0, "hkpMassChangerModifierConstraintAtom", 1)
	HK_PATCH_MEMBER_RENAMED("factorA", "old_factorA")
	HK_PATCH_MEMBER_RENAMED("factorB", "old_factorB")
	HK_PATCH_MEMBER_ADDED("factorA", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("factorB", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpMassChangerModifierConstraintAtom_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_factorA", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("old_factorB", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpVehicleInstanceWheelInfo", 0, "hkpVehicleInstanceWheelInfo",1)
	HK_PATCH_MEMBER_RENAMED("contactShapeKey", "old_contactShapeKey")
	HK_PATCH_MEMBER_ADDED("contactShapeKey", TYPE_TUPLE_INT, HK_NULL, 8)
	HK_PATCH_FUNCTION(hkpVehicleInstanceWheelInfo_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_contactShapeKey", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 1, "hkpWorldCinfo", 2)
	HK_PATCH_MEMBER_ADDED("numToisTillAllowedPenetrationSimplifiedToi", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numToisTillAllowedPenetrationToi", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numToisTillAllowedPenetrationToiHigher", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numToisTillAllowedPenetrationToiForced", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpWorldCinfo_1_to_2) // set defaults
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpConstraintInstanceSmallArraySerializeOverrideType", 0)
	HK_PATCH_MEMBER_ADDED("data", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("size", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("capacityAndFlags", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpBallSocketConstraintAtom", 0, "hkpBallSocketConstraintAtom", 1)
	HK_PATCH_MEMBER_ADDED("stabilizationFactor", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpBallSocketConstraintAtom_0_to_1) // set defaults
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpBreakableBody", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeMeshSubpartStorage", 1, "hkpStorageExtendedMeshShapeMeshSubpartStorage", 2)
	HK_PATCH_MEMBER_ADDED("indices8", TYPE_ARRAY_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpExtendedMeshShapeTrianglesSubpart", 1, "hkpExtendedMeshShapeTrianglesSubpart", 2)
	HK_PATCH_FUNCTION(hkpExtendedMeshShapeTrianglesSubpart_1_to_2)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeMaterial", 0, "hkpStorageExtendedMeshShapeMaterial", 1)
	HK_PATCH_MEMBER_RENAMED("restitution", "old_restitution")
	HK_PATCH_MEMBER_RENAMED("friction", "old_friction")
	HK_PATCH_MEMBER_ADDED("restitution", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("friction", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpStorageExtendedMeshShapeMaterial_0_to_1)
	HK_PATCH_MEMBER_REMOVED("old_restitution", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_MEMBER_REMOVED("old_friction", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpMotion", 1, "hkpMotion", 2)
	HK_PATCH_MEMBER_RENAMED("gravityFactor", "old_gravityFactor")
	HK_PATCH_MEMBER_ADDED("gravityFactor", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_FUNCTION(hkpMotion_1_to_2)
	HK_PATCH_MEMBER_REMOVED("old_gravityFactor", TYPE_STRUCT, "hkHalf", 0)
	HK_PATCH_DEPENDS("hkHalf", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpSpatialRigidBodyDeactivatorSample", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("refPosition", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("refRotation", TYPE_VEC_4, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpSpatialRigidBodyDeactivator", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpRigidBodyDeactivator", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("highFrequencySample", TYPE_STRUCT, "hkpSpatialRigidBodyDeactivatorSample", 0)
	HK_PATCH_MEMBER_REMOVED("lowFrequencySample", TYPE_STRUCT, "hkpSpatialRigidBodyDeactivatorSample", 0)
	HK_PATCH_MEMBER_REMOVED("radiusSqrd", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minHighFrequencyTranslation", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minHighFrequencyRotation", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minLowFrequencyTranslation", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minLowFrequencyRotation", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpRigidBodyDeactivator", 0)
	HK_PATCH_DEPENDS("hkpEntityDeactivator", 0)
	HK_PATCH_DEPENDS("hkpSpatialRigidBodyDeactivatorSample", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpRigidBodyDeactivator", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpEntityDeactivator", HK_NULL)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpEntityDeactivator", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpFakeRigidBodyDeactivator", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpRigidBodyDeactivator", HK_NULL)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpRigidBodyDeactivator", 0)
	HK_PATCH_DEPENDS("hkpEntityDeactivator", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpEntityDeactivator", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkReferencedObject", HK_NULL)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexVerticesShape", 0, "hkpConvexVerticesShape", 1)
	HK_PATCH_MEMBER_REMOVED("connectivity", TYPE_OBJECT, "hkpConvexVerticesConnectivity", 0) // force realloc
	HK_PATCH_MEMBER_ADDED("connectivity", TYPE_OBJECT, "hkpConvexVerticesConnectivity", 0) // force recalc
	HK_PATCH_DEPENDS("hkpConvexVerticesConnectivity", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpConvexDecompositionShapeConfig", 0)
	HK_PATCH_MEMBER_ADDED("createConnectivity", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("removeUnreferencedVertices", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("minCosAngle", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("minimumArea", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("radius", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()


HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpConvexDecompositionConfiguration2", 1)
	HK_PATCH_MEMBER_ADDED("minReachableDistance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("minOverlapDistance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("minExtrusion", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxExtrusion", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("minEdgeTetraVolume", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxMergeError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxOverlapMergeError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxOverlapPenetration", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxSkinError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxOverlapVolume", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxPieces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("maxDepth", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("forcedOverlapResolution", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("searchForTetrahedrons", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("enableFlooding", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("enableMerging", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("enableIterativeOverlapSolver", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 2, "hkpWorldCinfo", 3)
	HK_PATCH_MEMBER_ADDED("mtPostponeAndSortBroadPhaseBorderCallbacks", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleLinearCastWheelCollide", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpVehicleWheelCollide")
	HK_PATCH_MEMBER_ADDED("wheelCollisionFilterInfo", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("wheelStates", TYPE_ARRAY_STRUCT, "hkpVehicleLinearCastWheelCollideWheelState", 0)
	HK_PATCH_MEMBER_ADDED("rejectChassisListener", TYPE_STRUCT, "hkpRejectChassisListener", 0)
	HK_PATCH_MEMBER_ADDED("maxExtraPenetration", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("startPointTolerance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpVehicleWheelCollide", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleLinearCastWheelCollideWheelState", 0)
	HK_PATCH_DEPENDS("hkpRejectChassisListener", 1)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpRejectRayChassisListener", 0, "hkpRejectChassisListener", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleRayCastBatchingManager", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpVehicleCastBatchingManager")
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleCastBatchingManager", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleManager", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleManager", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
	HK_PATCH_MEMBER_ADDED("registeredVehicles", TYPE_ARRAY_OBJECT, "hkpVehicleInstance", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleInstance", 0)
	HK_PATCH_DEPENDS("hkpAction", 0)
	HK_PATCH_DEPENDS("hkpUnaryAction", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpVehicleRaycastWheelCollide", 0, "hkpVehicleRayCastWheelCollide", 1)
	HK_PATCH_DEPENDS("hkpAabbPhantom", 0)
	HK_PATCH_DEPENDS("hkpPhantom", 0)
	HK_PATCH_DEPENDS("hkpVehicleWheelCollide", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpWorldObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpRejectChassisListener", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleLinearCastBatchingManager", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpVehicleCastBatchingManager")
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleCastBatchingManager", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleManager", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleCastBatchingManager", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpVehicleManager")
	HK_PATCH_MEMBER_ADDED("totalNumWheels", TYPE_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpVehicleManager", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpVehicleLinearCastWheelCollideWheelState", 0)
	HK_PATCH_MEMBER_ADDED("phantom", TYPE_OBJECT, "hkpAabbPhantom", 0)
	HK_PATCH_MEMBER_ADDED("shape", TYPE_OBJECT, "hkpShape", 0)
	HK_PATCH_MEMBER_ADDED("transform", TYPE_VEC_16, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("to", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpAabbPhantom", 0)
	HK_PATCH_DEPENDS("hkpPhantom", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpWorldObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpVehicleData", 0, "hkpVehicleData", 1)
	HK_PATCH_MEMBER_RENAMED("normalClippingAngle", "normalClippingAngleCos")
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexDecompositionShapeConfig", 0, "hkpConvexDecompositionShapeConfig", 1)
	HK_PATCH_MEMBER_ADDED("margin", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexDecompositionShapeConfig", 1, "hkpConvexDecompositionShapeConfig", 2)
	HK_PATCH_MEMBER_REMOVED("margin", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStabilizedSphereMotion", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpSphereMotion", HK_NULL)
	HK_PATCH_DEPENDS("hkpSphereMotion", 0)
	HK_PATCH_DEPENDS("hkpMotion", 2)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStabilizedBoxMotion", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpBoxMotion", HK_NULL)
	HK_PATCH_DEPENDS("hkpMotion", 2)
	HK_PATCH_DEPENDS("hkpBoxMotion", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpMotion", 2, "hkpMotion", 3)
	HK_PATCH_FUNCTION(hkpMotion_2_to_3)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexDecompositionShapeConfig", 2, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("createConnectivity", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("removeUnreferencedVertices", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minCosAngle", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minimumArea", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("radius", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexDecompositionConfiguration2", 1, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("minReachableDistance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minOverlapDistance", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minExtrusion", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxExtrusion", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("minEdgeTetraVolume", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxMergeError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxOverlapMergeError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxOverlapPenetration", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxSkinError", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxOverlapVolume", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxPieces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("maxDepth", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("forcedOverlapResolution", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("searchForTetrahedrons", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("enableFlooding", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("enableMerging", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("enableIterativeOverlapSolver", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpExtendedMeshShape", 1, "hkpExtendedMeshShape", 2)
	HK_PATCH_MEMBER_ADDED("cachedNumChildShapes", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()


HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCompressedMeshShapeBigTriangle", 0)
	HK_PATCH_MEMBER_ADDED("a", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("b", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("c", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("m", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("weldingInfo", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCompressedMeshShapeChunk", 0)
	HK_PATCH_MEMBER_ADDED("offset", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("vertices", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("indices", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("stripLengths", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("weldingInfo", TYPE_ARRAY_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCompressedMeshShape", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpShapeCollection")
	HK_PATCH_MEMBER_ADDED("radius", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("weldingType", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bigVertices", TYPE_ARRAY_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bigTriangles", TYPE_ARRAY_STRUCT, "hkpCompressedMeshShapeBigTriangle", 0)
	HK_PATCH_MEMBER_ADDED("chunks", TYPE_ARRAY_OBJECT, "hkpCompressedMeshShapeChunk", 0)
	HK_PATCH_MEMBER_ADDED("error", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bounds", TYPE_STRUCT, "hkAabb", 0)
	HK_PATCH_DEPENDS("hkpShapeCollection", 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkpCompressedMeshShapeBigTriangle", 0)
	HK_PATCH_DEPENDS("hkAabb", 0)
	HK_PATCH_DEPENDS("hkpCompressedMeshShapeChunk", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 0, "hkpCompressedMeshShape", 1)
	HK_PATCH_MEMBER_ADDED("numChunkTriangles", TYPE_ARRAY_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeBigTriangle", 0, "hkpCompressedMeshShapeBigTriangle", 1)
	HK_PATCH_MEMBER_REMOVED("m", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("material", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 1, "hkpCompressedMeshShape", 2)
	HK_PATCH_MEMBER_ADDED("bitsPerIndex", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("bitsPerWIndex", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("wIndexMask", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("indexMask", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materialStridingType", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materialStriding", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeChunk", 0, "hkpCompressedMeshShapeChunk", 1)
	HK_PATCH_MEMBER_ADDED("materials", TYPE_ARRAY_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 2, "hkpCompressedMeshShape", 3)
	HK_PATCH_MEMBER_REMOVED("materialStridingType", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("materialStriding", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materialType", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materials", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materials16", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materials8", TYPE_ARRAY_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeChunk", 1, "hkpCompressedMeshShapeChunk", 2)
	HK_PATCH_MEMBER_REMOVED("materials", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("materialInfo", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 3, "hkpCompressedMeshShape", 4)
	HK_PATCH_MEMBER_ADDED("transforms", TYPE_ARRAY_VEC_16, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeChunk", 2, "hkpCompressedMeshShapeChunk", 3)
	HK_PATCH_MEMBER_ADDED("chunkRef", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("transformIndex", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpEntity", 1, "hkpEntity", 2)
	HK_PATCH_MEMBER_RENAMED("processContactCallbackDelay", "contactPointCallbackDelay")
	HK_PATCH_MEMBER_RENAMED("numUserDatasInContactPointProperties", "numShapeKeysInContactPointProperties")
	HK_PATCH_MEMBER_ADDED("responseModifierFlags", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCompressedMeshShapeConvexPiece", 0)
	HK_PATCH_MEMBER_ADDED("offset", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("vertices", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("faceVertices", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("faceOffsets", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("reference", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("transformIndex", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 4, "hkpCompressedMeshShape", 5)
	HK_PATCH_MEMBER_ADDED("convexPieces", TYPE_ARRAY_OBJECT, "hkpCompressedMeshShapeConvexPiece", 0)
	HK_PATCH_DEPENDS("hkpCompressedMeshShapeConvexPiece", 0)
	//HK_PATCH_FUNCTION(hkpCompressedMeshShape_4_to_5)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpPolytopeShape", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpConvexShape")
	HK_PATCH_MEMBER_ADDED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("externalFacesBitfield", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numExternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("facesOffsets", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numInternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("faceVertices", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("edgesWeldingValues", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("vertices", TYPE_ARRAY_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpConvexShape", 0)
	HK_PATCH_DEPENDS("hkpSphereRepShape", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeConvexPiece", 0, "hkpCompressedMeshShapeConvexPiece", 1)
	HK_PATCH_MEMBER_REMOVED("faceOffsets", TYPE_ARRAY_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("faceOffsets", TYPE_ARRAY_INT, HK_NULL, 0)
	//HK_PATCH_FUNCTION(hkpCompressedMeshShapeConvexPiece_0_to_1)
HK_PATCH_END()

HK_PATCH_BEGIN("hkWorldMemoryAvailableWatchDog", 0, "hkWorldMemoryAvailableWatchDog", 1)
	HK_PATCH_MEMBER_REMOVED("minMemoryAvailable", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpDefaultWorldMemoryWatchDog", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkWorldMemoryAvailableWatchDog")
	HK_PATCH_MEMBER_ADDED("minHeapMemoryAvailable", TYPE_INT, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkWorldMemoryAvailableWatchDog", 1)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpNamedMeshMaterial", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpMeshMaterial")
	HK_PATCH_MEMBER_ADDED("name", TYPE_CSTRING, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeMeshSubpartStorage", 2, "hkpStorageExtendedMeshShapeMeshSubpartStorage", 3)
	HK_PATCH_MEMBER_ADDED("namedMaterials", TYPE_ARRAY_STRUCT, "hkpNamedMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpNamedMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
HK_PATCH_END()

HK_PATCH_BEGIN(HK_NULL, HK_CLASS_ADDED, "hkpCompressedPolytopeShape", 0)
	HK_PATCH_PARENT_SET(HK_NULL, "hkpConvexShape")
	HK_PATCH_MEMBER_ADDED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("externalFacesBitfield", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numExternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("facesOffsets", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numInternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("faceVertices", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("edgesWeldingValues", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("vertices", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("quantization", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("offset", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("transform", TYPE_VEC_16, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpConvexShape", 0)
	HK_PATCH_DEPENDS("hkpSphereRepShape", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpPolytopeShape", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpConvexShape", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("externalFacesBitfield", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numExternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("facesOffsets", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numInternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("faceVertices", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("edgesWeldingValues", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("vertices", TYPE_ARRAY_VEC_4, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpConvexShape", 0)
	HK_PATCH_DEPENDS("hkpSphereRepShape", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 5, "hkpCompressedMeshShape", 6)
	HK_PATCH_MEMBER_ADDED("meshMaterials", TYPE_OBJECT, "hkpMeshMaterial", 0)
	HK_PATCH_MEMBER_ADDED("materialStriding", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("numMaterials", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("namedMaterials", TYPE_ARRAY_STRUCT, "hkpNamedMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpNamedMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
	//HK_PATCH_FUNCTION(hkpCompressedMeshShape_5_to_6)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeConvexPiece", 1, "hkpCompressedMeshShapeConvexPiece", 2)
	HK_PATCH_MEMBER_ADDED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeChunk", 3, "hkpCompressedMeshShapeChunk", 4)
	HK_PATCH_MEMBER_REMOVED("chunkRef", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("reference", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 3, "hkpWorldCinfo", 4)
	HK_PATCH_MEMBER_ADDED("fireCollisionCallbacks", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConstraintInstanceSmallArraySerializeOverrideType", 0, "hkpConstraintInstanceSmallArraySerializeOverrideType", 1)
	HK_PATCH_MEMBER_REMOVED("data", TYPE_OBJECT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpEntitySmallArraySerializeOverrideType", 0, "hkpEntitySmallArraySerializeOverrideType", 1)
	HK_PATCH_MEMBER_REMOVED("data", TYPE_OBJECT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpDefaultWorldMemoryWatchDog", 0, "hkpDefaultWorldMemoryWatchDog", 1)
	HK_PATCH_MEMBER_REMOVED("minHeapMemoryAvailable", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("freeHeapMemoryRequested", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpDisplayBindingData", 0, "hkpDisplayBindingData", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkpPhysicsSystemDisplayBinding", 0, "hkpDisplayBindingDataPhysicsSystem", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkpRigidBodyDisplayBinding", 0, "hkpDisplayBindingDataRigidBody", 1)
	HK_PATCH_PARENT_SET(HK_NULL, "hkReferencedObject")
HK_PATCH_END()

HK_PATCH_BEGIN("hkpSimpleShapePhantomCollisionDetail", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("collidable", TYPE_OBJECT, "hkpCollidable", 0)
	HK_PATCH_DEPENDS("hkpCdBody", 0)
	HK_PATCH_DEPENDS("hkpCollidable", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpShapeRayCastInput", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("from", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("to", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("filterInfo", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("rayShapeCollectionFilter", TYPE_OBJECT, "hkpRayShapeCollectionFilter", 0)
	HK_PATCH_DEPENDS("hkpRayShapeCollectionFilter", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpShapeRayBundleCastInput", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_MEMBER_REMOVED("from", TYPE_TUPLE_VEC_4, HK_NULL, 3)
	HK_PATCH_MEMBER_REMOVED("to", TYPE_TUPLE_VEC_4, HK_NULL, 3)
	HK_PATCH_MEMBER_REMOVED("filterInfo", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("rayShapeCollectionFilter", TYPE_OBJECT, "hkpRayShapeCollectionFilter", 0)
	HK_PATCH_DEPENDS("hkpRayShapeCollectionFilter", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCdBody", 0, "hkpCdBody", 1)
	HK_PATCH_MEMBER_REMOVED("parent", TYPE_OBJECT, "hkpCdBody", 0)
	HK_PATCH_MEMBER_ADDED("parent", TYPE_VOID, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 4, "hkpWorldCinfo", 5)
	HK_PATCH_MEMBER_ADDED("treeUpdateType", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_ADDED("useMultipleTree", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedPolytopeShape", 0, HK_NULL, HK_CLASS_REMOVED)
	HK_PATCH_PARENT_SET("hkpConvexShape", HK_NULL)
	HK_PATCH_MEMBER_REMOVED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("externalFacesBitfield", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numExternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("facesOffsets", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("numInternalFaces", TYPE_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("faceVertices", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("edgesWeldingValues", TYPE_OBJECT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("vertices", TYPE_ARRAY_INT, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("quantization", TYPE_REAL, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("offset", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("transform", TYPE_VEC_16, HK_NULL, 0)
	HK_PATCH_DEPENDS("hkBaseObject", 0)
	HK_PATCH_DEPENDS("hkpShape", 0)
	HK_PATCH_DEPENDS("hkReferencedObject", 0)
	HK_PATCH_DEPENDS("hkpConvexShape", 0)
	HK_PATCH_DEPENDS("hkpSphereRepShape", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShapeConvexPiece", 2, "hkpCompressedMeshShapeConvexPiece", 3)
	HK_PATCH_MEMBER_REMOVED("aabbCenter", TYPE_VEC_4, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("aabbHalfExtents", TYPE_VEC_4, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpConvexVerticesShape", 1, "hkpConvexVerticesShape", 2)
	HK_PATCH_MEMBER_ADDED("convexPiece", TYPE_OBJECT, "hkpCompressedMeshShapeConvexPiece", 0)
	HK_PATCH_DEPENDS("hkpCompressedMeshShapeConvexPiece", 2)
	//HK_PATCH_FUNCTION(hkpConvexVerticesShape_1_to_2)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 5, "hkpWorldCinfo", 6)
	HK_PATCH_MEMBER_ADDED("maxNumToiCollisionPairsSinglethreaded", TYPE_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 6, "hkpWorldCinfo", 7)
	HK_PATCH_MEMBER_ADDED("maxConstraintViolation", TYPE_REAL, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 6, "hkpCompressedMeshShape", 7)
	HK_PATCH_MEMBER_REMOVED("meshMaterials", TYPE_OBJECT, "hkpMeshMaterial", 0)
	HK_PATCH_DEPENDS("hkpMeshMaterial", 0)
	//HK_PATCH_FUNCTION(hkpCompressedMeshShape_6_to_7)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpCompressedMeshShape", 7, "hkpCompressedMeshShape", 8)
	HK_PATCH_MEMBER_REMOVED("numChunkTriangles", TYPE_ARRAY_INT, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpStorageExtendedMeshShapeShapeSubpartStorage", 1, "hkpStorageExtendedMeshShapeShapeSubpartStorage", 2)
	HK_PATCH_MEMBER_REMOVED("shapes", TYPE_ARRAY_OBJECT, "hkpConvexShape", 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpExtendedMeshShapeShapesSubpart", 0, "hkpExtendedMeshShapeShapesSubpart", 1)
	HK_PATCH_MEMBER_REMOVED("offsetSet", TYPE_BYTE, HK_NULL, 0)
	HK_PATCH_MEMBER_REMOVED("rotationSet", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

HK_PATCH_BEGIN("hkpWorldCinfo", 7, "hkpWorldCinfo", 8)
        HK_PATCH_MEMBER_ADDED("allowIntegrationOfIslandsWithoutConstraintsInASeparateJob", TYPE_BYTE, HK_NULL, 0)
HK_PATCH_END()

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20091222)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/