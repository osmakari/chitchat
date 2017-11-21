#sh koodi buildaamiseen

echo "Buildataan clientti g++ compilerilla kansioon \"buildit\" \n"

echo "Tässä pitäisi näkyä compilerin errorit (jos niitä on)\n...\n"

g++ sorsa/*.cpp -std=c++11 -pthread -o buildit/chitchat

echo "Buildi valmis\n"