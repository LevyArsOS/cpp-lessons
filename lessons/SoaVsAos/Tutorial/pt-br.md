# SoA vs AoS (Física de Partículas e Culling)

## Por que AoS Polui Linhas de Cache
Em um Array of Structures (`AoS`), campos quentes de física (`x, y, z, vx, vy, vz`) dividem espaço com campos frios de renderização (`color`, `transform`, `flags`). Ao processar `500.000` partículas, a CPU carrega structs inteiras de 128 bytes para usar apenas 24 bytes—desperdiçando ~81% da banda de memória.

## Structure of Arrays (`SoA`)
Separar os canais quentes em vetores `std::vector<float>` paralelos empacota 16 coordenadas por linha de cache de 64 bytes e facilita a auto-vetorização SIMD:

```cpp
// AoS: 228.95 ms (carrega 104B de dados frios por partícula)
for (auto& p : particles_) {
    p.x += p.vx * dt; p.y += p.vy * dt; p.z += p.vz * dt;
}

// SoA: 31.53 ms (7.26x mais rápido, 100% de uso útil da cache line)
for (std::size_t i = 0; i < n; ++i) {
    px[i] += pvx[i] * dt; py[i] += pvy[i] * dt; pz[i] += pvz[i] * dt;
}
```

## Pontos Principais
- Separe canais quentes de simulação de metadados frios quando o volume de dados ultrapassar o cache L3.
- Use `__restrict` nos ponteiros de cada canal em C++ para liberar otimizações SIMD do compilador.
