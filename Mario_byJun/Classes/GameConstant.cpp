#include "GameConstant.h"
#include "UsuallyFunctions.h"
#include "AudioEngine.h"

using cocos2d::experimental::AudioEngine;

const char * g_szEffects[14] = {
	"sound/effect/add_life.ogg",
	"sound/effect/destroy_tile.ogg",
	"sound/effect/die.ogg",
	"sound/effect/eat_swamm.ogg",
	"sound/effect/get_coin.ogg",
	"sound/effect/jack_solid.ogg",
	"sound/effect/jump.ogg",
	"sound/effect/kill_monster.ogg",
	"sound/effect/reduce_life.ogg",
	"sound/effect/swamm_show.ogg",
	"sound/effect/time_lack.ogg",
	"sound/effect/tumble.ogg",
	"sound/effect/drop_flag.ogg",
	"sound/effect/win.ogg",
};

int playEffect(enEffectId enId, bool bLoop/* = false*/)
{
	CCASSERT(enId >= 0 && enId < CountArray(g_szEffects), "effect id error");
	return AudioEngine::play2d(g_szEffects[enId], bLoop);
}
