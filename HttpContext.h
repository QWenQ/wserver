#ifndef HTTP_CONTEXT_H
#define HTTP_CONTEXT_H

#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpContext {
    public:
        HttpContext(Buffer* buffer);
        ~HttpContext();

        void start();
    private:
        void setResponseMessage();
        std::string m_response_msg;
        HttpRequest m_request;
        HttpResponse m_response;
}; // class HttpContext

#endif // HTTP_CONTEXT_H