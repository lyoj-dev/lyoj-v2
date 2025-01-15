auto ConfigurationsLanguages = [](client_conn conn, http_request request, param argv) {
    Json::Value judge = json_decode(readFile("../judge/judge.json"));

    Json::Value object;
    object["code"] = 200;
    object["msg"] = http_code[200];
    object["items"] = judge["languages"];

    string responseBody = json_encode(object);
    __api_default_response["Content-Length"] = to_string(responseBody.size());
    putRequest(conn, 200, __api_default_response);
    send(conn, responseBody);
    exitRequest(conn);
};