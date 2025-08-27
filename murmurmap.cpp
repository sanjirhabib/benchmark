#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdint>

// MurmurHash3_x86_32 implementation (based on Austin Appleby's public domain code)
uint32_t MurmurHash3_x86_32(const void* key, int len, uint32_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    const int nblocks = len / 4;
    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // Body
    const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> (32 - 15));
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> (32 - 13));
        h1 = h1 * 5 + 0xe6546b64;
    }

    // Tail
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
    uint32_t k1 = 0;

    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
                k1 *= c1;
                k1 = (k1 << 15) | (k1 >> (32 - 15));
                k1 *= c2;
                h1 ^= k1;
    }

    // Finalization
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

// Custom hash function for std::unordered_map using MurmurHash3
struct Murmur3Hasher {
    size_t operator()(const std::string& key) const {
        return MurmurHash3_x86_32(key.c_str(), key.length(), 0);
    }
};

int main() {
    // Use std::unordered_map with Murmur3Hasher
    std::unordered_map<std::string, std::string, Murmur3Hasher> ret;
    
    // Populate the map
    for (int i = 0; i < 5000000; i++) {
        ret["id." + std::to_string(i % 500000)] = "val." + std::to_string(i);
    }

    // Print map size
    std::cout << ret.size() << std::endl;

    // Print value for key "id.10000"
    std::cout << ret["id.10000"] << std::endl;

    return 0;
}
