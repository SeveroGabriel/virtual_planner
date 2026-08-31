# Imagem independente do frontend. Contexto de build: front-end/.
#
#   docker build -t virtual-planner-frontend ./front-end

# --------------------------------------------------------------------------
# Estagio de build
# --------------------------------------------------------------------------
FROM node:22-alpine AS builder

WORKDIR /app

# package.json e lock primeiro: enquanto as dependencias nao mudarem, o docker
# reaproveita a camada do npm ci e o rebuild so refaz o bundle.
COPY package.json package-lock.json ./
RUN npm ci

COPY . ./

# O Vite embute as VITE_* em tempo de BUILD, entao a base da API precisa entrar
# aqui e nao como variavel do container. O padrao "/api" e relativo: o bundle
# chama a propria origem e o nginx faz o proxy para o servico `api`. Assim o
# navegador nunca ve duas origens, e o cookie de sessao SameSite=Strict viaja.
ARG VITE_API_URL=/api
ENV VITE_API_URL=${VITE_API_URL}

RUN npm run build

# --------------------------------------------------------------------------
# Estagio de runtime
# --------------------------------------------------------------------------
FROM nginx:1.27-alpine AS runtime

COPY --from=builder /app/dist /usr/share/nginx/html
COPY nginx.conf /etc/nginx/templates/default.conf.template

# O entrypoint oficial renderiza o template em runtime. O filtro preserva as
# variaveis do nginx ($uri, $host etc.) mesmo que existam no ambiente.
ENV PORT=80 \
    BACKEND_URL=http://api:8080 \
    NGINX_ENTRYPOINT_LOCAL_RESOLVERS=1 \
    NGINX_ENVSUBST_FILTER="^(PORT|BACKEND_URL|NGINX_LOCAL_RESOLVERS)$"

# Sem Node e sem node_modules na imagem final: o que vai para producao e o
# bundle estatico. Nenhuma credencial embutida: a unica VITE_* e a base da API,
# que e um caminho relativo. Tudo com prefixo VITE_ fica visivel no navegador e
# por isso nunca pode ser segredo.
EXPOSE 80
