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

