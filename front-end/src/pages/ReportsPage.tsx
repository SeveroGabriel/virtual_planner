import { useEffect, useState } from "react";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";

const REPORT_PERIODS = ["Semana", "Mês", "Ano"] as const;

type ReportPeriod = (typeof REPORT_PERIODS)[number];

interface ReportStats {
  tasks: { total: number; completed: number; percentage: number };
  goals: { total: number; completed: number; percentage: number };
  topTaskCategory: string;
  topGoalCategory: string;
  bestShift: string;
}

export function ReportsPage() {
  //Memória estado da tela
  const [filterType, setFilterType] = useState<ReportPeriod>("Semana");

  const [isLoading, setIsLoading] = useState(true);

  const [stats, setStats] = useState<ReportStats>({
    tasks: { total: 0, completed: 0, percentage: 0 },
    goals: { total: 0, completed: 0, percentage: 0 },
    topTaskCategory: "-",
    topGoalCategory: "-",
    bestShift: "_",
  });

  //Buscar dados
  useEffect(() => {
    async function fetchAndCalculateStats() {
      setIsLoading(true);

      try {
        // Consome os dados brutos da API simultanemante (em paralelo)
        const [allTasks, allGoals] = await Promise.all([
          virtualPlannerApi.getTasks(),
          virtualPlannerApi.getGoals(),
        ]);

        const totalTasks = allTasks.length;

        //Filtra apenas as tarefas concluídas
        const completedTasks = allTasks.filter(
          (t) => t.status === "Executed",
        );

        const taskPercentage =
          totalTasks === 0
            ? 0
            : Math.round((completedTasks.length / totalTasks) * 100);

        const totalGoals = allGoals.length;

        const completedGoals = allGoals.filter((g) => g.status === "Completed");

        const goalPercentage =
          totalGoals === 0
            ? 0
            : Math.round((completedGoals.length / totalGoals) * 100);

        //Ranking de Categorias
        const taskCategoriesCount: Record<string, number> = {};

        completedTasks.forEach((t) => {
          taskCategoriesCount[t.category] =
            (taskCategoriesCount[t.category] || 0) + 1;
        });

        // Pega as chaves (nomes das categorias), ordena pela quantidade (do maior pro menor) e pega a primeira [0].
        const topTaskCat =
          Object.keys(taskCategoriesCount).sort(
            (a, b) => taskCategoriesCount[b] - taskCategoriesCount[a],
          )[0] || "Nenhuma";

        const goalCategoriesCount: Record<string, number> = {};

        completedGoals.forEach((g) => {
          goalCategoriesCount[g.category] =
            (goalCategoriesCount[g.category] || 0) + 1;
        });

        const topGoalCat =
          Object.keys(goalCategoriesCount).sort(
            (a, b) => goalCategoriesCount[b] - goalCategoriesCount[a],
          )[0] || "Nenhuma";

        // Turno mais produtivo
        const shiftCounts = { Manhã: 0, Tarde: 0, Noite: 0 };

        completedTasks.forEach((t) => {
          // A verificação !== undefined garante ao TypeScript que é um número válido
          if (t.startMinutes !== undefined) {
            if (t.startMinutes < 720) {
              shiftCounts["Manhã"]++;
            } else if (t.startMinutes < 1080) {
              shiftCounts["Tarde"]++;
            } else {
              shiftCounts["Noite"]++;
            }
          }
        });

        // Loop Reduce: Compara Manhã, Tarde e Noite para ver quem tem a maior pontuação.
        const bestShift = Object.keys(shiftCounts).reduce((a, b) =>
          shiftCounts[a as keyof typeof shiftCounts] >
          shiftCounts[b as keyof typeof shiftCounts]
            ? a
            : b,
        );

        // Atualiza a memória da tela com os números finais processados.
        setStats({
          tasks: {
            total: totalTasks,
            completed: completedTasks.length,
            percentage: taskPercentage,
          },
          goals: {
            total: totalGoals,
            completed: completedGoals.length,
            percentage: goalPercentage,
          },
          topTaskCategory: topTaskCat,
          topGoalCategory: topGoalCat,
          // Garante que se não houver tarefas, mostre "Nenhum" ao invés do padrão que sobrar
          bestShift:
            shiftCounts[bestShift as keyof typeof shiftCounts] > 0
              ? bestShift
              : "Nenhum",
        });
      } catch (error) {
        console.error("Erro ao gerar relatório:", error);
      } finally {
        setIsLoading(false);
      }
    }

    fetchAndCalculateStats();
  }, [filterType]);

  return (
    <div className="p-6 max-w-[1200px] mx-auto space-y-8">
      <header className="flex flex-col md:flex-row md:items-center justify-between gap-4 border-b border-gray-200 dark:border-purple-900/30 pb-6">
        <div>
          <h1 className="text-3xl font-bold text-gray-900 dark:text-white">
            Painel Analítico
          </h1>
          <p className="text-sm text-gray-500 dark:text-gray-400 mt-1">
            Estatísticas e produtividade do seu planejamento
          </p>
        </div>

        {/* BOTÕES DE FILTRO (Semana, Mês, Ano) */}
        <div className="flex bg-gray-100 dark:bg-gray-900 p-1 rounded-lg border border-gray-200 dark:border-gray-800">
          {REPORT_PERIODS.map((period) => (
            <button
              key={period}
              onClick={() => setFilterType(period)}
              className={`px-4 py-2 text-sm font-medium rounded-md transition-colors ${
                filterType === period
                  ? "bg-white dark:bg-purple-600 text-purple-700 dark:text-white shadow-sm"
                  : "text-gray-600 dark:text-gray-400 hover:text-gray-900 dark:hover:text-gray-200"
              }`}
            >
              {period}
            </button>
          ))}
        </div>
      </header>

      {isLoading ? (
        // Se isLoading for true, mostra apenas este texto piscando
        <div className="text-center py-20 text-purple-500 animate-pulse font-medium">
          Calculando métricas...
        </div>
      ) : (
        // Se isLoading for false, abre a grade principal de relatórios
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          {/* Porcentagens Principais */}
          <div className="bg-gray-50 dark:bg-gray-900/60 border border-gray-200 dark:border-gray-800 rounded-2xl p-6 flex flex-col justify-center">
            <h2 className="text-lg font-semibold text-gray-900 dark:text-gray-100 mb-6">
              Taxa de Conclusão
            </h2>

            <div className="space-y-6">
              {/* Barra de Progresso de Metas */}
              <div>
                <div className="flex justify-between text-sm mb-2">
                  <span className="text-gray-600 dark:text-gray-300 font-medium">
                    Metas Cumpridas
                  </span>
                  <span className="text-gray-900 dark:text-white font-bold">
                    {stats.goals.percentage}%
                  </span>
                </div>
                {/* O "Trilho" da barra (Fundo cinza escuro) */}
                <div className="w-full bg-gray-200 dark:bg-gray-800 rounded-full h-3 overflow-hidden">
                  {/* A barra de preenchimento real */}
                  <div
                    className="bg-purple-600 h-3 rounded-full transition-all duration-1000 ease-out"
                    style={{ width: `${stats.goals.percentage}%` }}
                  ></div>
                </div>
                <p className="text-xs text-gray-500 mt-1">
                  {stats.goals.completed} de {stats.goals.total} metas
                </p>
              </div>

              {/* Barra de Progresso de Tarefas */}
              <div>
                <div className="flex justify-between text-sm mb-2">
                  <span className="text-gray-600 dark:text-gray-300 font-medium">
                    Tarefas Executadas
                  </span>
                  <span className="text-gray-900 dark:text-white font-bold">
                    {stats.tasks.percentage}%
                  </span>
                </div>
                <div className="w-full bg-gray-200 dark:bg-gray-800 rounded-full h-3 overflow-hidden">
                  <div
                    className="bg-emerald-500 h-3 rounded-full transition-all duration-1000 ease-out"
                    style={{ width: `${stats.tasks.percentage}%` }}
                  ></div>
                </div>
                <p className="text-xs text-gray-500 mt-1">
                  {stats.tasks.completed} de {stats.tasks.total} tarefas
                </p>
              </div>
            </div>
          </div>

          {/* SEÇÃO 2: Destaques (Bento Grid Style) */}
          <div className="grid grid-cols-2 gap-4">
            {/* Card 1: Turno */}
            <div className="bg-gray-50 dark:bg-gray-900/60 border border-gray-200 dark:border-gray-800 rounded-2xl p-5 flex flex-col justify-between hover:border-purple-500/50 transition-colors">
              <span className="text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                Turno mais Produtivo
              </span>
              <div className="mt-4">
                <span className="text-3xl font-bold text-gray-900 dark:text-white block">
                  {stats.bestShift}
                </span>
                <span className="text-sm text-purple-600 dark:text-purple-400">
                  Maior foco registrado
                </span>
              </div>
            </div>

            {/* Card 2: Top Categoria Tarefas */}
            <div className="bg-gray-50 dark:bg-gray-900/60 border border-gray-200 dark:border-gray-800 rounded-2xl p-5 flex flex-col justify-between hover:border-emerald-500/50 transition-colors">
              <span className="text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                Top Categoria (Tarefas)
              </span>
              <div className="mt-4">
                <span className="text-xl font-bold text-gray-900 dark:text-white block truncate">
                  {stats.topTaskCategory}
                </span>
                <span className="text-sm text-emerald-600 dark:text-emerald-400">
                  Mais executada
                </span>
              </div>
            </div>

            {/* Card 3: Top Categoria Metas (Ocupa as duas colunas: col-span-2) */}
            <div className="bg-gray-50 dark:bg-gray-900/60 border border-gray-200 dark:border-gray-800 rounded-2xl p-5 flex flex-col justify-between hover:border-amber-500/50 transition-colors col-span-2">
              <span className="text-xs font-semibold text-gray-500 dark:text-gray-400 uppercase tracking-wider">
                Top Categoria (Metas)
              </span>
              <div className="mt-3 flex items-center gap-3">
                <div className="p-3 bg-amber-100 dark:bg-amber-900/30 rounded-lg text-amber-600 dark:text-amber-400">
                  🎯
                </div>
                <div>
                  <span className="text-xl font-bold text-gray-900 dark:text-white block">
                    {stats.topGoalCategory}
                  </span>
                  <span className="text-sm text-gray-500 dark:text-gray-400">
                    Categoria com mais metas cumpridas
                  </span>
                </div>
              </div>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}
