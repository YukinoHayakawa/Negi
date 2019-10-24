#pragma once

#include <Usagi/Core/Event/Event.hpp>

namespace usagi::negi
{
class Character;

struct CharacterMessageEvent : Event
{
    Character *character = nullptr;
    bool continued = false;

    CharacterMessageEvent(Character *character, bool continued = false)
        : character(character)
        , continued(continued)
    {
    }
};
}
