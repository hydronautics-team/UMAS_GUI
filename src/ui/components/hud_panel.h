#pragma once

#include <QFrame>

class HudPanel : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool accentVisible READ accentVisible WRITE setAccentVisible)

public:
    explicit HudPanel(QWidget *parent = nullptr);

    bool accentVisible() const;
    void setAccentVisible(bool visible);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool accentVisible_ = true;
};
