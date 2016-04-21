g_config.good = {
  none : 0,
  wheat : 1,
  fish : 2,
  meat : 3,
  fruit : 4,
  vegetable : 5,
  olive : 6,
  oil: 7,
  grape : 8,
  wine : 9,
  timber : 10,
  furniture : 11,
  clay :    12,
  pottery :    13,
  iron :       14,
  weapon :     15,
  marble :     16,
  denaries :   17,
  prettyWine : 18,
};

g_config.order = {
  accept : 0,
  reject : 1,
  deliver : 2,
  none : 3
}

g_config.good.basicfood = [ g_config.good.wheat, g_config.good.fish, g_config.good.meat, g_config.good.fruit, g_config.good.vegetable ];
g_config.good.storable = [ g_config.good.wheat, g_config.good.fish, g_config.good.meat, g_config.good.fruit, g_config.good.vegetable,
                           g_config.good.olive, g_config.good.oil, g_config.good.grape, g_config.good.wine, g_config.good.timber,
                           g_config.good.furniture, g_config.good.clay, g_config.good.pottery, g_config.good.iron,
                           g_config.good.weapon, g_config.good.marble, g_config.good.prettyWine ];
g_config.good.grararable = [ g_config.good.wheat, g_config.good.meat, g_config.good.fruit, g_config.good.vegetable ];                           

g_config.good.cache = {}

g_config.good.getInfo = function(type)
{
  if (g_config.good.cache.hasOwnProperty(type))
    return g_config.good.cache[type];

  var info = g_config.good.initInfo(type);
  g_config.good.cache[type] = info;
  g_config.good.cache[info.name] = info;

  return info;
}

g_config.good.initInfo = function(type)
{
  var info = {};
  switch(type) {
    case g_config.good.none: info = { emp:0, local:0, name:"none" }; break;
    case g_config.good.wheat: info = { emp:11, local:317, name:"wheat" }; break;
    case g_config.good.vegetable: info = { emp:12, local:318, name:"vegetable" }; break;
    case g_config.good.fruit: info = { emp:13, local:319, name:"fruit" };break;
    case g_config.good.olive: info = { emp:14, local:320, name:"olive" };break;
    case g_config.good.grape: info = { emp:15, local:321, name:"grape" };break;
    case g_config.good.meat: info = { emp:16, local:322, name:"meat" };break;
    case g_config.good.wine: info = { emp:17, local:323, name:"wine" };break;
    case g_config.good.oil: info = { emp:18, local:324, name:"oil" };break;
    case g_config.good.iron: info = { emp:19, local:325, name:"iron" };break;
    case g_config.good.timber: info = { emp:20, local:326, name:"timber" };break;
    case g_config.good.clay: info = { emp:21, local:327, name:"clay" };break;
    case g_config.good.marble: info = { emp:22, local:328, name:"marble" };break;
    case g_config.good.weapon: info = { emp:23, local:329, name:"weapon" };break;
    case g_config.good.furniture: info = { emp:24, local:330, name:"furniture" };break;
    case g_config.good.pottery: info = { emp:25, local:331, name:"pottery" };break;
    case g_config.good.denaries: info = { emp:26, local:332, name:"denaries" };break;
    case g_config.good.prettyWine: info = { emp:17, local:323, name:"prettyWine" };break;
    case g_config.good.fish: info = { emp: 27, local:333, name:"fish" };break;
  }

  info.picture = {}
  info.type = type;
  info.picture.emp = g_render.picture(g_config.rc.panel, info.emp)
                             .fallback(g_config.rc.panel, g_config.rc.invalidIndex);

  info.picture.local = g_render.picture(g_config.rc.panel, info.local)
                               .fallback(g_config.rc.panel, g_config.rc.invalidIndex);

  return info;
}

Store.prototype.qty = function(gtype) {
    if (typeof gtype == "undefined") {
      return this.qty_void();
    } else {
      return this.qty_product(gtype);
    }
}

Store.prototype.free = function(gtype) {
    if (typeof gtype == "undefined") {
      return this.freeQty_void();
    } else {
      return this.freeQty_product(gtype);
    }
}

Store.prototype.capacity = function(gtype) {
    if (typeof gtype == "undefined") {
      return this.capacity_void();
    } else {
      return this.capacity_product(gtype);
    }
}
