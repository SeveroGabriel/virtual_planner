import type { Task } from "../../types/domain";

interface TaskCardProps {
  task: Task;
}

export function TaskCard({ task }: TaskCardProps) {
  // Lógica para converter minutos (ex: 480) em relógio (08:00)
  const formatTime = (minutes: number) => {
    const h = Math.floor(minutes / 60)
      .toString()
      .padStart(2, "0");
    const m = (minutes % 60).toString().padStart(2, "0");
    return `${h}:${m}`;
  };

  // Exibir Turno OU Horário exato
  const timeDisplay = task.shift
    ? task.shift === "Morning"
      ? "Manhã"
      : task.shift === "Afternoon"
        ? "Tarde"
        : "Noite"
    : task.startMinutes !== undefined && task.endMinutes !== undefined
      ? `${formatTime(task.startMinutes)} - ${formatTime(task.endMinutes)}`
      : "Sem horário";

  const cardColor = task.color || "#8B5CF6";

  return (
    <div
      className="group relative flex flex-col p-4 rounded-xl shadow-sm transition-all duration-200 hover:-translate-y-1 cursor-pointer border border-zinc-800/80"
      // Fundo translúcido (cor + opacidade 15) para o Dark Mode
      style={{ backgroundColor: `${cardColor}15` }}
    >
      {/* Borda lateral sólida com a cor exata */}
      <div
        className="absolute left-0 top-0 bottom-0 w-1.5 rounded-l-xl"
        style={{ backgroundColor: cardColor }}
      />

      <div className="pl-2">
        {/* Cabeçalho do Card */}
        <div className="flex justify-between items-start mb-2">
          <span className="text-xs font-semibold text-zinc-400 uppercase tracking-wider">
            {task.category}
          </span>

          {/* Indicadores Visuais de Status/Prioridade */}
          {task.status === "Executed" && (
            <span title="Executada" className="text-xs">
              ✅
            </span>
          )}
          {task.status === "Pending" && task.priority === "High" && (
            <span title="Prioridade Alta" className="text-xs">
              ❗
            </span>
          )}
        </div>

        {/* Título da Tarefa */}
        <h3 className="text-zinc-100 font-medium text-sm mb-3 leading-snug">
          {task.description}
        </h3>

        {/* Rodapé de Tempo */}
        <div className="flex items-center text-zinc-400 text-xs font-medium bg-zinc-900/50 w-fit px-2 py-1 rounded-md">
          <span className="mr-1.5 text-xs" style={{ color: cardColor }}>
            🕒
          </span>
          {timeDisplay}
        </div>
      </div>
    </div>
  );
}
