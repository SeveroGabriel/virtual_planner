import { useEffect, useState } from "react";
import { Link } from "react-router";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import type { Task } from "../types/domain";

export function TasksPage() {
  const [tasks, setTasks] = useState<Task[]>([]);
  const [isLoading, setIsLoading] = useState(true);
  const [selectedStatus, setSelectedStatus] = useState<string>("ALL");
  const [selectedCategory, setSelectedCategory] = useState<string>("ALL");

  useEffect(() => {
    async function loadTasks() {
      setIsLoading(true);
      try {
        const data = await virtualPlannerApi.getTasks();
        setTasks(data);
      } catch (error) {
        console.error("Erro ao buscar tarefas:", error);
      } finally {
        setIsLoading(false);
      }
    }

    loadTasks();
  }, []);

  async function handleDelete(id: number) {
    if (!window.confirm("Deseja realmente excluir esta tarefa?")) return;
    try {
      await virtualPlannerApi.deleteTask(id);
      setTasks((prev) => prev.filter((t) => t.id !== id));
    } catch (error) {
      console.error("Erro ao deletar tarefa:", error);
    }
  }

  const filteredTasks = tasks.filter((task) => {
    const matchStatus =
      selectedStatus === "ALL" || task.status === selectedStatus;
    const matchCategory =
      selectedCategory === "ALL" || task.category === selectedCategory;
    return matchStatus && matchCategory;
  });

  return (
    <div className="p-6 space-y-6">
      <header className="flex justify-between items-center border-b border-purple-900/30 pb-4">
        <div>
          <h1 className="text-3xl font-bold text-white">Tarefas</h1>
          <p className="text-sm text-gray-400">
            Gerencie suas atividades diárias.
          </p>
        </div>
        <Link
          to="/tasks/new"
          className="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-md font-medium shadow-lg shadow-purple-900/20"
        >
          + Nova Tarefa
        </Link>
      </header>

      <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 bg-gray-900 p-4 rounded-lg border border-purple-900/30">
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
            <option value="Pending">Pending</option>
            <option value="Executed">Executed</option>
            <option value="PartiallyExecuted">Partially Executed</option>
            <option value="Cancelled">Cancelled</option>
            <option value="Postponed">Postponed</option>
          </select>
        </div>

        <div>
          <label className="block text-xs font-semibold text-purple-300 mb-1">
            CATEGORIA
          </label>
          <select
            value={selectedCategory}
            onChange={(e) => setSelectedCategory(e.target.value)}
            className="w-full bg-gray-800 border border-purple-900/50 text-gray-200 rounded p-2 focus:outline-none focus:border-purple-500"
          >
            <option value="ALL">Todas as Categorias</option>
            <option value="College">College</option>
            <option value="Work">Work</option>
            <option value="Health">Health</option>
            <option value="Leisure">Leisure</option>
            <option value="PersonalProjects">Personal Projects</option>
            <option value="Study">Study</option>
          </select>
        </div>
      </div>

      <div className="bg-gray-900 rounded-lg border border-purple-900/30 overflow-hidden shadow-xl">
        {isLoading ? (
          <div className="p-8 text-center text-purple-400 animate-pulse font-medium">
            Carregando tarefas...
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="bg-gray-800/50 border-b border-gray-800 text-xs text-purple-300 uppercase tracking-wider">
                  <th className="p-4">Descrição</th>
                  <th className="p-4">Categoria</th>
                  <th className="p-4">Status</th>
                  <th className="p-4 text-right">Ações</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-800 text-sm">
                {filteredTasks.map((task) => (
                  <tr
                    key={task.id}
                    className="hover:bg-gray-800/40 transition-colors"
                  >
                    <td className="p-4 font-medium text-gray-200">
                      {task.description}
                    </td>
                    <td className="p-4 text-gray-400">{task.category}</td>
                    <td className="p-4">
                      <span
                        className={`px-2 py-1 rounded text-xs font-semibold ${
                          task.status === "Executed"
                            ? "text-green-400 bg-green-950/40"
                            : task.status === "Cancelled"
                              ? "text-red-400 bg-red-950/40"
                              : task.status === "PartiallyExecuted"
                                ? "text-yellow-400 bg-yellow-950/40"
                                : task.status === "Postponed"
                                  ? "text-orange-400 bg-orange-950/40"
                                  : "text-blue-400 bg-blue-950/40"
                        }`}
                      >
                        {task.status}
                      </span>
                    </td>
                    <td className="p-4 text-right space-x-2">
                      <Link
                        to={`/tasks/${task.id}/edit`}
                        className="text-purple-400 hover:text-purple-300 font-medium text-xs px-2 py-1 bg-purple-950/30 rounded border border-purple-900/50"
                      >
                        Editar
                      </Link>
                      <button
                        onClick={() => handleDelete(task.id)}
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
