# Glossary

Shared vocabulary of this project. Lessons, code names, commits and discussions use these terms
exactly; a concept that needs a sentence to explain gets an entry here instead.

| Term | Meaning | Not to be confused with |
|---|---|---|
| Lesson | One topic folder under `lessons/` with examples, a benchmark and a tutorial in English and Brazilian Portuguese | A single tutorial file |
| Tutorial | The explanatory text of a lesson (`Tutorial/en.md`, `Tutorial/pt-br.md`) | The benchmark report (`Benchmark.md`) |
| Benchmark | A benchmark executable measuring the lesson's scenarios; run on demand, never in `scripts/check` | A unit test |
| Hot path | Per-frame or per-tick simulation code executed thousands of times per frame | One-time initialization or loading code |
| Arena allocator | Contiguous buffer allocator that advances a pointer per allocation and resets in bulk | General-purpose heap (`new`/`delete`) |
| Free-list pool | A fixed-capacity object allocator maintaining an intrusive singly linked list of recycled slots | Linear arena allocator |
| Slab allocator | An allocator carving memory into fixed-size chunks for same-sized object types | General-purpose variable allocator |
| Swap-and-pop | $O(1)$ removal from a contiguous array by overwriting the target slot with the back element and popping | Order-preserving `std::vector::erase` ($O(n)$) |
| Pointer chasing | Following scattered heap pointers during iteration, defeating CPU hardware prefetching | Linear contiguous traversal |
| SoA (Structure of Arrays) | Storing each field of an entity collection in its own contiguous array (`x[]`, `y[]`, `z[]`) for maximum cache-line density | AoS (`std::vector<Entity>`) |
| Static dispatch (CRTP) | Compile-time polymorphism via Curiously Recurring Template Pattern enabling full inlining without vtable indirection | Dynamic `virtual` dispatch |
| Heterogeneous lookup | Querying an associative container (`std::unordered_map`) via `std::string_view` (`is_transparent`) without constructing a temporary `std::string` key | Allocating a `std::string` key per lookup |
| Hardware prefetcher | CPU unit that detects sequential or strided memory access patterns and loads upcoming 64-byte cache lines into L1/L2 before instructions request them | Software `__builtin_prefetch` hints |
| Translation Lookaside Buffer (TLB) | Small hardware cache translating virtual memory addresses to physical page addresses; scattered heap allocations increase TLB misses | CPU L1/L2/L3 data cache |
| Branch Target Buffer (BTB) | CPU predictor caching the target address of indirect jumps and `virtual` calls; shuffled polymorphic types trigger frequent BTB mispredictions | Conditional branch direction predictor |
| Inlining | Compiler optimization replacing a function call with the callee body directly at the call site, unlocking constant propagation and SIMD vectorization | Indirect `virtual` call (`call *(%rax)`) |
| Sparse set | Dual-array ECS container pairing an `EntityId`-indexed `sparse` array with packed `dense_entities` and `dense_data` arrays for $O(1)$ lookup, $O(1)$ swap-and-pop removal, and contiguous iteration | Hash-bucketed `std::unordered_map` |
| Polymorphic memory resource (`std::pmr`) | C++17 `<memory_resource>` abstraction decoupling container types (`std::pmr::vector`) from their backing `memory_resource*` allocation strategy | Compile-time template allocator (`std::allocator<T>`) |

