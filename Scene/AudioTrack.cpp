#include "AudioTrack.hpp"

#include <Usagi/Extensions/AssetAudioFormats/PassthroughAudioAssetConverter.hpp>
#include "Scene.hpp"
#include <Negi/NegiGame.hpp>
#include <Usagi/Asset/AssetRoot.hpp>

namespace usagi::negi
{
AudioTrack::AudioTrack(Element *parent, std::string name, Scene *scene)
    : PredefinedElement<BasicAudioStreamComponent>(parent, std::move(name))
    , mScene(scene)
{
}

void AudioTrack::play(std::string_view asset_path, bool loop)
{
    auto *track = comp<BasicAudioStreamComponent>();
    track->playing = true;
    track->buffer = mScene->game()->assets()
        ->res<PassthroughAudioAssetConverter>(std::string(asset_path));
    track->cursor.reset(track->buffer->frames);
    track->loop = loop;
}

void AudioTrack::stop()
{
    auto *track = comp<BasicAudioStreamComponent>();
    track->playing = false;
}

void AudioTrack::exportScript(sol::table ns)
{
    ns.new_usertype<AudioTrack>(
        "AudioTrack",
        "play", &AudioTrack::play,
        "stop", &AudioTrack::stop
    );
}
}
