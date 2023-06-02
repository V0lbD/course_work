#include "logger.h"

std::string logger::severity_to_string_logger(severity s) {
    switch (s) {
        case severity::trace:
            return "[ trace ] ";
        case severity::debug:
            return "[ debug ] ";
        case severity::information:
            return "[ information ] ";
        case severity::warning:
            return "[ warning ] ";
        case severity::error:
            return "[ error ] ";
        case severity::critical:
            return "[ critical ] ";
        default:
            throw severity_exception("invalid severity value used");
    }
}

logger::severity logger::from_string_to_severity_parse(const std::string &s) {
    if (s == "trace") {
        return logger::severity::trace;
    }
    if (s == "debug") {
        return logger::severity::debug;
    }
    if (s == "information") {
        return logger::severity::information;
    }
    if (s == "warning") {
        return logger::severity::warning;
    }
    if (s == "error") {
        return logger::severity::error;
    }
    if (s == "critical") {
        return logger::severity::critical;
    } else {
        throw logger::severity_exception("invalid severity string passed");
    }
}

logger_impl::~logger_impl() {
    std::pair<std::ofstream *, size_t> stream_pair;
    for (auto &this_logger_stream: this_logger_streams) {
        if ((stream_pair = logger_impl::_all_loggers_streams[this_logger_stream.first]).second == 1) {
            if (stream_pair.first != nullptr) {
                stream_pair.first->flush();
                stream_pair.first->close();
            }
            logger_impl::_all_loggers_streams.erase(this_logger_stream.first);
//            this_logger_streams.erase(this_logger_stream.first);
        } else {
            logger_impl::_all_loggers_streams[this_logger_stream.first].second--;
        }
    }
}

logger_impl::logger_impl(std::map<std::string, severity> const &info_to_construct_from)
        : this_logger_streams(info_to_construct_from) {
    std::pair<std::ofstream *, size_t> stream_pair;
    for (auto &this_logger_stream: this_logger_streams) {
        auto iterator = _all_loggers_streams.find(this_logger_stream.first);
        if (iterator == _all_loggers_streams.end()) {
            std::ofstream *new_stream = nullptr;
            if (this_logger_stream.first != "console") {
                new_stream = new std::ofstream();
                new_stream->open(this_logger_stream.first);
                // todo: if new_stream is not opened, throw a message
                *new_stream << "<--- " << severity_to_string_logger(this_logger_stream.second) << " --->" << std::endl;
            }
            _all_loggers_streams.insert(std::make_pair(this_logger_stream.first, std::make_pair(new_stream, 1)));
        } else {
            iterator->second.second++;
        }
    }
}

logger const *logger_impl::log(std::string const &target, severity level) const {
    for (const auto &this_loggers_stream: this_logger_streams) {
        if (this_loggers_stream.second <= level) {
            time_t raw_time;
            struct tm *time_info;
            char curr_time[80];

            time(&raw_time);
            time_info = localtime(&raw_time);
            strftime(curr_time, 80, "[ %d/%m/%Y %H:%M:%S ]", time_info);

            if (this_loggers_stream.first == "console") {
                std::cout << curr_time << severity_to_string_logger(level) << target << std::endl;
            } else {
                (*logger_impl::_all_loggers_streams[this_loggers_stream.first].first) << curr_time
                                                                                      << severity_to_string_logger(
                                                                                              level) << target
                                                                                      << std::endl;
            }
        }
    }
    return this;
}

std::map<std::string, std::pair<std::ofstream *, size_t>> logger_impl::_all_loggers_streams =
        std::map<std::string, std::pair<std::ofstream *, size_t>>();

