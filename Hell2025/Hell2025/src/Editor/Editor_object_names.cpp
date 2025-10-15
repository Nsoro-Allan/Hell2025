#include "Editor.h"
#include "HellLogging.h"
#include "World/World.h"

namespace Editor {

    bool NameAvailable(const std::string& desiredName, ObjectType objectType) {
        if (objectType == ObjectType::TREE) {
            for (const Tree& tree : World::GetTrees()) {
                if (tree.GetEditorName() == desiredName) {
                    return false;
                }
            }
        }
        if (objectType == ObjectType::DRAWER) {
            for (const GenericObject& drawers : World::GetGenericObjects()) {
                if (drawers.GetEditorName() == desiredName) {
                    return false;
                }
            }
        }
        else {
            Logging::Warning() << "Editor::NameAvailable() is missing implementation for object type '" << Util::ObjectTypeToString(objectType) << "'";
        }

        return true;
    }

    std::string GetNextEditorName(const std::string& desiredName, ObjectType objectType) {
        if (NameAvailable(desiredName, objectType)) {
            return desiredName;
        }

        std::string possibleName;
        int suffix = 2;

        // Lazy brute force search. Does repeated full scans. Could be implemented in a single pass, but this is cleaner and easier to maintain for now.
        while (true) {
            possibleName = desiredName + " " + std::to_string(suffix);
            if (NameAvailable(possibleName, objectType)) {
                return possibleName;
            }
            else {
                suffix++;
            }
        }

        return UNDEFINED_STRING; // Should never reach here!
    }

    std::string GetNextAvailableTreeName(TreeType type) {
        std::string desiredName = "Tree";
        switch (type) {
            case TreeType::TREE_LARGE_0:    desiredName = "Tree Large"; break;
            case TreeType::TREE_LARGE_1:    desiredName = "Tree Large"; break;
            case TreeType::TREE_LARGE_2:    desiredName = "Tree Large"; break;
            case TreeType::BLACK_BERRIES:   desiredName = "Blackberries"; break;
        }
        return GetNextEditorName(desiredName, ObjectType::TREE);
    }

    std::vector<std::string> GetTreeNames() {
        std::vector<std::string> names;
        
        names.clear();
        names.reserve(World::GetTrees().size());

        for (const Tree& tree : World::GetTrees()) {
            names.push_back(tree.GetEditorName());
        }

        return names;
    }

    std::vector<std::string> GetDrawersNames() {
        std::vector<std::string> names;

        names.clear();
        names.reserve(World::GetGenericObjects().size());

        for (const GenericObject& drawers : World::GetGenericObjects()) {
            names.push_back(drawers.GetEditorName());
        }

        return names;
    }
}