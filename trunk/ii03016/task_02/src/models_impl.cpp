// В этом файле мы "подтягиваем" код моделей из первой лабы,
// но переименовываем функцию main, чтобы она не мешала тестам.
#define main old_main_from_task01
#include "../../task_01/src/main.cpp"
#undef main