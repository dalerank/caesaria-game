
export LANG=fr_FR.utf8
export LANGUAGE=fr
valgrind --tool=memcheck --leak-check=full ./caesar 2> valgrind.log

