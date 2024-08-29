#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include <QWidget>
#include <QQuickView>
#include <QVBoxLayout>
#include <QUrl>

class MapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

public slots:

private:
};

#endif // MAP_WIDGET_H
