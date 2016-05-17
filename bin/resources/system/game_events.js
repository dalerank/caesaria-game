function OnScribesStatusChanged() { game.eventmgr.dispatchEvent(game.events.OnScribesStatusChanged, {})}
function OnGameDayChanged() { game.eventmgr.dispatchEvent(game.events.OnDayChanged,{}); }
function OnLevelDestroyed() { game.eventmgr.dispatchEvent(game.events.OnLevelDestroyed, {}); }

function OnCityLoaded() { g_city = new PlayerCity(); }
function OnCityUnloaded() { g_city = null; }
