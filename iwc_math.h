#ifndef IWC_MATH_H
#define IWC_MATH_H

typedef struct {
    float x, y;
} iwc_Vec2;
typedef struct {
    float x, y, z;
} iwc_Vec3;
typedef struct {
    float aa, ab, ba, bb;
} iwc_Mtx22;
typedef struct {
    float aa,ab,ac,ba,bb,bc,ca,cb,cc;
} iwc_Mtx33;

#define IWC_MAT33_IDENTITY (iwc_Mtx33){\
        1, 0, 0,\
        0, 1, 0,\
        0, 0, 1,\
    }

iwc_Vec2 iwc_v2Sub(iwc_Vec2 lhs, iwc_Vec2 rhs);
iwc_Vec2 iwc_v2Add(iwc_Vec2 lhs, iwc_Vec2 rhs);
iwc_Vec2 iwc_v2Scale(iwc_Vec2 v, float amount);
float iwc_v2LenSqr(iwc_Vec2 v);
float iwc_v2Len(iwc_Vec2 v);
iwc_Vec2 iwc_v2Norm(iwc_Vec2 v);

iwc_Vec3 iwc_v3Sub(iwc_Vec3 lhs, iwc_Vec3 rhs);
iwc_Vec3 iwc_v3Add(iwc_Vec3 lhs, iwc_Vec3 rhs);
iwc_Vec3 iwc_v3Scale(iwc_Vec3 v, float amount);
iwc_Vec3 iwc_v3Cross(iwc_Vec3 lhs, iwc_Vec3 rhs);
float iwc_v3LenSqr(iwc_Vec3 v);
float iwc_v3Len(iwc_Vec3 v);
iwc_Vec3 iwc_v3Norm(iwc_Vec3 v);

iwc_Vec3 iwc_mtx33MulVec(iwc_Mtx33 mtx, iwc_Vec3 v);
iwc_Mtx33 iwc_mtx33Mul(iwc_Mtx33 lhs, iwc_Mtx33 rhs);
/**
 * Inverts a 3x3 matrix, but without scaling by the determinant. Useful if you
 * know the determinant of your matrix is 1
 */
iwc_Mtx33 iwc_mtx33InvUnscaled(iwc_Mtx33 mtx);
iwc_Mtx33 iwc_mtx33LookAt(iwc_Vec3 v);

iwc_Vec2 iwc_mtx22MulVec(iwc_Mtx22 mtx, iwc_Vec2 v);
iwc_Mtx22 iwc_mtx22Mul(iwc_Mtx22 lhs, iwc_Mtx22 rhs);


iwc_Mtx33 rotX(float theta);
iwc_Mtx33 rotY(float theta);
iwc_Mtx33 rotZ(float theta);

#ifdef IWC_MATH_IMPLEMENTATION

#include <math.h>

iwc_Vec2 iwc_v2Sub(iwc_Vec2 lhs, iwc_Vec2 rhs) {
    return (iwc_Vec2){
        lhs.x - rhs.x,
        lhs.y - rhs.y,
    };
}
iwc_Vec2 iwc_v2Add(iwc_Vec2 lhs, iwc_Vec2 rhs) {
    return (iwc_Vec2){
        lhs.x + rhs.x,
        lhs.y + rhs.y,
    };
}
iwc_Vec2 iwc_v2Scale(iwc_Vec2 v, float amount) {
    return (iwc_Vec2){v.x * amount, v.y * amount};
}
float iwc_v2LenSqr(iwc_Vec2 v) {
    return v.x*v.x + v.y*v.y;
}
float iwc_v2Len(iwc_Vec2 v) {
    return sqrtf(iwc_v2LenSqr(v));
}
iwc_Vec2 iwc_v2Norm(iwc_Vec2 v) {
    return iwc_v2Scale(v, 1/iwc_v2Len(v));
}

iwc_Vec3 iwc_v3Sub(iwc_Vec3 lhs, iwc_Vec3 rhs) {
    return (iwc_Vec3){
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
    };
}
iwc_Vec3 iwc_v3Add(iwc_Vec3 lhs, iwc_Vec3 rhs) {
    return (iwc_Vec3){
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
    };
}
iwc_Vec3 iwc_v3Scale(iwc_Vec3 v, float amount) {
    return (iwc_Vec3){v.x * amount, v.y * amount, v.z * amount};
}
iwc_Vec3 iwc_v3Cross(iwc_Vec3 lhs, iwc_Vec3 rhs) {
    return (iwc_Vec3){
        lhs.y * rhs.z,
        lhs.x * rhs.z,
        lhs.x * rhs.y,
    };
}
float iwc_v3LenSqr(iwc_Vec3 v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}
float iwc_v3Len(iwc_Vec3 v) {
    return sqrtf(iwc_v3LenSqr(v));
}
iwc_Vec3 iwc_v3Norm(iwc_Vec3 v) {
    return iwc_v3Scale(v, 1/iwc_v3Len(v));
}

iwc_Vec3 iwc_mtx33MulVec(iwc_Mtx33 mtx, iwc_Vec3 v) {
    return (iwc_Vec3){
        .x = mtx.aa*v.x + mtx.ab*v.y + mtx.ac*v.z,
        .y = mtx.ba*v.x + mtx.bb*v.y + mtx.bc*v.z,
        .z = mtx.ca*v.x + mtx.cb*v.y + mtx.cc*v.z,
    };
}
iwc_Mtx33 iwc_mtx33Mul(iwc_Mtx33 lhs, iwc_Mtx33 rhs) {
    return (iwc_Mtx33){
        lhs.aa*rhs.aa + lhs.ab*rhs.ba + lhs.ac*rhs.ca, lhs.aa*rhs.ab + lhs.ab*rhs.bb + lhs.ac*rhs.cb, lhs.aa*rhs.ac + lhs.ab*rhs.bc + lhs.ac*rhs.cc,
        lhs.ba*rhs.aa + lhs.bb*rhs.ba + lhs.bc*rhs.ca, lhs.ba*rhs.ab + lhs.bb*rhs.bb + lhs.bc*rhs.cb, lhs.ba*rhs.ac + lhs.bb*rhs.bc + lhs.bc*rhs.cc,
        lhs.ca*rhs.aa + lhs.cb*rhs.ba + lhs.cc*rhs.ca, lhs.ca*rhs.ab + lhs.cb*rhs.bb + lhs.cc*rhs.cb, lhs.ca*rhs.ac + lhs.cb*rhs.bc + lhs.cc*rhs.cc,
    };
}

iwc_Mtx33 iwc_mtx33InvUnscaled(iwc_Mtx33 mtx) {
    return (iwc_Mtx33){
        mtx.bb*mtx.cc - mtx.bc*mtx.cb, mtx.ac*mtx.cb - mtx.ab*mtx.cc, mtx.ab*mtx.bc - mtx.ac*mtx.bb,
        mtx.bc*mtx.ca - mtx.ba*mtx.cc, mtx.aa*mtx.cc - mtx.ac*mtx.ca, mtx.ac*mtx.ba - mtx.aa*mtx.bc,
        mtx.ba*mtx.cb - mtx.bb*mtx.ca, mtx.ab*mtx.ca - mtx.aa*mtx.cb, mtx.aa*mtx.bb - mtx.ba*mtx.ab,
    };
}

iwc_Mtx33 iwc_mtx33LookAt(iwc_Vec3 v) {
    iwc_Vec3 zax = iwc_v3Norm(v);
    iwc_Vec3 xax = iwc_v3Norm(iwc_v3Cross(v, (iwc_Vec3){0, 1, 0}));
    iwc_Vec3 yax = iwc_v3Cross(zax, xax);
    return (iwc_Mtx33){
        xax.x, yax.x, zax.x,
        xax.y, yax.y, zax.y,
        xax.z, yax.z, zax.z,
    };
}

iwc_Vec2 iwc_mtx22MulVec(iwc_Mtx22 mtx, iwc_Vec2 v) {
    return (iwc_Vec2){
        mtx.aa*v.x + mtx.ab*v.y,
        mtx.ba*v.x + mtx.bb*v.y,
    };
}

iwc_Mtx22 iwc_mtx22Mul(iwc_Mtx22 lhs, iwc_Mtx22 rhs) {
    return (iwc_Mtx22){
        lhs.aa*rhs.aa + lhs.ab*rhs.ba, lhs.aa*rhs.ab + lhs.ab*rhs.bb,
        lhs.ba*rhs.aa + lhs.bb*rhs.ba, lhs.ba*rhs.ab + lhs.bb*rhs.bb,
    };
}


iwc_Mtx33 rotX(float theta) {
    float s = sinf(theta);
    float c = cosf(theta);
    return (iwc_Mtx33) {
        1, 0, 0,
        0, c,-s,
        0, s, c,
    };
}
iwc_Mtx33 rotY(float theta) {
    float s = sinf(theta);
    float c = cosf(theta);
    return (iwc_Mtx33) {
        c, 0, s,
        0, 1, 0,
       -s, 0, c,
    };
}
iwc_Mtx33 rotZ(float theta) {
    float s = sinf(theta);
    float c = cosf(theta);
    return (iwc_Mtx33) {
        c,-s, 0,
        s, c, 0,
        0, 0, 1,
    };
}


#endif // IWC_MATH_IMPLEMENTATION

#endif // IWC_MATH_H
