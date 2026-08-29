import type { FC } from "react";

interface SidebarProps {
  currentTab: string;
  setCurrentTab: (tab: string) => void;
}

export const Sidebar: FC<SidebarProps> = ({ currentTab, setCurrentTab }) => {
  const navItems = [
    { id: "dashboard", label: "Dashboard", icon: "📊" },
    { id: "tasks", label: "Tarefas", icon: "✅" },
    { id: "goals", label: "Metas", icon: "🎯" },
    { id: "reminders", label: "Lembretes", icon: "🔔" },
    { id: "reports", label: "Relatórios", icon: "📈" },
  ];

  return (
    <aside className="w-64 bg-slate-50 dark:bg-slate-900 border-r border-slate-200 dark:border-slate-800 min-h-screen p-4 flex flex-col shrink-0 transition-colors duration-300">
      <div className="w-10 h-10 rounded-xl bg-purple-600 flex items-center justify-center font-bold text-xl shadow-lg shadow-purple-600/30 text-white">
        V
      </div>

      <div className="mb-6">
        <h1 className="font-bold text-base leading-tight text-slate-100">
          Virtual Planner
        </h1>

        <span className="text-xs text-purple-400 font-medium">
          Painel Integrado
        </span>
      </div>

      <nav className="flex-1 space-y-1.5">
        {navItems.map((item) => {
          const isActive = currentTab === item.id;

          return (
            <button
              key={item.id}
              onClick={() => setCurrentTab(item.id)}
              className={`w-full flex items-center gap-3 px-4 py-3 rounded-xl text-sm font-medium transition-all duration-200 ${
                isActive
                  ? "bg-purple-600 text-white shadow-lg shadow-purple-600/30"
                  : "text-slate-400 hover:bg-slate-800/60 hover:text-slate-200"
              }`}
            >
              <span className="text-lg">{item.icon}</span>
              <span>{item.label}</span>
            </button>
          );
        })}
      </nav>
    </aside>
  );
};
