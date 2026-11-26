set(TASK01_SRC "../../task_01/src")

add_executable(OTIS_Lab3
    main.cpp
    pid.cpp
    "${TASK01_SRC}/model19.cpp"
    "${TASK01_SRC}/model23.cpp"
    "${TASK01_SRC}/model37.cpp"
)
