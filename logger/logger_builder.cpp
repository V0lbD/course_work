#include "logger_builder.h"

logger_builder *logger_builder_impl::with_stream(const std::string &stream, logger::severity severity) {
    _under_construction_logger_setup[stream] = severity;
    return this;
}

logger *logger_builder_impl::build() const {
    return new logger_impl(_under_construction_logger_setup);
}

logger *logger_builder_impl::config_from_json(const std::string &filename) {
    std::ifstream config_file_ifs(filename);
    if (!(config_file_ifs.is_open())) {
        throw std::runtime_error("Configuration file not opened");
    }

    rapidjson::IStreamWrapper config_file_isw(config_file_ifs);
    rapidjson::Document doc;
    doc.ParseStream(config_file_isw);

    if (doc.HasParseError()) {
        config_file_ifs.close();
        throw std::runtime_error("Error parsing configuration file");
    }

    logger *log;
    const rapidjson::Value &streams = doc["streams"];
    rapidjson::Value::ConstValueIterator iter;
    for (iter = streams.Begin(); iter != streams.End(); ++iter) {
        this->with_stream(iter->GetObject()["name"].GetString(),
                          logger::from_string_to_severity_parse(iter->GetObject()["severity"].GetString()));
    }

    log = this->build();
    config_file_ifs.close();
    return log;
}