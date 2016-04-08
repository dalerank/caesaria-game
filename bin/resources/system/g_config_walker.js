g_config.walker.flag = {
  infiniteWait:-1,
  showDebugInfo:1,
  vividly:2,
  showPath:3,
  userFlag:0x80,
  count:0xff
}

g_config.walker.places = {
  plOrigin:0,
  plDestination:1,
  pcCount:2
}

g_config.walker.getBigPicture = function(typename) {
  var index = -1;
  switch(typename)
  {
  case "immigrant": index=4; break;
  case "emigrant": index=9; break;
  case "doctor": index = 2; break;
  case "cartPusher": index=51; break;
  case "marketLady": index=12; break;
  case "marketKid": index=38; break;
  case "merchant": index=25; break;
  case "prefect": index=19; break;
  case "engineer": index=7; break;
  case "taxCollector": index=6; break;
  case "sheep": index = 54; break;
  case "seaMerchant": index = 61; break;
  case "merchantCamel" : index = 25; break;
  case "recruter": index=13; break;
  case "lionTamer": index=11; break;
  default: index=8; break;
  break;
  }

  return g_render.picture("bigpeople", index)
                 .fallback("bigpeople", 1 );
}
