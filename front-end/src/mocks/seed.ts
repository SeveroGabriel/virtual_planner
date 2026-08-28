import type { Task, Goal, Reminder, User } from "../types/domain";
import { formatDateForInput } from "../lib/formatters";

const dateFromToday = (offsetInDays: number): string => {
  const date = new Date();
  date.setDate(date.getDate() + offsetInDays);
  return formatDateForInput(date);
};

export const mockUser: User = {
  id: 1,
  name: "Desenvolvedor Principal",
  email: "dev@virtualplanner.com",
};

export const mockTasks: Task[] = [
  {
    id: 1,
    description: "Treino da manhã — força + mobilidade",
    category: "Health",
    date: dateFromToday(-3),
    startMinutes: 480, // 08:00
    endMinutes: 570, // 09:30
    priority: "Medium",
    status: "Executed",
    color: "#10B981", // Verde para saúde
  },
  {
    id: 2,
    description: "Reunião de alinhamento do projeto",
    category: "Work",
    date: dateFromToday(-2),
    startMinutes: 540,
    endMinutes: 660,
    priority: "High",
    status: "Executed",
  },
  {
    id: 3,
    description: "Sessão de estudos — React & TypeScript",
    category: "Study",
    date: dateFromToday(-1),
    startMinutes: 510,
    endMinutes: 600,
    priority: "High",
    status: "Executed",
    color: "#8B5CF6", // O nosso roxo para destaque de estudos
  },
  {
    id: 4,
    description: "Ajustar tipos do front-end com o C++",
    category: "Study",
    date: dateFromToday(0),
    startMinutes: 540,
    endMinutes: 600,
    priority: "High",
    status: "Executed",
  },
  {
    id: 5,
    description: "Implementar a estrutura do AppShell e Sidebar",
    category: "Work",
    date: dateFromToday(0),
    shift: "Morning",
    priority: "High",
    status: "Pending",
    color: "#8B5CF6",
  },
  {
    id: 6,
    description: "Conflito proposital: Reunião de emergência",
    category: "Work",
    date: dateFromToday(0),
    startMinutes: 570,
    endMinutes: 630,
    priority: "High",
    status: "Pending",
  },
  {
    id: 7,
    description: "Curso — prática de visualização de dados",
    category: "Study",
    date: dateFromToday(1),
    shift: "Afternoon",
    priority: "Low",
    status: "Pending",
  },
];

export const mockGoals: Goal[] = [
  {
    id: 1,
    description: "Concluir a integração do front-end com a API",
    category: "Work",
    status: "In Progress",
    period: "Weekly",
    reference_date: dateFromToday(0),
  },
  {
    id: 2,
    description: "Manter rotina de treino 4x por semana",
    category: "Health",
    status: "Completed",
    period: "Weekly",
    reference_date: dateFromToday(0),
  },
];

export const mockReminders: Reminder[] = [
  {
    id: 1,
    description: "Beber água e esticar as pernas",
    category: "Health",
    date: dateFromToday(0),
    startMinutes: 600,
    endMinutes: 610,
    type: "Exercise",
    recurrence: "Daily",
  },
];
