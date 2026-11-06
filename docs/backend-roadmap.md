# Backend roadmap

Stage 1 establishes the `/api/v1` transport contract, module read use-cases, thread-local database
connection ownership, health/readiness probes, and a testable HTTP server. Auth, submissions, and
client applications are explicitly outside Stage 1. Follow-up implementation remains subject to
the real-PostgreSQL validation described in the backend development section of the README.

1. **Authentication and authorization** — account lifecycle, password/session policy, and protected
   API scopes.
2. **Lessons** — lesson DTOs and read use-cases beneath curriculum modules.
3. **Tasks** — task descriptions and test-case read APIs.
4. **Asynchronous submissions** — submission lifecycle, persistence, background processing, and
   polling/event delivery.
5. **Sandbox runner** — isolated execution service, resource limits, and result reporting.
6. **Duels** — matchmaking, realtime state, and gameplay APIs after the submission model is stable.
7. **React frontend** — a separately delivered web client using the versioned JSON contract.
8. **QML client** — a future UI consumer of the same HTTP contract; the desktop migration remains
   outside this backend slice.

Each step should extend the OpenAPI contract and fixtures first, then add use-cases, persistence,
and tests without coupling clients to Qt or Crow.
