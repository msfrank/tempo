#ifndef TEMPO_CONFIG_MERGE_MAP_H
#define TEMPO_CONFIG_MERGE_MAP_H

#include "config_types.h"

namespace tempo_config {

    ConfigMap merge_map(const ConfigMap &base, const ConfigMap &overrides);
};

#endif // TEMPO_CONFIG_MERGE_MAP_H
