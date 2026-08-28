# Contrato das métricas de relatório

> **ID do plano:** `P-63` (issue #38).

Este documento fecha as decisões de fórmula que a
[P-62](reporting-metrics-data.md) deixou em aberto. Ele é a fonte de verdade
para a **P-23** (serviço, #45), a **P-34** (endpoints, #54) e a **P-43**
(dashboard) — as três precisam produzir o mesmo número para a mesma entrada.

Ele **não** implementa nada. Se a implementação divergir daqui, o errado é a
implementação.

## Entrada

O serviço recebe um intervalo explícito, **inclusivo nas duas pontas**:

```cpp
struct ReportRequest
{
    domain::Date start_date;  // incluído
    domain::Date end_date;    // incluído
};
```

`start_date > end_date` lança `std::invalid_argument`, como
`ListRemindersUseCase` já faz. Um intervalo de um único dia (`start == end`) é
válido.

**Por que intervalo e não `period=weekly|monthly|yearly`:** o serviço fica puro e
testável com datas explícitas. A conversão de `period` + data-âncora para
intervalo é responsabilidade da camada de API (P-34), com esta regra:

| `period` | Intervalo a partir da data-âncora `d` |
|---|---|
| `weekly` | segunda-feira da semana de `d` até o domingo seguinte |
| `monthly` | primeiro até o último dia do mês de `d` |
| `yearly` | 1º de janeiro até 31 de dezembro do ano de `d` |

A semana é **ISO: começa na segunda-feira**. Isso resolve a lacuna E da P-62 e
vale também para o agrupamento por semana descrito abaixo.

## O que entra no período

| Entidade | Campo de data | Observação |
|---|---|---|
| `Goal` | `reference_date` | conta **uma vez**, mesmo que `period` seja `Weekly` |
| `Task` | `date` | |
| `Reminder` | — | **não entra em nenhuma métrica de produtividade** |

`Reminder` não tem status, logo não há como saber se foi cumprido. Ele fica fora
por decisão, não por esquecimento.

### Uma `Goal` semanal conta uma vez, não uma por semana

Decisão da ambiguidade 3 da P-62. `Goal` é uma entidade, não uma série. Se uma
meta `Weekly` contasse uma vez por semana, ela valeria 52 num relatório anual e
esmagaria qualquer meta `Yearly` na mesma conta.

## Peso dos status

Decisão da ambiguidade 1 da P-62: **o status parcial vale meio**.

| `GoalStatus` | Peso | | `TaskStatus` | Peso |
|---|---|---|---|---|
| `Completed` | 1,0 | | `Executed` | 1,0 |
| `PartiallyCompleted` | 0,5 | | `PartiallyExecuted` | 0,5 |
| `InProgress` | 0,0 | | `Pending` | 0,0 |
| `Failed` | 0,0 | | `Cancelled` | 0,0 |
| | | | `Postponed` | 0,0 |

Descartar o parcial perde informação que o domínio faz questão de registrar —
`PartiallyCompleted` só existe como status porque alguém achou o caso relevante.
Contá-lo como inteiro premia o incompleto. Meio é o meio-termo defensável.

A saída expõe **as contagens brutas separadas**, além da razão ponderada, para
que o dashboard possa mostrar "3 concluídas, 2 parciais" em vez de só "4,0".

## Denominador: tudo que estava previsto no período

Decisão da ambiguidade 2 da P-62. O denominador inclui **todos** os itens cuja
data cai no período, inclusive `Cancelled` e `Postponed`.

O teste que decide isso é o do incentivo perverso: se `Cancelled` saísse do
denominador, cancelar todas as tarefas daria **100% de execução**. Cancelar uma
tarefa é uma forma de não executá-la.

## Fórmulas

Para um período `P`, sejam:

```
G   = metas com reference_date em P
T   = tarefas com date em P
w(x) = peso do status de x, pela tabela acima
```

### 1. Metas cumpridas

```
goals_total      = |G|
goals_completed  = contagem de status == Completed
goals_partial    = contagem de status == PartiallyCompleted
goals_ratio      = ( Σ w(g) para g em G ) / |G|        se |G| > 0
                 = null                                 se |G| == 0
```

### 2. Tarefas executadas

```
tasks_total      = |T|
tasks_executed   = contagem de status == Executed
tasks_partial    = contagem de status == PartiallyExecuted
tasks_ratio      = ( Σ w(t) para t em T ) / |T|        se |T| > 0
                 = null                                 se |T| == 0
```

### 3 e 4. Semanas e meses mais produtivos

Agrupe `T` por semana ISO e por mês. O **score** de um balde é a soma dos pesos
das tarefas dele:

```
score(balde) = Σ w(t) para t no balde
```

Decisão da ambiguidade 4 da P-62: **o critério é o score absoluto, não o
percentual.** Uma semana com 8 de 10 tarefas produziu mais que uma com 2 de 2. O
percentual continua disponível por balde para quem quiser exibi-lo, mas não é o
que ordena.

Decisão da ambiguidade 5: **empate devolve todos os empatados**, em ordem
cronológica. Não há desempate arbitrário — se duas semanas produziram o mesmo,
as duas são as mais produtivas.

Baldes com score zero **não** entram na lista. Um período inteiro sem execução
devolve lista vazia, não "todas empatadas em zero".

### 5. Turnos mais produtivos

**Esta métrica depende de uma lacuna aberta.** `Task` não tem turno; `Shift` é um
enum órfão (lacuna A da P-62, escopo da #34/P-18, que está fechada sem entrega).

**Decisão: derivar o turno de `TimeSlot::start()`**, por faixas fixas, em vez de
esperar a #34. Isso destrava a P-23 sem alterar uma entidade de outra pessoa.

| Turno | Faixa de `start()` |
|---|---|
| `Morning` | `[00:00, 12:00)` |
| `Afternoon` | `[12:00, 18:00)` |
| `Evening` | `[18:00, 24:00)` |

**Uma tarefa que atravessa turnos conta apenas no turno em que começa.** Uma
tarefa das 11h às 15h é `Morning`. É uma simplificação deliberada: dividir o peso
entre turnos tornaria a soma dos turnos diferente da soma das tarefas, e ninguém
consegue explicar o número resultante.

Ordenação e empate: idênticos aos de semanas e meses.

> Se a #34 for reaberta e `Task` ganhar turno de verdade, **este contrato deve
> passar a ler o campo** em vez de derivar, e esta seção vira obsoleta. A
> derivação é uma ponte, não o destino.

### 6 e 7. Categorias mais realizadas

Agrupe por `Category`, separadamente para `Task` e `Goal`, usando o mesmo score
ponderado.

A saída é **completa, não esparsa**: toda categoria que aparece no período entra,
inclusive com score zero. Categorias sem nenhum item no período ficam de fora —
listar as seis sempre encheria o dashboard de zeros sem significado.

Diferente de semanas, meses e turnos, **categorias não são filtradas para o
máximo**: a lista traz todas, ordenadas por score decrescente. O desempate é
pelo rótulo em ordem alfabética, e existe só para a saída ser determinística —
sem ele, duas execuções sobre o mesmo dado poderiam ordenar diferente.

### 8. Indicador geral de produtividade

```
productivity_index = 0,5 × goals_ratio + 0,5 × tasks_ratio

  se apenas um dos dois existir  -> usa esse sozinho
  se nenhum existir              -> null
```

Resultado sempre em `[0, 1]`. Metas e tarefas pesam igual porque não há critério
objetivo para dizer que uma vale mais; se a equipe quiser outro peso, muda aqui e
em nenhum outro lugar.

## Período sem dados e divisão por zero

Esta é a regra que mais quebra implementação, então está explícita:

**Nenhuma razão é calculada com denominador zero.** Quando não há itens, o
resultado é **`null`**, não `0` e nunca `NaN`.

| Situação | `goals_ratio` | `tasks_ratio` | `productivity_index` | listas |
|---|---|---|---|---|
| Sem metas e sem tarefas | `null` | `null` | `null` | vazias |
| Metas, sem tarefas | valor | `null` | `= goals_ratio` | vazias |
| Tarefas, sem metas | `null` | valor | `= tasks_ratio` | preenchidas |
| Tudo cancelado | valor (`0,0`) | valor (`0,0`) | `0,0` | vazias |

A diferença entre as duas últimas linhas é o ponto: **`null` significa "não há o
que medir"; `0,0` significa "havia o que medir e não foi feito"**. Um dashboard
que mostre `0%` nos dois casos está mentindo em um deles.

## Saída

```cpp
struct BucketScore     // semana, mês, turno ou categoria
{
    std::string label;          // "2026-W35", "2026-08", "Morning", "Study"
    std::uint32_t total;        // itens no balde
    double score;               // soma ponderada
    std::optional<double> ratio; // score / total, ou nullopt se total == 0
};

struct ReportSummary
{
    domain::Date start_date;
    domain::Date end_date;

    std::uint32_t goals_total;
    std::uint32_t goals_completed;
    std::uint32_t goals_partially_completed;
    std::optional<double> goals_ratio;

    std::uint32_t tasks_total;
    std::uint32_t tasks_executed;
    std::uint32_t tasks_partially_executed;
    std::optional<double> tasks_ratio;

    std::vector<BucketScore> most_productive_weeks;
    std::vector<BucketScore> most_productive_months;
    std::vector<BucketScore> most_productive_shifts;
    std::vector<BucketScore> task_categories;
    std::vector<BucketScore> goal_categories;

    std::optional<double> productivity_index;
};
```

`std::optional<double>` é o que carrega a distinção `null` × `0,0` até a
serialização. Em JSON, `nullopt` vira `null`.

Enums nos rótulos usam a representação de `api::json` — `"Morning"`, `"Study"` —
que é a única representação de enum do projeto. Ver [api.md](api.md).

## Arredondamento

As razões são devolvidas **sem arredondamento**, como `double` em `[0, 1]`.
Formatar como porcentagem é decisão de apresentação, do frontend. O serviço não
arredonda porque arredondar cedo faz o índice geral divergir da média das partes.

## Onde isto vive

- Serviço: `application/reporting/reporting_service.{hpp,cpp}` (P-23). Sem SQL e
  sem HTTP. Lê `GoalRepository::find_all()` e `TaskRepository::find_all()`.
- Endpoints: `api/http`, consumindo o serviço (P-34). **A camada HTTP não
  recalcula nada.**
- Dashboard: consome o JSON (P-43).

## Revisão pendente

O critério de aceite desta issue pede revisão com os donos de `Goal` e `Task`.
Os dois pontos que mais os afetam:

- **Dani (`Goal`):** meta semanal conta uma vez pela `reference_date`, e
  `PartiallyCompleted` vale meio.
- **Bel (`Task`):** o turno é derivado de `TimeSlot::start()` enquanto a #34 não
  for entregue, e `Cancelled`/`Postponed` permanecem no denominador.
