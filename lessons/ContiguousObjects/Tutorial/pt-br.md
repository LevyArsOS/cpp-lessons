# Objetos Contíguos (`std::vector<T>` vs `std::vector<std::unique_ptr<T>>`)

## O Custo Oculto de Arrays de Ponteiros
Envolver cada entidade em `std::unique_ptr<Particle>` garante endereços estáveis, mas coloca cada objeto em uma alocação separada no heap. Iterar `100.000` ponteiros por frame gera cache misses constantes na CPU conforme o processador salta para endereços espalhados.

## Armazenamento Contíguo por Valor
Armazenar os valores diretamente em `std::vector<Particle>` elimina alocações individuais no heap e permite que o prefetcher da CPU leia linhas de cache de 64 bytes linearmente:

```cpp
// Pointer-chasing: 21.188 ms para 100k partículas x 60 frames
std::vector<std::unique_ptr<Particle>> scattered;

// Valores contíguos: 7.357 ms (2.88x mais rápido)
std::vector<Particle> contiguous;
```

## Pontos Principais
- Prefira `std::vector<T>` por valor para entidades numerosas (partículas, projéteis, tiles).
- Use `reserve(count)` na inicialização para evitar realocações no meio do frame.
- Reserve `std::unique_ptr<T>` apenas para objetos polimórficos raros ou que exijam estabilidade de ponteiro.
