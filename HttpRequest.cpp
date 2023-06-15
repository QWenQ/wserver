#include <strings.h>
#include "HttpRequest.h"

HttpRequest::HttpRequest(int fd) 
:   m_fd(fd), 
    m_method(GET), 
    m_version(HTTP_1_0),
    m_connection(CLOSE),
    m_current_state(CHECK_STATE_REQUEST_LINE), 
    m_uri(),
    m_new_line(),
    m_input_buffer(m_fd),
    m_output_buffer(m_fd)
    { }

HttpRequest::~HttpRequest() { }

void HttpRequest::start() {
    // get the http request
    m_input_buffer.read();
    // parse the request
    REQUEST_STATE ret = parseHTTPRequeset();
    if (ret == NO_REQUEST) {
        // todo: more reqeust data needed
    }
    // set the http response
    setHTTPResponse(ret);
    // write response to client(m_fd)
    m_input_buffer.write();
}


LINE_STATE HttpRequest::parseLine() {
    m_new_line = m_input_buffer.getAnHTTPLine();
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

REQUEST_STATE HttpRequest::parseHTTPRequeset() {
    REQUEST_STATE ret;
    LINE_STATE line_status = parseLine();
    while (line_status == LINE_OK) {
        if (m_current_state == CHECK_STATE_REQUEST_LINE) {
            ret = parseRequestLine();
            if (ret == BAD_REQUEST) {
                return BAD_REQUEST;
            }
        }
        else if (m_current_state == CHECK_STATE_HEADER) {
            ret = parseMessageHeader();
            if (ret == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            
        }
        else if (m_current_state == CHECK_STATE_BODY) {
            ret = parseMessageBody();
            if (ret == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            break;
        }
        line_status = parseLine();
    }

    if (line_status == LINE_OPEN) {
        return NO_REQUEST;
    }
    else if (line_status == LINE_BAD){
        return BAD_REQUEST;
    }
    return ret;
}

REQUEST_STATE HttpRequest::parseRequestLine() {
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
        return BAD_REQUEST;
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
        return BAD_REQUEST;
    }

    m_current_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

REQUEST_STATE HttpRequest::parseMessageHeader() {
    // todo
    // headers end with line "\r\n"
    if (m_new_line == "\r\n") {
        m_current_state = CHECK_STATE_BODY;
        return NO_REQUEST;
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
            return BAD_REQUEST;
        }
    }
    else {
        // todo: other headers
    }
    return NO_REQUEST;
}

REQUEST_STATE HttpRequest::parseMessageBody() {
    // todo
    m_current_state = CHECK_STATE_END;
    return GET_REQUEST;
}

void HttpRequest::setHTTPResponse(REQUEST_STATE state) {
    std::string msg;
    if (m_version == HTTP_1_0) {
        msg += "Http/1.0 ";
    }
    else {
        msg += "Http/1.1 ";
    }

    if (state == NO_REQUEST) {
        // need more request data
        
    }
    else if (state == GET_REQUEST) {
        msg += "200 OK\r\n";
    }
    else if (state == BAD_REQUEST) {
        msg += "400 Bad Request\r\n";
    }
    else if (state == FORBIDDEN_REQUEST) {
        msg += "403 Forbidden Error\r\n";
    }
    else if (state == INTERNAL_ERROR) {
        msg += "500 Internal Server Error\r\n";
    }
    else if (state == CLOSED_CONNECTION) {
        // todo
        msg += "\r\n";
    }
    else {
        // more states
    }
    msg += "\r\n";
    m_output_buffer.append(msg);
}