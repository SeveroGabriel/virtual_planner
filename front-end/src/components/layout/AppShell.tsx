import type { FC, ReactNode } from "react";
import { Sidebar } from "./Sidebar";
import { Header } from "./Header";

type ThemeMode = "light" | "dark" | "system";

interface AppShellProps {
  children?: ReactNode;
  currentTab: string;
  setCurrentTab: (tab: string) => void;
  onQuickCreate: () => void;
  theme: ThemeMode;
  onThemeChange?: (theme: ThemeMode) => void;
}

export const AppShell: FC<AppShellProps> = ({
  children,
  currentTab,
  setCurrentTab,
  onQuickCreate,
  theme,
  onThemeChange,
}) => {
  // Garante que sub-rotas (ex: /tasks/new -> tasks) acendam a aba correta no Sidebar
  const activeTab = currentTab ? currentTab.split("/")[0] : "dashboard";

  return (
    <div className="flex h-screen bg-[#191919] text-slate-200 overflow-hidden">
      <Sidebar currentTab={activeTab} setCurrentTab={setCurrentTab} />

      {/* Coluna Direita (Header + Miolo) */}
      <div className="flex flex-col flex-1 w-full overflow-hidden">
        {/* Topo Fixo da Coluna Direita */}
        <Header
          currentTab={activeTab}
          onQuickCreate={onQuickCreate}
          theme={theme}
          onThemeChange={onThemeChange}
        />

        {/* Área de Injeção das Telas */}
        <main className="flex-1 overflow-y-auto p-4 md:p-8 bg-[#121212]">
          {children}
        </main>
      </div>
    </div>
  );
};
