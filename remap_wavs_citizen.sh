#!/bin/bash

usage()
{
echo "wavs_citizen install script
Usage: remap_wavs_citizen.sh --wav-dir path1
Options:
   --wav-dir path : set the path to the original wav files (eg: ~/SIERRA/caesar3/wavs)
                    the files will be renamed in lowercase.
   --data-dir path: set the path to the caesaria installation (eg: ~/caesaria-test)
                    it contains the Exe directory.
   -h, --help     : display this help
"
}


WAV_DIR="."
DATA_DIR="."
TMP_DIR="./tmp"

while true; do
   case "$1" in
   --wav-dir ) WAV_DIR="$2"; shift 2;;
   --data-dir ) DATA_DIR="$2"; shift 2;;
   -h | --help ) usage; exit 0;;
   -- ) shift; break;;
   * ) break;;
   esac
done

# make the tmp directories
mkdir -v -p $TMP_DIR

# rename all wav files to lower case
# had to use pushd : do not translate $WAV_DIR into lowercase

pushd .

cd $WAV_DIR

for f in * ; do mv -- "$f" "$(tr [:upper:] [:lower:] <<< "$f")" ; done

popd

# Copy files of interest into $TMP_DIR
cp $WAV_DIR/*_great1.wav $TMP_DIR
cp $WAV_DIR/*_great2.wav $TMP_DIR
cp $WAV_DIR/*_nofun1.wav $TMP_DIR
cp $WAV_DIR/*_nojob1.wav $TMP_DIR
cp $WAV_DIR/*_needjob1.wav $TMP_DIR
cp $WAV_DIR/*_relig1.wav $TMP_DIR
cp $WAV_DIR/*_starv1.wav $TMP_DIR
cp $WAV_DIR/*_exact*.wav $TMP_DIR

# Rename...
# SUGGESTION: _average_life -> _good_life
#    and _good_life    -> _great_life 

rename 's/_great1/_average_life/' $TMP_DIR/*.wav
rename 's/_great2/_good_life/' $TMP_DIR/*.wav
rename 's/_nofun1/_low_entairtenment/' $TMP_DIR/*.wav
rename 's/_nojob1/_high_workless/' $TMP_DIR/*.wav
rename 's/_needjob1/_need_workers/' $TMP_DIR/*.wav
rename 's/_relig1/_gods_angry/' $TMP_DIR/*.wav
rename 's/_starv1/_so_hungry/' $TMP_DIR/*.wav

# These are talks for _normal_life and some situations
# Need individual file name
rename 's/_exact/_say_/' $TMP_DIR/*.wav

mv $TMP_DIR/prefect_say_1.wav $TMP_DIR/prefect_find_goods.wav
mv $TMP_DIR/prefect_say_2.wav $TMP_DIR/prefect_return.wav
mv $TMP_DIR/prefect_say_3.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_4.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_5.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_6.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_7.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_8.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_9.wav $TMP_DIR/prefect_out_goods.wav
mv $TMP_DIR/prefect_say_10.wav $TMP_DIR/prefect_out_goods.wav

rename 's/actors_/actor_/' $TMP_DIR/*.wav

rename 's/crtpsh_/cartPusher_/' $TMP_DIR/*.wav
# Normal life action : assess working
mv $TMP_DIR/cartPusher_say_1.wav $TMP_DIR/cartPusher_no_personnel_here.wav
mv $TMP_DIR/cartPusher_say_2.wav $TMP_DIR/cartPusher_cantfind_destination.wav
mv $TMP_DIR/cartPusher_say_3.wav $TMP_DIR/cartPusher_route_too_long.wav

rename 's/charot_/charioteer_/' $TMP_DIR/*.wav
rename 's/emigrate_/emigrant_/' $TMP_DIR/*.wav
# leave the city, ranting.
mv $TMP_DIR/emigrant_say_1.wav $TMP_DIR/emigrant_high_workless.wav
mv $TMP_DIR/emigrant_say_2.wav $TMP_DIR/emigrant_so_hungry.wav
mv $TMP_DIR/emigrant_say_3.wav $TMP_DIR/emigrant_high_tax.wav
mv $TMP_DIR/emigrant_say_4.wav $TMP_DIR/emigrant_low_salary.wav

rename 's/engine_/engineer_/' $TMP_DIR/*.wav
# Normal life action : assess building
mv $TMP_DIR/engineer_say_1.wav $TMP_DIR/engineer_have_trouble_buildings.wav
mv $TMP_DIR/engineer_say_2.wav $TMP_DIR/engineer_building_allok.wav

rename 's/gladtr_/gladiator_/' $TMP_DIR/*.wav
mv $TMP_DIR/gladiator_say_1.wav $TMP_DIR/gladiator_find_ennemy.wav

# TODO: check if should be emigrant (leaving city)
# immigrant comes to the city
mv $TMP_DIR/immigrant_say_1.wav $TMP_DIR/immigrant_where_my_home.wav
mv $TMP_DIR/immigrant_say_2.wav $TMP_DIR/immigrant_want_to_be_liontamer.wav
mv $TMP_DIR/immigrant_say_3.wav $TMP_DIR/immigrant_much_food_here.wav

# TODO: check homeless -> immigrant
mv $TMP_DIR/homeless_say_1.wav $TMP_DIR/immigrant_thrown_from_house.wav
mv $TMP_DIR/homeless_say_2.wav $TMP_DIR/immigrant_no_home.wav
#duplicate files
rm $TMP_DIR/homles_say_1.wav 
rm $TMP_DIR/homles_say_2.wav 
rm $TMP_DIR/mission_say_4.wav 

rename 's/liontr_/lionTamer_/' $TMP_DIR/*.wav
mv $TMP_DIR/lionTamer_say_1.wav $TMP_DIR/lionTamer_good_education.wav
mv $TMP_DIR/lionTamer_say_2.wav $TMP_DIR/lionTamer_normal_life.wav
mv $TMP_DIR/lionTamer_say_3.wav $TMP_DIR/lionTamer_find_ennemy.wav

rename 's/granboy_/market_kid_/' $TMP_DIR/*.wav
# random market_kid_say_1 .. 3

# SUGGESTION: marketTrader instead of marketBuyer
rename 's/market_/marketBuyer_/' $TMP_DIR/*.wav
# marketLady is the marketBuyer
# Normal life action
mv $TMP_DIR/marketBuyer_say_1.wav $TMP_DIR/marketLady_find_goods.wav
mv $TMP_DIR/marketBuyer_say_2.wav $TMP_DIR/marketLady_return.wav
mv $TMP_DIR/marketBuyer_say_3.wav $TMP_DIR/marketLady_out_goods.wav

rename 's/donkey_/merchant_/' $TMP_DIR/*.wav
# waiting at warehouse
# merchant_goto_warehouse -> sell
# merchant_return_from_warehouse -> buy
mv $TMP_DIR/merchant_say_4.wav $TMP_DIR/merchant_goto_warehouse.wav
mv $TMP_DIR/merchant_say_5.wav $TMP_DIR/merchant_return_from_warehouse.wav
# moving 
mv $TMP_DIR/merchant_say_1.wav $TMP_DIR/merchant_notany_trade.wav
mv $TMP_DIR/merchant_say_2.wav $TMP_DIR/merchant_greeting.wav
# coming to the city
mv $TMP_DIR/merchant_say_3.wav $TMP_DIR/merchant_impatientto_sell.wav

rename 's/patric_/patrician_/' $TMP_DIR/*.wav

rename 's/vigils_/prefect_/' $TMP_DIR/*.wav
# Normal life action : assess crime level
mv $TMP_DIR/prefect_say_1.wav $TMP_DIR/prefect_no_crime.wav
mv $TMP_DIR/prefect_say_2.wav $TMP_DIR/prefect_some_crime.wav
mv $TMP_DIR/prefect_say_3.wav $TMP_DIR/prefect_high_crime.wav
# fight fire
mv $TMP_DIR/prefect_say_4.wav $TMP_DIR/prefect_goto_fire.wav
mv $TMP_DIR/prefect_say_5.wav $TMP_DIR/prefect_fight_fire.wav
mv $TMP_DIR/prefect_say_6.wav $TMP_DIR/prefect_fight_fire2.wav
# fight ennemy
mv $TMP_DIR/prefect_say_7.wav $TMP_DIR/prefect_morale_very_low.wav
mv $TMP_DIR/prefect_say_8.wav $TMP_DIR/prefect_morale_low.wav
mv $TMP_DIR/prefect_say_9.wav $TMP_DIR/prefect_morale_medium.wav
mv $TMP_DIR/prefect_say_10.wav $TMP_DIR/prefect_morale_high.wav

rename 's/pleb_/recruter_/' $TMP_DIR/*.wav

# nothing to do for rioter : random say

rename 's/pupils_/scholar_/' $TMP_DIR/*.wav

rename 's/boats_/seaMerchant_/' $TMP_DIR/*.wav
# waiting at dock
mv $TMP_DIR/seaMerchant_say_1.wav $TMP_DIR/seaMerchant_goto_warehouse.wav
mv $TMP_DIR/seaMerchant_say_2.wav $TMP_DIR/seaMerchant_return_from_warehouse.wav
# moving 
mv $TMP_DIR/seaMerchant_say_3.wav $TMP_DIR/seaMerchant_notany_trade.wav
mv $TMP_DIR/seaMerchant_say_5.wav $TMP_DIR/seaMerchant_happy_tohome.wav
# coming to the city
mv $TMP_DIR/seaMerchant_say_4.wav $TMP_DIR/seaMerchant_impatientto_sell.wav

rename 's/taxman_/taxCollector_/' $TMP_DIR/*.wav
# taxCollector assesses the tax income of the city
mv $TMP_DIR/taxCollector_say_1.wav $TMP_DIR/taxCollector_high_tax.wav
mv $TMP_DIR/taxCollector_say_2.wav $TMP_DIR/taxCollector_normal_life.wav
mv $TMP_DIR/taxCollector_say_3.wav $TMP_DIR/taxCollector_very_little_tax.wav

rename 's/teach_/teacher_/' $TMP_DIR/*.wav

rename 's/wallguard_/romeGuard_/' $TMP_DIR/*.wav
# Normal life action : assess ennemy
mv $TMP_DIR/romeGuard_say_1.wav $TMP_DIR/romeGuard_no_barbarians.wav
mv $TMP_DIR/romeGuard_say_2.wav $TMP_DIR/romeGuard_no_ennemy.wav
# fight ennemy
mv $TMP_DIR/romeGuard_say_3.wav $TMP_DIR/romeGuard_morale_high.wav
mv $TMP_DIR/romeGuard_say_4.wav $TMP_DIR/romeGuard_morale_medium.wav
mv $TMP_DIR/romeGuard_say_5.wav $TMP_DIR/romeGuard_morale_low.wav

# TODO: unemploy_say

# make the output directories
mkdir -v -p resources
mkdir -v -p resources/audio

mv resources/audio/wavs_citizen.zip resources/audio/wavs_citizen.zip.0
# rm resources/audio/wavs_citizen.zip
zip -j resources/audio/wavs_citizen.zip $TMP_DIR/*.wav

echo "All installed in directory: resources"
echo "DONE."


