import { useState, useEffect } from "react";
import {
  BrowserRouter,
  Routes,
  Route,
  useLocation,
  useNavigate,
} from "react-router";
import { AppShell } from "./components/layout/AppShell";

import { DashboardPage } from "./pages/DashboardPage";
import { TasksPage } from "./pages/TasksPage";
import { TaskFormPage } from "./pages/TaskFormPage";
import { GoalsPage } from "./pages/GoalsPage";
import { GoalFormPage } from "./pages/GoalFormPage";
import { RemindersPage } from "./pages/RemindersPage";
import { ReminderFormPage } from "./pages/ReminderFormPage";
import { PlannerPage } from "./pages/PlannerPage";
import { ProfilePage } from "./pages/ProfilePage";
import { SettingsPage } from "./pages/SettingsPage";
import { ReportsPage } from "./pages/ReportsPage";

type ThemeMode = "light" | "dark" | "system";

function MainLayout() {
  const location = useLocation();
  const navigate = useNavigate();
  const [theme, setTheme] = useState<ThemeMode>("dark");

  // Sincroniza o tema com o documento HTML
  useEffect(() => {
    const root = window.document.documentElement;
    const systemPrefersDark = window.matchMedia(
      "(prefers-color-scheme: dark)",
    ).matches;

    root.classList.remove("dark");

    if (theme === "dark" || (theme === "system" && systemPrefersDark)) {
      root.classList.add("dark");
    }
  }, [theme]);

  // Mapeia a URL atual para a aba ativa do AppShell
  const currentTab = location.pathname.split("/")[1] || "dashboard";

  const handleTabChange = (tab: string) => {
    navigate(tab === "dashboard" ? "/" : `/${tab}`);
  };

  const handleQuickCreate = () => {
    navigate("/tasks/new");
  };

  return (
    <AppShell
      currentTab={currentTab}
      setCurrentTab={handleTabChange}
      theme={theme}
      onThemeChange={setTheme}
      onQuickCreate={handleQuickCreate}
    >
      <Routes>
        <Route path="/" element={<DashboardPage />} />

        <Route path="/tasks" element={<TasksPage />} />
        <Route path="/tasks/new" element={<TaskFormPage />} />
        <Route path="/tasks/:id/edit" element={<TaskFormPage />} />

        <Route path="/planner" element={<PlannerPage />} />

        <Route path="/goals" element={<GoalsPage />} />
        <Route path="/goals/new" element={<GoalFormPage />} />
        <Route path="/goals/:id/edit" element={<GoalFormPage />} />

        <Route path="/reminders" element={<RemindersPage />} />
        <Route path="/reminders/new" element={<ReminderFormPage />} />
        <Route path="/reminders/:id/edit" element={<ReminderFormPage />} />

        <Route path="/reports" element={<ReportsPage />} />

        <Route path="/profile" element={<ProfilePage />} />
        <Route path="/settings" element={<SettingsPage />} />
      </Routes>
    </AppShell>
  );
}

export default function App() {
  return (
    <BrowserRouter>
      <MainLayout />
    </BrowserRouter>
  );
}
