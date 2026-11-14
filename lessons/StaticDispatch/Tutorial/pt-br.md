# Static Dispatch (Virtual vs CRTP vs Tabelas de Função)

## O Custo de Loops Polimórficos com `virtual`
Chamar `e->update(dt)` em `std::vector<std::unique_ptr<IEntity>>` paga três custos por frame:
1. Pointer chasing no heap entre `unique_ptr` espalhados
2. Indireção da vtable impedindo inlining do compilador
3. Erros de predição de desvio na CPU quando 8 comportamentos distintos estão misturados

## Buckets Homogêneos com CRTP
Agrupar entidades por tipo em `std::tuple<std::vector<CrtpEntity<0>>, ...>` usando CRTP (`CrtpBase<Derived>`) elimina vtable e erros de predição de branch—rodando `8.62x` mais rápido (`4.381 ms` vs `37.782 ms`) em `50.000` entidades por `60` frames.

## Pontos Principais
- Memória contígua sozinha (`TagSwitch` / `FnTable`) ganha apenas `1.19x`–`1.30x` se os tipos continuarem embaralhados.
- Agrupar entidades em buckets homogêneos com dispatch estático (`CRTP`) libera inlining completo e entrega `8.62x` de ganho.
