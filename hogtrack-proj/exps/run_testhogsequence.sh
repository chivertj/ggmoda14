#./testhogsequence ~/Videos/Jerome/20110421_142353_origdata.txt ./result/20110421_142353/hogseqres_
#./testhogsequence ~/Videos/Jerome/20110421_142353_origdata_sub.txt ./result_20110421_142353/hogseqres_

filebase="20110422_105820"
filelist=${filebase}_origdata.txt
#rear hit threshold
hitThreshold=1.51204

mkdir result_${filebase}
./hogtrack ~/Videos/Jerome/${filebase}_origdata.txt ./result_${filebase}/hogseqres_ ${hitThreshold}

