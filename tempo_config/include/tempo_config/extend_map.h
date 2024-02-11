#ifndef TEMPO_CONFIG_EXTEND_MAP_H
#define TEMPO_CONFIG_EXTEND_MAP_H

#include <tempo_utils/result.h>

#include "config_types.h"

namespace tempo_config {

    tempo_utils::Result<ConfigMap> extend_map(const ConfigMap &base, const ConfigMap &overrides);
};

#endif // TEMPO_CONFIG_EXTEND_MAP_H