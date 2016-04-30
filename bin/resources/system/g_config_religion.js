g_config.gods = {
  ceres : "ceres",
  mars : "mars",
  neptune : "neptune",
  venus : "venus",
  mercury : "mercury"
}

g_config.festival = {
  small :  { id:1, limiter:20, minCost : 10, title:"small_festival" },
  middle : { id:2, limiter:10, minCost : 20, title:"middle_festival"},
  big :    { id:3, limiter:5,  minCost : 50,  title:"big_festival"},
  great :  { id:4, limiter:3,  minCost : 100, title:"great_festival", wineLimiter:250}
}

g_config.religion.gods_config = [
  { id: g_config.gods.ceres,   name : "Ceres",   service : "srvc_religionCeres",   image:  "panelwindows_00017", smallt : "small_ceres_temple", bigt : "big_ceres_temple" },
  { id: g_config.gods.mars,    name : "Mars",    service : "srvc_religionMars",    image : "panelwindows_00020", smallt : "small_mars_temple", bigt : "big_mars_temple" },
  { id: g_config.gods.neptune, name : "Neptune", service : "srvc_religionNeptune", image : "panelwindows_00018", smallt : "small_neptune_temple", bigt : "big_neptune_temple" },
  { id: g_config.gods.venus,   name : "Venus",	 service : "srvc_religionVenus",   image : "panelwindows_00021", smallt : "small_venus_temple", bigt : "big_venus_temple"	},
  { id: g_config.gods.mercury, name : "Mercury", service : "srvc_religionMercury", image : "panelwindows_00019", smallt : "small_mercury_temple", bigt : "big_mercury_temple" }
]
