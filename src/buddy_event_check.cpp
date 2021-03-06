#include <vector>
#include <algorithm>
#include <iostream>
#define _USE_MATH_DEFINES
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <exception>
#include "titanlib.h"

ivec titanlib::buddy_event_check(const fvec& lats,
        const fvec& lons,
        const fvec& elevs,
        const fvec& values,
        const fvec& radius,
        const ivec& buddies_min,
        const fvec& event_thresholds,
        const fvec& thresholds,
        float diff_elev_max,
        float elev_gradient,
        const ivec obs_to_check) {

    bool debug = false;
    const int s = values.size();
    // assert that the arrays we expect are of size s
    if( lats.size() != s || lons.size() != s || elevs.size() != s || values.size() != s) {
        throw std::runtime_error("Dimension mismatch");
    }
    if( radius.size() != s && radius.size() != 1 ) {
        throw std::runtime_error("Dimension mismatch");
    }
    if( (buddies_min.size() != s && buddies_min.size() != 1) || (thresholds.size() != s && thresholds.size() != 1) ) {
        throw std::runtime_error("Dimension mismatch");
    }
    if( (obs_to_check.size() != s && obs_to_check.size() != 1 && obs_to_check.size() !=0) ) {
        throw std::runtime_error("Dimension mismatch");
    }

    // Check that buddies min is more than 0
    for(int i = 0; i < buddies_min.size(); i++) {
        if(buddies_min[i] <= 0)
            throw std::runtime_error("Buddies_min must be > 0");
    }

    // create the KD tree to be used later
    titanlib::KDTree tree(lats, lons);
    // resize the flags and set them to 0
    ivec flags(s, 0);
    // if obs_to_check is empty then check all
    bool check_all = (obs_to_check.size() == s) ? false : true;

    // loop over all the observations
    for(int i = 0; i < values.size(); i++) {
        // is this one we are supposed to check?
        int b_i = (buddies_min.size() == s) ? i : 0;
        int d_i = (radius.size() == s) ? i : 0;
        int t_i = (thresholds.size() == s) ? i : 0;
        if( ((!check_all && obs_to_check[i] == 1) || check_all) ) {
            if(debug) {
                std::cout << "point: " << lats[i] << " " << lons[i] << " " << elevs[i]; 
                std::cout << ", and min buddies: " << buddies_min[b_i];
                std::cout << '\n';
            }

            // get all neighbours that are close enough                 
            ivec neighbour_indices = tree.get_neighbours(lats[i], lons[i], radius[d_i], 0, false);

            int n_buddies = 0;
            fvec list_buddies;
            // based on tree do have enough neighbours? 
            if(neighbour_indices.size() > buddies_min[b_i]) {
                // loop over everything that was near enough
                // count buddies and make list of values (adjusting for height diff if needed)
                for(int j = 0; j < neighbour_indices.size(); j++) {
                    // don't use ones that differ too much in height (diff_elev_max)
                    if(diff_elev_max > 0) {
                        float elev_diff = fabs(elevs[neighbour_indices[j]] - elevs[i]);
                        if(elev_diff <= diff_elev_max) {
                            // correction for the elevation differences (add or subtract -0.0065 degC/m)
                            // m difference from point in question
                            float elev_diff = elevs[neighbour_indices[j]] - elevs[i];
                            //std::cout << "height diff: " << elev_diff;
                            float adjusted_value = values[neighbour_indices[j]] + (elev_diff * elev_gradient);
                            //std::cout << ", adjusted value: " << adjusted_value;
                            //std::cout << '\n';
                            float event_value = 0;
                            if(adjusted_value < event_thresholds[0])
                                event_value = 1;

                            list_buddies.push_back(event_value);
                            n_buddies++;
                        }
                        else {
                            if(debug) {
                                std::cout << "too much height difference: " << elev_diff << '\n';
                            }
                        }
                    }
                    // if diff_elev_max is negative then don't check elevation difference
                    else {
                        // can use this station
                        list_buddies.push_back(values[neighbour_indices[j]]);
                        n_buddies++;
                    }
                }

            }
            if(debug) {
                std::cout << "buddies: " << n_buddies << '\n';
            }
            if(n_buddies >= buddies_min[b_i]) {
                // compute the average and standard deviation of the values
                boost::accumulators::accumulator_set<float, boost::accumulators::features<boost::accumulators::tag::mean, boost::accumulators::tag::variance>> acc;
                int count = 0;
                int count_below = 0;
                for(int k = 0; k < list_buddies.size(); k++) {
                    acc(list_buddies[k]);
                    if(list_buddies[k] < event_thresholds[0])
                        count_below++;
                    count++;
                }
                float mean = boost::accumulators::mean(acc);
                float fraction = float(count_below) / count;
                float curr_event = 0;
                if(values[i] < event_thresholds[0])
                    curr_event = 1;

                if(thresholds[t_i] < 1) {
                    if(curr_event < event_thresholds[0] && fraction <= thresholds[t_i])
                        flags[i] = 1;
                    else if(curr_event >= event_thresholds[0] && (1 - fraction) <= thresholds[t_i])
                        flags[i] = 1;
                }
                else {
                    if(curr_event < event_thresholds[0] && count_below <= thresholds[t_i])
                        flags[i] = 1;
                    else if(curr_event >= event_thresholds[0] && (count - count_below <= thresholds[t_i]))
                        flags[i] = 1;
                }
                if(debug && values[i] > 40) {
                    std::cout << "value: " << values[i] << '\n';
                    std::cout << "curr_value: " << curr_event << '\n';
                    std::cout << "event_threshold: " << event_thresholds[0] << '\n';
                    std::cout << "threshold: " << thresholds[t_i] << '\n';
                    std::cout << "count: " << count << '\n';
                    std::cout << "count_below: " << count_below << '\n';
                    std::cout << "fraction: " << fraction << '\n';
                    std::cout << "flag: " << flags[i] << '\n';
                }
            }
        }
    }

    return flags;
}
