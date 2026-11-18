#include "barriers.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <new>
#include <vector>

#if defined(USE_MIMALLOC)
#include <mimalloc-new-delete.h>
#endif

/** The workload allocates arrays whose element count is known only at run time. */
struct dynamic_array_workload
{
    /** The generator advances a 64-bit xorshift state and returns the new state. */
    static std::uint64_t next_random(std::uint64_t& state)
    {
        state ^= state << 13;
        state ^= state >> 7;
        state ^= state << 17;
        return state;
    }

    /** The selector returns an element count whose binary magnitude is uniform over the given number of levels. */
    static std::size_t random_count(std::uint64_t& state, std::size_t levels)
    {
        std::uint64_t word = next_random(state);
        std::uint64_t base = std::uint64_t{1} << ((word >> 32) % levels);
        return static_cast< std::size_t >(base + ((word & 4294967295u) % base));
    }

    /** The function allocates an uninitialized array and writes the identity to its first and last elements. */
    static std::uint64_t* acquire(std::size_t count, std::uint64_t identity)
    {
        std::uint64_t* result = std::allocator< std::uint64_t >().allocate(count);
        ALLOCATION_WRITE_WORD(result, identity);
        ALLOCATION_WRITE_WORD(result + (count - 1), identity);
        return result;
    }

    /** The function reads the first and last elements, releases the array with its count, and returns their sum. */
    static std::uint64_t release(std::uint64_t* array, std::size_t count)
    {
        std::uint64_t sum = array[0] + array[count - 1];
        std::allocator< std::uint64_t >().deallocate(array, count);
        return sum;
    }

    /** The rolling workload retains a window of live arrays and replaces the oldest array after the window fills. */
    static std::uint64_t rolling(std::size_t n, std::size_t batch, std::size_t elements, bool random,
                                 std::vector< std::uint64_t* >& arrays, std::vector< std::size_t >& lengths)
    {
        std::size_t levels = 0;
        while ((std::size_t{1} << levels) < elements) { ++levels; }
        std::uint64_t state = 88172645463325252ull;
        std::uint64_t checksum = 0;
        std::size_t window = batch < n ? batch : n;
        for (std::size_t index = 0; index < window; ++index)
        {
            std::size_t count = random ? random_count(state, levels) : elements;
            lengths[index] = count;
            arrays[index] = acquire(count, index + 1);
        }
        std::size_t position = 0;
        for (std::size_t completed = window; completed < n; ++completed)
        {
            checksum += release(arrays[position], lengths[position]);
            std::size_t count = random ? random_count(state, levels) : elements;
            lengths[position] = count;
            arrays[position] = acquire(count, completed + 1);
            if (++position == window) { position = 0; }
        }
        for (std::size_t index = 0; index < window; ++index)
        { checksum += release(arrays[index], lengths[index]); }
        return checksum;
    }

    /** The growth workload repeats the capacity sequence of an appended array without copying elements. */
    static std::uint64_t growth(std::size_t n, std::size_t elements)
    {
        std::uint64_t checksum = 0;
        std::uint64_t identity = 0;
        for (std::size_t sequence = 0; sequence < n; ++sequence)
        {
            std::size_t capacity = 1;
            std::uint64_t* array = acquire(capacity, ++identity);
            while (capacity < elements)
            {
                std::uint64_t* next = acquire(capacity * 2, ++identity);
                checksum += release(array, capacity);
                array = next;
                capacity *= 2;
            }
            checksum += release(array, capacity);
        }
        return checksum;
    }
};

/** The entry point selects a fixed, random, or growth pattern and a maximum array size in bytes. */
int main(int argc, char** argv)
{
    if (argc != 5) { return 2; }
    std::size_t n = std::strtoull(argv[1], nullptr, 10);
    std::size_t size = std::strtoull(argv[2], nullptr, 10);
    std::size_t batch = std::strtoull(argv[3], nullptr, 10);
    std::size_t pattern = std::strtoull(argv[4], nullptr, 10);
    if (n == 0 || batch == 0 || batch > 4096 || pattern > 2) { return 2; }
    if (size < 16 || size > 1073741824 || (size & (size - 1)) != 0) { return 2; }
    std::size_t elements = size / 8;
    std::vector< std::uint64_t* > arrays(batch);
    std::vector< std::size_t > lengths(batch);
    std::uint64_t start = ALLOCATION_TICKS();
    std::uint64_t checksum = pattern == 2 ? dynamic_array_workload::growth(n, elements)
                                          : dynamic_array_workload::rolling(n, batch, elements, pattern == 1, arrays, lengths);
    std::uint64_t elapsed = ALLOCATION_TICKS() - start;
    std::printf("%llu %llu %llu\n", static_cast< unsigned long long >(elapsed),
                static_cast< unsigned long long >(ALLOCATION_FREQUENCY()), static_cast< unsigned long long >(checksum));
    return 0;
}
