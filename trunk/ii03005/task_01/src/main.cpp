#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>

enum InputType
{
    Step,
    Impulse,
    Harmonic
};

class Model{
protected:
    int n, t;
    InputType inputType;
    std::vector<double> y;
    std::vector<double> u;
    std::string filename;

    virtual double GetTime(int i){return i;}

public:

    virtual ~Model() = default;

    virtual void NextStep() = 0;

    std::string GetFilename(){return filename;}

    void StartModel(){
        while (t < n - 1)
            NextStep();
    }

    void CountU(double val_u = 0){
        if (inputType == InputType::Step){
            u.assign(n, val_u);
        } else if(inputType == InputType::Impulse){
            u.assign(n, 0);
            u[0] = 1;
        } else if(inputType == InputType::Harmonic){
            u.assign(n, 0);
            for (int i = 0; i<n; i++){
                u[i] = std::sin(GetTime(i));
            }
        }
    }

    void show_y(){
        for (int i = 0; i < n; i++)
            std::cout << i << " - " << y[i] << '\n';
        std::cout << "\n\n";
    }

    void saveCSV(const std::string& filename) {
    this->filename = filename+".csv";
    std::ofstream file(this->filename);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл\n";
        return;
    }
    file << "t,u,y\n";
    for (int i = 0; i < n; i++) {
        file << GetTime(i) << ";" << u[i] << ";" << y[i] << "\n";
    }

    file.close();
    }

    
};


class Liner: public Model{
private:
    double a1, a2, b;
    int k;
public:
    Liner(double a1, double a2, double b, int k, int n, InputType inputType, double u = 0):
    a1(a1), a2(a2), b(b), k(k){
        this->n = n;
        this->inputType = inputType;
        CountU(u);
        y.assign(n, 0.0);
        t = 0;
    }

    void NextStep() override {
        double y_t = y[t];
        double y_tk, y_new;
        if (t < k)
            y_tk = 0;
        else
            y_tk = y[t-k];

        y_new = a1 * y_t + a2 * y_tk + b*u[t];
        y[++t] = y_new;
    }

};


class NonLiner: public Model{
private:
    double a, b, e;
public:
    NonLiner(double a, double b, double e, int n, InputType inputType, double u = 0):
    a(a), b(b), e(e) {
        this->n = n;
        this->inputType = inputType;
        CountU(u);
        y.assign(n, 0.0);
        t = 0;
    }

    double relay(double ut, double yt){
        if (ut > e || (ut >= -e && yt > 0))
            return 1;
        else if (ut < -e || (ut <= e && yt <= 0))
            return -1;
        else
            return 0;
    }

    void NextStep() override {
        double y_t = y[t];
        double y_new = a * y_t + b * relay(u[t], y_t);
        y[++t] = y_new;
    }
};


class Differential: public Model{
private:
    double b, dt;
protected:
    double GetTime(int i)override{return i*dt;}
public:
    Differential(double b, double dt, int n, InputType inputType, double u = 0):
    b(b), dt(dt){
        this->n = n;
        this->inputType = inputType;
        this->CountU(u);
        y.assign(n, 0.0);
        t = 0;
    }

    void NextStep() override {
        double yt = y[t];
        double y_new = yt + dt * b * std::tanh(u[t]);
        y[++t] = y_new;        
    }
 
};


InputType chooseInputType()
{
    int choice;

    while (true)
    {
        std::cout << "\n========== INPUT TYPE ==========\n";
        std::cout << "1. Step\n";
        std::cout << "2. Impulse\n";
        std::cout << "3. Harmonic\n";
        std::cout << "Choose input: ";

        std::cin >> choice;

        switch (choice)
        {
            case 1:
                return InputType::Step;

            case 2:
                return InputType::Impulse;

            case 3:
                return InputType::Harmonic;

            default:
                std::cout << "Invalid choice. Try again.\n";
        }
    }
}


void showGraph(const std::string& filename)
{   
    std::string command =  std::string(PYTHON_EXECUTABLE) + " " + std::string(PLOT_SCRIPT) + " " + filename;

    int result = std::system(command.c_str());

    if (result != 0)
    {
        std::cerr << "Не удалось запустить Python\n";
    }
}


int main(){
    setlocale(LC_ALL, "ru_RU.UTF-8");

    Model* model = nullptr;
    
     while (true)
    {
        std::cout << "\n========== MODEL MENU ==========\n";
        std::cout << "1. Linear model\n";
        std::cout << "2. Nonlinear model\n";
        std::cout << "3. Differential model\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose model: ";

        int modelChoice;
        std::cin >> modelChoice;

        if (modelChoice == 0)
        {
            std::cout << "Exit\n";
            break;
        }

        InputType inputType = chooseInputType();

        int n;

        std::cout << "\nEnter number of steps n: ";
        std::cin >> n;

        // LINEAR MODEL
        if (modelChoice == 1)
        {
            double a1, a2, b, u;
            int k;

            std::cout << "\n========== LINEAR MODEL ==========\n";

            std::cout << "Enter a1: ";
            std::cin >> a1;

            std::cout << "Enter a2: ";
            std::cin >> a2;

            std::cout << "Enter b: ";
            std::cin >> b;

            std::cout << "Enter k: ";
            std::cin >> k;

            std::cout << "Enter u: ";
            std::cin >> u;

            model = new Liner(a1, a2, b, k, n, inputType, u);
        }

        // NONLINEAR MODEL
        else if (modelChoice == 2)
        {
            double a, b, e, u;

            std::cout << "\n========== NONLINEAR MODEL ==========\n";

            std::cout << "Enter a: ";
            std::cin >> a;

            std::cout << "Enter b: ";
            std::cin >> b;

            std::cout << "Enter e: ";
            std::cin >> e;

            std::cout << "Enter u: ";
            std::cin >> u;

            model = new NonLiner(a, b, e, n, inputType, u);
        }

        // DIFFERENTIAL MODEL
        else if (modelChoice == 3)
        {
            double b, dt, u;

            std::cout << "\n========== DIFFERENTIAL MODEL ==========\n";

            std::cout << "Enter b: ";
            std::cin >> b;

            std::cout << "Enter dt: ";
            std::cin >> dt;

            std::cout << "Enter u: ";
            std::cin >> u;

            model = new Differential(b, dt, n, inputType, u);
        }


        else
        {
            std::cout << "Unknown model.\n";
            continue;
        }

        while (true)
        {
            std::cout << "\n========== MODEL MENU ==========\n";
            std::cout << "1. Start model\n";
            std::cout << "2. Show y\n";
            std::cout << "3. Save CSV\n";
            std::cout << "4. make graph\n";
            std::cout << "0. Back\n";
            std::cout << "Choose: ";

            int choice;
            std::cin >> choice;

            if (choice == 0)
                break;

            switch (choice)
            {
                case 1:
                    model->StartModel();
                    std::cout << "Model completed.\n";
                    break;

                case 2:
                    model->show_y();
                    break;

                case 3:
                {
                    std::string filename;

                    std::cout << "Enter filename: ";
                    std::cin >> filename;

                    model->saveCSV(filename);

                    std::cout << "CSV saved.\n";
                    break;
                }
                case 4:
                {   
                    const std::string name = model->GetFilename();
                    if(!name.empty())
                        showGraph(name);
                    else
                        std::cout<<"Yuo need to save data to CSV file";
                    break;
                }

                default:
                    std::cout << "Unknown command.\n";
                    break;
            }
        }
        delete model;
    }

    return 0;
}