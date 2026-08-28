import { useEffect, useState } from "react";
import { Link } from "react-router";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import type { Reminder } from "../types/domain";

export function RemindersPage() {
  const [reminders, setReminders] = useState<Reminder[]>([]);
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [selectedType, setSelectedType] = useState<string>("ALL");

  useEffect(() => {
    async function loadReminders() {
      setIsLoading(true);
      try {
        const data = await virtualPlannerApi.getReminders();
        setReminders(data);
      } catch (error) {
        console.error("Erro ao buscar lembretes:", error);
      } finally {
        setIsLoading(false);
      }
    }

    loadReminders();
  }, []);

  async function handleDelete(id: number) {
    if (!window.confirm("Deseja realmente excluir este lembrete?")) return;
    try {
      await virtualPlannerApi.deleteReminder(id);
      setReminders((prev) => prev.filter((r) => r.id !== id));
    } catch (error) {
      console.error("Erro ao deletar lembrete:", error);
    }
  }

  const filteredReminders = reminders.filter((reminder) => {
    return selectedType === "ALL" || reminder.type === selectedType;
  });

  return (
    <div className="p-6 space-y-6">
      <header className="flex flex-col md:flex-row justify-between items-start md:items-center gap-4 border-b border-purple-900/30 pb-4">
        <div>
          <h1 className="text-3xl font-bold text-white">Lembretes</h1>
          <p className="text-sm text-gray-400">
            Gerencie seus avisos e notificações importantes.
          </p>
        </div>
        <Link
          to="/reminders/new"
          className="bg-purple-600 hover:bg-purple-700 text-white px-4 py-2 rounded-md font-medium transition-colors shadow-lg shadow-purple-900/20"
        >
          + Novo Lembrete
        </Link>
      </header>

      <div className="grid grid-cols-1 sm:grid-cols-2 gap-4 bg-gray-900 p-4 rounded-lg border border-purple-900/30">
        <div>
          <label className="block text-xs font-semibold text-purple-300 mb-1">
            TIPO DE LEMBRETE
          </label>
          <select
            value={selectedType}
            onChange={(e) => setSelectedType(e.target.value)}
            className="w-full bg-gray-800 border border-purple-900/50 text-gray-200 rounded p-2 focus:outline-none focus:border-purple-500"
          >
            <option value="ALL">Todos os Tipos</option>
            <option value="Meeting">Meeting</option>
            <option value="PhoneCall">Phone Call</option>
            <option value="Shopping">Shopping</option>
            <option value="Study">Study</option>
            <option value="Exercise">Exercise</option>
            <option value="Assignment">Assignment</option>
          </select>
        </div>
      </div>

      <div className="bg-gray-900 rounded-lg border border-purple-900/30 overflow-hidden shadow-xl">
        {isLoading ? (
          <div className="p-8 text-center text-purple-400 animate-pulse font-medium">
            Carregando lembretes...
          </div>
        ) : (
          <div className="overflow-x-auto">
            <table className="w-full text-left border-collapse">
              <thead>
                <tr className="bg-gray-800/50 border-b border-gray-800 text-xs text-purple-300 uppercase tracking-wider">
                  <th className="p-4">Descrição</th>
                  <th className="p-4">Tipo</th>
                  <th className="p-4">Data</th>
                  <th className="p-4 text-right">Ações</th>
                </tr>
              </thead>
              <tbody className="divide-y divide-gray-800 text-sm">
                {filteredReminders.map((reminder) => (
                  <tr
                    key={reminder.id}
                    className="hover:bg-gray-800/40 transition-colors"
                  >
                    <td className="p-4 font-medium text-gray-200">
                      {reminder.description}
                    </td>
                    <td className="p-4">
                      <span className="px-2 py-1 rounded text-xs font-semibold bg-purple-900/30 text-purple-300 border border-purple-800/50">
                        {reminder.type}
                      </span>
                    </td>
                    <td className="p-4 text-gray-400">{reminder.date}</td>
                    <td className="p-4 text-right space-x-2">
                      <Link
                        to={`/reminders/${reminder.id}/edit`}
                        className="text-purple-400 hover:text-purple-300 font-medium text-xs px-2 py-1 bg-purple-950/30 rounded border border-purple-900/50"
                      >
                        Editar
                      </Link>
                      <button
                        onClick={() => handleDelete(reminder.id)}
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
