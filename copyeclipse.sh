cp ../0.0041/.settings ./ -r -f
cp ../0.0041/.project ./
cp ../0.0041/.cproject ./

sed -i -e 's/0.0041/0.0045/g' .cproject
sed -i -e 's/0.0041/0.0045/g' .project

