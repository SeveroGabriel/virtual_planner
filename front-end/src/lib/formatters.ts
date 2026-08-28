// src/lib/formatters.ts
import type { Category, Priority, TaskStatus } from "../types/domain";

// Converte minutos acumulados do dia (ex: 540) para HH:mm (ex: "09:00")
export function formatMinutesToTime(minutes: number): string {
  const hours = Math.floor(minutes / 60);
  const mins = minutes % 60;
  return `${String(hours).padStart(2, "0")}:${String(mins).padStart(2, "0")}`;
}

export function formatDateForInput(date: Date = new Date()): string {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, "0");
  const day = String(date.getDate()).padStart(2, "0");
  return `${year}-${month}-${day}`;
}

// Mapeamentos para exibição na UI em Português
export const CATEGORY_LABELS: Record<Category, string> = {
  College: "Faculdade",
  Work: "Trabalho",
  Health: "Saúde",
  Leisure: "Lazer",
  PersonalProjects: "Projetos Pessoais",
  Study: "Estudos",
};

export const CATEGORY_COLORS: Record<Category, string> = {
  College: "#3B82F6", // Blue
  Work: "#F59E0B", // Amber
  Health: "#10B981", // Green
  Leisure: "#EC4899", // Pink
  PersonalProjects: "#8B5CF6", // Purple (Identidade)
  Study: "#6366F1", // Indigo
};

export const PRIORITY_LABELS: Record<Priority, string> = {
  Low: "Baixa",
  Medium: "Média",
  High: "Alta",
};

export const TASK_STATUS_LABELS: Record<TaskStatus, string> = {
  Pending: "Pendente",
  Executed: "Concluída",
  PartiallyExecuted: "Parcial",
  Cancelled: "Cancelada",
  Postponed: "Adiada",
};
