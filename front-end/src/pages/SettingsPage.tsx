export function SettingsPage() {
  return (
    <div className="p-6 max-w-3xl mx-auto space-y-6">
      <h1 className="text-3xl font-bold text-white mb-8">
        Configurações & Status
      </h1>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
        <div className="bg-gray-900 rounded-xl p-6 border border-gray-800 shadow-lg">
          <h3 className="text-sm font-semibold text-gray-400 uppercase tracking-wider mb-4">
            Informações da Aplicação
          </h3>
          <ul className="space-y-3">
            <li className="flex justify-between">
              <span className="text-gray-400">Nome</span>
              <span className="font-medium text-gray-200">Virtual Planner</span>
            </li>
            <li className="flex justify-between">
              <span className="text-gray-400">Versão</span>
              <span className="font-medium text-gray-200">1.0.0</span>
            </li>
            <li className="flex justify-between">
              <span className="text-gray-400">Ambiente</span>
              <span className="font-medium text-purple-400">
                Desenvolvimento
              </span>
            </li>
          </ul>
        </div>

        <div className="bg-gray-900 rounded-xl p-6 border border-gray-800 shadow-lg">
          <h3 className="text-sm font-semibold text-gray-400 uppercase tracking-wider mb-4">
            Saúde do Sistema
          </h3>
          <ul className="space-y-4">
            <li className="flex items-center gap-3">
              <div className="w-2.5 h-2.5 rounded-full bg-slate-500"></div>
              <div>
                <p className="text-sm font-medium text-gray-200">API Status</p>
                <p className="text-xs text-gray-500">
                  Não verificado (dados simulados)
                </p>
              </div>
            </li>
            <li className="flex items-center gap-3">
              <div className="w-2.5 h-2.5 rounded-full bg-slate-500"></div>
              <div>
                <p className="text-sm font-medium text-gray-200">PostgreSQL</p>
                <p className="text-xs text-gray-500">
                  Não configurado no frontend
                </p>
              </div>
            </li>
          </ul>
        </div>
      </div>
    </div>
  );
}
