#pragma once
#include <bit>
#include <hooks/memory.hpp>
#include <print>

class ScreenContext;
class Tessellator;
class BedrockTextureData;

class HashedString {
public:
    int64_t hash;
private:
    std::string string;
private:
    HashedString* lastCompare;

public:
    HashedString(int64_t hash, std::string text) : hash(hash), string(text.c_str()), lastCompare(nullptr) {};

    std::string getString() {
        return string;
    }

    HashedString(std::string const& str) : string(str.c_str()), lastCompare(nullptr) {
        static constexpr uint64_t FNV_OFFSET_BASIS = 0xcbf29ce484222325ULL;
        static constexpr uint64_t FNV_PRIME = 0x100000001b3ULL;

        uint64_t result = FNV_OFFSET_BASIS;
        for (unsigned char c : string) {
            result *= FNV_PRIME;
            result ^= c;
        }
        hash = static_cast<int64_t>(result);

        string = str;
    }

    HashedString(HashedString const& that) {
        this->string = that.string;
        this->hash = that.hash;
        this->lastCompare = nullptr;
    }

    HashedString& operator=(HashedString const& right) {
        this->string = right.string;
        this->hash = right.hash;
        this->lastCompare = nullptr;
        return *this;
    }

    HashedString() = default;

    bool operator==(uint64_t right) {
        return right == hash;
    }

    bool operator==(std::string const& right) {
        return string == right;
    }

    bool operator!=(uint64_t right) {
        return !operator==(right);
    }

    bool operator!=(std::string const& right) {
        return !operator==(right);
    }
};

namespace mce {
    class MaterialPtr {
    public:
        static MaterialPtr* createMaterial(const HashedString& name) {
            static auto sig = reinterpret_cast<uintptr_t>(Selaura::FindSignature<"48 8D 15 ? ? ? ? E8 ? ? ? ? 90 49 8D 8E ? ? ? ? E8">());
            static auto final = sig + 3 + 4 + *reinterpret_cast<int32_t*>(sig + 3);
            return Selaura::CallVirtual<mce::MaterialPtr*, const HashedString&>(reinterpret_cast<void*>(final), 1, name);
        }
    };
};

namespace MeshHelpers {
    inline void renderMeshImmediately(ScreenContext* screenContext, Tessellator* tessellator, const mce::MaterialPtr* material, char* a4) {
        using fn_t = decltype(&MeshHelpers::renderMeshImmediately);
        static fn_t fn = std::bit_cast<fn_t>(Selaura::FindSignature<"48 89 5C 24 ? 55 56 57 41 56 41 57 48 81 EC ? ? ? ? 49 8B F1 4D 8B F0 48 8B FA">());
        return (*fn)(screenContext, tessellator, material, a4);
    }

    /*

    inline void renderMeshImmediately(ScreenContext* screenContext, Tessellator* tessellator, const mce::MaterialPtr* material, BedrockTextureData& data, char* a4) {
    using fn_t = decltype(&MeshHelpers::renderMeshImmediately);
    static fn_t fn = std::bit_cast<fn_t>(Selaura::FindSignature<"40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 49 8B F9 4D 8B F8">());
    return (*fn)(screenContext, tessellator, material, data, a4);
    }
    */
};