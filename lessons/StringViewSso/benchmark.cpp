#include "include/string_lookup.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

int main() {
    using namespace string_view_sso;
    constexpr std::size_t kQueryCount = 10000;
    constexpr int kFrames = 200;

    auto queries = make_sample_queries(kQueryCount);
    NaiveStringCatalog naive;
    StringViewCatalog sv_cat;
    InternedCatalog intern_cat;
    auto query_ids = intern_cat.preintern_queries(queries);

    // Warmup
    volatile std::int64_t sink = 0;
    sink += naive.resolve_batch(queries, 5);
    sink += sv_cat.resolve_batch(queries, 5);
    sink += intern_cat.resolve_batch(query_ids, 5);

    auto t0 = std::chrono::steady_clock::now();
    std::int64_t r1 = naive.resolve_batch(queries, kFrames);
    auto t1 = std::chrono::steady_clock::now();

    std::int64_t r2 = sv_cat.resolve_batch(queries, kFrames);
    auto t2 = std::chrono::steady_clock::now();

    std::int64_t r3 = intern_cat.resolve_batch(query_ids, kFrames);
    auto t3 = std::chrono::steady_clock::now();

    sink += r1 + r2 + r3;

    double ms_naive = std::chrono::duration<double, std::milli>(t1 - t0).count();
    double ms_sv = std::chrono::duration<double, std::milli>(t2 - t1).count();
    double ms_intern = std::chrono::duration<double, std::milli>(t3 - t2).count();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Queries: " << kQueryCount << " x " << kFrames << " frames\n";
    std::cout << "Naive std::string substr: " << ms_naive << " ms (checksum " << r1 << ")\n";
    std::cout << "std::string_view lookup : " << ms_sv << " ms (" << (ms_naive / ms_sv) << "x faster, checksum " << r2 << ")\n";
    std::cout << "Interned uint32_t ID    : " << ms_intern << " ms (" << (ms_naive / ms_intern) << "x faster, checksum " << r3 << ")\n";
    return sink == 0 ? 1 : 0;
}
