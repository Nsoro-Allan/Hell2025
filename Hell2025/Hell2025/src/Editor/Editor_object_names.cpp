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
        else if (objectType == ObjectType::HOUSE_PLANE) {
            for (const HousePlane& housePlane : World::GetHousePlanes()) {
                if (housePlane.GetEditorName() == desiredName) {
                    return false;
                }
            }
        }
        else if (objectType == ObjectType::GENERIC_OBJECT) {
            for (const GenericObject& genericObject : World::GetGenericObjects()) {
                if (genericObject.GetEditorName() == desiredName) {
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

    std::string GetNextAvailableGenericObjectName(GenericObjectType type) {
        std::string desiredName = "Generic Object";
        switch (type) {
            case GenericObjectType::BATHROOM_BASIN:         desiredName = "Basin"; break;
            case GenericObjectType::BATHROOM_CABINET:       desiredName = "Cabinet"; break;
            case GenericObjectType::BATHROOM_TOWEL_RACK:    desiredName = "Towel Rack"; break;
            case GenericObjectType::TOILET:                 desiredName = "Toilet"; break;
            case GenericObjectType::DRAWERS_LARGE:          desiredName = "Drawers Large"; break;
            case GenericObjectType::DRAWERS_SMALL:          desiredName = "Drawers Small"; break;
            case GenericObjectType::COUCH:                  desiredName = "Couch"; break;
        }
        return GetNextEditorName(desiredName, ObjectType::GENERIC_OBJECT);
    }

    std::string GetNextAvailableHousePlaneName(HousePlaneType type) {
        std::string desiredName = UNDEFINED_STRING;
        switch (type) {
            case HousePlaneType::FLOOR:     desiredName = "Floor"; break;
            case HousePlaneType::CEILING:   desiredName = "Ceiling"; break;
            case HousePlaneType::UNDEFINED: desiredName = UNDEFINED_STRING; break;
        }
        return GetNextEditorName(desiredName, ObjectType::HOUSE_PLANE);
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

    const std::vector<std::string>& GetCeilingNames() {
        static std::vector<std::string> names;
        names.clear();
        names.reserve(World::GetHousePlanes().size());

        for (const HousePlane& housePlane : World::GetHousePlanes()) {
            if (housePlane.GetType() == HousePlaneType::CEILING) {
                names.push_back(housePlane.GetEditorName());
            }
        }
        return names;
    }

    const std::vector<std::string>& GetGenericObjectNames() {
        static std::vector<std::string> names;

        names.clear();
        names.reserve(World::GetGenericObjects().size());

        for (const GenericObject& genericObjects : World::GetGenericObjects()) {
            names.push_back(genericObjects.GetEditorName());
        }
        return names;
    }

    const std::vector<std::string>& GetFloorNames() {
        static std::vector<std::string> names;
        names.clear();
        names.reserve(World::GetHousePlanes().size());

        for (const HousePlane& housePlane : World::GetHousePlanes()) {
            if (housePlane.GetType() == HousePlaneType::FLOOR) {
                names.push_back(housePlane.GetEditorName());
            }
        }
        return names;
    }

    const std::vector<std::string>& GetTreeNames() {
        static std::vector<std::string> names;
        
        names.clear();
        names.reserve(World::GetTrees().size());

        for (const Tree& tree : World::GetTrees()) {
            names.push_back(tree.GetEditorName());
        }

        return names;
    }

    const std::vector<std::string>& GetUndefinedHousePlanes() {
        static std::vector<std::string> names;
        names.clear();
        names.reserve(World::GetHousePlanes().size());

        for (const HousePlane& housePlane : World::GetHousePlanes()) {
            if (housePlane.GetType() == HousePlaneType::UNDEFINED) {
                names.push_back(housePlane.GetEditorName());
            }
        }
        return names;
    }
}