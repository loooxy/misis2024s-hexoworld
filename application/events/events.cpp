#include "events.hpp">
#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/polymorphic.hpp>

CEREAL_REGISTER_TYPE(ChangeHeight)
CEREAL_REGISTER_TYPE(ChangeColor)
CEREAL_REGISTER_TYPE(ChangeRoadState)
CEREAL_REGISTER_TYPE(ChangeFarmState)
CEREAL_REGISTER_TYPE(ChangeFloodState)


CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeHeight)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeColor)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeRoadState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeFarmState)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Event, ChangeFloodState)