# C++ Lessons

Practical C++20 lessons for game development with runnable examples, tutorials in English and
Brazilian Portuguese, and benchmarks that measure the cost of each approach.

[![CI](https://github.com/LevyArsOS/cpp-lessons/actions/workflows/ci.yml/badge.svg?branch=develop)](https://github.com/LevyArsOS/cpp-lessons/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/LevyArsOS/cpp-lessons?sort=semver)](https://github.com/LevyArsOS/cpp-lessons/releases)
[![License: MIT](https://img.shields.io/github/license/LevyArsOS/cpp-lessons)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-00599C?logo=cplusplus&logoColor=white)](https://en.cppreference.com/w/cpp/20)
[![Languages: EN | PT-BR](https://img.shields.io/badge/tutorials-EN%20%7C%20PT--BR-green)](#topics)

## Contents

- [Motivation](#motivation)
- [Topics](#topics)
- [Getting started](#getting-started)
- [Project structure](#project-structure)
- [Contributing](#contributing)
- [License](#license)

## Motivation

This project is motivated by [learning by teaching](https://en.wikipedia.org/wiki/Learning_by_teaching),
in the spirit of the **Feynman technique**: explaining a topic in simple terms to find the gaps in
your own understanding. Each lesson contrasts a naive approach with a cache-friendly, allocation-aware
optimized approach used in game loops, backed by reproducible benchmarks and explanations in both
English and Brazilian Portuguese.

## Topics

| Lesson | Tutorial | Benchmark |
|---|---|---|
| Memory Pool | [English](lessons/MemoryPool/Tutorial/en.md) · [Português](lessons/MemoryPool/Tutorial/pt-br.md) | [Results](lessons/MemoryPool/Benchmark.md) |


## Getting started

Prerequisite: a C++20 compiler (`g++ >= 13` or `clang++ >= 16`) and CMake `>= 3.24`.

```bash
git clone https://github.com/LevyArsOS/cpp-lessons.git
cd cpp-lessons
scripts/check    # build (warnings as errors) + unit tests
```

## Project structure

Each topic is a lesson under `lessons/<Lesson>/` with practical examples, a tutorial, unit tests,
and benchmarks that compare the approaches.

```text
lessons/<Lesson>/
  include/           naive and optimized header/implementation types
  benchmark.cpp      benchmark runner for the lesson
  Benchmark.md       recorded benchmark results and analysis
  Tutorial/en.md     tutorial in English
  Tutorial/pt-br.md  tutorial in Brazilian Portuguese
tests/<Lesson>/      unit tests for the example types
scripts/check        build and test entry point (also run by CI)
```

## Contributing

Changes land through pull requests to `develop`; see [CONTRIBUTING.md](CONTRIBUTING.md) (including
how to add a lesson) and the shared vocabulary in [GLOSSARY.md](GLOSSARY.md). Report
vulnerabilities privately as described in [SECURITY.md](SECURITY.md).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

**[LevyArsOS](https://github.com/LevyArsOS)**
