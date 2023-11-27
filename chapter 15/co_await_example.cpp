#include <experimental/coroutine>
#include <iostream>
size_t level = 0;
std::string INDENT = "-";

class Trace
{
public:
    Trace()
    {
        in_level();
    }
    ~Trace()
    {
        level -= 1;
    }
    void in_level()
    {
        level += 1;
        std::string res(INDENT);
        for (size_t i = 0; i < level; i++)
        {
            res.append(INDENT);
        };
        std::cout << res;
    }
};
template <typename T>
struct sync
{
    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    handle_type coro;

    sync(handle_type h)
        : coro(h)
    {
        Trace t;
        std::cout << "Created a sync object" << std::endl;
    }
    sync(const sync &) = delete;
    sync(sync &&s)
        : coro(s.coro)
    {
        Trace t;
        std::cout << "Sync moved leaving behind a husk" << std::endl;
        s.coro = nullptr;
    }
    ~sync()
    {
        Trace t;
        std::cout << "Sync gone" << std::endl;
        if (coro)
            coro.destroy();
    }
    sync &operator=(const sync &) = delete;
    sync &operator=(sync &&s)
    {
        coro = s.coro;
        s.coro = nullptr;
        return *this;
    }

    T get()
    {
        Trace t;
        std::cout << "We got asked for the return value..." << std::endl;
        return coro.promise().value;
    }
    struct promise_type
    {
        T value;
        promise_type()
        {
            Trace t;
            std::cout << "Promise created" << std::endl;
        }
        ~promise_type()
        {
            Trace t;
            std::cout << "Promise died" << std::endl;
        }

        auto get_return_object()
        {
            Trace t;
            std::cout << "Send back a sync" << std::endl;
            return sync<T>{handle_type::from_promise(*this)};
        }
        auto initial_suspend()
        {
            Trace t;
            std::cout << "Started the coroutine, don't stop now!" << std::endl;
            return std::experimental::suspend_never{};
            //std::cout << "--->Started the coroutine, put the brakes on!" << std::endl;
            //return std::experimental::suspend_always{};
        }
        auto return_value(T v)
        {
            Trace t;
            std::cout << "Got an answer of " << v << std::endl;
            value = v;
            return std::experimental::suspend_never{};
        }
        auto final_suspend() noexcept
        {
            Trace t;
            std::cout << "Finished the coro" << std::endl;
            return std::experimental::suspend_always{};
        }
        void unhandled_exception()
        {
            std::exit(1);
        }
    };
};

template <typename T>
struct lazy
{
    struct promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    handle_type coro;

    lazy(handle_type h)
        : coro(h)
    {
        Trace t;
        std::cout << "Created a lazy object" << std::endl;
    }
    lazy(const lazy &) = delete;
    lazy(lazy &&s)
        : coro(s.coro)
    {
        Trace t;
        std::cout << "lazy moved leaving behind a husk" << std::endl;
        s.coro = nullptr;
    }
    ~lazy()
    {
        Trace t;
        std::cout << "lazy gone" << std::endl;
        if (coro)
            coro.destroy();
    }
    lazy &operator=(const lazy &) = delete;
    lazy &operator=(lazy &&s)
    {
        coro = s.coro;
        s.coro = nullptr;
        return *this;
    }

    T get()
    {
        Trace t;
        std::cout << "We got asked for the return value..." << std::endl;
        return coro.promise().value;
    }
    struct promise_type
    {
        T value;
        promise_type()
        {
            Trace t;
            std::cout << "Promise created" << std::endl;
        }
        ~promise_type()
        {
            Trace t;
            std::cout << "Promise died" << std::endl;
        }

        auto get_return_object()
        {
            Trace t;
            std::cout << "Send back a lazy" << std::endl;
            return lazy<T>{handle_type::from_promise(*this)};
        }
        auto initial_suspend()
        {
            Trace t;
            //std::cout << "Started the coroutine, don't stop now!" << std::endl;
            //return std::experimental::suspend_never{};
            std::cout << "Started the coroutine, put the brakes on!" << std::endl;
            return std::experimental::suspend_always{};
        }
        auto return_value(T v)
        {
            Trace t;
            std::cout << "Got an answer of " << v << std::endl;
            value = v;
            return std::experimental::suspend_never{};
        }
        auto final_suspend() noexcept
        {
            Trace t;
            std::cout << "Finished the coro" << std::endl;
            return std::experimental::suspend_always{};
        }
        void unhandled_exception()
        {
            std::exit(1);
        }
        
    };
    bool await_ready()
    {
        const auto ready = this->coro.done();
        Trace t;
        std::cout << "Await " << (ready ? "is ready" : "isn't ready") << std::endl;
        return this->coro.done();
    }
    void await_suspend(std::experimental::coroutine_handle<> awaiting)
    {
        {
            Trace t;
            std::cout << "About to resume the lazy" << std::endl;
            this->coro.resume();
        }
        Trace t;
        std::cout << "About to resume the awaiter" << std::endl;
        awaiting.resume();
    }
    auto await_resume()
    {
        const auto r = this->coro.promise().value;
        Trace t;
        std::cout << "Await value is returned: " << r << std::endl;
        return r;
    }
};
lazy<std::string> read_data()
{
    Trace t;
    std::cout << "Reading data..." << std::endl;
    co_return "billion$!";
}

lazy<std::string> write_data()
{
    Trace t;
    std::cout << "Write data..." << std::endl;
    co_return "I'm rich!";
}
sync<int> reply()
{
    std::cout << "Started await_answer" << std::endl;
    auto a = co_await read_data();
    std::cout << "Data we got is " << a  << std::endl;
    auto v = co_await write_data();
    std::cout << "write result is " << v << std::endl;
    co_return 42;
}

int main()
{
    std::cout<< "Start main()\n";
    auto a = reply();
    return a.get();
}
