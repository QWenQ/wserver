#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

class HttpRequest;

class HttpResponse {
    public:
        HttpResponse();
        ~HttpResponse();

        void start(HttpRequest* request);
        // convert http response to a string message
        std::string toString() const;
    private:
        
}; // class HttpResponse
#endif // HTTP_RESPONSE_H