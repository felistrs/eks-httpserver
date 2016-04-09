#include <map>

#include "httpThings.h"
#include "utils/logger.h"
#include "utils/htmlWrapper.h"


namespace server {

HttpResponse::HttpResponse() {
    ChangeStatus(HttpResponse::Status::OK); // TODO: OK ?
}

void HttpResponse::ChangeStatus(unsigned status_code)
{
    _response_status = status_code;

    auto it = c_status_string.find(status_code);
    if (it != c_status_string.end()) {
        _response_status_text = it->second;
    }
    else {
        _response_status_text = "Error";
        warning("Unknown status : " + std::to_string(status_code));
    }

    // Set headers to default
    HtmlWrapper html;
    html.push_text( std::to_string(_response_status) + " : " + _response_status_text );
    std::string content = html.GenerateHtml();

    push_header(HttpResponse::Header::ContentType, "text/html");
    push_header(HttpResponse::Header::ContentLength, std::to_string(content.size()) );
    set_content(content);
}

DataBuffer server::HttpResponse::Generate()
{
    // Generate response
    std::string data =
            _protocol + " " +
            std::to_string(_response_status) + " " +
            _response_status_text + c_new_line;

    for (const auto& header : _headers) {
        data += header.first + ": " + header.second + c_new_line;
    }

    if (_content.size()) {
        data += c_new_line + _content;
    }

    data.push_back('\0');

    // Convert to vector
    std::vector<char> v;
    std::copy(data.begin(), data.end(), std::back_inserter(v));
    v.push_back(0);
    return DataBuffer(std::move(v), v.size());
}

void HttpResponse::push_header(Header header, std::string text) {
    std::string header_name = c_header_string.at(header);
    _headers[header_name] = text;
}


}
