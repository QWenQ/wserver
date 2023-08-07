#include "timeUtils.h"

std::string timeStamp() {
    using namespace std::chrono;
    using clock = high_resolution_clock;

    const auto current_time_point { clock::now() };
    const auto current_time { clock::to_time_t(current_time_point) };
    const auto current_localtime { *std::localtime(&current_time) };
    const auto current_time_since_epoch { current_time_point.time_since_epoch() };
    const auto current_seconds { duration_cast<seconds>(current_time_since_epoch).count() };
    const auto current_microseconds { duration_cast<microseconds>(current_time_since_epoch).count() };

    std::ostringstream stream;
    stream << std::put_time(&current_localtime, "%F %T") << ":" << std::setfill('0') << (current_microseconds % current_seconds);
    return stream.str();
}