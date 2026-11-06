# Backend roadmap

The initial `/api/v1` vertical slice establishes the transport contract, application use-cases,
database connection ownership, and a testable HTTP server. The following capabilities are
intentionally not implemented in this stage.

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
