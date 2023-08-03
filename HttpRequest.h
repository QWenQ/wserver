#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <memory>
#include "Buffer.h"

enum METHOD { GET, HEAD };
enum CHECK_STATE { CHECK_STATE_REQUEST_LINE, CHECK_STATE_HEADER, CHECK_STATE_BODY, CHECK_STATE_END };
enum LINE_STATE { LINE_OK, LINE_BAD, LINE_OPEN };
/*
NO_REQUEST          : the request is not complete, please read more request data
GET_REQUEST         : get a complete request
BAD_REQUEST         : the request has syntax errors
FORBIDDEN_REQUEST   : the access to the resources is not authorized
INTERNAL_ERROR      : errors in the server
CLOSED_CONNECTION   : the client has closed the connection
*/
enum REQUEST_STATE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
enum HTTP_VERSION { HTTP_1_0, HTTP_1_1 };
enum CONNECTION { CLOSE, KEEP_ALIVE };

class HttpRequest {
    public:
        HttpRequest(Buffer* buffer);
        ~HttpRequest();

        void start();
        std::string getResponseMessage() const;

        bool isLongConnection() const { return m_connection == KEEP_ALIVE; }

        // debug:
        METHOD getMethod() const { return m_method; }
        HTTP_VERSION getVersion() const { return m_version; }
        CONNECTION getConnection() const { return m_connection; }
        std::string getURI() const { return m_uri; }
        CHECK_STATE getParseResult() const { return m_check_state; }


    private:
        LINE_STATE parseLine();
        void parseRequestLine();
        void parseMessageHeader();
        void parseMessageBody();
        void parseHTTPRequeset();


        METHOD m_method;
        HTTP_VERSION m_version;
        CONNECTION m_connection;

        REQUEST_STATE m_request_state;
        CHECK_STATE m_check_state;
        std::string m_uri;
        std::string m_new_line;
        // buffer contain http request data
        Buffer* m_buffer;
}; // class HttpRequest

#endif // HTTP_REQUEST_H