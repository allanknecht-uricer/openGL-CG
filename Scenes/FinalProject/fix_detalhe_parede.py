"""Triangulate ngons in detalhe_parede.obj (Max export fan breaks on non-convex faces)."""
from __future__ import annotations

import math
import re
import sys
from pathlib import Path


def cross(a, b):
    return (
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    )


def dot(a, b):
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def sub(a, b):
    return (a[0] - b[0], a[1] - b[1], a[2] - b[2])


def polygon_normal(points):
    normal = (0.0, 0.0, 0.0)
    for i in range(len(points)):
        a = points[i]
        b = points[(i + 1) % len(points)]
        cx, cy, cz = cross(a, b)
        normal = (normal[0] + cx, normal[1] + cy, normal[2] + cz)
    length = math.sqrt(dot(normal, normal))
    if length < 1e-8:
        return (0.0, 0.0, 1.0)
    return (normal[0] / length, normal[1] / length, normal[2] / length)


def project_2d(points, normal):
    axis = (0.0, 0.0, 1.0)
    if abs(dot(normal, axis)) > 0.9:
        axis = (0.0, 1.0, 0.0)
    u = cross(normal, axis)
    u_len = math.sqrt(dot(u, u))
    u = (u[0] / u_len, u[1] / u_len, u[2] / u_len)
    v = cross(normal, u)
    return [(dot(p, u), dot(p, v)) for p in points]


def area2(a, b, c):
    return abs((b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])) * 0.5


def is_convex(prev, curr, nxt):
    return (curr[0] - prev[0]) * (nxt[1] - prev[1]) - (curr[1] - prev[1]) * (nxt[0] - prev[0]) >= 0


def point_in_tri(p, a, b, c):
    s1 = area2(p, a, b)
    s2 = area2(p, b, c)
    s3 = area2(p, c, a)
    total = area2(a, b, c)
    if total < 1e-8:
        return False
    return abs((s1 + s2 + s3) - total) < 1e-4


def ear_clip(indices, pts2):
    remaining = indices[:]
    tris = []
    guard = 0
    while len(remaining) > 3 and guard < 10000:
        guard += 1
        ear_found = False
        n = len(remaining)
        for i in range(n):
            prev_i = remaining[(i - 1) % n]
            curr_i = remaining[i]
            next_i = remaining[(i + 1) % n]
            a = pts2[prev_i]
            b = pts2[curr_i]
            c = pts2[next_i]
            if area2(a, b, c) < 1e-8:
                remaining.pop(i)
                ear_found = True
                break
            if not is_convex(a, b, c):
                continue
            if any(
                point_in_tri(pts2[remaining[j]], a, b, c)
                for j in range(n)
                if remaining[j] not in (prev_i, curr_i, next_i)
            ):
                continue
            tris.append((prev_i, curr_i, next_i))
            remaining.pop(i)
            ear_found = True
            break
        if not ear_found:
            # fallback fan
            base = remaining[0]
            for j in range(1, len(remaining) - 1):
                tris.append((base, remaining[j], remaining[j + 1]))
            remaining = remaining[:3]
            break
    if len(remaining) == 3:
        tris.append((remaining[0], remaining[1], remaining[2]))
    return tris


def parse_face_token(token):
    parts = token.split("/")
    return int(parts[0])


def fix_obj(src: Path, dst: Path):
    lines = src.read_text(encoding="utf-8", errors="ignore").splitlines()
    verts = []
    out_lines = []
    ngons_fixed = 0

    face_re = re.compile(r"^f[ \t]+")

    for line in lines:
        if line.startswith("v "):
            verts.append(tuple(map(float, line.split()[1:4])))
            out_lines.append(line)
            continue

        if not face_re.match(line):
            out_lines.append(line)
            continue

        tokens = line.split()[1:]
        vert_indices = [parse_face_token(t) - 1 for t in tokens]
        if len(vert_indices) <= 3:
            out_lines.append(line)
            continue

        points3 = [verts[i] for i in vert_indices]
        normal = polygon_normal(points3)
        pts2_map = {}
        for local_i, global_i in enumerate(vert_indices):
            pts2_map[local_i] = project_2d([points3[local_i]], normal)[0]

        pts2 = [pts2_map[i] for i in range(len(vert_indices))]
        local_order = list(range(len(vert_indices)))
        tris = ear_clip(local_order, pts2)
        ngons_fixed += 1
        for a, b, c in tris:
            out_lines.append("f " + " ".join([tokens[a], tokens[b], tokens[c]]))

    dst.write_text("\n".join(out_lines) + "\n", encoding="utf-8")
    print(f"Wrote {dst}")
    print(f"Triangulated {ngons_fixed} ngons")


if __name__ == "__main__":
    folder = Path(__file__).parent
    src = folder / "detalhe_parede.obj"
    dst = folder / "detalhe_parede_fixed.obj"
    if len(sys.argv) > 1:
        src = Path(sys.argv[1])
    if len(sys.argv) > 2:
        dst = Path(sys.argv[2])
    fix_obj(src, dst)
