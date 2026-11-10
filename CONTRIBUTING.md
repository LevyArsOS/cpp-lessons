# Contributing

Thanks for helping improve C++ Lessons. Fixes, new lessons, translations and better benchmarks are
all welcome.

## Development setup

Prerequisite: a C++20 compiler (`g++ >= 13` or `clang++ >= 16`) and CMake `>= 3.24`.

```bash
git clone https://github.com/LevyArsOS/cpp-lessons.git
cd cpp-lessons
git switch develop
scripts/check
```

## Workflow

The project follows gitflow: `main` holds releases, `develop` integrates the next one.

1. Open or pick an issue; every change belongs to one.
2. Agree on the design in the issue before coding (maintainers keep the detailed specification
   outside this repository).
3. Branch from `develop` as `feature/<TASK>/<slug>` (`bugfix/` for fixes; `hotfix/` from `main`),
   where `<TASK>` is the issue number and `<slug>` is lowercase kebab-case.
4. Run `scripts/check` before pushing; CI runs it again.

## Commit messages

- [Conventional Commits](https://www.conventionalcommits.org/) in English:
  `<type>(<scope>)?: <description>`.
- Signed commits only.
- At most 200 changed lines per commit; split larger work into logical steps.

## Adding a lesson

A lesson is a folder under `lessons/<Lesson>/`:

| Path | Content |
|---|---|
| `include/` | Naive and optimized example types |
| `benchmark.cpp` | Benchmark measuring the lesson scenarios |
| `Benchmark.md` | Recorded results and analysis (machine, compiler, numbers) |
| `Tutorial/en.md` | Tutorial in English |
| `Tutorial/pt-br.md` | Tutorial in Brazilian Portuguese |

Then:

1. Add unit tests under `tests/<Lesson>/`.
2. Add a row to the Topics table in [README.md](README.md#topics).
3. Add new terms to [GLOSSARY.md](GLOSSARY.md).

By contributing, you agree that your contributions are licensed under the [MIT License](LICENSE).
