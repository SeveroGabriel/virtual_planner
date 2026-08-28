import { useEffect, useState } from "react";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import { formatDateForInput } from "../lib/formatters";
import type { Task } from "../types/domain";

type TimeSlotItem = {
  id: string;
  type: "Task" | "Reminder";
  description: string;
  startMinutes: number;
  endMinutes: number;
  hasConflict?: boolean;
};

const hasExactTime = (
  task: Task,
): task is Task & { startMinutes: number; endMinutes: number } =>
  task.startMinutes !== undefined && task.endMinutes !== undefined;

export function PlannerPage() {
  const [items, setItems] = useState<TimeSlotItem[]>([]);
  const [isLoading, setIsLoading] = useState(true);

  const selectedDate = formatDateForInput();

  useEffect(() => {
    async function loadAgenda() {
      setIsLoading(true);
      try {
        const [tasks, reminders] = await Promise.all([
          virtualPlannerApi.getTasks(),
          virtualPlannerApi.getReminders(),
        ]);

        // Filtra pelo dia e mapeia para um formato único de TimeSlot
        const dayTasks: TimeSlotItem[] = tasks
          .filter((task) => task.date === selectedDate)
          .filter(hasExactTime)
          .map((task) => ({
            id: `task-${task.id}`,
            type: "Task",
            description: task.description,
            startMinutes: task.startMinutes,
            endMinutes: task.endMinutes,
          }));

        const dayReminders: TimeSlotItem[] = reminders
          .filter((r) => r.date === selectedDate)
          .map((r) => ({ ...r, id: `rem-${r.id}`, type: "Reminder" }));

        let agenda = [...dayTasks, ...dayReminders];

        // Lógica de Conflito (TimeSlot::overlaps)
        // Duas faixas se sobrepõem se: (StartA < EndB) e (EndA > StartB)
        agenda = agenda.map((item, i, arr) => {
          const hasConflict = arr.some(
            (other, j) =>
              i !== j &&
              item.startMinutes < other.endMinutes &&
              item.endMinutes > other.startMinutes,
          );
          return { ...item, hasConflict };
        });

        // Ordena cronologicamente
        agenda.sort((a, b) => a.startMinutes - b.startMinutes);
        setItems(agenda);
      } catch (error) {
        console.error("Erro ao carregar planejamento:", error);
      } finally {
        setIsLoading(false);
      }
    }
    loadAgenda();
  }, [selectedDate]);

  const formatTime = (mins: number) => {
    const h = Math.floor(mins / 60)
      .toString()
      .padStart(2, "0");
    const m = (mins % 60).toString().padStart(2, "0");
    return `${h}:${m}`;
  };

  return (
    <div className="p-6 space-y-6">
      <header className="border-b border-purple-900/30 pb-4">
        <h1 className="text-3xl font-bold text-white">Planejamento Diário</h1>
        <p className="text-sm text-gray-400">Agenda para {selectedDate}</p>
      </header>

      <div className="bg-gray-900 rounded-lg p-6 border border-gray-800 shadow-xl">
        {isLoading ? (
          <p className="text-purple-400 animate-pulse">
            Carregando horários...
          </p>
        ) : items.length === 0 ? (
          <p className="text-gray-500 italic">
            Nenhum evento agendado para hoje.
          </p>
        ) : (
          <div className="space-y-4">
            {items.map((item) => (
              <div
                key={item.id}
                className={`flex items-center gap-4 p-4 rounded-lg border-l-4 ${
                  item.hasConflict
                    ? "bg-red-950/20 border-red-500"
                    : "bg-gray-800 border-purple-500"
                }`}
              >
                <div className="w-24 shrink-0 text-center">
                  <span className="block text-sm font-bold text-purple-300">
                    {formatTime(item.startMinutes)}
                  </span>
                  <span className="block text-xs text-gray-500">
                    até {formatTime(item.endMinutes)}
                  </span>
                </div>

                <div className="flex-1">
                  <span className="text-xs font-semibold uppercase tracking-wider text-gray-400 block mb-1">
                    {item.type === "Task" ? "Tarefa" : "Lembrete"}
                  </span>
                  <p
                    className={`text-lg font-medium ${item.hasConflict ? "text-red-200" : "text-gray-200"}`}
                  >
                    {item.description}
                  </p>
                  {item.hasConflict && (
                    <span className="text-xs text-red-400 font-medium bg-red-950/50 px-2 py-0.5 rounded mt-1 inline-block">
                      ⚠️ Conflito de Horário detectado
                    </span>
                  )}
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  );
}
