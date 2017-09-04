#sh koodi buildaamiseen

echo "Buildataan serveri g++ compilerilla kansioon \"buildit\" \n"

echo "Tässä pitäisi näkyä compilerin errorit (jos niitä on)\n...\n"

g++ sorsa/*.cpp -pthread -o buildit/chatti_serveri

echo "Buildi valmis\n"