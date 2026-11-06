# vcpkg overlay ports

Directory is registered in `vcpkg.json` via
`"vcpkg-configuration": { "overlay-ports": ["tools/vcpkg-ports"] }`
and overrides the upstream `vcpkg` ports of the same name.

## pcre2

Copy of the upstream `pcre2` port (10.49#2) with one change: instead of the
portfile's `api.github.com` lookup of the `sljit` gitlink (rate limited to 403
for shared CI runner IPs, and `GITHUB_TOKEN` is not honored for downloads),
the sljit commit recorded for that PCRE2 tag
(`de0259c7aaf36aa40cba8014f3fad3edde9307f9`) is pinned directly in
`portfile.cmake`.

Keep this port in sync with the upstream port when bumping the `pcre2`
version in `vcpkg.json`.
