import type { User } from "../types/domain";

export function ProfilePage() {
  // Mock do Usuário logado
  const user: User = {
    id: 1,
    name: "Desenvolvedor Backend",
    email: "dev@academico.br",
  };

  return (
    <div className="p-6 max-w-3xl mx-auto space-y-6">
      <h1 className="text-3xl font-bold text-white mb-8">Meu Perfil</h1>

      <div className="bg-gray-900 rounded-xl p-8 border border-gray-800 shadow-xl flex items-center gap-6">
        <div className="w-24 h-24 rounded-full bg-purple-600 flex items-center justify-center text-4xl font-bold text-white shadow-lg shadow-purple-600/30">
          {user.name.charAt(0)}
        </div>

        <div>
          <h2 className="text-2xl font-bold text-gray-100">{user.name}</h2>
          <p className="text-purple-400">{user.email}</p>
          <span className="inline-block mt-3 px-3 py-1 bg-gray-800 text-xs font-medium text-gray-300 rounded-full border border-gray-700">
            ID: {user.id}
          </span>
        </div>
      </div>
    </div>
  );
}
