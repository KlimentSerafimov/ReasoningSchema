//
// Created by kliment on 4/20/20.
//

#ifndef COMPACTPOSET_METRICTYPE_H
#define COMPACTPOSET_METRICTYPE_H

#include <string>
using namespace std;
enum MetricType {min_imp_set, most_progress, first_from_user_prior};
const string metric_type_name[30] = {"min_imp_set", "most_progress", "first_from_user_prior"};

enum ModeType {progressive_prior_mode};
const string mode_type_name[30] = {"progressive_prior_mode"};


#endif //COMPACTPOSET_METRICTYPE_H
