#pragma once

#include<string>
#include<jsoncpp/json/json.h>

std::string json_encode(Json::Value val) {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";
    builder["emitUTF8"] = true;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ostringstream os;
    writer->write(val, &os);
    return os.str();
}

std::string json_pretty_encode(Json::Value val) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    builder["emitUTF8"] = true;
    builder["enableYAMLCompatibility"] = true;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ostringstream os;
    writer->write(val, &os);
    return os.str();
}

bool json_decode(std::string json, Json::Value& res) {
    Json::Reader reader;
    return reader.parse(json, res);
}

Json::Value json_decode(std::string json) {
    Json::Value res;
    Json::Reader reader;
    reader.parse(json, res);
    return res;
}