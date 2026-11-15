#include "StringViewSso/include/string_lookup.hpp"

#include <cassert>
#include <iostream>
#include <new>

namespace {
std::size_t g_allocations = 0;
}  // namespace

void* operator new(std::size_t size) {
    ++g_allocations;
    void* p = std::malloc(size);
    if (!p) {
        throw std::bad_alloc();
    }
    return p;
}

void operator delete(void* p) noexcept {
    std::free(p);
}

void operator delete(void* p, std::size_t) noexcept {
    std::free(p);
}

int main() {
    using namespace string_view_sso;

    auto queries = make_sample_queries(1000);
    NaiveStringCatalog naive;
    StringViewCatalog sv_cat;
    InternedCatalog intern_cat;
    auto query_ids = intern_cat.preintern_queries(queries);

    // 1. Equivalence across all 3 catalogs
    std::int64_t sum_naive = naive.resolve_batch(queries, 5);
    std::int64_t sum_sv = sv_cat.resolve_batch(queries, 5);
    std::int64_t sum_intern = intern_cat.resolve_batch(query_ids, 5);

    assert(sum_naive > 0);
    assert(sum_naive == sum_sv);
    assert(sum_naive == sum_intern);

    // 2. Verify Naive allocates while StringView and Interned allocate 0 bytes on hot path
    std::size_t before_naive = g_allocations;
    (void)naive.resolve_batch(queries, 1);
    std::size_t naive_allocs = g_allocations - before_naive;
    assert(naive_allocs >= queries.size());

    std::size_t before_sv = g_allocations;
    (void)sv_cat.resolve_batch(queries, 1);
    (void)intern_cat.resolve_batch(query_ids, 1);
    std::size_t hot_allocs = g_allocations - before_sv;
    assert(hot_allocs == 0);

    // 3. Fault injection / invalid keys
    assert(naive.lookup("malformed_without_colon") == -1);
    assert(sv_cat.lookup("malformed_without_colon") == -1);
    assert(sv_cat.lookup("ns:") == -1);
    assert(sv_cat.lookup("ns:nonexistent_attribute_key_name_999") == -1);
    assert(intern_cat.lookup_id(InternedCatalog::kInvalidId) == -1);

    std::cout << "All StringViewSso tests passed successfully.\n";
    return 0;
}
