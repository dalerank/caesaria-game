g_config.ranks = [
  {  name: "citizen",   pretty : "##citizen##",  salary : 1 },
  {  name: "clerk"  ,   pretty : "##clerk##",    salary : 2 },
  {  name: "engineer",  pretty : "##engineer##", salary : 5 },
  {  name: "architect", pretty : "##architect##",salary : 8 },
  {  name: "questor",   pretty : "##questor##",  salary : 12},
  {  name: "procurate", pretty : "##procurate##",salary : 20},
  {  name: "aedil",     pretty : "##aedil##",    salary : 30},
  {  name: "preator",   pretty : "##preator##",  salary : 40},
  {  name: "consul",    pretty : "##consul##",   salary : 60},
  {  name: "proconsul", pretty : "##proconsul##",salary : 80},
  {  name: "caesar" ,   pretty : "##caesar##",   salary : 100}
]

for (var i in g_config.ranks)
{
  var r = g_config.ranks[i]
  g_session.setRank(i, r.name, r.pretty, r.salary)
}
