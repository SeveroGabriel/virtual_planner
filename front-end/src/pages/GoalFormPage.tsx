import { useState, useEffect } from "react";
import type { SubmitEvent } from "react";
import { useNavigate, useParams, Link } from "react-router";
import type { Goal } from "../types/domain";
import { virtualPlannerApi } from "../lib/api/virtualPlannerApi";
import { formatDateForInput } from "../lib/formatters";

export type GoalFormData = Omit<Goal, "id">;

export function GoalFormPage() {
  const { id } = useParams<{ id: string }>();
  const navigate = useNavigate();
  const isEditing = Boolean(id);

  const [isLoading, setIsLoading] = useState(isEditing);
  const [formData, setFormData] = useState<GoalFormData>({
    description: "",
    category: "Study", // Usando estritamente a Category do domain.ts
    status: "In Progress", // Usando estritamente o GoalStatus do domain.ts
    period: "Monthly", // Usando estritamente o GoalPeriod do domain.ts
    reference_date: formatDateForInput(),
  });

  useEffect(() => {
    if (isEditing && id) {
      virtualPlannerApi
        .getGoals()
        .then((goals) => {
          const goalFound = goals.find((g) => g.id === Number(id));
          if (goalFound) {
            setFormData({
              description: goalFound.description,
              category: goalFound.category,
              status: goalFound.status,
              period: goalFound.period,
              reference_date: goalFound.reference_date,
            });
          }
        })
        .catch((err) => console.error("Erro ao carregar meta:", err))
        .finally(() => setIsLoading(false));
    }
  }, [id, isEditing]);

  const handleChange = (
    e: React.ChangeEvent<HTMLInputElement | HTMLSelectElement>,
  ) => {
    const { name, value } = e.target;
    setFormData((prev) => ({
      ...prev,
      [name]: value,
    }));
  };

  const handleSubmit = async (e: SubmitEvent<HTMLFormElement>) => {
    e.preventDefault();
    setIsLoading(true);

    try {
      if (isEditing && id) {
        await virtualPlannerApi.updateGoal(Number(id), formData);
      } else {
        await virtualPlannerApi.createGoal(formData);
      }
      navigate("/goals");
    } catch (error) {
      console.error("Erro ao persistir a meta:", error);
    } finally {
      setIsLoading(false);
    }
  };

  return (
    <div className="max-w-2xl mx-auto py-8">
      <div className="flex items-center justify-between mb-8">
        <h1 className="text-2xl font-bold text-slate-100">
          {isEditing ? "Editar Meta" : "Nova Meta"}
        </h1>
        <Link
          to="/goals"
          className="text-slate-400 hover:text-purple-400 transition-colors"
        >
          Voltar
        </Link>
      </div>

      <form
        onSubmit={handleSubmit}
        className="bg-white dark:bg-slate-900 border border-purple-900/30 rounded-2xl p-6 space-y-6 shadow-xl"
      >
        <div>
          <label className="block text-sm font-medium text-purple-300 mb-2">
            Descrição da Meta
          </label>
          <input
            type="text"
            name="description"
            value={formData.description}
            onChange={handleChange}
            required
            disabled={isLoading}
            className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            placeholder="Ex: Concluir projeto do semestre"
          />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Categoria
            </label>
            <select
              name="category"
              value={formData.category}
              onChange={handleChange}
              disabled={isLoading}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
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
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Data de Referência
            </label>
            <input
              type="date"
              name="reference_date"
              value={formData.reference_date}
              onChange={handleChange}
              required
              disabled={isLoading}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all [color-scheme:dark] disabled:opacity-50"
            />
          </div>

          <div>
            <label className="block text-sm font-medium text-purple-300 mb-2">
              Período
            </label>
            <select
              name="period"
              value={formData.period}
              onChange={handleChange}
              disabled={isLoading}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            >
              <option value="Weekly">Semanal</option>
              <option value="Monthly">Mensal</option>
              <option value="Yearly">Anual</option>
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
              disabled={isLoading}
              className="w-full bg-slate-950 border border-purple-900/50 rounded-lg px-4 py-2.5 text-slate-200 focus:outline-none focus:border-purple-600 focus:ring-1 focus:ring-purple-600 transition-all disabled:opacity-50"
            >
              <option value="In Progress">Em Andamento</option>
              <option value="Completed">Concluida</option>
              <option value="Partially Completed">
                Parcialmente Concluída
              </option>
              <option value="Failed">Falhou</option>
            </select>
          </div>
        </div>

        <div className="pt-4 border-t border-purple-900/30 flex justify-end gap-3">
          <Link
            to="/goals"
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
                : "Criar Meta"}
          </button>
        </div>
      </form>
    </div>
  );
}
