# Contrato público: `Date` e `TimeSlot`

Código-fonte:

- `back-end/include/virtual_planner/domain/value_objects/date.hpp`
- `back-end/src/domain/value_objects/date.cpp`
- `back-end/include/virtual_planner/domain/value_objects/time_slot.hpp`
- `back-end/src/domain/value_objects/time_slot.cpp`

## `Date`

Representa uma data de calendário (dia/mês/ano), no calendário Gregoriano.

### Construtor

```cpp
Date(std::uint32_t day, std::uint32_t month, std::uint32_t year);
```

Invariantes, validadas no construtor (lança `std::invalid_argument` quando violadas):

- `month` deve estar entre 1 e 12.
- `year` deve ser `>= 1900`. Não há limite superior.
- `day` deve estar entre 1 e o número de dias do `month`/`year` informados
  (28, 29, 30 ou 31, conforme o mês e se `year` é bissexto).
- Ano bissexto segue a regra Gregoriana padrão: divisível por 400, ou
  divisível por 4 e não por 100.

`Date` é imutável: não há setters. Para "mudar" uma data, construa uma nova
instância.

### Métodos

| Método | Retorno | Descrição |
|---|---|---|
| `day()` | `std::uint32_t` | Dia do mês, como passado no construtor. |
| `month()` | `std::uint32_t` | Mês, como passado no construtor. |
| `year()` | `std::uint32_t` | Ano, como passado no construtor. |
| `to_string()` | `std::string` | Formato `dd/mm/yyyy`, com zero à esquerda em dia e mês. |
| `operator==`, `operator!=` | `bool` | Igualdade por valor (dia, mês e ano). |
| `operator<`, `operator<=`, `operator>`, `operator>=` | `bool` | Ordenação cronológica (compara ano, depois mês, depois dia). |

## `TimeSlot`

Representa um intervalo de tempo dentro de um único dia, em minutos desde a
meia-noite (`std::chrono::minutes`).

### Construtor

```cpp
using Minutes = std::chrono::minutes;

TimeSlot(Minutes start, Minutes end);
```

Invariantes, validadas no construtor (lança `std::invalid_argument` quando violadas):

- `start.count() >= 0` — não é permitido tempo negativo.
- `end <= 24h` (1440 minutos) — o intervalo não pode ultrapassar o próprio dia.
- `end > start` — o intervalo não pode ser vazio nem invertido.

`TimeSlot` é imutável: não há setters.

### Métodos

| Método | Retorno | Descrição |
|---|---|---|
| `start()` | `Minutes` | Instante inicial, como passado no construtor. |
| `end()` | `Minutes` | Instante final, como passado no construtor. |
| `duration()` | `Minutes` | `end() - start()`. |
| `contains(Minutes time)` | `bool` | `true` se `time` está dentro do intervalo. |
| `overlaps(const TimeSlot& other)` | `bool` | `true` se os dois intervalos têm interseção não vazia. |

### Semântica de `contains` e `overlaps` — intervalo semiaberto `[start, end)`

`TimeSlot` trata seu próprio intervalo como **semiaberto**: o instante `start`
pertence ao intervalo, o instante `end` não pertence.

```cpp
bool TimeSlot::contains(Minutes time) const
{
    return time >= start_ && time < end_;
}

bool TimeSlot::overlaps(const TimeSlot& other) const
{
    return start_ < other.end_ && end_ > other.start_;
}
```

Consequência direta e deliberada: **dois `TimeSlot`s adjacentes (o fim de um
igual ao início do outro) NÃO se sobrepõem.**

```cpp
TimeSlot morning{std::chrono::hours{9}, std::chrono::hours{10}};   // [9h, 10h)
TimeSlot next{std::chrono::hours{10}, std::chrono::hours{11}};     // [10h, 11h)

morning.overlaps(next); // false — 10h é o fim de "morning" e o início de "next",
                         // mas 10h não pertence a "morning" (intervalo semiaberto)
```

Isso é consistente com `contains()`: `morning.contains(10h)` também é `false`.
Ou seja, é possível agendar uma tarefa das 9h às 10h e outra das 10h às 11h sem
que `overlaps` acuse conflito — esse é o comportamento esperado, não um caso
de borda a corrigir.

Sobreposição real (`true`) só ocorre quando os intervalos têm ao menos um
instante em comum dentro de ambos os `[start, end)`:

```cpp
TimeSlot a{std::chrono::hours{9}, std::chrono::hours{10}};   // [9h, 10h)
TimeSlot b{std::chrono::hours{9, 30}, std::chrono::hours{10, 30}}; // [9h30, 10h30)

a.overlaps(b); // true — [9h30, 10h) é comum aos dois
```

## API congelada — Onda 1

**Status:** a partir do fechamento da Onda 1 (data deste documento), a API
pública de `Date` e `TimeSlot` descrita acima está **congelada**:

- Assinaturas de construtor, métodos públicos e seus tipos de retorno.
- As invariantes de validação e as mensagens de erro que elas lançam.
- A semântica semiaberta de `contains`/`overlaps`, incluindo o caso adjacente
  documentado acima.

**Congelado significa:** nenhuma mudança de comportamento, assinatura ou
semântica nesses dois VOs sem:

1. Uma issue específica abrindo a mudança (não é permitido alterar como efeito
   colateral de outra issue).
2. Aviso prévio a **Laysa** (consumidora via `Reminder`) e ao **Arquiteto**
   (responsável pelo JSON compartilhado da P-29.0), já que ambos dependem
   deste contrato.

Motivo: `Date` e `TimeSlot` são consumidos por `Reminder` e serão serializados
no JSON compartilhado entre módulos. Uma mudança silenciosa nesta API durante
a Onda 2 quebraria esses dois consumidores no meio do trabalho deles.

Adições que não quebram compatibilidade (ex: um novo método) ainda exigem
issue própria, mas não necessariamente aviso prévio — a exigência de aviso é
para mudanças de comportamento ou remoção/alteração de assinatura existente.
