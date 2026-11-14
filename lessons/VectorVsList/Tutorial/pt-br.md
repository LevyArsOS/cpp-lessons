# Vector vs List (Remoção com Swap-and-Pop)

## Por que `std::list` prejudica o game loop
Quando entidades surgem e são destruídas a cada frame, escolher `std::list` parece natural porque `erase(it)` não desloca os elementos seguintes. Na prática, cada nó de `std::list` é uma alocação separada no heap espalhada pela memória, destruindo a localidade de cache da CPU durante a iteração do frame e tornando buscas por índice $O(n)$.

## O padrão Swap-and-Pop em $O(1)$
Quando a ordem dos elementos não importa (projéteis, partículas, inimigos, timers ativos), `std::vector` remove qualquer elemento em `index` em tempo $O(1)$:

```cpp
bool remove_at(std::size_t index) noexcept {
    if (index >= entities_.size()) {
        return false;
    }
    if (index + 1 < entities_.size()) {
        entities_[index] = std::move(entities_.back());
    }
    entities_.pop_back();
    return true;
}
```

## Trade-offs
- **Vantagens**: Memória contígua, prefetching de hardware, zero overhead de ponteiros por nó e remoção em $O(1)$ (`161x`–`184x` mais rápido que `std::list` em cargas mistas de atualização + remoção).
- **Desvantagens**: A ordem dos elementos muda após a remoção, e índices/iteradores para o último elemento movido são invalidados.
