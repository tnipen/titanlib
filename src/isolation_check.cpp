#include <vector>
#include <math.h>
#include "titanlib.h"
#include <assert.h>
#include <iostream>

bool titanlib::isolation_check(const fvec lats,
        const fvec lons,
        int nmin,
        float radius,
        ivec& flags) {
    titanlib::KDTree tree(lats, lons);
    flags.resize(lats.size());

    for(int i = 0; i < lats.size(); i++) {
        int num = tree.get_num_neighbours(lats[i], lons[i], radius);
        // std::cout << i << " " << num << std::endl;
        if(num < nmin) {
            flags[i] = 1;
        }
    }

    return true;

}

bool titanlib::isolation_check(const fvec lats,
        const fvec lons,
        const fvec elevs,
        int nmin,
        float radius,
        float dz,
        ivec& flags) {

    titanlib::KDTree tree(lats, lons);
    flags.resize(lats.size());

    for(int i = 0; i < lats.size(); i++) {
        ivec indices = tree.get_neighbours(lats[i], lons[i], radius);
        int num = 0;
        for(int j = 0; j < indices.size(); j++) {
            int index = indices[j];
            if(fabs(elevs[index] - elevs[i]) < dz)
                num++;
        }
        // std::cout << i << " " << num << std::endl;
        if(num < nmin ) {
            flags[i] = 1;
        }
    }

    return true;
}
