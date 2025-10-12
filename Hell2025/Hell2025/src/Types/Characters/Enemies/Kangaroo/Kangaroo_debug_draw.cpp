#include "Kangaroo.h"
#include "Renderer/Renderer.h"

void Kangaroo::DebugDraw() {
    // Forward vector
    Renderer::DrawLine(m_position, m_position + m_forward, WHITE);
    Renderer::DrawPoint(m_position, RED);
    Renderer::DrawPoint(m_position + m_forward, RED);

    //std::cout << "Kangaroo forward: " << m_forward << "\n";
    //std::cout << "m_position: " << m_position << "\n";
    //std::cout << "m_position + m_forward: " << m_position + m_forward << "\n";
}