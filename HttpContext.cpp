#include "HttpContext.h"
#include "base/timeUtils.h"
#include <strings.h>
#include <string>

char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};


HttpContext::HttpContext(Buffer* buffer)
:   m_close_connection(true),
    m_method(GET), 
    m_version(HTTP_1_0),
    m_connection(CLOSE),
    m_request_state(NO_REQUEST),
    m_check_state(CHECK_STATE_REQUEST_LINE), 
    m_uri(),
    m_new_line(),
    m_buffer(buffer)
{ }

HttpContext::~HttpContext() { }

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

void HttpContext::handleHttpRequest() {
    LINE_STATE line_status = parseLine();
    while (line_status == LINE_OK) {
        if (m_check_state == CHECK_STATE_REQUEST_LINE) {
            parseRequestLine();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
            line_status = parseLine();
        }
        else if (m_check_state == CHECK_STATE_HEADER) {
            parseMessageHeader();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
            if (m_check_state != CHECK_STATE_BODY) {
                line_status = parseLine();
            }
        }
        else if (m_check_state == CHECK_STATE_BODY) {
            parseMessageBody();
            if (m_request_state == BAD_REQUEST) {
                return;
            }
            break;
        }
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
    if (m_new_line.substr(start, end - start) == "HTTP/1.0") {
        m_version = HTTP_1_0;
    }
    else if (m_new_line.substr(start, end -start) == "HTTP/1.1") {
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
    // if get a "\0\0" line, which means get the end of the header lines
    if (!m_new_line.empty() && m_new_line[0] == '\0') {
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
            // long connection
            m_connection = KEEP_ALIVE;
            m_close_connection = false;
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

void HttpContext::getHttpResponseMessage(std::string& msg) {
    if (m_version == HTTP_1_0) {
        msg += "HTTP/1.0 ";
    }
    else {
        msg += "HTTP/1.1 ";
    }

    // if (m_request_state == NO_REQUEST) {
    //     // todo:
    //     // need more request data
    //     // register read event again 
    //     return;
    // }
    if (m_request_state == GET_REQUEST) {
        // todo: if status code is 200, set headers and body according to the URI info
        if (m_uri == "/") {
            // response status line
            msg += "200 OK\r\n";
            // response headers
            msg += "Content-Type: text/html\r\n";
            msg += "Header: QWQ-SERVER\r\n"; 
            msg += "Connection: close\r\n";
            msg += "\r\n";
            // response body
            std::string now = timeStamp();
            msg += "<html><head><title>This is title</title></head><body><h1>Hello</h1>Now is ";
            msg += now;
            msg += "</body></html>";
        }
        else if (m_uri == "/favicon.ico") {
            msg += "200 OK\r\n";
            msg += "Content-Type: image/png\r\n";
            msg += "Connection: close\r\n";
            msg += "Header: QWQ-SERVER\r\n"; 
            msg += "\r\n";
            msg += std::string(favicon);
        }
        else if (m_uri == "/hello") {
            msg += "200 OK\r\n";
            msg += "Content-Type: text/plain\r\n";
            msg += "Connection: close\r\n";
            msg += "Header: QWQ-SERVER\r\n"; 
            msg += "\r\n";
            msg += "<html><head>";
            msg += "hello, world!\n";
            msg += "</head></html>";
        }
        else {
            msg += "404 Not Found\r\n";
            msg += "\r\n";
        }
    }
    else if (m_request_state == BAD_REQUEST || m_request_state == NO_REQUEST) {
        msg += "400 Bad Request\r\n";
        msg += "Connection: close\r\n";
        msg += "\r\n";
    }
    else if (m_request_state == FORBIDDEN_REQUEST) {
        msg += "403 Forbidden Error\r\n";
        msg += "\r\n";
    }
    else if (m_request_state == INTERNAL_ERROR) {
        msg += "500 Internal Server Error\r\n";
        msg += "\r\n";
    }
    else if (m_request_state == CLOSED_CONNECTION) {
        // todo
        return;
    }
    else {
        // more states
    }
}