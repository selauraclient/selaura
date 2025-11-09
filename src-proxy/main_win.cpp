#include "main_win.hpp"

#include <dwmapi.h>
#include <filesystem>
#include <functional>
#include <iostream>
#include <print>
#include <thread>

#include <winrt/windows.applicationmodel.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.storage.h>

#include <runtime.hpp>

#include <platform/windows.hpp>

std::thread::id mcThreadId;
HANDLE mcThreadHandle = nullptr;

DWORD WINAPI SelauraRuntimeLoaderProc() {
    auto platform = std::make_unique<Selaura::WindowsPlatform>(mcThreadHandle);
    auto* ctx = new Selaura::RuntimeContext;
    ctx->mPlatform = std::move(platform);

    ctx->mPlatform->InitConsole();

    std::println("[Selaura Runtime Loader] Thread ID: {}, Thread Handle: {}", mcThreadId, mcThreadHandle);
    std::println("[Selaura Runtime Loader] Press Numpad1 to End");

    auto folder = ctx->mPlatform->GetSelauraFolder();

    const auto runtime_path = folder / "selaura_runtime.dll";
    if (!std::filesystem::exists(runtime_path)) {
        std::println("\x1b[91m[Selaura Runtime Loader] ERROR: '{}' not found. The runtime is required.\x1b[0m",
                     runtime_path.string());
    } else {
        std::function<void(Selaura::Runtime*)> load_mods = [=](Selaura::Runtime* rt) {
            // To load mods we need LoadLibrary, and other Windows-specific
            // functions, and this will be called in the runtime. However,
            // due to the goal of cross-platform, the function must be
            // created here, and then further passed to the runtime as an
            // argument in the Init function.
            std::filesystem::path mods_folder = folder/ "mods";

            if (!std::filesystem::exists(mods_folder)) {
                std::filesystem::create_directory(mods_folder);
                return;
            }

            int plugins_loaded = 0;

            using init_fn = void(*)(Selaura::Runtime*);
            std::vector<init_fn> init_fn_list = {};

            for (const auto& entry : std::filesystem::directory_iterator(mods_folder)) {
                if (!entry.is_regular_file()) continue;
                const auto& path = entry.path();
                if (path.extension() == ".dll") {
                    HMODULE mod = LoadLibraryExW(entry.path().c_str(), nullptr, DONT_RESOLVE_DLL_REFERENCES);
                    if (mod) {
                        FreeLibrary(mod);
                        mod = LoadLibraryW(entry.path().c_str());
                    }

                    auto init = reinterpret_cast<init_fn>(GetProcAddress(mod, "SelauraPluginInit"));
                    init_fn_list.push_back(init);

                    if (mod) {
                        plugins_loaded++;
                        std::println("[Selaura Plugin Loader] Loaded: {}", path.filename().string());
                    } else {
                        std::println("[Selaura Plugin Loader] Failed to load: {} (Error {})",
                                     path.filename().string(), GetLastError());
                    }
                }
            }

            std::println("[Selaura Plugin Loader] Loaded {} plugin(s).", plugins_loaded);

            for (auto fn : init_fn_list) {
                fn(rt);
            }
        };

        SuspendThread(mcThreadHandle);

        ctx->mThreadId = mcThreadId;

        DWORD handle = 0;
        wchar_t path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);

        DWORD size = GetFileVersionInfoSizeW(path, &handle);
        if (size) {
            std::string buffer(size, '\0');
            if (GetFileVersionInfoW(path, handle, size, buffer.data())) {
                VS_FIXEDFILEINFO* file_info = nullptr;
                UINT len = 0;
                if (VerQueryValueW(buffer.data(), L"\\", reinterpret_cast<LPVOID*>(&file_info), &len) && file_info) {
                    ctx->mVersionMajor = HIWORD(file_info->dwFileVersionMS);
                    ctx->mVersionMinor = LOWORD(file_info->dwFileVersionMS);
                    ctx->mVersionBuild = HIWORD(file_info->dwFileVersionLS);
                    ctx->mVersionRevision = LOWORD(file_info->dwFileVersionLS);
                }
            }
        }

        HMODULE mod = LoadLibraryExW((folder / "selaura_runtime.dll").c_str(), nullptr, 0);
        using runtime_init_fn = void(*)(Selaura::RuntimeContext*, std::function<void(Selaura::Runtime*)>);
        auto runtime_init = reinterpret_cast<runtime_init_fn>(GetProcAddress(mod, "SelauraRuntimeInit"));
        runtime_init(ctx, load_mods);

        ResumeThread(mcThreadHandle);
    }

    HWND hwnd = nullptr;
    DWORD pid = GetCurrentProcessId();

    while (!hwnd) {
        EnumWindows([](HWND h, LPARAM lParam) -> BOOL {
            DWORD windowPid;
            GetWindowThreadProcessId(h, &windowPid);
            if (windowPid == GetCurrentProcessId() && GetParent(h) == nullptr && IsWindowVisible(h)) {
                *reinterpret_cast<HWND*>(lParam) = h;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)&hwnd);

        if (!hwnd)
            Sleep(100);
    }

    auto find_game_window = []() -> HWND {
        HWND found = nullptr;
        DWORD myPid = GetCurrentProcessId();

        EnumWindows([](HWND h, LPARAM lParam) -> BOOL {
            DWORD windowPid;
            GetWindowThreadProcessId(h, &windowPid);
            if (windowPid == GetCurrentProcessId() && GetParent(h) == nullptr && IsWindowVisible(h)) {
                wchar_t className[256];
                GetClassNameW(h, className, 256);

                if (wcscmp(className, L"ConsoleWindowClass") == 0)
                    return TRUE;

                *reinterpret_cast<HWND*>(lParam) = h;
                return FALSE;
            }
            return TRUE;
        }, (LPARAM)&found);

        return found;
    };

    hwnd = find_game_window();
    int tries = 0;
    while (!hwnd && tries < 50) {
        Sleep(100);
        hwnd = find_game_window();
        tries++;
    }

    if (!hwnd) {
        std::println("[Selaura Runtime Loader] Failed to find game window.");
    } else {
        std::println("[Selaura Runtime Loader] Found game window: 0x{:X}", (uintptr_t)hwnd);

        // apply immersive dark mode if available
        BOOL value_true = TRUE;
        (void)DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value_true, sizeof(value_true));

        std::string title = std::format("Selaura Runtime ({}.{}.{}/{}-{})",
            ctx->mVersionMajor,
            ctx->mVersionMinor,
            ctx->mVersionBuild,
            RUNTIME_VERSION,
            GIT_BRANCH
        );

        char className[256];
        GetClassNameA(hwnd, className, 256);
        std::println("[Selaura Runtime Loader] Window class: {}", className);

        SetWindowTextA(hwnd, title.c_str());
        RedrawWindow(hwnd, nullptr, nullptr, RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ERASE);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    while (true) {
        Sleep(10);
        if (GetAsyncKeyState(VK_NUMPAD1)) break;
    }

    FreeConsole();

    ExitProcess(0);

    return 0;
}

BOOL APIENTRY DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        proxy::init_runtime();
        mcThreadId = std::this_thread::get_id();
        mcThreadHandle = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());

        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)SelauraRuntimeLoaderProc, nullptr, 0, nullptr);
    }

    return TRUE;
}
