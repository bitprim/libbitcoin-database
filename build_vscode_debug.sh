mkdir build
cd build
# rm -rf *
conan install .. -o use_domain=False -o db=new  -o with_tests=True -s build_type=Debug
conan build ..