#include <gtest/gtest.h>
#include <sstream>
#include "TableOutput.h"
#include "Linear_Model.h"
#include <cstdio>
#include <fstream>

TEST(TableOutputTest, FullWorkflow) {
    Linear_Model m(0.5, 1.0, 0.0, 0.0, 0, 2.0);
    
    // Тестируем simulate()
    auto rows = simulate(m, 5);
    EXPECT_EQ(rows.size(), 6); // 5 шагов + нулевой

    // Тестируем save_table_to_file()
    std::string filename = "test_output_temp.csv";
    EXPECT_TRUE(save_table_to_file(rows, filename));
    
    // Проверяем, что файл реально создался
    std::ifstream f(filename);
    EXPECT_TRUE(f.is_open());
    f.close();
    
    // Тестируем run_and_report()
    Linear_Model m2(0.5, 1.0, 0.0, 0.0, 0, 2.0);
    std::string report_file = "test_report_temp.csv";
    EXPECT_TRUE(run_and_report(m2, 2, "Test Title", report_file));
    
    // Убираем за собой (чтобы не мусорить)
    std::remove(filename.c_str());
    std::remove(report_file.c_str());
}

TEST(TableOutputTest, WriteToStream) {
    // Создаем фейковые данные
    std::vector<SimRow> rows = {
        {0, 0.0, 0.0, false},
        {1, 1.5, 2.0, true}
    };
    
    // Создаем строковый поток (он ведет себя как консоль/файл, но хранит данные в памяти)
    std::stringstream ss;
    write_table(rows, ss, ',');
    
    // Проверяем, что в потоке появился нужный заголовок
    std::string output = ss.str();
    EXPECT_NE(output.find("step,y,u_used"), std::string::npos);
}