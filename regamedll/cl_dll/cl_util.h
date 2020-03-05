/*

Created Date: 05 Mar 2020

*/

#pragma once

#ifdef VectorSubtract
#undef VectorSubtract
#endif
#ifdef VectorAdd
#undef VectorAdd
#endif
#ifdef VectorCopy
#undef VectorCopy
#endif
#ifdef VectorClear
#undef VectorClear
#endif

template<class VectorTypeA, class VectorTypeB> auto DotProduct(const VectorTypeA& x, const VectorTypeB& y) -> decltype((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]) { return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]); }
template<class VectorTypeA, class VectorTypeB, class VectorTypeC> void VectorSubtract(const VectorTypeA& a, const VectorTypeB& b, VectorTypeC& c) { (c)[0] = (a)[0] - (b)[0]; (c)[1] = (a)[1] - (b)[1]; (c)[2] = (a)[2] - (b)[2]; }
template<class VectorTypeA, class VectorTypeB, class VectorTypeC> void VectorAdd(const VectorTypeA& a, const VectorTypeB& b, VectorTypeC& c) { (c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2]; }
template<class VectorTypeA, class VectorTypeB> void VectorCopy(const VectorTypeA& a, VectorTypeB& b) { (b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2]; }
template<class VectorTypeA> void VectorClear(VectorTypeA& a) { a[0] = 0.0; a[1] = 0.0; a[2] = 0.0; }
template<class VectorTypeA> auto VectorLength(const VectorTypeA& a) -> decltype(sqrt(DotProduct(a, a))) { return sqrt(DotProduct(a, a)); }
template<class VectorTypeA, class ScaleType, class VectorTypeB, class VectorTypeC> void VectorMA(const VectorTypeA& a, ScaleType scale, const VectorTypeB& b, VectorTypeC& c) { ((c)[0] = (a)[0] + (scale) * (b)[0], (c)[1] = (a)[1] + (scale) * (b)[1], (c)[2] = (a)[2] + (scale) * (b)[2]); }
template<class VectorTypeA, class ScaleType, class VectorTypeB> void VectorScale(const VectorTypeA& in, ScaleType scale, VectorTypeB& out) { ((out)[0] = (in)[0] * (scale), (out)[1] = (in)[1] * (scale), (out)[2] = (in)[2] * (scale)); }
template<class VectorTypeA> void VectorInverse(VectorTypeA& x) { ((x)[0] = -(x)[0], (x)[1] = -(x)[1], (x)[2] = -(x)[2]); }
template<class VectorTypeA> void AngleVectors(const VectorTypeA& vecAngles, float* forward, float* right, float* up) { return gEngfuncs.pfnAngleVectors(vecAngles, forward, right, up); }
