# Imagem do frontend. Contexto de build e a RAIZ do repositorio, nao
# front-end/, para casar com o contexto usado pelo backend no compose.
#
#   docker build -f front-end/Dockerfile .

# --------------------------------------------------------------------------
# Estagio de build
# --------------------------------------------------------------------------
FROM node:22-alpine AS builder

WORKDIR /app

# package.json e lock primeiro: enquanto as dependencias nao mudarem, o docker
# reaproveita a camada do npm ci e o rebuild so refaz o bundle.
COPY front-end/package.json front-end/package-lock.json ./
RUN npm ci

COPY front-end/ ./
RUN npm run build

# --------------------------------------------------------------------------
# Estagio de runtime
# --------------------------------------------------------------------------
FROM nginx:1.27-alpine AS runtime

COPY --from=builder /app/dist /usr/share/nginx/html
COPY front-end/nginx.conf /etc/nginx/conf.d/default.conf

# Sem Node e sem node_modules na imagem final: o que vai para producao e o
# bundle estatico. Nenhuma credencial embutida — o frontend nao le variavel de
# ambiente nenhuma hoje, e quando ler, tudo com prefixo VITE_ fica visivel no
# navegador e por isso nunca pode ser segredo.
EXPOSE 80
