#ifndef LAB5_LOGGER_BUILDER_H
#define LAB5_LOGGER_BUILDER_H

#include "logger.h"

class logger_builder {
public:
    virtual logger_builder *with_stream(std::string const &, logger::severity) = 0;

    [[nodiscard]] virtual logger *build() const = 0;

    virtual ~logger_builder() {}

    virtual logger *config_from_json(const std::string &) = 0;
};

class logger_builder_impl final : public logger_builder {
private:
    std::map<std::string, logger::severity> _under_construction_logger_setup;
public:
    logger_builder *with_stream(const std::string &stream, logger::severity severity) override;

    [[nodiscard]] logger *build() const override;

    logger *config_from_json(const std::string &filename) override;
};

#endif //LAB5_LOGGER_BUILDER_H
