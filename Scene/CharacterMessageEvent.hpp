#pragma once

#include <Usagi/Core/Event/Event.hpp>

namespace usagi::moeloop
{
class Character;

struct CharacterMessageEvent : Event
{
    Character *character = nullptr;

    explicit CharacterMessageEvent(Character *character)
        : character(character)
    {
    }
};
}
