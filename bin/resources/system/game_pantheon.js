game.religion.gods_config = [
  { id: g_config.gods.ceres,   name : "Ceres",   service : "srvc_religionCeres",   image:  "panelwindows_00017" },
  { id: g_config.gods.mars,    name : "Mars",    service : "srvc_religionMars",    image : "panelwindows_00020" },
  { id: g_config.gods.neptune, name : "Neptune", service : "srvc_religionNeptune", image : "panelwindows_00018" },
  { id: g_config.gods.venus,   name : "Venus",	 service : "srvc_religionVenus",   image : "panelwindows_00021"	},
	{ id: g_config.gods.mercury, name : "Mercury", service : "srvc_religionMercury", image : "panelwindows_00019" }
]

function OnInitRomePantheon() {
  game.religion.pantheon.init()
}

game.religion.pantheon.init = function()
{
  for (var i in game.religion.gods_config)
  {
    var config = game.religion.gods_config[i]
    var god = g_session.addGod(config.id);
    var pic = g_render.picture(config.image)
                      .fallback("dlcperf",1)
    god.setName( config.name )
    god.setService( config.service )
    god.setPicture( pic )
    god.setRelation( 50 )
  }
}
