import { useEffect, useState } from "react";
import { Link } from "react-router";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import type { Goal } from "../types/domain";

export function GoalsPage() {
  const [goals, setGoals] = useState<Goal[]>([]);
  const [isLoading, setIsLoading] = useState<boolean>(true);

  // Filtros locais
  const [selectedPeriod, setSelectedPeriod] = useState<string>("ALL");
  const [selectedStatus, setSelectedStatus] = useState<string>("ALL");

  useEffect(() => {
    async function loadGoals() {
      setIsLoading(true);
      try {
        const data = await virtualPlannerApi.getGoals();
        setGoals(data);
      } catch (error) {
        console.error("Erro ao buscar metas:", error);
      } finally {
        setIsLoading(false);
      }
    }

    loadGoals();
  }, []);

  async function handleDelete(id: number) {
    if (!window.confirm("Deseja realmente excluir esta meta?")) return;

    try {
      await virtualPlannerApi.deleteGoal(id);
      setGoals((prev) => prev.filter((g) => g.id !== id));
    } catch (error) {
      console.error("Erro ao deletar meta:", error);
    }
  }

  // Filtragem no cliente
  const filteredGoals = goals.filter((goal) => {
    const matchPeriod =
      selectedPeriod === "ALL" || goal.period === selectedPeriod;
    const matchStatus =
      selectedStatus === "ALL" || goal.status === selectedStatus;
    return matchPeriod && matchStatus;
  });

  return (
    <div className="p-6 space-y-6">
      {/* Cabeçalho */}
      <header className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4 border-b border-purple-900/30 pb-4">
        <div>
          <h1 className="text-3xl font-bold text-white">Metas e Objetivos</h1>
          <p className="text-sm text-gray-400">
            Acompanhe seu progresso semanal, mensal e anual.
          </p>
        </div>

        <Link
          to="/goals/new"
          className="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-md font-medium transition-colors shadow-lg shadow-purple-900/20"
        >
          + Nova Meta
        </Link>
      </header>

      {/* Área de Filtros */}
      <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 bg-gray-900 p-4 rounded-lg border border-purple-900/30">
        <div>
          <label className="block text-xs font-semibold text-purple-300 mb-1">
            PERÍODO
          </label>
          <select
            value={selectedPeriod}
            onChange={(e) => setSelectedPeriod(e.target.value)}
            className="w-full bg-gray-800 border border-purple-900/50 text-gray-200 rounded p-2 focus:outline-none focus:border-purple-500"
          >
            <option value="ALL">Todos os Períodos</option>
            <option value="Weekly">Weekly</option>
            <option value="Monthly">Monthly</option>
            <option value="Yearly">Yearly</option>
          </select>
        </div>

        <div>
          <label className="block text-xs font-semibold text-purple-300 mb-1">
            STATUS
          </label>
          <select
            value={selectedStatus}
            onChange={(e) => setSelectedStatus(e.target.value)}
            className="w-full bg-gray-800 border border-purple-900/50 text-gray-200 rounded p-2 focus:outline-none focus:border-purple-500"
          >
            <option value="ALL">Todos os Status</option>
            <option value="In Progress">In Progress</option>
            <option value="Completed">Completed</option>
            <option value="Partially Completed">Partially Completed</option>
            <option value="Failed">Failed</option>
          </select>
        </div>
      </div>

      {/* Grade de Metas */}
      <div className="bg-gray-900 rounded-lg border border-purple-900/30 overflow-hidden shadow-xl">
        {isLoading ? (
          <div className="p-8 text-center text-purple-400 animate-pulse font-medium">
            Carregando metas...
          </div>
        ) : filteredGoals.length === 0 ? (
          <div className="p-8 text-center text-gray-500 italic">
            Nenhuma meta encontrada com os filtros atuais.
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="bg-gray-800/50 border-b border-gray-800 text-xs text-purple-300 uppercase tracking-wider">
                  <th className="p-4">Descrição</th>
                  <th className="p-4">Categoria</th>
                  <th className="p-4">Período</th>
                  <th className="p-4">Status</th>
                  <th className="p-4 text-right">Ações</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-800 text-sm">
                {filteredGoals.map((goal) => (
                  <tr
                    key={goal.id}
                    className="hover:bg-gray-800/40 transition-colors"
                  >
                    <td className="p-4 font-medium text-gray-200">
                      {goal.description}
                    </td>
                    <td className="p-4 text-gray-400">{goal.category}</td>

                    <td className="p-4">
                      <span className="px-2 py-1 rounded text-xs font-medium bg-purple-900/30 text-purple-300 border border-purple-800/50">
                        {goal.period === "Weekly"
                          ? "Semanal"
                          : goal.period === "Monthly"
                            ? "Mensal"
                            : "Anual"}
                      </span>
                    </td>

                    <td className="p-4">
                      <span
                        className={`px-2 py-1 rounded text-xs font-semibold ${
                          goal.status === "Completed"
                            ? "text-green-400 bg-green-950/40"
                            : goal.status === "Failed"
                              ? "text-red-400 bg-red-950/40"
                              : goal.status === "Partially Completed"
                                ? "text-yellow-400 bg-yellow-950/40"
                                : "text-blue-400 bg-blue-950/40"
                        }`}
                      >
                        {goal.status}
                      </span>
                    </td>

                    <td className="p-4 text-right space-x-2">
                      <Link
                        to={`/goals/${goal.id}/edit`}
                        className="text-purple-400 hover:text-purple-300 font-medium text-xs px-2 py-1 bg-purple-950/30 rounded border border-purple-900/50"
                      >
                        Editar
                      </Link>
                      <button
                        onClick={() => handleDelete(goal.id)}
                        className="text-red-400 hover:text-red-300 font-medium text-xs px-2 py-1 bg-red-950/30 rounded border border-red-900/50"
                      >
                        Excluir
                      </button>
                    </td>
                  </tr>
                ))}
              </tbody>
            </table>
          </div>
        )}
      </div>
    </div>
  );
}
