import { useState, useEffect } from "react";
import { useNavigate, useParams, Link } from "react-router";
import type { Shift, Task } from "../types/domain";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import { formatDateForInput } from "../lib/formatters";

export type TaskFormData = Omit<Task, "id">;

export function TaskFormPage() {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();
  const isEditing = Boolean(id);
  const [isLoading, setIsLoading] = useState(isEditing);
  const [validationError, setValidationError] = useState<string | null>(null);
  const [timeMode, setTimeMode] = useState<"exact" | "shift">("exact");
  const [shift, setShift] = useState<Shift>("Morning");

  const [formData, setFormData] = useState<TaskFormData>({
    description: "",
    category: "Study",
    date: formatDateForInput(),
    startMinutes: 480,
    endMinutes: 540,
    priority: "Medium",
    status: "Pending",
  });

  useEffect(() => {
    if (isEditing && id) {
      virtualPlannerApi
        .getTasks()
        .then((tasks) => {
          const taskFound = tasks.find((t) => t.id === Number(id));
          if (taskFound) {
            setFormData({
              description: taskFound.description,
              category: taskFound.category,
              date: taskFound.date,
              startMinutes: taskFound.startMinutes,
              endMinutes: taskFound.endMinutes,
              shift: taskFound.shift,
              priority: taskFound.priority,
              status: taskFound.status,
              color: taskFound.color,
            });
            if (taskFound.shift) {
              setShift(taskFound.shift);
              setTimeMode("shift");
            } else {
              setTimeMode("exact");
            }
          }
        })
        .catch((err) => console.error("Erro ao carregar tarefa:", err))
        .finally(() => setIsLoading(false));
    }
  }, [id, isEditing]);

  const handleChange = (
    e: React.ChangeEvent<HTMLInputElement | HTMLSelectElement>,
  ) => {
    const { name, value } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]:
        name === "startMinutes" || name === "endMinutes"
          ? Number(value)
          : value,
    }));
  };

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();

    const commonTaskData = {
      description: formData.description,
      category: formData.category,
      date: formData.date,
      priority: formData.priority,
      status: formData.status,
      color: formData.color,
    };

    let taskPayload: TaskFormData;
    if (timeMode === "shift") {
      taskPayload = { ...commonTaskData, shift };
    } else {
      const { startMinutes, endMinutes } = formData;
      if (
        startMinutes === undefined ||
        endMinutes === undefined ||
        startMinutes >= endMinutes
      ) {
        setValidationError("O horário final deve ser posterior ao horário inicial.");
        return;
      }
      taskPayload = { ...commonTaskData, startMinutes, endMinutes };
    }

    setValidationError(null);
    setIsLoading(true);
    try {
      if (isEditing && id) {
        await virtualPlannerApi.updateTask(Number(id), taskPayload);
      } else {
        await virtualPlannerApi.createTask(taskPayload);
      }
      navigate("/tasks");
    } catch (error) {
      console.error("Erro ao persistir a tarefa:", error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="max-w-3xl mx-auto py-8">
      <div className="flex items-center justify-between mb-8">
        <h1 className="text-2xl font-bold text-slate-100">
          {isEditing ? "Editar Tarefa" : "Nova Tarefa"}
        </h1>
        <Link
          to="/tasks"
          className="text-slate-400 hover:text-purple-400 transition-colors"
        >
          Voltar
        </Link>
      </div>

      <form
        onSubmit={handleSubmit}
        className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-2xl p-6 space-y-6 shadow-xl"
      >
        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <div className="md:col-span-2">
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Descrição
            </label>
            <input
              type="text"
              name="description"
              value={formData.description}
              onChange={handleChange}
              required
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Categoria
            </label>
            <select
              name="category"
              value={formData.category}
              onChange={handleChange}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600"
            >
              <option value="College">Faculdade</option>
              <option value="Work">Trabalho</option>
              <option value="Health">Saúde</option>
              <option value="Leisure">Lazer</option>
              <option value="PersonalProjects">Projetos Pessoais</option>
              <option value="Study">Estuda</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Data
            </label>
            <input
              type="date"
              name="date"
              value={formData.date}
              onChange={handleChange}
              required
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 [color-scheme:dark]"
            />
          </div>

          <div className="md:col-span-2 space-y-4">
            <div className="flex items-center gap-4 bg-slate-950 p-2 rounded-lg border border-purple-900/30 w-fit">
              <button
                type="button"
                onClick={() => {
                  setTimeMode("exact");
                  setFormData((previous) => ({
                    ...previous,
                    startMinutes: previous.startMinutes ?? 480,
                    endMinutes: previous.endMinutes ?? 540,
                  }));
                }}
                className={`px-4 py-2 rounded-md text-sm font-medium transition-colors ${timeMode === "exact" ? "bg-purple-600 text-white shadow-md" : "text-slate-400 hover:text-purple-300"}`}
              >
                Horário Exato
              </button>
              <button
                type="button"
                onClick={() => setTimeMode("shift")}
                className={`px-4 py-2 rounded-md text-sm font-medium transition-colors ${timeMode === "shift" ? "bg-purple-600 text-white shadow-md" : "text-slate-400 hover:text-purple-300"}`}
              >
                Turno do Dia
              </button>
            </div>

            {timeMode === "exact" ? (
              <div className="grid grid-cols-2 gap-6">
                <div>
                  <label className="block text-sm font-medium text-purple-300 mb-2">
                    Início (Minutos)
                  </label>
                  <input
                    type="number"
                    name="startMinutes"
                    value={formData.startMinutes ?? ""}
                    onChange={handleChange}
                    min={0}
                    max={1440}
                    required
                    className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600"
                  />
                </div>
                <div>
                  <label className="block text-sm font-medium text-purple-300 mb-2">
                    Fim (Minutos)
                  </label>
                  <input
                    type="number"
                    name="endMinutes"
                    value={formData.endMinutes ?? ""}
                    onChange={handleChange}
                    min={0}
                    max={1440}
                    required
                    className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600"
                  />
                </div>
              </div>
            ) : (
              <div>
                <label className="block text-sm font-medium text-purple-300 mb-2">
                  Selecione o Turno
                </label>
                <select
                  value={shift}
                  onChange={(event) => {
                    const value = event.currentTarget.value;
                    if (
                      value === "Morning" ||
                      value === "Afternoon" ||
                      value === "Evening"
                    ) {
                      setShift(value);
                    }
                  }}
                  className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600"
                >
                  <option value="Morning">Manhã</option>
                  <option value="Afternoon">Tarde</option>
                  <option value="Evening">Noite</option>
                </select>
              </div>
            )}
          </div>

          {validationError && (
            <p className="md:col-span-2 text-sm text-red-400" role="alert">
              {validationError}
            </p>
          )}

          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Prioridade
            </label>
            <select
              name="priority"
              value={formData.priority}
              onChange={handleChange}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600"
            >
              <option value="Low">Baixa</option>
              <option value="Medium">Media</option>
              <option value="High">Alta</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Status
            </label>
            <select
              name="status"
              value={formData.status}
              onChange={handleChange}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600"
            >
              <option value="Pending">Pendente</option>
              <option value="Executed">Executada</option>
              <option value="PartiallyExecuted">Parcialmente Executada</option>
              <option value="Cancelled">Cancelada</option>
              <option value="Postponed">Adiada</option>
            </select>
          </div>
        </div>

        <div className="pt-4 border-t border-purple-900/30 flex justify-end gap-3">
          <Link
            to="/tasks"
            className="px-5 py-2.5 rounded-lg text-sm font-medium text-purple-300 hover:bg-slate-800 transition-colors"
          >
            Cancelar
          </Link>
          <button
            type="submit"
            disabled={isLoading}
            className="px-5 py-2.5 rounded-lg text-sm font-medium bg-purple-600 text-white hover:bg-purple-700 shadow-lg shadow-purple-900/50 transition-all disabled:opacity-50"
          >
            {isLoading
              ? "Salvando..."
              : isEditing
                ? "Salvar Alterações"
                : "Criar Tarefa"}
          </button>
        </div>
      </form>
    </div>
  );
}
