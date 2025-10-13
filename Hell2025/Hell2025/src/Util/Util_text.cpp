#include "Util.h"

namespace Util {

    const char* ShortcutToString(Shortcut shortcut) {
        switch (shortcut) {
            case Shortcut::F1:  return "F1";
            case Shortcut::F2:  return "F2";
            case Shortcut::F3:  return "F3";
            case Shortcut::F4:  return "F4";
            case Shortcut::F5:  return "F5";
            case Shortcut::F6:  return "F6";
            case Shortcut::F7:  return "F7";
            case Shortcut::F8:  return "F8";
            case Shortcut::F9:  return "F9";
            case Shortcut::F10: return "F10";
            case Shortcut::F11: return "F11";
            case Shortcut::F12: return "F12";

            case Shortcut::CTRL_A: return "Ctrl+A";
            case Shortcut::CTRL_B: return "Ctrl+B";
            case Shortcut::CTRL_C: return "Ctrl+C";
            case Shortcut::CTRL_D: return "Ctrl+D";
            case Shortcut::CTRL_E: return "Ctrl+E";
            case Shortcut::CTRL_F: return "Ctrl+F";
            case Shortcut::CTRL_G: return "Ctrl+G";
            case Shortcut::CTRL_H: return "Ctrl+H";
            case Shortcut::CTRL_I: return "Ctrl+I";
            case Shortcut::CTRL_J: return "Ctrl+J";
            case Shortcut::CTRL_K: return "Ctrl+K";
            case Shortcut::CTRL_L: return "Ctrl+L";
            case Shortcut::CTRL_M: return "Ctrl+M";
            case Shortcut::CTRL_N: return "Ctrl+N";
            case Shortcut::CTRL_O: return "Ctrl+O";
            case Shortcut::CTRL_P: return "Ctrl+P";
            case Shortcut::CTRL_Q: return "Ctrl+Q";
            case Shortcut::CTRL_R: return "Ctrl+R";
            case Shortcut::CTRL_S: return "Ctrl+S";
            case Shortcut::CTRL_T: return "Ctrl+T";
            case Shortcut::CTRL_U: return "Ctrl+U";
            case Shortcut::CTRL_V: return "Ctrl+V";
            case Shortcut::CTRL_W: return "Ctrl+W";
            case Shortcut::CTRL_X: return "Ctrl+X";
            case Shortcut::CTRL_Y: return "Ctrl+Y";
            case Shortcut::CTRL_Z: return "Ctrl+Z";

            case Shortcut::ESC:  return "Esc";
            case Shortcut::NONE: return "";
            default:             return "UNDEFINED";
        }
    }

    bool StrCmp(const char* queryA, const char* queryB) {
        if (strcmp(queryA, queryB) == 0)
            return true;
        else
            return false;
    }

    std::string Lowercase(std::string& str) {
        std::string result;
        for (auto& c : str) {
            result += std::tolower(c);
        }
        return result;
    }

    std::string Uppercase(std::string& str) {
        std::string result;
        for (auto& c : str) {
            result += std::toupper(c);
        }
        return result;
    }

    std::string BoolToString(bool b) {
        return b ? "TRUE" : "FALSE";
    }

    std::string Vec2ToString(glm::vec2 v) {
        return std::format("({:.2f}, {:.2f})", v.x, v.y);
    }

    std::string Vec3ToString(glm::vec3 v) {
        return std::format("({:.2f}, {:.2f}, {:.2f})", v.x, v.y, v.z);
    }

    glm::vec3 Vec3Min(const glm::vec3& a, const glm::vec3& b) {
        return glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
    }

    glm::vec3 Vec3Max(const glm::vec3& a, const glm::vec3& b) {
        return glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
    }

    std::string Mat4ToString(glm::mat4 m) {
        return std::format("{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}\n{:.2f} {:.2f} {:.2f} {:.2f}",
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]);
    }

    const char* CopyConstChar(const char* text) {
        char* b = new char[strlen(text) + 1] {};
        std::copy(text, text + strlen(text), b);
        return b;
    }
}