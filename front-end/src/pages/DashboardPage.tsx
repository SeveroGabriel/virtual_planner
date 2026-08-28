import { useEffect, useState } from "react";
import { Link } from "react-router";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import { formatDateForInput } from "../lib/formatters";
import type { Task, Goal, Reminder } from "../types/domain";

export function DashboardPage() {
  const [tasks, setTasks] = useState<Task[]>([]);
  const [goals, setGoals] = useState<Goal[]>([]);
  const [reminders, setReminders] = useState<Reminder[]>([]);
  const [isLoading, setIsLoading] = useState<boolean>(true);

  const today = formatDateForInput();

  useEffect(() => {
    async function loadDashboardData() {
      setIsLoading(true);
      try {
        // Dispara as 3 requisições ao mesmo tempo (Concorrência)
        const [fetchedTasks, fetchedGoals, fetchedReminders] =
          await Promise.all([
            virtualPlannerApi.getTasks(),
            virtualPlannerApi.getGoals(),
            virtualPlannerApi.getReminders(),
          ]);

        setTasks(fetchedTasks);
        setGoals(fetchedGoals);
        setReminders(fetchedReminders);
      } catch (error) {
        console.error("Erro ao carregar dados do painel:", error);
      } finally {
        setIsLoading(false);
      }
    }

    loadDashboardData();
  }, []);

  // Lógica de Negócio: Filtragem de dados com complexidade O(N)
  const todayTasks = tasks.filter((t) => t.date === today);
  const inProgressGoals = goals.filter((g) => g.status === "In Progress");
  const todayReminders = reminders.filter((r) => r.date === today);

  const completedTasksCount = todayTasks.filter(
    (t) => t.status === "Executed",
  ).length;
  const pendingTasksCount = todayTasks.filter(
    (t) => t.status === "Pending" || t.status === "PartiallyExecuted",
  ).length;
  const productivityRate =
    todayTasks.length > 0
      ? Math.round((completedTasksCount / todayTasks.length) * 100)
      : 0;

  //(Se estiver carregando, bloqueia a renderização principal)
  if (isLoading) {
    return (
      <div className="flex h-full items-center justify-center">
        <span className="text-purple-500 font-bold text-xl animate-pulse">
          Carregando seu planejamento...
        </span>
      </div>
    );
  }

  return (
    <div className="p-6 space-y-6">
      <header className="flex justify-between items-center border-b border-purple-900/30 pb-4">
        <h1 className="text-3xl font-bold text-white">Resumo do Dia</h1>

        {/* Botões de Ação Rápida */}
        <div className="flex gap-3">
          <Link
            to="/tasks/new"
            className="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-md font-medium transition-colors"
          >
            + Nova Tarefa
          </Link>
          <Link
            to="/goals/new"
            className="border border-purple-500 text-purple-400 hover:bg-purple-500/10 px-4 py-2 rounded-md font-medium transition-colors"
          >
            + Nova Meta
          </Link>
        </div>
      </header>

      <div className="space-y-6">
        {/* Indicador Geral de Produtividade */}
        <div className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-xl p-5 shadow-lg">
          <div className="flex justify-between items-center mb-2">
            <h2 className="text-sm font-semibold text-purple-300 uppercase tracking-wider">
              Produtividade de Hoje
            </h2>
            <span className="text-xl font-bold text-white">
              {productivityRate}%
            </span>
          </div>
          <div className="w-full bg-slate-800 rounded-full h-2.5">
            <div
              className="bg-purple-500 h-2.5 rounded-full transition-all duration-500"
              style={{ width: `${productivityRate}%` }}
            ></div>
          </div>
        </div>

        {/* 4 Cards Analíticos */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
          <div className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-xl p-5 shadow-lg">
            <h3 className="text-purple-400 font-medium mb-1">Pendentes</h3>
            <p className="text-3xl font-bold text-slate-100">
              {pendingTasksCount}
            </p>
          </div>
          <div className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-xl p-5 shadow-lg">
            <h3 className="text-green-400 font-medium mb-1">Concluídas</h3>
            <p className="text-3xl font-bold text-slate-100">
              {completedTasksCount}
            </p>
          </div>
          <div className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-xl p-5 shadow-lg">
            <h3 className="text-blue-400 font-medium mb-1">Metas Ativas</h3>
            <p className="text-3xl font-bold text-slate-100">
              {inProgressGoals.length}
            </p>
          </div>
          <div className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-xl p-5 shadow-lg">
            <h3 className="text-amber-400 font-medium mb-1">Lembretes</h3>
            <p className="text-3xl font-bold text-slate-100">
              {todayReminders.length}
            </p>
          </div>
        </div>
      </div>

      {/* Grid Layout (Equivalente a matrizes visuais) */}
      <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
        {/* Coluna 1: Tarefas */}
        <section className="col-span-2 bg-gray-900 rounded-lg p-5 border border-gray-800 shadow-lg">
          <h2 className="text-xl font-semibold mb-4 text-purple-300">
            Tarefas de Hoje
          </h2>
          {todayTasks.length === 0 ? (
            <p className="text-gray-500 italic">Nenhuma tarefa para hoje.</p>
          ) : (
            <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
              {todayTasks.map((task) => (
                <div
                  key={task.id}
                  className="bg-white dark:bg-slate-800 p-4 rounded-xl border border-gray-200 dark:border-purple-900/50 shadow-sm flex flex-col gap-3 border-l-4 border-l-purple-500 transition-all hover:shadow-md"
                >
                  <span className="font-semibold text-gray-800 dark:text-gray-200">
                    {task.description}
                  </span>
                  <div className="flex justify-between items-center mt-auto">
                    <span className="text-xs font-medium text-purple-600 dark:text-purple-400">
                      {task.category === "Study" ? "Estudos" : task.category}
                    </span>
                    <span
                      className={`text-xs font-bold px-2 py-1 rounded ${
                        task.status === "Executed"
                          ? "bg-green-100 text-green-700 dark:bg-green-900/50 dark:text-green-400"
                          : "bg-yellow-100 text-yellow-700 dark:bg-yellow-900/50 dark:text-yellow-400"
                      }`}
                    >
                      {task.status === "Executed" ? "Concluída" : "Pendente"}
                    </span>
                  </div>
                </div>
              ))}
            </div>
          )}
        </section>

        {/* Coluna 2: Metas e Lembretes */}
        <div className="space-y-6">
          <section className="bg-gray-900 rounded-lg p-5 border border-gray-800 shadow-lg">
            <h2 className="text-lg font-semibold mb-4 text-purple-300">
              Metas em Andamento
            </h2>
            <ul className="space-y-2">
              {inProgressGoals.map((goal) => (
                <li
                  key={goal.id}
                  className="text-sm text-gray-300 flex items-center gap-2"
                >
                  <div className="w-2 h-2 rounded-full bg-purple-500"></div>
                  {goal.description}
                </li>
              ))}
            </ul>
          </section>

          <section className="bg-gray-900 rounded-lg p-5 border border-gray-800 shadow-lg">
            <h2 className="text-lg font-semibold mb-4 text-purple-300">
              Próximos Lembretes
            </h2>
            <ul className="space-y-2">
              {todayReminders.map((reminder) => (
                <li
                  key={reminder.id}
                  className="text-sm text-gray-300 bg-purple-900/20 p-2 rounded border border-purple-500/30"
                >
                  <span className="block font-medium text-purple-400">
                    {reminder.type}
                  </span>
                  {reminder.description}
                </li>
              ))}
            </ul>
          </section>
        </div>
      </div>
    </div>
  );
}
