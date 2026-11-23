# Собираем все .cpp файлы из текущей папки
file(GLOB LR2_SOURCES "*.cpp")

# Создаем статическую библиотеку с именем lab2_lib
add_library(lab2_lib STATIC ${LR2_SOURCES})