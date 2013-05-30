#ifndef ROXLU_CANON_UTILS_H
#define ROXLU_CANON_UTILS_H

#include <string>

std::string canon_data_type_to_string(EdsDataType dt);
std::string canon_error_to_string(EdsError err);
std::string canon_event_to_string(EdsUInt32 ev);
std::string canon_property_to_string(EdsUInt32 prop);

#endif
