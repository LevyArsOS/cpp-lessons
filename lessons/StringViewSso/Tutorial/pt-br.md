# String View, SSO e Interning de Strings

## Problema
Loops de jogos frequentemente resolvem chaves de assets, tags de entidades ou identificadores de localização no formato `"combat_ns:entity_attribute_modifier_value_42"`. Quando uma busca fatia a string com `std::string::substr` e consulta um `std::unordered_map<std::string, T>`, qualquer substring maior que a capacidade de *Small String Optimization* (SSO, 15 bytes no `libstdc++`) aloca e libera memória na heap a cada consulta.

## Solução
1. **Usar `std::string_view` com busca heterogênea (`is_transparent`):** `std::string_view::substr` apenas ajusta o ponteiro e o tamanho em registradores, sem alocar na heap. Com um hash transparente (`using is_transparent = void`), o `std::unordered_map` aceita `std::string_view` diretamente sem criar uma `std::string` temporária.
2. **Internar strings em IDs `uint32_t` no carregamento:** Para caminhos críticos executados milhares de vezes por frame, converta as chaves de string em IDs inteiros contíguos na inicialização e acesse um `std::vector` plano em tempo de execução.

## Resumo do Benchmark
Em `10.000` consultas por `200` frames (`g++ -O2`), `std::string_view` reduz o tempo de `86.858 ms` para `39.524 ms` (`2.20x` mais rápido, `0` alocações na heap), enquanto IDs `uint32_t` pré-internados concluem em `0.900 ms` (`96.47x` mais rápido).
