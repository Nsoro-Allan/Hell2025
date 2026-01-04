#include "Bible/Bible.h"
#include "Util.h"

namespace Bible {
    const std::string& PickRandom(const std::vector<std::string>& vector) {
        int rand = Util::RandomInt(0, vector.size() - 1);
        return vector[rand];
    }

    const std::string& RandomMermaidGreeting() {
        static const std::vector<std::string> mermaidGreetings = {
           "Lost boys welcome, but gotta pay the toll.",
           "Come spend your clams with the queen, Sugar.",
           "Drop your dabloons Honey, this aint a charity.",
           "I aint got all day, spill the loot.",
           "Careful, Sweetheart. I charge by the minute.",
           "Stop staring. Start paying.",
           "A private performance perhaps?"
        };

        return PickRandom(mermaidGreetings);
    }
}