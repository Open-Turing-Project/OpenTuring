echo "copying..."
cp ./test/turing.exe ./package
cp -R ./test/support ./package
cp -R ./test/Examples ./package
echo "zipping..."
zip -r ./package ./package
