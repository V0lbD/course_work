#ifndef LOGGER_BASE_CLASS_H
#define LOGGER_BASE_CLASS_H

#include <iostream>
#include <exception>
#include "C:\rapidjson-master\include\rapidjson\document.h"
#include "C:\rapidjson-master\include\rapidjson\istreamwrapper.h"

#include <fstream>
#include <map>
#include <string>
#include <chrono>

class logger_builder;

class logger {
public:
    enum class severity {
        trace = 0,
        debug,
        information,
        warning,
        error,
        critical
    };

    class severity_exception final : public std::exception {
    private:
        std::string _message;

    public:
        explicit severity_exception(std::string message)
                : _message(std::move(message)) {
        }

        [[nodiscard]] char const *what() const noexcept override {
            return _message.c_str();
        }

    };

    static std::string severity_to_string_logger(severity s);

    static logger::severity from_string_to_severity_parse(const std::string &s);

    virtual logger const *log(std::string const &target, severity level) const = 0;

    virtual ~logger() noexcept = default;
};

class logger_impl : public logger {
private:
    std::map<std::string, severity> this_logger_streams;
    static std::map<std::string, std::pair<std::ofstream *, size_t> > _all_loggers_streams;

    explicit logger_impl(std::map<std::string, severity> const &info_to_construct_from);

public:
    logger_impl(logger_impl const &log) = delete;        // copying
    logger_impl &operator=(logger_impl const &) = delete; // assignment

    friend class logger_builder_impl;

    // static logger_builder builder();
    ~logger_impl();

    logger const *log(std::string const &target, severity level) const override;
};

#endif //LOGGER_BASE_CLASS_H
