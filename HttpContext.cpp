#include "HttpContext.h"
#include <strings.h>

HttpContext::HttpContext(Buffer* buffer)
:   m_method(GET), 
    m_version(HTTP_1_0),
    m_connection(CLOSE),
    m_request_state(NO_REQUEST),
    m_check_state(CHECK_STATE_REQUEST_LINE), 
    m_uri(),
    m_new_line(),
    m_buffer(buffer)
{ }

HttpContext::~HttpContext() { }

void HttpContext::handleHttpRequest() {
    parseHTTPRequeset();
    // error handling according return state
    std::string response_msg = getHttpResponseMessage();
}

LINE_STATE HttpContext::parseLine() {
    m_new_line = m_buffer->getAnHTTPLine();
    for (std::size_t i = 0; i < m_new_line.size(); ++i) {
        if (m_new_line[i] == '\r' && i + 1 < m_new_line.size()) {
            // a complete http line must be end up with "\r\n"
            if (m_new_line[i + 1] == '\n') {
                m_new_line[i] = '\0';
                m_new_line[i + 1] = '\0';
                return LINE_OK;
            }
            else {
                return LINE_BAD;
            }
        }
    }
    return LINE_OPEN; 
}

void HttpContext::parseHTTPRequeset() {
    LINE_STATE line_status = parseLine();
    while (line_status == LINE_OK) {
        if (m_check_state == CHECK_STATE_REQUEST_LINE) {
            parseRequestLine();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
        }
        else if (m_check_state == CHECK_STATE_HEADER) {
            parseMessageHeader();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
            
        }
        else if (m_check_state == CHECK_STATE_BODY) {
            parseMessageBody();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
            break;
        }
        line_status = parseLine();
    }

    if (line_status == LINE_OPEN) {
        m_request_state = NO_REQUEST;
    }
    else if (line_status == LINE_BAD){
        m_request_state = BAD_REQUEST;
    }
}

void HttpContext::parseRequestLine() {
    std::size_t start = 0;
    std::size_t end = 0;
    while (m_new_line[end] != ' ' && m_new_line[end] != '\0') {
        ++end;
    }
    // get method
    std::string method = m_new_line.substr(start, end - start);
    if (method == "GET") {
        m_method = GET;
    }
    else if (method == "HEAD") {
        m_method = HEAD;
    }
    else {
        // error
        m_request_state = BAD_REQUEST;
        return ;
    }

    // get URI
    while (m_new_line[end] == ' ' && m_new_line[end] != '\0') {
        ++end;
    }
    start = end;
    while (m_new_line[end] != ' ' && m_new_line[end] != '\0') {
        ++end;
    }

    m_uri = m_new_line.substr(start, end - start);

    // get version
    while (m_new_line[end] == ' ' && m_new_line[end] != '\0') {
        ++end;
    }
    start = end;
    while (m_new_line[end] != ' ' && m_new_line[end] != '\0') {
        ++end;
    }
    if (m_new_line.substr(start, end - start) == "http/1.0") {
        m_version = HTTP_1_0;
    }
    else if (m_new_line.substr(start, end -start) == "http/1.1") {
        m_version = HTTP_1_1;
    }
    else {
        m_request_state = BAD_REQUEST;
        return;
    }

    m_check_state = CHECK_STATE_HEADER;
    m_request_state = NO_REQUEST;
}

void HttpContext::parseMessageHeader() {
    // todo
    // headers end with line "\r\n"
    if (m_new_line == "\r\n") {
        m_check_state = CHECK_STATE_BODY;
        m_request_state = NO_REQUEST;
        return;
    }
    if (strncasecmp(m_new_line.data(), "Connection:", 11) == 0) {
        int start = 11;
        while (m_new_line[start] == ' ') {
            ++start;
        }
        if (strncasecmp(m_new_line.data() + start, "Keep-Alive", 10) == 0) {
            m_connection = KEEP_ALIVE;
        }
        else {
            m_request_state = BAD_REQUEST;
            return;
        }
    }
    else {
        // todo: other headers
    }
    m_request_state = NO_REQUEST;
}

void HttpContext::parseMessageBody() {
    // todo
    m_check_state = CHECK_STATE_END;
    m_request_state = GET_REQUEST;
}

std::string HttpContext::getHttpResponseMessage() const {
    std::string msg;
    if (m_version == HTTP_1_0) {
        msg += "Http/1.0 ";
    }
    else {
        msg += "Http/1.1 ";
    }

    if (m_request_state == NO_REQUEST) {
        // todo:
        // need more request data
        // register read event again 
    }
    else if (m_request_state == GET_REQUEST) {
        msg += "200 OK\r\n";
    }
    else if (m_request_state == BAD_REQUEST) {
        msg += "400 Bad Request\r\n";
    }
    else if (m_request_state == FORBIDDEN_REQUEST) {
        msg += "403 Forbidden Error\r\n";
    }
    else if (m_request_state == INTERNAL_ERROR) {
        msg += "500 Internal Server Error\r\n";
    }
    else if (m_request_state == CLOSED_CONNECTION) {
        // todo
        msg += "\r\n";
    }
    else {
        // more states
    }
    msg += "\r\n";
    return msg;
}