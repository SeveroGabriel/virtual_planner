import { useState, useEffect } from "react";
import type { SubmitEvent } from "react";
import { useNavigate, useParams, Link } from "react-router";
import type { Reminder } from "../types/domain";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import { formatDateForInput } from "../lib/formatters";

export type ReminderFormData = Omit<Reminder, "id">;

export function ReminderFormPage() {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();
  const isEditing = Boolean(id);

  const [isLoading, setIsLoading] = useState(isEditing);
  const [validationError, setValidationError] = useState<string | null>(null);
  const [formData, setFormData] = useState<ReminderFormData>({
    description: "",
    category: "Study",
    date: formatDateForInput(),
    startMinutes: 480, // 08:00
    endMinutes: 540, // 09:00
    type: "Meeting",
    recurrence: "Once",
  });

  useEffect(() => {
    if (isEditing && id) {
      virtualPlannerApi
        .getReminders()
        .then((reminders) => {
          const reminderFound = reminders.find((r) => r.id === Number(id));
          if (reminderFound) {
            setFormData({
              description: reminderFound.description,
              category: reminderFound.category,
              date: reminderFound.date,
              startMinutes: reminderFound.startMinutes,
              endMinutes: reminderFound.endMinutes,
              type: reminderFound.type,
              recurrence: reminderFound.recurrence,
            });
          }
        })
        .catch((err) => console.error("Erro ao carregar lembrete:", err))
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

  const handleSubmit = async (e: SubmitEvent<HTMLFormElement>) => {
    e.preventDefault();

    if (formData.startMinutes >= formData.endMinutes) {
      setValidationError("O horário final deve ser posterior ao horário inicial.");
      return;
    }

    setValidationError(null);
    setIsLoading(true);

    try {
      if (isEditing && id) {
        await virtualPlannerApi.updateReminder(Number(id), formData);
      } else {
        await virtualPlannerApi.createReminder(formData);
      }
      navigate("/reminders");
    } catch (error) {
      console.error("Erro ao persistir o lembrete:", error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="max-w-2xl mx-auto py-8">
      <div className="flex items-center justify-between mb-8">
        <h1 className="text-2xl font-bold text-slate-100">
          {isEditing ? "Editar Lembrete" : "Novo Lembrete"}
        </h1>
        <Link
          to="/reminders"
          className="text-slate-400 hover:text-slate-200 transition-colors"
        >
          Voltar
        </Link>
      </div>

      <form
        onSubmit={handleSubmit}
        className="bg-white dark:bg-slate-900 border border-slate-800 rounded-2xl p-6 space-y-6 shadow-xl"
      >
        <div>
          <label className="block text-sm font-medium text-slate-400 mb-2">
            Descrição do Lembrete
          </label>
          <input
            type="text"
            name="description"
            value={formData.description}
            onChange={handleChange}
            required
            disabled={isLoading}
            className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            placeholder="Ex: Reunião de alinhamento com a equipe"
          />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Categoria
            </label>
            <select
              name="category"
              value={formData.category}
              onChange={handleChange}
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            >
              <option value="College">Faculdade</option>
              <option value="Work">Trabalho</option>
              <option value="Health">Saúde</option>
              <option value="Leisure">Lazer</option>
              <option value="PersonalProjects">Projetos Pessoais</option>
              <option value="Study">Estudo</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Tipo
            </label>
            <select
              name="type"
              value={formData.type}
              onChange={handleChange}
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            >
              <option value="Meeting">Reunião</option>
              <option value="PhoneCall">Ligação</option>
              <option value="Shopping">Compras</option>
              <option value="Study">Estudo</option>
              <option value="Exercise">Exercício</option>
              <option value="Assignment">Atividade</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Data
            </label>
            <input
              type="date"
              name="date"
              value={formData.date}
              onChange={handleChange}
              required
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all [color-scheme:dark] disabled:opacity-50"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Recorrência
            </label>
            <select
              name="recurrence"
              value={formData.recurrence}
              onChange={handleChange}
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            >
              <option value="Once">Uma vez</option>
              <option value="Daily">Diária</option>
              <option value="Weekly">Semanal</option>
              <option value="Monthly">Mensal</option>
            </select>
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Início (minutos)
            </label>
            <input
              type="number"
              name="startMinutes"
              value={formData.startMinutes}
              onChange={handleChange}
              min={0}
              max={1439}
              required
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-slate-400 mb-2">
              Fim (minutos)
            </label>
            <input
              type="number"
              name="endMinutes"
              value={formData.endMinutes}
              onChange={handleChange}
              min={1}
              max={1440}
              required
              disabled={isLoading}
              className="w-full bg-slate-950 border border-slate-800 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            />
          </div>
        </div>

        {validationError && (
          <p className="text-sm text-red-400" role="alert">
            {validationError}
          </p>
        )}

        <div className="pt-4 border-t border-slate-800 flex justify-end gap-3">
          <Link
            to="/reminders"
            className="px-5 py-2.5 rounded-lg text-sm font-medium text-slate-300 hover:bg-slate-800 transition-colors"
          >
            Cancelar
          </Link>
          <button
            type="submit"
            disabled={isLoading}
            className="px-5 py-2.5 rounded-lg text-sm font-medium bg-purple-600 text-white hover:bg-purple-700 shadow-lg shadow-purple-600/20 transition-all disabled:opacity-50"
          >
            {isLoading
              ? "Salvando..."
              : isEditing
                ? "Salvar Alterações"
                : "Criar Lembrete"}
          </button>
        </div>
      </form>
    </div>
  );
}
