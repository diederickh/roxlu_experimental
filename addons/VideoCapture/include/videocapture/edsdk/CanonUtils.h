#ifndef ROXLU_CANON_UTILS_H
#define ROXLU_CANON_UTILS_H

#include <EDSDKTypes.h>
#include <string>

/* Helpers */
bool canon_can_start_live_view(EdsAEMode m);

/* Debug */
std::string canon_data_type_to_string(EdsDataType dt);
std::string canon_error_to_string(EdsError err);
std::string canon_event_to_string(EdsUInt32 ev);
std::string canon_property_to_string(EdsUInt32 prop);
std::string canon_ae_mode_to_string(EdsAEMode mode);
std::string canon_metering_mode_to_string(EdsUInt32 mode);
std::string canon_evf_output_device_to_string(EdsUInt32 d);
std::string canon_evf_af_mode_to_string(EdsUInt32 m);
std::string canon_evf_zoom_to_string(EdsUInt32 z);
#endif
