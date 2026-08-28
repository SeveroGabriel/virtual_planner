# Métricas de relatório: dados necessários e casos de teste

> **ID do plano:** `P-62` (issue #28).

Este documento levanta, para cada métrica de relatório exigida pelo trabalho,
**quais dados do domínio ela precisa** e **se esses dados existem hoje**. Ele
não define fórmulas — isso é a P-63 (#38) — nem implementa o serviço, que é a
P-23 (#45).

O objetivo é descobrir agora, e não durante a implementação, que falta dado no
domínio.

## Estado do domínio hoje

| Entidade | Campos | Repositório |
|---|---|---|
| `Goal` | `id`, `description`, `category`, `status`, `period`, `reference_date` | `find_all`, `find_by_id`, `find_by_date_range`, `save`, `update`, `remove` |
| `Task` | `id`, `description`, `category`, `date`, `time_slot`, `priority`, `status` | `find_all`, `find_by_id`, `save`, `remove` |
| `Reminder` | `id`, `description`, `category`, `date`, `time_slot`, `type`, `recurrence` | `find_all`, `find_by_id`, `save`, `update`, `remove` |
| `User` | `id`, `name`, `email` | `find_all`, `find_by_id`, `save`, `remove` |

Enums relevantes: `GoalStatus` (`InProgress`, `Completed`, `PartiallyCompleted`,
`Failed`), `TaskStatus` (`Pending`, `Executed`, `PartiallyExecuted`, `Cancelled`,
`Postponed`), `Category`, `Priority`, `Shift` (`Morning`, `Afternoon`, `Evening`).

## Dados por métrica

| # | Métrica | Precisa de | Existe? |
|---|---|---|---|
| 1 | Quantidade e % de metas cumpridas | `Goal.status`, `Goal.reference_date` | **Sim** |
| 2 | Quantidade e % de tarefas executadas | `Task.status`, `Task.date` | **Sim** |
| 3 | Semanas mais produtivas | `Task.date`, `Task.status` (+ `Goal`) e conversão data → semana | **Parcial** — falta o cálculo de semana |
| 4 | Meses mais produtivos | `Task.date`, `Task.status` (+ `Goal`) | **Sim** |
| 5 | Turnos mais produtivos | turno da `Task` | **Não** — ver lacuna A |
| 6 | Categorias de tarefa mais realizadas | `Task.category`, `Task.status` | **Sim** |
| 7 | Categorias de meta mais realizadas | `Goal.category`, `Goal.status` | **Sim** |
| 8 | Indicador geral de produtividade | composição das anteriores | depende de 1–7 |

## Lacunas

### A. `Shift` é um enum órfão — bloqueia a métrica 5

`Shift` existe, tem `to_string`/`shift_from_string` e é serializado por
`api::json`, mas **nenhuma entidade o usa**. `Task` só tem `TimeSlot`. Sem turno
não há "turnos mais produtivos".

Isto era exatamente o escopo da **[P-18 (#34)](https://github.com/tiagojose76/virtual-planner/issues/34)**,
cujos critérios incluíam "É possível criar tarefa por turno e por intervalo" e
"`Shift` deixa de ser enum órfão". **A issue está fechada e nenhum dos dois foi
entregue** — `Task` continua sem turno. A P-23 não consegue implementar a
métrica 5 enquanto isso não for resolvido.

Duas saídas possíveis, e a escolha é da P-63:

1. Reabrir a P-18 e dar a `Task` um agendamento por turno.
2. **Derivar** o turno de `TimeSlot::start()` por faixas fixas. É mais barato,
   mas não é a mesma coisa: uma tarefa das 11h às 15h atravessa dois turnos, e a
   regra de desempate precisa ser escrita.

### B. Nenhuma entidade tem `user_id` — todo relatório é de um usuário só

`Goal`, `Task` e `Reminder` não têm dono. Os adapters PostgreSQL gravam com um
`kSingleTenantUserId` fixo igual a `1`. Ou seja, "as metas do usuário X" não é
uma pergunta que o domínio saiba responder hoje.

A ADR-002 prevê `user_id` em `Goal` na Onda 3. Enquanto isso, **toda métrica
deste documento é global**, e o serviço de relatórios não deve receber um
identificador de usuário que ele não conseguiria honrar.

### C. Só `GoalRepository` filtra por intervalo de datas

`TaskRepository` e `ReminderRepository` têm apenas `find_all()`. Um relatório de
período vai carregar tudo e filtrar em memória.

Para o volume de um trabalho acadêmico isso é aceitável e **não deve virar
otimização prematura**. Fica registrado para não surpreender: se o volume
crescer, o lugar de resolver é o contrato do repositório, não o serviço.

### D. `Reminder` não tem status

Lembretes não têm noção de "cumprido". Portanto **não entram em nenhuma métrica
de produtividade** — só serviriam para uma contagem de volume. A P-63 deve dizer
isso explicitamente, senão alguém vai tentar somá-los.

### E. `Date` não calcula semana

`Date` tem dia, mês e ano, e sua API está congelada pela P-61. Não há conversão
para número de semana. A métrica 3 precisa dessa conversão **na camada de
aplicação**, e a P-63 precisa definir a regra: semana ISO (segunda a domingo) ou
semana do calendário local. Sem isso, "semana mais produtiva" é ambíguo na
virada de ano.

## Ambiguidades para a P-63 decidir

Nenhuma delas é falta de dado — são decisões de fórmula, e cada uma muda o
resultado:

1. `PartiallyCompleted` conta como meta cumprida? E `PartiallyExecuted` como
   tarefa executada? Meio? Zero?
2. O denominador da porcentagem inclui itens `Cancelled` e `Postponed`, ou só o
   que estava previsto para o período?
3. Uma `Goal` `Weekly` com `reference_date` dentro do período conta uma vez, ou
   uma vez por semana do período?
4. "Mais produtivo" é contagem absoluta ou percentual? Uma semana com 2 de 2
   tarefas é mais produtiva que uma com 8 de 10?
5. Empate em "mais produtivo": devolve todos os empatados, ou desempata por
   ordem cronológica?

## Cenários de teste

Os cenários abaixo são o mínimo que a P-23 precisa cobrir. Os três primeiros são
os que costumam quebrar.

### Período sem dados

- Repositórios vazios, período válido.
- **Esperado:** todas as contagens em zero e **nenhuma divisão por zero**. As
  porcentagens precisam de um valor definido — a P-63 decide se é `0` ou
  "indefinido", mas não pode ser `NaN` nem exceção.
- Variante: existem metas e tarefas, mas todas **fora** do período.

### Bordas do período

- Item exatamente na data inicial e item exatamente na data final: os dois
  entram. O intervalo é inclusivo nas duas pontas, como em
  `ListRemindersUseCase`.
- Item um dia antes do início e um dia depois do fim: nenhum dos dois entra.
- Período de um único dia (`start == end`).
- Período invertido (`start > end`): rejeitado, como `ListRemindersUseCase` já
  faz com `std::invalid_argument`.

### Fevereiro e ano bissexto

- Período cobrindo 29/02/2024 e o mesmo período em 2023, que não tem 29/02.
- Período cruzando a virada de ano, que é onde a regra de semana da lacuna E
  aparece.

### Status

- Todas as tarefas `Cancelled`: a contagem de executadas é zero, e o
  denominador segue a regra da ambiguidade 2.
- Mistura de `Executed`, `PartiallyExecuted` e `Pending` no mesmo período.
- Todas as metas `Failed`.
- Uma meta de cada `GoalStatus` no mesmo período.

### Empates e ordenação

- Duas semanas com exatamente a mesma contagem.
- Dois meses empatados.
- Todas as categorias com a mesma contagem.

### Categorias

- Categoria que não aparece nenhuma vez no período não deve sumir do relatório
  se o contrato disser que todas aparecem com zero — a P-63 decide se a saída é
  esparsa ou completa.

## Pendências antes da P-23 começar

Todas resolvidas em [reporting-metrics-contract.md](reporting-metrics-contract.md) (P-63).

- [x] Lacuna A: o turno passa a ser **derivado** de `TimeSlot::start()` enquanto a #34 não for entregue
- [x] Ambiguidades 1 a 5 decididas
- [x] Lacuna E: a semana é ISO, começando na segunda-feira

As lacunas B (sem `user_id`), C (só `Goal` filtra por intervalo) e D (`Reminder`
sem status) continuam válidas e estão refletidas no contrato: os relatórios são
globais e `Reminder` fica fora das métricas de produtividade.
