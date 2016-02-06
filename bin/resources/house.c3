{
	/* 0;Terrain vague;0;;;;;;;;;;;;;;; */
	terrain_vacant : 
		{
			level : 0,
      title : "##freehouse_caption##",
      habitants : 5,
			minDesirability : 0,
		  maxDesirability : 0,
			entertainment : 0,
			water : 0,
			religion : 0,
      education : 0,
      health : 0,
      food : 0,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 0,
      prosperity : 0,
      tax : 0
      txs : {
        s1: [ "housng1a_00045" ]
      }
    },

  /* 1;Petite tente;5;-99;-10;0;0;0;0;0;0;0;0;0;0;3;5;1 */
  hovel :
    {
      level : 1,
      title : "##small_hovel##",
      habitants : 5,
      minDesirability : -99,
      maxDesirability : -10,
      desirability : -3,
      desstep : 1
      desrange : 3,
      entertainment : 0,
      water : 0,
      religion : 0,
      education : 0,
      health : 0,
      food : 0,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 5,
      tax : 1
      txs : {
        s1: [ "housng1a_00001", "housng1a_00002" ]
        s2: [ "housng1a_00005" ]
      }
    },

  /*2;Grande tente;7;-12;-5;0;1;0;0;0;0;0;0;0;0;3;10;1*/
  tent :
    {
      level : 2,
      title : "##small_tent##",
      habitants : 7,
      minDesirability : -12,
      maxDesirability : -5,
      entertainment : 0,
      desirability : -3,
      desstep : 1
      desrange : 3,
      water : 1,
      religion : 0,
      education : 0,
      health : 0,
      food : 0,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 10,
      tax : 1
      txs : {
        s1: [ "housng1a_00003", "housng1a_00004" ]
        s2: [ "housng1a_00006" ]
      }
    },

  /* 3;Petite hutte;9;-7;0;0;1;0;0;0;1;0;0;0;0;3;15;1 */
  shack :
    {
      level : 3,
      title : "##small_shack##",
      habitants : 9,
      minDesirability : -9,
      maxDesirability : 0,
      entertainment : 0,
      desirability : -2,
      desstep : 1
      desrange : 2,
      water : 1,
      religion : 0,
      education : 0,
      health : 0,
      food : 1,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 15,
      tax : 1
      txs : {
        s1: [ "housng1a_00007", "housng1a_00008" ]
        s2: [ "housng1a_00011" ]
      }
    },

  /*4;Grande hutte;11;-2;4;0;1;1;0;0;1;0;0;0;0;3;20;1*/
  hut :
    {
      level : 4,
      title : "##small_hut##",
      habitants : 11,
      minDesirability : -2,
      maxDesirability : 4,
      entertainment : 0,
      desirability : -2,
      desstep : 1
      desrange : 2,
      water : 1,
      religion : 1,
      education : 0,
      health : 0,
      food : 1,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 20,
      tax : 1
      txs : {
        s1: [ "housng1a_00009", "housng1a_00010" ]
        s2: [ "housng1a_00012" ]
      }
    },

  /* 5;Petit taudis;13;2;8;0;2;1;0;0;1;0;0;0;0;3;25;2	*/
  small_domus :
    {
      level : 5,
      title : "##small_domus##",
      habitants : 13,
      minDesirability : 2,
      maxDesirability : 8,
      entertainment : 0,
      desirability : -2,
      desstep : 1
      desrange : 2,
      water : 2
      religion : 1,
      education : 0,
      health : 0,
      food : 1,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 25,
      tax : 2
      txs : {
        s1: [ "housng1a_00013", "housng1a_00014" ]
        s2: [ "housng1a_00017" ]
      }
    },
  /* 6;Grand taudis;15;6;12;10;2;1;0;0;1;0;0;0;0;3;30;2 */
  big_domus :
    {
      level : 6,
      title : "##big_domus##",
      habitants : 15,
      minDesirability : 6,
      maxDesirability : 12,
      entertainment : 10,
      desirability : -2,
      desstep : 1
      desrange : 2,
      water : 2
      religion : 1,
      education : 0,
      health : 0,
      food : 1,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 3,
      prosperity : 30,
      tax : 2
      txs : {
       s1: [ "housng1a_00015", "housng1a_00016" ]
       s2: [ "housng1a_00018" ]
      }
    },

  /* 7;Petite maison;17;10;16;10;2;1;1;0;1;0;0;0;0;2;35;2 */
  small_mansion :
    {
      level : 7,
      title : "##small_mansion##",
      habitants : 17,
      minDesirability : 10,
      maxDesirability : 16,
      entertainment : 10,
      desirability : -1,
      desstep : 1
      desrange : 1,
      water : 2,
      religion : 1,
      education :1,
      health : 0,
      food : 1,
      pottery : 0,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 2,
      prosperity : 35,
      tax : 2
      txs : {
        s1: [ "housng1a_00019", "housng1a_00020" ]
        s2: [ "housng1a_00023" ]
      }
    },

  /* 8;Grande maison;19;14;20;10;2;1;1;1;1;1;0;0;0;2;45;2 */
  big_mansion :
    {
      level : 8,
      title : "##big_mansion##",
      habitants : 19,
      minDesirability : 14,
      maxDesirability : 20,
      entertainment : 10,
      desirability : -1,
      desstep : 1
      desrange : 1,
      water : 2,
      religion : 1,
      education :1,
      health : 1,
      food : 1,
      pottery : 1,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 2,
      prosperity : 45,
      tax : 2
      txs : {
        s1: [ "housng1a_00021", "housng1a_00022" ]
        s2: [ "housng1a_00024" ]
      }
    },

  /*9;Petite insula;19;18;25;25;2;1;1;1;1;1;0;0;0;2;50;3*/
  small_insula :
    {
      level : 9,
      title : "##small_insula##",
      habitants : 19,
      minDesirability : 18,
      maxDesirability : 25,
      entertainment : 25,
      desirability : 0,
      desstep : 0
      desrange : 0,
      water : 2,
      religion : 1,
      education :1,
      health : 1,
      food : 1,
      pottery : 1,
      oil : 0,
      furniture : 0,
      wine : 0,
      crime : 2,
      prosperity : 50,
      tax : 3
      txs : {
        s1: [ "housng1a_00025", "housng1a_00026" ]
        s2: [ "housng1a_00029" ]
      }
    },

  /*10;Insula moyenne;20;22;32;25;2;1;1;2;1;1;0;1;0;2;58;3*/
  middle_insula :
    {
      level : 10,
      title : "##middle_insula##",
      habitants : 19,
      minDesirability : 22,
      maxDesirability : 32,
      entertainment : 25,
      desirability : 0,
      desstep : 0
      desrange : 0
      water : 2,
      religion : 1,
      education :1,
      health : 2,
      food : 1,
      pottery : 1,
      oil : 0,
      furniture : 1,
      wine : 0,
      crime : 2,
      prosperity : 58,
      tax : 3
      txs : {
        s1: [ "housng1a_00027", "housng1a_00028" ]
        s2: [ "housng1a_00030" ]
      }
    },

  /*11;Grande insula;21;29;40;25;2;1;2;3;1;1;1;1;0;2;65;3*/
  big_insula :
    {
      level : 11,
      title : "##big_insula##",
      habitants : 21,
      minDesirability : 29,
                        maxDesirability : 40,
      entertainment : 25,
      desirability : 1,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 1,
      education :2,
      health : 3,
      food : 1,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 0,
      crime : 2,
      prosperity : 65,
      tax : 3
      txs : {
        s2 : [ "housng1a_00031", "housng1a_00032" ]
      }
    },

  /* 12;Insula imposante;21;37;48;35;2;1;2;3;2;1;1;1;0;2;80;4 */
   beatyfull_insula :
    {
      level : 12,
      title : "##beatyfull_insula##",
      habitants : 21,
      minDesirability : 37,
                        maxDesirability : 48,
      entertainment : 35,
      desirability : 1,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 1,
      education : 2,
      health : 3,
      food : 2,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 0,
      crime : 2,
      prosperity : 80,
      tax : 4
      txs : {
        s2 : [ "housng1a_00033", "housng1a_00034" ]
      }
    },

  /*13;Petite villa;10;45;53;35;2;2;2;3;2;1;1;1;1;1;150;9*/
  small_villa :
    {
      level : 13,
      title : "##small_villa##",
      habitants : 10,
      minDesirability : 45,
      maxDesirability : 53,
      entertainment : 35,
      desirability : 2,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 2,
      education : 2,
      health : 3,
      food : 2,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 1,
      crime : 1,
      prosperity : 150,
      tax : 9
      txs : {
        s2 : [ "housng1a_00035", "housng1a_00036" ]
      }
    },

  /* 14;Villa moyenne;10;50;58;40;2;2;2;4;2;1;1;1;1;1;180;10 */
  middle_villa :
    {
      level : 14,
      title : "##middle_villa##",
      habitants : 10,
      minDesirability : 50,
      maxDesirability : 58,
      entertainment : 40,
      desirability : 2,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 2,
      education : 2,
      health : 4,
      food : 2,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 1,
      crime : 1,
      prosperity : 180,
      tax : 10
      txs : {
        s2: [ "housng1a_00037", "housng1a_00038" ]
      }
    },

  /*15;Grande villa;10;55;63;45;2;2;3;4;2;1;1;1;1;1;400;11*/
  big_villa :
    {
      level : 15,
      title : "##big_villa##",
      habitants : 10,
      minDesirability : 55,
      maxDesirability : 63,
      desirability : 2,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 2,
      education : 2,
      health : 4,
      food : 2,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 1,
      crime : 1,
      prosperity : 400,
      tax : 11
      txs : {
        s3 : [ "housng1a_00039" ]
      }
    },
  /*16;Villa imposante;11;60;68;50;2;3;3;4;3;1;1;1;1;1;600;11*/
  beatifull_villa :
    {
      level : 16,
      title : "##beatifull_villa##",
      habitants : 11,
      minDesirability : 60,
        maxDesirability : 68,
      entertainment : 50,
      desirability : 2,
      desstep : -1
      desrange : 2,
      water : 2,
      religion : 3,
      education : 3,
      health : 4,
      food : 3,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 1,
      crime : 1,
      prosperity : 600,
      tax : 11
      txs : {
        s3 : [ "housng1a_00040" ]
      }
    },

  /*17;Petit palais;12;65;74;55;2;3;3;4;3;1;1;1;2;1;700;12*/
  "small_palace" :
    {
      level : 17,
      title : "##small_palace##",
      habitants : 12,
      minDesirability : 65,
        maxDesirability : 74,
      entertainment : 55,
      desirability : 3,
      desstep : -1
      desrange : 3,
      water : 2,
      religion : 3,
      education : 3,
      health : 4,
      food : 3,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 1,
      crime : 1,
      prosperity : 700,
      tax : 12
      txs : {
        s3: [ "housng1a_00041" ]
      }
    },

  /*18;Palais moyen;12;70;80;60;2;4;3;4;3;1;1;1;2;1;900;12*/
  "middle_palace" :
    {
      level : 18,
      title : "##middle_palace##",
      habitants : 12,
      minDesirability : 70,
        maxDesirability : 80,
      entertainment : 60,
      desirability : 3,
      desstep : -1
      desrange : 3,
      water : 2,
      religion : 4,
      education : 3,
      health : 4,
      food : 3,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 2,
      crime : 1,
      prosperity : 900,
      tax : 12
      txs : {
        s3 : [ "housng1a_00042" ]
      }
    },

  /*19;Grand palais;12;76;90;70;2;4;3;4;3;1;1;1;2;1;1500;15*/
  "big_palace" :
    {
      level : 19,
      title : "##big_palace##",
      habitants : 12,
      minDesirability : 76,
        maxDesirability : 90,
      entertainment : 70,
      desirability : 4
      desstep : -1
      desrange : 4,
      water : 2,
      religion : 4,
      education : 3,
      health : 4,
      food : 3,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 2,
      crime : 1,
      prosperity : 1500,
      tax : 15
      txs : {
        s4 : [ "housng1a_00043" ]
      }
    },

  /*20;Palais luxueux;12;85;100;80;2;4;3;4;3;1;1;1;2;1;1750;16*/
  "luxury_palace" :
    {
      level : 20,
      title : "##luxury_palace##",
      habitants : 12,
      minDesirability : 85,
      maxDesirability : 100,
      entertainment : 80,
      desirability : 4,
      desstep : -1
      desrange : 4,
      water : 2,
      religion : 4,
      education : 3,
      health : 4,
      food : 3,
      pottery : 1,
      oil : 1,
      furniture : 1,
      wine : 2,
      crime : 1,
      prosperity : 1750,
      tax : 16
      txs : {
        s4 : [ "housng1a_00044" ]
      }
    },

}
