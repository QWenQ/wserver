#ifndef HTTP_CONTEXT_H
#define HTTP_CONTEXT_H

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

class HttpContext {
    public:
        enum HttpStatusCode {
            kUnKnown,
            k200OK = 200,
            k301MovedPermanently = 301,
            k400BadRequest = 400,
            k404NotFound = 404
        };


        HttpContext(Buffer* input_buffer, Buffer* output_buffer);
        ~HttpContext();

        void handleHttpRequest();
        void getHttpResponse();

        bool isLongConnection() const { return m_connection == KEEP_ALIVE; }


        void setStatusCode(HttpStatusCode code) { m_status_code = code; }
        void setStatusMessage(const std::string& msg) { m_status_message = msg; }
        void setCloseConnection(bool close) { m_close_connection = close; }
        bool isClosed() const { return m_close_connection; }

        
        METHOD getMethod() const { return m_method; }
        HTTP_VERSION getVersion() const { return m_version; }
        CONNECTION getConnection() const { return m_connection; }
        std::string getURI() const { return m_uri; }
        CHECK_STATE getParseResult() const { return m_check_state; }

        void work();


    private:
        LINE_STATE parseLine();
        void parseRequestLine();
        void parseMessageHeader();
        void parseMessageBody();


        bool m_close_connection;
        METHOD m_method;
        HTTP_VERSION m_version;
        CONNECTION m_connection;

        HttpStatusCode m_status_code;
        std::string m_status_message;

        REQUEST_STATE m_request_state;
        CHECK_STATE m_check_state;
        std::string m_uri;
        std::string m_new_line;
        // buffer for reading the http request from the client
        Buffer* m_input_buffer;
        // buffer for writing back the http response to the client
        Buffer* m_output_buffer;
}; // class HttpContext

#endif // HTTP_CONTEXT_H