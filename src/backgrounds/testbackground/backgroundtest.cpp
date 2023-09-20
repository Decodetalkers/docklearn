#include "backgroundtest.h"

#include <QRandomGenerator>
#include <QTimer>

#include <string>
#include <string_view>

using namespace std::string_view_literals;

constexpr auto SVG_TEST =
  R"("data:image/svg+xml;utf8,<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 96 105\"> <g fill=\"#97C024\" stroke=\"#97C024\" stroke-linejoin=\"round\" stroke-linecap=\"round\"> <path d=\"M14,40v24M81,40v24M38,68v24M57,68v24M28,42v31h39v-31z\" stroke-width=\"12\"/> <path d=\"M32,5l5,10M64,5l-6,10 \" stroke-width=\"2\"/> </g> <path d=\"M22,35h51v10h-51zM22,33c0-31,51-31,51,0\" fill=\"#97C024\"/> <g fill=\"#FFF\"> <circle cx=\"36\" cy=\"22\" r=\"2\"/> <circle cx=\"59\" cy=\"22\" r=\"2\"/> </g> </svg>")"sv;

TestBackground::TestBackground(QObject *parent)
{
    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, &QTimer::timeout, this, [this, timer] {
        if (m_count == 10) {
            timer->stop();
            timer->deleteLater();
            return;
        }
        quint64 id            = QRandomGenerator::global()->generate64();
        WindowElement *window = new WindowElement(QString::number(id));
        window->setIcon(QString::fromStdString(SVG_TEST.data()));
        Q_EMIT windowGenerated(window);
        m_count++;
    });
    timer->start();
}
