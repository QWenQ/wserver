#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H
class noncopyable {
    public:
        noncopyable(const noncopyable& arg) = delete;
        void operator=(const noncopyable& arg) = delete;
    protected:
        noncopyable() = default;
        ~noncopyable() = default;
}
#endif // NONCOPYABLE_H