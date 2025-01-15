#include<jsoncpp/json/json.h>

string json_encode(Json::Value val) {
    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = "";
    builder["emitUTF8"] = true;
    unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    ostringstream os;
    writer->write(val, &os);
    return os.str();
}

string json_pretty_encode(Json::Value val) {
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "    ";
    builder["emitUTF8"] = true;
    builder["enableYAMLCompatibility"] = true;
    unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    ostringstream os;
    writer->write(val, &os);
    return os.str();
}

bool json_decode(string json, Json::Value& res) {
    Json::Reader reader;
    return reader.parse(json, res);
}

Json::Value json_decode(string json) {
    Json::Value res;
    Json::Reader reader;
    reader.parse(json, res);
    return res;
}