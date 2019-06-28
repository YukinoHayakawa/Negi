#include "NegiGameState.hpp"

#include <Negi/NegiGame.hpp>

namespace usagi::negi
{
NegiGameState::NegiGameState(
    Element *parent,
    std::string name,
    NegiGame *game)
    : GraphicalGameState(parent, std::move(name), game)
{
}

NegiGame * NegiGameState::game() const
{
    return static_cast<NegiGame*>(mGame);
}
}
