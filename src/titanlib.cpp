#include <iostream>
#include <vector>
#include "titanlib.h"
#include <boost/math/distributions/gamma.hpp>

float calc_gamma(float shape, float scale) {
    boost::math::gamma_distribution<> dist(shape, scale);
    float value = boost::math::quantile(dist, 0.5);
    return value;
}
bool sct(const fvec lats,
        const fvec lons,
        const fvec elevs,
        const fvec values,
        int nmin,
        int nmax,
        int nminprof,
        float dzmin,
        float dhmin,
        float dz,
        const fvec t2pos,
        const fvec t2neg,
        const fvec eps2,
        ivec& flags) {
    flags.resize(lats.size());
    for(int i = 0; i < lats.size(); i++) {
        flags[i] = values[i] > 0;
    }
    return true;
}
/*
bool range_check(const fvec lats,
        const fvec lons,
        const fvec elevs,
        const fvec values,
        const fvec min,
        const fvec max,
        ivec& flags) {

    // loop over all the lats/lons/elevs + value 
    // either min/max has length 1 or is the same length as the other vecs
    const int s = lats.size();
    if( lons.size() != s || elevs.size() != s || values.size() != s ) { return false; }
    if( (min.size() != s && max.size() != s) || (min.size() != 1 && max.size() != 1) ) { return false; }

    flags.resize(s, 0);

    for(int i = 0; i < s; i++) {
        // leave the index to 0 if its the same max/min applied to everything
        // else same as loop
        int min_i = (min.size() == s) ? i : 0;
        int max_i = (max.size() == s) ? i : 0;

        // loop over the vectors and set the flags (0 = ok and 1 = bad)
        if(values[i] < min[min_i] || values[i] > max[max_i]) {
            flags[i] = 1;
        }
    }

    return true;

}
*/