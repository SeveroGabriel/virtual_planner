import { defineRailway, github, project, ref, service } from "railway/iac";

// Desired configuration only. Review the live environment before any apply:
// resources or variables omitted from IaC can be deleted by Railway.
export default defineRailway((ctx) => {
  // Refer to the existing database without choosing another image/version.
  const db = service("Postgres");

  const backend = service("backend", {
    source: github("tiagojose76/virtual-planner", {
      branch: "main",
      rootDirectory: "/back-end",
    }),
    build: {
      builder: "DOCKERFILE",
      dockerfilePath: "Dockerfile",
    },
    deploy: {
      preDeployCommand: ["/app/migrations/db-migrate.sh"],
    },
    healthcheck: "/health",
    healthcheckTimeout: 300,
    replicas: 1,
    env: {
      VP_APP_NAME: "virtual-planner",
      VP_PROFILE: "production",
      VP_LOG_LEVEL: "info",
      VP_USE_POSTGRES: "true",
      VP_HTTP_HOST: "0.0.0.0",
      PORT: "8080",
      RAILWAY_DOCKERFILE_PATH: "Dockerfile",
      VP_HTTP_ALLOWED_ORIGINS: "https://${{frontend.RAILWAY_PUBLIC_DOMAIN}}",
      POSTGRES_HOST: ref(db, "PGHOST"),
      POSTGRES_PORT: ref(db, "PGPORT"),
      POSTGRES_DB: ref(db, "PGDATABASE"),
      POSTGRES_USER: ref(db, "PGUSER"),
      POSTGRES_PASSWORD: ref(db, "PGPASSWORD"),
      POSTGRES_SSLMODE: "require",
      POSTGRES_CONNECT_TIMEOUT: "5",
      POSTGRES_APPLICATION_NAME: "virtual-planner",
    },
  });

  const frontend = service("frontend", {
    source: github("tiagojose76/virtual-planner", {
      branch: "main",
      rootDirectory: "/front-end",
    }),
    build: {
      builder: "DOCKERFILE",
      dockerfilePath: "Dockerfile",
    },
    healthcheck: "/health",
    healthcheckTimeout: 300,
    replicas: 1,
    env: {
      PORT: "8080",
      RAILWAY_DOCKERFILE_PATH: "Dockerfile",
      VITE_API_URL: "/api",
      BACKEND_URL: "http://${{backend.RAILWAY_PRIVATE_DOMAIN}}:${{backend.PORT}}",
    },
  });

  // The SDK's service() removes null fields. Keep explicit resets in the
  // resulting nodes so the plan can clear old build/start overrides.
  for (const app of [backend, frontend]) {
    app.build = { ...app.build, buildCommand: null };
    app.deploy = { ...app.deploy, startCommand: null };
  }

  return project(ctx.projectName ?? "virtual-planner", {
    resources: [db, backend, frontend],
  });
});
