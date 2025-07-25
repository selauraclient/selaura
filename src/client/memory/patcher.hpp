#pragma once
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <cstring>
#include <future>

#include <safetyhook.hpp>
#include "signatures.hpp"
#include "spdlog/spdlog.h"

namespace selaura {
    template <auto Fn, typename T = decltype(Fn)>
    struct as_void_ptr;

    template <auto Fn, typename Ret, typename... Args>
    struct as_void_ptr<Fn, Ret(*)(Args...)> {
        static void* get() { return reinterpret_cast<void*>(Fn); }
    };

    template <auto Fn, typename Class, typename Ret, typename... Args>
    struct as_void_ptr<Fn, Ret(Class::*)(Args...)> {
        static void* get() {
#ifdef SELAURA_WINDOWS
            union {
                Ret(Class::*mpf)(Args...);
                void* ptr;
            } u{ Fn };
            return u.ptr;
#else
            void* out = nullptr;
            std::memcpy(&out, &Fn, sizeof(void*));
            return out;
#endif
        }
    };

    template <auto Fn, typename Class, typename Ret, typename... Args>
    struct as_void_ptr<Fn, Ret(Class::*)(Args...) const> {
        static void* get() {
#ifdef SELAURA_WINDOWS
            union {
                Ret(Class::*mpf)(Args...) const;
                void* ptr;
            } u{ Fn };
            return u.ptr;
#else
            void* out = nullptr;
            std::memcpy(&out, &Fn, sizeof(void*));
            return out;
#endif
        }
    };

    template<typename T>
    struct fn_traits;

    template<typename Ret, typename... Args>
    struct fn_traits<Ret(*)(Args...)> {
        using return_type = Ret;
    };

    template<typename Class, typename Ret, typename... Args>
    struct fn_traits<Ret(Class::*)(Args...)> {
        using return_type = Ret;
    };

    template<typename Class, typename Ret, typename... Args>
    struct fn_traits<Ret(Class::*)(Args...) const> {
        using return_type = Ret;
    };

    template <typename T>
    struct fn_pointer_traits;

    template <typename Ret, typename... Args>
    struct fn_pointer_traits<Ret(*)(Args...)> {
        using pointer_type = Ret(*)(Args...);
    };

    template <typename Class, typename Ret, typename... Args>
    struct fn_pointer_traits<Ret(Class::*)(Args...)> {
        using pointer_type = Ret(Class::*)(Args...);
    };

    template <typename Class, typename Ret, typename... Args>
    struct fn_pointer_traits<Ret(Class::*)(Args...) const> {
        using pointer_type = Ret(Class::*)(Args...) const;
    };

    template<typename fn>
    struct to_free_function;

    template<typename Ret, typename Class, typename... Args>
    struct to_free_function<Ret(Class::*)(Args...)> {
        using type = Ret(*)(Class*, Args...);
    };

    template<typename Ret, typename Class, typename... Args>
    struct to_free_function<Ret(Class::*)(Args...) const> {
        using type = Ret(*)(const Class*, Args...);
    };


    inline std::unordered_map<std::size_t, SafetyHookInline> hook_map;

    template <auto fn>
    constexpr std::size_t fn_hash() {
        if constexpr (std::is_pointer_v<decltype(fn)>) {
            return reinterpret_cast<std::size_t>(fn);
        } else if constexpr (std::is_member_function_pointer_v<decltype(fn)>) {
            std::size_t hash = 0xcbf29ce484222325;
            const unsigned char* p = reinterpret_cast<const unsigned char*>(&fn);
            for (std::size_t i = 0; i < sizeof(fn); ++i) {
                hash ^= p[i];
                hash *= 0x100000001b3;
            }
            return hash;
        } else {
            static_assert([] { return false; }(), "Unsupported Fn type in fn_hash");
        }
    }

    template <auto fn>
    void patch_fn(void* target) {
        auto key = fn_hash<fn>();
        auto it = hook_map.find(key);
        if (it == hook_map.end()) {
            auto [inserted, _] = hook_map.emplace(key,
                safetyhook::create_inline(
                    target,
                    selaura::as_void_ptr<fn>::get()
                )
            );
        }
    }

    template <auto fn>
    void patch_fn() {
        void* target = reinterpret_cast<void*>(selaura::resolve_signature<fn>());
        if (hook_map.contains(fn_hash<fn>())) return;

        auto hook = safetyhook::create_inline(target, selaura::as_void_ptr<fn>::get());
        hook_map.emplace(fn_hash<fn>(), std::move(hook));
    }

    template <auto fn>
    void patch_vtable_fn(void* vtable, std::size_t index) {
        void** obj = *reinterpret_cast<void***>(vtable);
        patch_fn<fn>(obj[index]);
    }

    template <auto fn, typename... Args>
    decltype(auto) call_fn(Args&&... args) {
        using fn_t = decltype(fn);
        using pointer_t = typename fn_pointer_traits<fn_t>::pointer_type;

        auto key = fn_hash<fn>();
        auto it = hook_map.find(key);
        if (it == hook_map.end())
            throw std::runtime_error("Hook not found");

        auto trampoline = it->second.trampoline().address();

        pointer_t original;
        std::memcpy(&original, &trampoline, sizeof(trampoline));

        return std::invoke(original, std::forward<Args>(args)...);
    }

    template <auto fn, typename... Args>
    decltype(auto) call_original(Args&&... args) {
        using fn_t = decltype(fn);
        using pointer_t = typename selaura::fn_pointer_traits<fn_t>::pointer_type;

        void* target = reinterpret_cast<void*>(selaura::resolve_signature<fn>());
        pointer_t original;
        std::memcpy(&original, &target, sizeof(original));

        return std::invoke(original, std::forward<Args>(args)...);
    }


    template <auto... fn>
    void patch_fns() {
        auto futures = std::array{
            std::async(std::launch::async, [] { patch_fn<fn>(); })...
        };

        for (auto& fut : futures) {
            fut.wait();
        }
    }

}
