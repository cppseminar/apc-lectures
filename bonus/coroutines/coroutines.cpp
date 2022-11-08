#include <future>
#include <iostream>
#include <fstream>
#include <string>
#include <coroutine>
#include <experimental/generator>
#include "common.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// Simplest coroutine example

namespace Test01
{
    struct return_object
    {
        struct promise_type
        {
            promise_type() = default;
            return_object get_return_object() { return {}; }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void unhandled_exception() {}

            void return_void() {}
        };
    };

    return_object foo()
    {
        std::cout << "1. 01 hello from coroutine\n";
        co_await std::suspend_always{};
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Resume coroutine using handle

namespace Test02
{
    struct return_object
    {
        struct promise_type
        {
            promise_type() = default;
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void unhandled_exception() {}
            void return_void() {}

            return_object get_return_object()
            {
                return return_object{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
        };

        std::coroutine_handle<promise_type> handle_;

        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object foo()
    {
        std::cout << "1. 02 hello from coroutine\n";
        co_await std::suspend_always{};
        std::cout << "2. 02 hello from coroutine\n";
        co_await std::suspend_always{};
        std::cout << "3. 02 hello from coroutine\n";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Implement awaitable/awaiter object

namespace Test03
{
    int counter{ 0 };

    struct suspend_always
    {
        bool await_ready() const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_ready\n";
            return false;
        }
        void await_suspend(std::coroutine_handle<> h) const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_suspend\n";
        }
        //bool await_suspend(std::coroutine_handle<> h) const noexcept
        //{
        //    // conditional suspend, if false is returned, coroutine is not suspended at `co_await`
        //    return true; // this is the same as void version
        //}
        void await_resume() const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_resume\n";
        }
    };

    struct return_object
    {
        struct promise_type
        {
            promise_type() = default;
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() {}
            void return_void() {}

            return_object get_return_object()
            {
                return return_object{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
        };

        std::coroutine_handle<promise_type> handle_;

        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object foo()
    {
        std::cout << ++counter << ". first hello from coroutine\n";
        co_await suspend_always{};
        std::cout << ++counter << ". second hello from coroutine\n";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Implement awaitable/awaiter object with non-void handle

namespace Test04
{
    int counter{ 0 };

    template<typename PromiseType = void>
    struct suspend_always
    {
        bool await_ready() const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_ready\n";
            return false;
        }
        void await_suspend(std::coroutine_handle<PromiseType> h) const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_suspend\n";
        }
        //bool await_suspend(std::coroutine_handle<PromiseType> h) const noexcept
        //{
        //    // conditional suspend, if false is returned, coroutine is not suspended at `co_await`
        //    return true; // this is the same as void version
        //}
        void await_resume() const noexcept
        {
            std::cout << ++counter << ". suspend_always::await_resume\n";
        }
    };

    struct return_object
    {
        struct promise_type
        {
            promise_type() = default;
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() {}
            void return_void() {}

            return_object get_return_object()
            {
                return return_object{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
        };

        std::coroutine_handle<promise_type> handle_;

        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object foo()
    {
        std::cout << ++counter << ". first hello from coroutine\n";
        co_await suspend_always<return_object::promise_type>{};
        std::cout << ++counter << ". second hello from coroutine\n";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Return type of expression `co_await expr` is same as that of `awaiter::await_resume`

namespace Test05
{
    struct suspend_never
    {
        bool await_ready() const noexcept { return true; }
        void await_suspend(std::coroutine_handle<> h)const noexcept {}
        double await_resume() const noexcept { return 10.234; }
    };

    struct return_object
    {
        struct promise_type
        {
            unsigned value_{ 0 };
            return_object get_return_object()
            {
                return { std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() {}
            void return_void() {}
        };

        std::coroutine_handle<promise_type> handle_;
        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object foo()
    {
        double value = co_await suspend_never{};
        std::cout << "value printed from coroutine: " << value << "\n";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Storing `promise_type` inside awaiter and returning from it

namespace Test06
{
    int counter{ 0 };

    template<typename PromiseType = void>
    struct suspend_always
    {
        PromiseType* promise_;

        bool await_ready() const noexcept
        {
            return false;
        }
        void await_suspend(std::coroutine_handle<PromiseType> handle) noexcept
        {
            promise_ = &handle.promise();
        }
        PromiseType* await_resume() const noexcept
        {
            return promise_;
        }
    };

    struct return_object
    {
        struct promise_type
        {
            promise_type() = default;
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() {}
            void return_void() {}

            return_object get_return_object()
            {
                return return_object{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
        };

        std::coroutine_handle<promise_type> handle_;

        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object foo()
    {
        std::cout << ++counter << ". first hello from coroutine\n";
        co_await suspend_always<return_object::promise_type>{};
        std::cout << ++counter << ". second hello from coroutine\n";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Generator

namespace Test07
{
    template<typename PromiseType = void>
    struct suspend_never
    {
        PromiseType* promise_{ nullptr };
        bool await_ready() const noexcept { return false; }
        bool await_suspend(std::coroutine_handle<PromiseType> h) noexcept
        {
            promise_ = &h.promise();
            return false;
        }
        PromiseType* await_resume() const noexcept { return promise_; }
    };

    struct return_object
    {
        struct promise_type
        {
            int value_;
            return_object get_return_object()
            {
                return { std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() {}

            void return_void() {}
        };

        std::coroutine_handle<promise_type> handle_;
        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object generator()
    {
        auto promise = co_await suspend_never<return_object::promise_type>{};
        for (int i = 0; ; i++)
        {
            promise->value_ = i;
            co_await std::suspend_always{};
        }

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Keyword `co_yield` to suspend execution returning a value ...

namespace Test08
{
    struct return_object
    {
        struct promise_type
        {
            int value_;
            return_object get_return_object()
            {
                return { std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
            std::suspend_always yield_value(int value)
            {
                value_ = value;
                return {};
            }
            void unhandled_exception() {}

            void return_void() {}
        };

        std::coroutine_handle<promise_type> handle_;
        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    return_object generator()
    {
        for (int i = 0; ; i++)
            co_yield i;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Returning from coroutine

namespace Test09
{
    struct return_object
    {
        struct promise_type
        {
            int value_;
            return_object get_return_object()
            {
                return { std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_always final_suspend() const noexcept
            {
                std::cout << "suspending coroutine for one last time\n";
                return {};
            }
            void unhandled_exception() {}

            void return_void()
            {
                std::cout << "return_void\n";
            }
        };

        std::coroutine_handle<promise_type> handle_;
        return_object(std::coroutine_handle<promise_type> h) :handle_{ h } { }
        operator std::coroutine_handle<promise_type>() const { return handle_; }
    };

    using PromiseType = return_object::promise_type;

    return_object foo()
    {
        co_await std::suspend_never{};
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Coroutine

namespace Test10
{
    struct resumable_thing
    {
        struct promise_type
        {
            int value;

            resumable_thing get_return_object()
            {
                return resumable_thing(std::coroutine_handle<promise_type>::from_promise(*this));
            }
            auto initial_suspend() { return std::suspend_always{}; }
            auto final_suspend() noexcept { return std::suspend_never{}; }
            void return_void() {}
            std::suspend_always yield_value(int v)
            {
                value = v;

                return {};
            }

            void unhandled_exception() {}
        };

        std::coroutine_handle<promise_type> _coroutine = nullptr;
        explicit resumable_thing(std::coroutine_handle<promise_type> coroutine)
            : _coroutine(coroutine)
        {
        }
        ~resumable_thing()
        {
            if (_coroutine) { _coroutine.destroy(); }
        }
        resumable_thing() = default;
        resumable_thing(resumable_thing const&) = delete;
        resumable_thing& operator=(resumable_thing const&) = delete;
        resumable_thing(resumable_thing&& other)
            : _coroutine(other._coroutine)
        {
            other._coroutine = nullptr;
        }
        resumable_thing& operator=(resumable_thing&& other)
        {
            if (&other != this)
            {
                _coroutine = other._coroutine;
                other._coroutine = nullptr;
            }
        }
        int operator()()
        {
            _coroutine.resume();

            return _coroutine.promise().value;
        }
    };

    resumable_thing counter()
    {
        std::cout << "counter: called\n";
        for (unsigned i = 1; ; ++i)
        {
            co_yield i;

            std::cout << "counter: resumed (#" << i << ")\n";
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Generator

namespace Test11
{
    std::experimental::generator<int> my_interval(int a, int b)
    {
        for (int i = a; i < b; i++)
            co_yield i;
    }

    struct Point
    {
        float x;
        float y;
    };
    std::experimental::generator<Point> my_circle(Point position, float radius, float stepRadians = 0.1f)
    {
        for (float radians = 0.0f; radians < 6.28f; radians += stepRadians)
        {
            float x = radius * cos(radians);
            float y = radius * sin(radians);
            co_yield {x, y};
        }
    }

    std::experimental::generator<std::string> my_reader(const char* filename)
    {
        std::ifstream file(filename, std::ios_base::in);
        for (std::string line; std::getline(file, line);)
            co_yield line;
    }

    std::experimental::generator<std::tuple<float, float, float>> my_rainbow(float step = 0.01f)
    {
        for (float i = 0.0f; i < 1.0f; i += step)
            co_yield common::hsl2rgb(i * 0.8f, 0.95f, 0.5f);
    }
}

int main()
{
    {
        std::cout << "\n*** Test01\n";
        Test01::foo();
    }

    {
        std::cout << "\n*** Test02\n";
        std::coroutine_handle<Test02::return_object::promise_type> handle = Test02::foo();
        handle();
        handle.resume();
    }

    {
        std::cout << "\n*** Test03\n";
        std::coroutine_handle<Test03::return_object::promise_type> handle = Test03::foo();
        handle();
    }

    {
        std::cout << "\n*** Test04\n";
        std::coroutine_handle<Test04::return_object::promise_type> handle = Test04::foo();
        handle();
    }

    {
        std::cout << "\n*** Test05\n";
        std::coroutine_handle<Test05::return_object::promise_type> handle = Test05::foo();
    }

    {
        std::cout << "\n*** Test07\n";
        using PromiseType = Test07::return_object::promise_type;

        std::coroutine_handle<PromiseType> handle = Test07::generator();
        PromiseType& promise = handle.promise();
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "From main: " << promise.value_ << "\n";
            handle();
        }
    }

    {
        std::cout << "\n*** Test08\n";
        using PromiseType = Test08::return_object::promise_type;

        std::coroutine_handle<PromiseType> handle = Test08::generator();
        PromiseType& promise = handle.promise();
        for (int i = 0; i < 5; ++i)
        {
            std::cout << "From main: " << promise.value_ << "\n";
            handle();
        }
    }

    {
        std::cout << "\n*** Test09\n";
        using PromiseType = Test09::return_object::promise_type;

        std::coroutine_handle<PromiseType> handle = Test09::foo();
        PromiseType& promise = handle.promise();

        if (handle.done())
        {
            /*
              do something with prom one last time
            */
            std::cout << "done execution of coroutine\n";
            handle.destroy();
        }
    }

    {
        std::cout << "\n*** Test10\n";
        std::cout << "main: calling counter\n";
        Test10::resumable_thing the_counter = Test10::counter();
        std::cout << "main: resuming counter\n";
        std::cout << the_counter() << "\n";
        std::cout << the_counter() << "\n";
        std::cout << "main: done\n";
    }

    {
        std::cout << "\n*** Test11\n";
        for (int c : Test11::my_interval(5, 10))
            std::cout << "value " << c << "\n";

        auto gen = Test11::my_interval(10, 15);
    }
}
