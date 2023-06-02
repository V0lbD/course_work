#ifndef LOGGER_HOLDER_H
#define LOGGER_HOLDER_H

#include "logger.h"

class logger_holder
{

public:
    virtual ~logger_holder() noexcept = default;

public:

    logger_holder const *log_with_guard(
            std::string const &message,
            logger::severity severity) const;

    logger_holder const *trace_with_guard(std::string const &message) const;

    logger_holder const *debug_with_guard(std::string const &message) const;

    logger_holder const *information_with_guard(std::string const &message) const;

    logger_holder const *warning_with_guard(std::string const &message) const;

    logger_holder const *error_with_guard(std::string const &message) const;

    logger_holder const *critical_with_guard(std::string const &message) const;

protected:

    virtual logger *get_logger() const noexcept = 0;
};

#endif //LOGGER_HOLDER_H
