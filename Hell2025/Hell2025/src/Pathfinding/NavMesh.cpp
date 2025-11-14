#include "NavMesh.h"
#include "Core/Game.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "World/World.h"

#include "AssetManagement/AssetManager.h"
#include "clipper2/clipper.h"
#include "HellGlm.h"
#include "earcut/earcut.hpp"
#include <glm/gtx/intersect.hpp>
#include "Util.h"

#include "clipper2/clipper.h"
#include "earcut/earcut.hpp"
#include "Modelling/Clipping.h"

namespace mapbox {
    namespace util {
        template <std::size_t I>
        struct nth<I, glm::vec2> {
            static_assert(I < 2, "Index out of range for glm::vec2");

            inline static float get(const glm::vec2& t) {
                return (I == 0) ? t.x : t.y;
            }
        };
    }
}

const double kScale = 1000.0;

namespace NavMesh {
    std::vector<NavTri> g_navMesh;

    glm::vec3 g_destination = glm::vec3(666.0f);

    struct FloorLevelPaths {
        Clipper2Lib::PathsD paths;
        float y = 0.0f;
    };

    void RenderNavTri(const NavTri& navTri, const glm::vec3& color);
    bool PointInNavTriXZ(const NavTri& tri, const glm::vec3& position);
    std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& dest);
    std::vector<glm::vec3> PullPath(const std::vector<glm::vec3>& path);

    Clipper2Lib::PathD ConvertToClipperPath(const std::vector<glm::vec2>& points) {
        Clipper2Lib::PathD path;
        for (const auto& pt : points) {
            path.push_back(Clipper2Lib::PointD(pt.x, pt.y));
        }
        return path;
    }

    inline bool PointsEqual(const glm::vec3& a, const glm::vec3& b, float epsilon = 0.0001f) {
        glm::vec3 d = a - b;
        return d.x * d.x + d.y * d.y + d.z * d.z <= epsilon * epsilon;
    }

    inline bool EdgesMatch(const glm::vec3& a0, const glm::vec3& a1, const glm::vec3& b0, const glm::vec3& b1, float epsilon = 0.0001f) {
        bool sameDir = PointsEqual(a0, b0, epsilon) && PointsEqual(a1, b1, epsilon);
        bool oppoDir = PointsEqual(a0, b1, epsilon) && PointsEqual(a1, b0, epsilon);
        return sameDir || oppoDir;
    }

    void CreateNavMesh() {
        g_navMesh.clear();

        std::unordered_map<int, FloorLevelPaths> floorLevels;

        for (HousePlane& housePlane : World::GetHousePlanes()) {
            if (housePlane.GetType() != HousePlaneType::FLOOR) continue;

            const HousePlaneCreateInfo& ci = housePlane.GetCreateInfo();

            float y = ci.p0.y;
            int key = (int)std::round(y * 1000.0f);

            std::vector<glm::vec2> floorPoly;
            floorPoly.reserve(4);
            floorPoly.emplace_back(ci.p0.x, ci.p0.z);
            floorPoly.emplace_back(ci.p1.x, ci.p1.z);
            floorPoly.emplace_back(ci.p2.x, ci.p2.z);
            floorPoly.emplace_back(ci.p3.x, ci.p3.z);

            FloorLevelPaths& level = floorLevels[key];
            level.y = y;
            level.paths.push_back(ConvertToClipperPath(floorPoly));
        }

        for (auto& it : floorLevels) {
            FloorLevelPaths& level = it.second;
            if (level.paths.empty()) continue;

            Clipper2Lib::PathsD empty;
            Clipper2Lib::PathsD solution = Clipper2Lib::Union(level.paths, empty, Clipper2Lib::FillRule::NonZero);
            if (solution.empty()) continue;

            for (const Clipper2Lib::PathD& path : solution) {
                if (path.empty()) continue;

                std::vector<std::vector<glm::vec2>> polygon;
                polygon.emplace_back();
                auto& ring = polygon.back();
                ring.reserve(path.size());
                for (const auto& p : path) {
                    ring.emplace_back((float)p.x, (float)p.y);
                }

                std::vector<uint32_t> indices = mapbox::earcut<uint32_t>(polygon);
                const std::vector<glm::vec2>& verts = ring;

                for (size_t i = 0; i + 2 < indices.size(); i += 3) {
                    uint32_t i0 = indices[i + 0];
                    uint32_t i1 = indices[i + 1];
                    uint32_t i2 = indices[i + 2];

                    const glm::vec2& v0 = verts[i0];
                    const glm::vec2& v1 = verts[i1];
                    const glm::vec2& v2 = verts[i2];

                    NavTri tri;
                    tri.v[0] = glm::vec3(v0.x, level.y, v0.y);
                    tri.v[1] = glm::vec3(v1.x, level.y, v1.y);
                    tri.v[2] = glm::vec3(v2.x, level.y, v2.y);

                    g_navMesh.push_back(tri);
                }
            }
        }

        for (int i = 0; i < (int)g_navMesh.size(); ++i) {
            for (int e = 0; e < 3; ++e) {
                g_navMesh[i].neighbor[e] = -1;

                glm::vec3 a0 = g_navMesh[i].v[e];
                glm::vec3 a1 = g_navMesh[i].v[(e + 1) % 3];

                for (int j = 0; j < (int)g_navMesh.size(); ++j) {
                    if (j == i) continue;

                    for (int f = 0; f < 3; ++f) {
                        glm::vec3 b0 = g_navMesh[j].v[f];
                        glm::vec3 b1 = g_navMesh[j].v[(f + 1) % 3];

                        if (EdgesMatch(a0, a1, b0, b1)) {
                            g_navMesh[i].neighbor[e] = j;
                        }
                    }
                }
            }
        }
    }

    void RenderDebug() {
        glm::vec3 viewPos = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();

        // Place destination
        if (Input::KeyPressed(HELL_KEY_P)) {
            g_destination = Game::GetLocalPlayerByIndex(0)->GetCameraPosition();
        }

        CreateNavMesh();

        for (NavTri& navTri : g_navMesh) {
            RenderNavTri(navTri, RED);
        }

        //for (NavTri& navTri : g_navMesh) {
        //    if (PointInNavTriXZ(navTri, viewPos)) {
        //
        //        if (navTri.neighbor[0] != -1) RenderNavTri(g_navMesh[navTri.neighbor[0]], BLUE);
        //        if (navTri.neighbor[1] != -1) RenderNavTri(g_navMesh[navTri.neighbor[1]], BLUE);
        //        if (navTri.neighbor[2] != -1) RenderNavTri(g_navMesh[navTri.neighbor[2]], BLUE);
        //
        //        RenderNavTri(navTri, GREEN);
        //    }
        //}

        // Find path
        std::vector<glm::vec3> rawPath = FindPath(viewPos, g_destination);
        std::vector<glm::vec3> pulledPath = PullPath(rawPath);
        
        std::vector<glm::vec3>& path = pulledPath;

        if (path.size() >= 2) {
            for (int i = 0; i < path.size() - 1; i++) {
                Renderer::DrawLine(path[i], path[i + 1], WHITE);
            }
        }
    }

    void RenderNavTri(const NavTri& navTri, const glm::vec3& color) {
        Renderer::DrawLine(navTri.v[0], navTri.v[1], color);
        Renderer::DrawLine(navTri.v[1], navTri.v[2], color);
        Renderer::DrawLine(navTri.v[2], navTri.v[0], color);
    }

    bool PointInNavTriXZ(const NavTri& tri, const glm::vec3& position) {
        glm::vec2 p(position.x, position.z);

        glm::vec2 a(tri.v[0].x, tri.v[0].z);
        glm::vec2 b(tri.v[1].x, tri.v[1].z);
        glm::vec2 c(tri.v[2].x, tri.v[2].z);

        glm::vec2 v0 = b - a;
        glm::vec2 v1 = c - a;
        glm::vec2 v2 = p - a;

        float d00 = glm::dot(v0, v0);
        float d01 = glm::dot(v0, v1);
        float d11 = glm::dot(v1, v1);
        float d20 = glm::dot(v2, v0);
        float d21 = glm::dot(v2, v1);

        float denom = d00 * d11 - d01 * d01;
        if (denom == 0.0f) return false;

        float invDenom = 1.0f / denom;
        float v = (d11 * d20 - d01 * d21) * invDenom;
        float w = (d00 * d21 - d01 * d20) * invDenom;
        float u = 1.0f - v - w;

        const float eps = 0.0001f;
        return u >= -eps && v >= -eps && w >= -eps;
    }





    static int FindContainingTriIndex(const glm::vec3& p) {
        for (int i = 0; i < (int)g_navMesh.size(); ++i) {
            if (PointInNavTriXZ(g_navMesh[i], p)) return i;
        }
        return -1;
    }

    static float TriArea2(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

    static bool GetSharedPortalXZ(int triA, int triB, glm::vec2& out0, glm::vec2& out1) {
        const NavTri& tA = g_navMesh[triA];
        const NavTri& tB = g_navMesh[triB];

        glm::vec3 shared3[2];
        int count = 0;

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (PointsEqual(tA.v[i], tB.v[j])) {
                    shared3[count++] = tA.v[i];
                    if (count == 2) {
                        out0 = glm::vec2(shared3[0].x, shared3[0].z);
                        out1 = glm::vec2(shared3[1].x, shared3[1].z);
                        return true;
                    }
                }
            }
        }
        return false;
    }

    static glm::vec3 LiftToNavMesh(const glm::vec2& p2) {
        glm::vec3 p3(p2.x, 0.0f, p2.y);
        int triIndex = FindContainingTriIndex(p3);
        if (triIndex >= 0) {
            const NavTri& tri = g_navMesh[triIndex];
            p3.y = tri.v[0].y;
        }
        return p3;
    }






    std::vector<glm::vec3> FindPath(const glm::vec3& start, const glm::vec3& dest) {
        std::vector<glm::vec3> result;
        if (g_navMesh.empty()) return result;

        auto TriCenter = [](const NavTri& t) {
            return (t.v[0] + t.v[1] + t.v[2]) * (1.0f / 3.0f);
        };

        auto TriHeight = [](const NavTri& t) {
            return t.v[0].y;
        };

        auto FindContainingTri = [&](const glm::vec3& p) {
            for (int i = 0; i < (int)g_navMesh.size(); ++i) {
                if (PointInNavTriXZ(g_navMesh[i], p)) return i;
            }
            return -1;
        };

        int startTri = FindContainingTri(start);
        int endTri = FindContainingTri(dest);
        if (startTri == -1 || endTri == -1) return result;

        if (startTri == endTri) {
            glm::vec3 s = start;
            glm::vec3 d = dest;
            float h = TriHeight(g_navMesh[startTri]);
            s.y = h;
            d.y = h;
            result.push_back(s);
            result.push_back(d);
            return result;
        }

        const int triCount = (int)g_navMesh.size();
        std::vector<float> gScore(triCount, std::numeric_limits<float>::max());
        std::vector<float> fScore(triCount, std::numeric_limits<float>::max());
        std::vector<int> cameFrom(triCount, -1);
        std::vector<bool> inOpen(triCount, false);
        std::vector<bool> inClosed(triCount, false);
        std::vector<int> openSet;

        auto Heuristic = [&](int triIndex) {
            glm::vec3 c = TriCenter(g_navMesh[triIndex]);
            glm::vec2 d(c.x - dest.x, c.z - dest.z);
            return glm::length(d);
        };

        gScore[startTri] = 0.0f;
        fScore[startTri] = Heuristic(startTri);
        openSet.push_back(startTri);
        inOpen[startTri] = true;

        bool found = false;

        while (!openSet.empty()) {
            int current = -1;
            float bestF = std::numeric_limits<float>::max();
            int bestIndex = -1;

            for (int i = 0; i < (int)openSet.size(); ++i) {
                int triIndex = openSet[i];
                if (fScore[triIndex] < bestF) {
                    bestF = fScore[triIndex];
                    current = triIndex;
                    bestIndex = i;
                }
            }

            if (current == -1) break;
            if (current == endTri) {
                found = true;
                break;
            }

            openSet[bestIndex] = openSet.back();
            openSet.pop_back();
            inOpen[current] = false;
            inClosed[current] = true;

            glm::vec3 currentCenter = TriCenter(g_navMesh[current]);

            for (int e = 0; e < 3; ++e) {
                int neighbor = g_navMesh[current].neighbor[e];
                if (neighbor < 0) continue;
                if (inClosed[neighbor]) continue;

                glm::vec3 neighborCenter = TriCenter(g_navMesh[neighbor]);
                glm::vec2 delta(neighborCenter.x - currentCenter.x, neighborCenter.z - currentCenter.z);
                float stepCost = glm::length(delta);
                float tentativeG = gScore[current] + stepCost;

                if (!inOpen[neighbor]) {
                    openSet.push_back(neighbor);
                    inOpen[neighbor] = true;
                }
                else if (tentativeG >= gScore[neighbor]) {
                    continue;
                }

                cameFrom[neighbor] = current;
                gScore[neighbor] = tentativeG;
                fScore[neighbor] = tentativeG + Heuristic(neighbor);
            }
        }

        if (!found) return result;

        std::vector<int> triPath;
        int current = endTri;
        triPath.push_back(current);
        while (current != startTri) {
            current = cameFrom[current];
            if (current == -1) {
                result.clear();
                return result;
            }
            triPath.push_back(current);
        }
        std::reverse(triPath.begin(), triPath.end());

        glm::vec3 s = start;
        s.y = TriHeight(g_navMesh[startTri]);
        glm::vec3 d = dest;
        d.y = TriHeight(g_navMesh[endTri]);

        result.push_back(s);

        if (triPath.size() > 2) {
            for (size_t i = 1; i + 1 < triPath.size(); ++i) {
                int triIndex = triPath[i];
                glm::vec3 c = TriCenter(g_navMesh[triIndex]);
                c.y = TriHeight(g_navMesh[triIndex]);
                result.push_back(c);
            }
        }

        result.push_back(d);
        return result;
    }

   

    std::vector<glm::vec3> PullPath(const std::vector<glm::vec3>& path) {
        if (path.size() <= 2 || g_navMesh.empty()) return path;

        // 1) Rebuild triangle sequence
        std::vector<int> triPath;
        triPath.reserve(path.size());

        for (const glm::vec3& p : path) {
            int idx = FindContainingTriIndex(p);
            if (idx == -1) {
                return path; // fallback
            }
            if (triPath.empty() || triPath.back() != idx) {
                triPath.push_back(idx);
            }
        }

        if (triPath.size() <= 1) return path;

        // 2) Build portals: [0] start, [1..n-1] shared edges, [n] end
        struct Portal {
            glm::vec2 left;
            glm::vec2 right;
        };

        glm::vec2 start2D(path.front().x, path.front().z);
        glm::vec2 end2D(path.back().x, path.back().z);

        std::vector<Portal> portals;
        portals.reserve(triPath.size() + 1);

        // Start portal (degenerate)
        portals.push_back({ start2D, start2D });

        // Shared edges
        for (size_t i = 0; i + 1 < triPath.size(); ++i) {
            glm::vec2 p0, p1;
            if (!GetSharedPortalXZ(triPath[i], triPath[i + 1], p0, p1)) {
                return path; // should not happen if neighbors are correct
            }
            portals.push_back({ p0, p1 });
        }

        // End portal (degenerate)
        portals.push_back({ end2D, end2D });

        const int nPortals = (int)portals.size();

        // 3) Funnel (string pulling) in 2D
        std::vector<glm::vec2> out2D;
        out2D.reserve(nPortals);
        out2D.push_back(start2D);

        glm::vec2 apex = start2D;
        glm::vec2 left = start2D;
        glm::vec2 right = start2D;
        int apexIndex = 0;
        int leftIndex = 0;
        int rightIndex = 0;

        for (int i = 1; i < nPortals; ++i) {
            glm::vec2 leftNew = portals[i].left;
            glm::vec2 rightNew = portals[i].right;

            // Ensure portal is oriented left/right around current apex
            if (TriArea2(apex, leftNew, rightNew) < 0.0f) {
                std::swap(leftNew, rightNew);
            }

            // Update right side
            if (TriArea2(apex, right, rightNew) <= 0.0f) {
                if (TriArea2(apex, left, rightNew) < 0.0f) {
                    // Tighten to left, emit apex at left
                    apex = left;
                    apexIndex = leftIndex;
                    out2D.push_back(apex);

                    // Reset funnel
                    left = apex;
                    right = apex;
                    leftIndex = apexIndex;
                    rightIndex = apexIndex;
                    i = apexIndex + 1;
                    continue;
                }
                else {
                    right = rightNew;
                    rightIndex = i;
                }
            }

            // Update left side
            if (TriArea2(apex, left, leftNew) >= 0.0f) {
                if (TriArea2(apex, right, leftNew) > 0.0f) {
                    // Tighten to right, emit apex at right
                    apex = right;
                    apexIndex = rightIndex;
                    out2D.push_back(apex);

                    // Reset funnel
                    left = apex;
                    right = apex;
                    leftIndex = apexIndex;
                    rightIndex = apexIndex;
                    i = apexIndex + 1;
                    continue;
                }
                else {
                    left = leftNew;
                    leftIndex = i;
                }
            }
        }

        // Add final point
        out2D.push_back(end2D);

        // 4) Lift back to 3D on navmesh surface
        std::vector<glm::vec3> result;
        result.reserve(out2D.size());
        for (const glm::vec2& p2 : out2D) {
            result.push_back(LiftToNavMesh(p2));
        }
        return result;
    }
}

