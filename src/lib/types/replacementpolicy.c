#include "../helpers/helpers.h"
#include "cache.h"
#include <string.h>

char *ReplacementPolicy_to_string(ReplacementPolicy rp) {
    switch (rp) {
    case RP_ROUND_ROBIN:
        return "Round Robin";
        break;
    case RP_RANDOM:
        return "Random";
        break;
    default:
        return "";
    }
}

/**
 * @return The replacement policy for the given string or -1 if the replacement
 * policy could not be converted
 */
ReplacementPolicy ReplacementPolicy_from_string(char *replacement_policy) {
    char *rp_lower = str_lower(replacement_policy);
    if (!strcmp(rp_lower, "round robin") || !strcmp(rp_lower, "rr")) {
        return RP_ROUND_ROBIN;
    } else if (!strcmp(rp_lower, "random") || !strcmp(rp_lower, "rnd")) {
        return RP_RANDOM;
    }
    return RP_UNKNOWN;
}
