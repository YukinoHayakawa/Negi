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
    using namespace asset_path_prefix;
    auto *track = comp<BasicAudioStreamComponent>();
    // todo: fix race condition with streaming callback
    track->buffer = mScene->game()->assets()
        ->res<PassthroughAudioAssetConverter>(
            AUDIO + std::string(asset_path));
    track->cursor.reset(track->buffer->frames);
    track->loop = loop;
    // set playing at last to prevent reading nullptr
    track->playing = true;
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
