import type { FC } from "react";
import { useState } from "react";

type ThemeMode = "light" | "dark" | "system";

interface HeaderProps {
  currentTab: string;
  onQuickCreate: () => void;
  theme: ThemeMode;
  onThemeChange?: (theme: ThemeMode) => void;
  activeView?: string;
  onViewChange?: (id: string) => void;
}

export const Header: FC<HeaderProps> = ({
  currentTab,
  onQuickCreate,
  theme,
  onThemeChange,
  activeView,
  onViewChange,
}) => {
  const pageTitles: Record<string, string> = {
    dashboard: "Visão Geral",
    tasks: "Minhas Tarefas",
    goals: "Metas e Objetivos",
    reminders: "Lembretes",
  };

  const themeIcons: Record<ThemeMode, string> = {
    light: "🌞",
    dark: "🌙",
    system: "💻",
  };

  const mainTabs = [
    { id: "calendario", label: "Calendário", icon: "📅" },
    { id: "timeline", label: "Linha do tempo", icon: "📊" },
    { id: "categoria", label: "Por categoria", icon: "📁" },
    { id: "status", label: "Por status", icon: "✨" },
  ];

  const allViews = [
    { id: "calendario", label: "Calendário", icon: "📅" },
    { id: "timeline", label: "Linha do tempo", icon: "📊" },
    { id: "categoria", label: "Por categoria", icon: "📁" },
    { id: "status", label: "Por status", icon: "✨" },
    { id: "duracao-categoria", label: "Duração por categoria", icon: "📊" },
    { id: "duracao-status", label: "Duração por status", icon: "📊" },
    { id: "duracao-tempo", label: "Duração ao longo do tempo", icon: "📈" },
    { id: "prioridade", label: "Tarefas por prioridade", icon: "⭐" },
    { id: "todos-blocos", label: "Todos os blocos de tempo", icon: "📋" },
  ];

  const [isThemeOpen, setIsThemeOpen] = useState(false);
  // Estado para observar se menu está aberto
  const [isMoreOpen, setIsMoreOpen] = useState(false);

  // Calcula quantas abas ficam escondidas no menu
  const hiddenCount = allViews.length - mainTabs.length;
  return (
    <header className="h-16 border-b border-slate-200 dark:border-slate-800 bg-white dark:bg-slate-900 px-6 flex items-center justify-between shrink-0 transition-colors duration-300">
      {/* Título da Página */}
      <div className="flex items-center gap-6">
        <h2 className="text-lg font-bold text-slate-800 dark:text-slate-100">
          {pageTitles[currentTab] || "Painel"}
        </h2>
      </div>

      {onViewChange && (
        <div className="hidden lg:flex items-center gap-1.5">
          {/* cria abas principais */}
          {mainTabs.map((tab) => {
            const isSelected = activeView === tab.id;
            return (
              <button
                key={tab.id}
                type="button"
                onClick={() => onViewChange(tab.id)}
                className={`px-3 py-1.5 rounded-lg text-xs font-medium transition-all flex items-center gap-1.5 ${
                  isSelected
                    ? "bg-purple-600/10 dark:bg-purple-600/20 text-purple-600 dark:text-purple-400 font-semibold border border-purple-200 dark:border-purple-800/50"
                    : "text-slate-600 dark:text-slate-400 hover:text-slate-900 dark:hover:text-white hover:bg-slate-100 dark:hover:bg-slate-800"
                }`}
              >
                <span>{tab.icon}</span>
                <span>{tab.label}</span>
              </button>
            );
          })}

          <div className="relative">
            <button
              type="button"
              onClick={() => setIsMoreOpen(!isMoreOpen)}
              className="px-3 py-1.5 rounded-lg text-xs font-medium text-slate-600 dark:text-slate-400 hover:text-slate-900 dark:hover:text-white hover:bg-slate-100 dark:hover:bg-slate-800 transition-all flex items-center gap-1"
            >
              <span>{hiddenCount} more...</span>
            </button>

            {isMoreOpen && (
              <div className="absolute left-0 mt-2 w-64 bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 rounded-xl shadow-xl z-50 p-1.5">
                <div className="space-y-0.5">
                  {allViews.map((tab) => {
                    const isSelected = activeView === tab.id;
                    return (
                      <button
                        key={tab.id}
                        type="button"
                        onClick={() => {
                          onViewChange(tab.id);
                          setIsMoreOpen(false);
                        }}
                        className={`w-full text-left px-3 py-2 rounded-lg text-xs font-medium transition-colors flex items-center gap-2 ${
                          isSelected
                            ? "bg-purple-600 text-white font-semibold"
                            : "text-slate-600 dark:text-slate-300 hover:bg-slate-100 dark:hover:bg-slate-800"
                        }`}
                      >
                        <span>{tab.icon}</span>
                        <span>{tab.label}</span>
                      </button>
                    );
                  })}
                </div>
              </div>
            )}
          </div>
        </div>
      )}

      {/* Botões do Lado Direito */}
      <div className="flex items-center gap-4">
        {/* Botão de Tema */}

        {/* Seletor de Tema Estilo Dropdown */}
        <div className="relative">
          <button
            type="button"
            onClick={() => setIsThemeOpen(!isThemeOpen)}
            className="h-10 px-3 rounded-xl border border-slate-200 dark:border-slate-800 flex items-center gap-2 text-sm hover:bg-slate-100 dark:hover:bg-slate-800 transition-all active:scale-95 text-slate-700 dark:text-slate-200"
          >
            <span>{themeIcons[theme]}</span>
            <span className="text-xs">
              {theme === "light"
                ? "Claro"
                : theme === "dark"
                  ? "Escuro"
                  : "Sistema"}
            </span>
            <span className="text-[10px]">▾</span>
          </button>

          {isThemeOpen && (
            <div className="absolute right-0 mt-2 w-36 bg-white dark:bg-slate-900 border border-slate-200 dark:border-slate-800 rounded-xl shadow-xl z-50 p-1.5 space-y-0.5">
              {(["light", "dark", "system"] as ThemeMode[]).map((mode) => {
                const isSelected = theme === mode;
                return (
                  <button
                    key={mode}
                    type="button"
                    onClick={() => {
                      onThemeChange?.(mode);
                      setIsThemeOpen(false);
                    }}
                    className={`w-full text-left px-3 py-2 rounded-lg text-xs font-medium transition-colors flex items-center gap-2 capitalize ${
                      isSelected
                        ? "bg-purple-600 text-white font-semibold"
                        : "text-slate-600 dark:text-slate-300 hover:bg-slate-100 dark:hover:bg-slate-800"
                    }`}
                  >
                    <span>{themeIcons[mode]}</span>
                    <span>
                      {mode === "light"
                        ? "Claro"
                        : mode === "dark"
                          ? "Escuro"
                          : "Sistema"}
                    </span>
                  </button>
                );
              })}
            </div>
          )}
        </div>

        {/* Botão Nova Tarefa */}
        <button
          type="button"
          onClick={onQuickCreate}
          className="flex items-center gap-2 px-3.5 py-2 rounded-xl bg-purple-600 hover:bg-purple-700 text-white text-sm font-semibold shadow-md shadow-purple-600/20 active:scale-95"
        >
          <span>+</span>
          <span>Nova Tarefa</span>
        </button>
      </div>
    </header>
  );
};
