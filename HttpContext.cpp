#include "HttpContext.h"

HttpContext::HttpContext(Buffer* buffer) 
:   m_request(buffer),
    m_response()
{
    // todo
}

HttpContext::~HttpContext() {
    // todo
}

void HttpContext::start() {
    m_request.start();
    m_response.start(&m_request);
}
