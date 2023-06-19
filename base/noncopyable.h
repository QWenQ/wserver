#ifndef BASE_NONCOPYABLE_H
#define BASE_NONCOPYABLE_H
class noncopyable {
    public:
        noncopyable(const noncopyable& arg) = delete;
        void operator=(const noncopyable& arg) = delete;
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
}; // class noncopyable
#endif // BASE_NONCOPYABLE_H