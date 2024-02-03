#include "view/monitoringview.h"
#include "model/model.h"
#include "controller/controller.h"

#include <clocale>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    std::setlocale(LC_NUMERIC, "C");
    
    monsys::Model m;
    monsys::Controller c(&m);
    monsys::MonitoringView v(&c);
    v.show();
    return a.exec();
}



