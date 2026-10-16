#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "hud_panel.h"
#include <QAbstractButton>
#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QDateTime>
#include <QFontMetrics>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QTabBar>
#include <QTabWidget>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QtMath>
#include <algorithm>
#include <cmath>

enum EventCategory
{
    EventInfo = 1,
    EventSuccess,
    EventWarning,
    EventError
};

static EventCategory classifyEventMessage(const QString &message)
{
    const QString normalized = message.toLower();
    if (normalized.contains(QStringLiteral("ошиб")) ||
        normalized.contains(QStringLiteral("не обнаружен")) ||
        normalized.contains(QStringLiteral("недоступ")) ||
        normalized.contains(QStringLiteral("авар")))
        return EventError;
    if (normalized.contains(QStringLiteral("предуп")) ||
        normalized.contains(QStringLiteral("до дна")) ||
        normalized.contains(QStringLiteral("низк")))
        return EventWarning;
    if (normalized.contains(QStringLiteral("установлено")) ||
        normalized.contains(QStringLiteral("готов")) ||
        normalized.contains(QStringLiteral("успеш")) ||
        normalized.contains(QStringLiteral("активен")))
        return EventSuccess;
    return EventInfo;
}

static QColor eventCategoryColor(EventCategory category)
{
    switch (category)
    {
    case EventSuccess:
        return QColor(QStringLiteral("#17f49a"));
    case EventWarning:
        return QColor(QStringLiteral("#ff8a2a"));
    case EventError:
        return QColor(QStringLiteral("#f05268"));
    case EventInfo:
    default:
        return QColor(QStringLiteral("#83bce0"));
    }
}

class ManipulatorPreview final : public QWidget
{
public:
    explicit ManipulatorPreview(QWidget *parent = nullptr) : QWidget(parent)
    {
        setMinimumSize(190, 115);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(width() * 0.04, height() * 0.86);
        painter.rotate(-18.0);

        const QColor primary(0, 188, 232, 210);
        const QColor secondary(7, 109, 144, 145);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(secondary, 1, Qt::DashLine));
        painter.drawEllipse(QPointF(70, -28), 34, 34);
        painter.drawEllipse(QPointF(146, -42), 25, 25);

        painter.setPen(QPen(primary, 1.2));
        painter.drawRoundedRect(QRectF(-28, -24, 75, 39), 7, 7);
        painter.drawEllipse(QPointF(46, -5), 18, 18);
        QPolygonF forearm;
        forearm << QPointF(57, -14) << QPointF(129, -57) << QPointF(143, -37) << QPointF(67, 4);
        painter.drawPolygon(forearm);
        painter.drawEllipse(QPointF(142, -47), 15, 15);
        QPolygonF wristBody;
        wristBody << QPointF(154, -54) << QPointF(192, -73) << QPointF(199, -61)
                  << QPointF(157, -39);
        painter.drawPolygon(wristBody);

        const QPointF wrist(198, -67);
        painter.drawEllipse(wrist, 10, 10);
        painter.drawLine(wrist + QPointF(8, -3), wrist + QPointF(34, -23));
        painter.drawLine(wrist + QPointF(8, 3), wrist + QPointF(36, 18));
        painter.drawLine(wrist + QPointF(34, -23), wrist + QPointF(49, -18));
        painter.drawLine(wrist + QPointF(36, 18), wrist + QPointF(50, 11));

        painter.setPen(QPen(QColor(63, 223, 255, 110), 1));
        painter.drawLine(QPointF(-20, -4), QPointF(228, -66));
        painter.drawLine(QPointF(45, -5), QPointF(142, -47));
    }
};

class BathymetryMapPreview final : public QWidget
{
public:
    explicit BathymetryMapPreview(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("bathymetryMap"));
        setMinimumHeight(125);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setPosition(double x, double y)
    {
        const QPointF point(x, y);
        if (trail_.isEmpty() || QLineF(trail_.last(), point).length() > 0.02)
        {
            trail_.append(point);
            if (trail_.size() > 160)
                trail_.removeFirst();
        }
        position_ = point;
        hasPosition_ = true;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor("#00111d"));

        painter.setPen(QPen(QColor(6, 71, 92, 95), 1));
        for (int x = 0; x < width(); x += 24)
            painter.drawLine(x, 0, x, height());
        for (int y = 0; y < height(); y += 24)
            painter.drawLine(0, y, width(), y);

        painter.setBrush(Qt::NoBrush);
        for (int contour = 0; contour < 7; ++contour)
        {
            QPainterPath line;
            const qreal offset = contour * 13.0;
            line.moveTo(-15, height() * 0.28 + offset);
            line.cubicTo(width() * 0.24, height() * 0.02 + offset, width() * 0.48,
                         height() * 0.74 + offset * 0.2, width() + 15,
                         height() * 0.30 + offset * 0.55);
            painter.setPen(QPen(QColor(8, 124, 151, 70 + contour * 8), 1));
            painter.drawPath(line);
        }

        painter.setPen(QPen(QColor("#6fb7d2"), 1));
        painter.drawText(QRect(9, height() - 25, 55, 17), Qt::AlignLeft | Qt::AlignVCenter,
                         QStringLiteral("20 м"));
        painter.drawLine(9, height() - 9, 48, height() - 9);
        painter.drawLine(9, height() - 13, 9, height() - 5);
        painter.drawLine(48, height() - 13, 48, height() - 5);

        painter.drawEllipse(QPointF(width() - 20, 20), 13, 13);
        painter.drawText(QRect(width() - 33, 7, 26, 26), Qt::AlignCenter, QStringLiteral("N"));

        if (!hasPosition_)
        {
            painter.setPen(QColor("#668a9b"));
            painter.drawText(rect().adjusted(12, 12, -12, -12), Qt::AlignCenter | Qt::TextWordWrap,
                             QStringLiteral("НЕТ НАВИГАЦИОННЫХ ДАННЫХ"));
            return;
        }

        const QPointF center(width() * 0.48, height() * 0.58);
        QPainterPath track;
        for (int index = 0; index < trail_.size(); ++index)
        {
            const QPointF screen = center + (trail_.at(index) - position_) * 2.5;
            if (index == 0)
                track.moveTo(screen);
            else
                track.lineTo(screen);
        }
        painter.setPen(QPen(QColor("#00d9e6"), 2, Qt::DashLine));
        painter.drawPath(track);
        painter.setBrush(QColor("#56ecff"));
        painter.setPen(QPen(QColor("#c9fbff"), 1));
        QPolygonF vehicle;
        vehicle << center + QPointF(0, -9) << center + QPointF(7, 7) << center + QPointF(0, 4)
                << center + QPointF(-7, 7);
        painter.drawPolygon(vehicle);
    }

private:
    QVector<QPointF> trail_;
    QPointF position_;
    bool hasPosition_ = false;
};

class VehicleOrientationPreview final : public QWidget
{
public:
    explicit VehicleOrientationPreview(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("vehicleOrientationPreview"));
        setMinimumSize(170, 115);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setAttitude(float yaw, float pitch, float roll)
    {
        yaw_ = yaw;
        pitch_ = pitch;
        roll_ = roll;
        hasAttitude_ = true;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(rect(), QColor(QStringLiteral("#00111d")));

        const QPointF horizon(width() * 0.5, height() * 0.68);
        painter.setPen(QPen(QColor(7, 81, 107, 115), 1));
        for (int row = 0; row < 6; ++row)
        {
            const qreal factor = row / 5.0;
            const qreal y = horizon.y() + factor * (height() - horizon.y());
            const qreal halfWidth = 18 + factor * width() * 0.55;
            painter.drawLine(QPointF(horizon.x() - halfWidth, y),
                             QPointF(horizon.x() + halfWidth, y));
        }
        for (int column = -5; column <= 5; ++column)
            painter.drawLine(horizon, QPointF(horizon.x() + column * width() * 0.12, height()));

        painter.save();
        painter.translate(width() * 0.5, height() * 0.48 + pitch_ * 0.22);
        painter.rotate(-roll_);
        const qreal yawScale = 0.78 + 0.22 * std::abs(std::cos(qDegreesToRadians(yaw_)));
        painter.scale(yawScale, 1.0);

        const QColor outline(0, 174, 239, hasAttitude_ ? 220 : 95);
        const QColor detail(21, 216, 255, hasAttitude_ ? 170 : 70);
        painter.setBrush(QColor(0, 89, 122, hasAttitude_ ? 38 : 18));
        painter.setPen(QPen(outline, 1.3));
        painter.drawRoundedRect(QRectF(-54, -22, 108, 44), 20, 20);
        painter.drawRoundedRect(QRectF(-34, -15, 68, 30), 11, 11);

        for (int side : {-1, 1})
        {
            const qreal x = side * 47;
            painter.drawEllipse(QPointF(x, 0), 17, 17);
            painter.drawEllipse(QPointF(x, 0), 9, 9);
            painter.drawLine(QPointF(side * 33, -14), QPointF(x, -12));
            painter.drawLine(QPointF(side * 33, 14), QPointF(x, 12));
        }
        painter.setPen(QPen(detail, 1));
        painter.drawLine(QPointF(-28, 0), QPointF(28, 0));
        painter.drawLine(QPointF(0, -15), QPointF(0, 15));
        painter.drawEllipse(QPointF(0, 0), 5, 5);
        painter.restore();

        painter.setFont(QFont(QStringLiteral("IBM Plex Mono"), 7));
        painter.setPen(QColor(QStringLiteral("#668a9b")));
        painter.drawText(rect().adjusted(7, 5, -7, -5), Qt::AlignLeft | Qt::AlignTop,
                         hasAttitude_ ? QStringLiteral("IMU / ORIENTATION")
                                      : QStringLiteral("ОЖИДАНИЕ IMU"));
    }

private:
    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float roll_ = 0.0f;
    bool hasAttitude_ = false;
};

class DepthSonarPreview final : public QWidget
{
public:
    explicit DepthSonarPreview(QWidget *parent = nullptr) : QWidget(parent)
    {
        setObjectName(QStringLiteral("depthSonarPreview"));
        setMinimumHeight(145);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    void setPose(double z, double verticalSpeed)
    {
        depth_ = std::abs(z);
        verticalSpeed_ = verticalSpeed;
        hasPose_ = true;
        if (depthHistory_.isEmpty() || std::abs(depthHistory_.last() - depth_) > 0.002)
        {
            depthHistory_.append(depth_);
            if (depthHistory_.size() > 120)
                depthHistory_.removeFirst();
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QColor grid(12, 82, 105, 120);
        const QColor accent("#18d8de");
        const QColor muted("#668a9b");
        const QRectF metrics(12, 5, width() * 0.54, 39);
        const QRectF profile(12, 49, width() * 0.54, height() - 60);
        const QRectF sonar(width() * 0.62, 12, width() * 0.34, height() - 26);

        painter.setFont(QFont(QStringLiteral("Roboto Condensed"), 8));
        painter.setPen(muted);
        painter.drawText(QRectF(metrics.left(), metrics.top(), metrics.width() * 0.5, 14),
                         Qt::AlignLeft, QStringLiteral("ГЛУБИНА"));
        painter.drawText(QRectF(metrics.center().x(), metrics.top(), metrics.width() * 0.5, 14),
                         Qt::AlignLeft, QStringLiteral("ДО ДНА"));
        painter.setFont(QFont(QStringLiteral("Exo 2"), 15, QFont::DemiBold));
        painter.setPen(accent);
        painter.drawText(QRectF(metrics.left(), metrics.top() + 14, metrics.width() * 0.5, 24),
                         Qt::AlignLeft | Qt::AlignVCenter,
                         hasPose_ ? QStringLiteral("%1 м").arg(depth_, 0, 'f', 1)
                                  : QStringLiteral("—"));
        painter.setPen(QColor(QStringLiteral("#ff8a2a")));
        painter.drawText(
            QRectF(metrics.center().x(), metrics.top() + 14, metrics.width() * 0.5, 24),
            Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("—"));

        painter.setPen(QPen(grid, 1));
        for (int column = 0; column <= 8; ++column)
        {
            const qreal x = profile.left() + profile.width() * column / 8.0;
            painter.drawLine(QPointF(x, profile.top()), QPointF(x, profile.bottom()));
        }
        for (int row = 0; row <= 4; ++row)
        {
            const qreal y = profile.top() + profile.height() * row / 4.0;
            painter.drawLine(QPointF(profile.left(), y), QPointF(profile.right(), y));
        }

        if (hasPose_ && !depthHistory_.isEmpty())
        {
            const auto bounds = std::minmax_element(depthHistory_.cbegin(), depthHistory_.cend());
            const double minimum = *bounds.first;
            const double maximum = *bounds.second;
            const double span = qMax(1.0, maximum - minimum);
            QPainterPath depthTrack;
            for (int index = 0; index < depthHistory_.size(); ++index)
            {
                const qreal x =
                    profile.left() + profile.width() * index / qMax(1, depthHistory_.size() - 1);
                const qreal y =
                    profile.top() + 5 +
                    (depthHistory_.at(index) - minimum) / span * (profile.height() - 10);
                if (index == 0)
                    depthTrack.moveTo(x, y);
                else
                    depthTrack.lineTo(x, y);
            }
            painter.setPen(QPen(accent, 1.6));
            painter.drawPath(depthTrack);
            painter.setFont(QFont(QStringLiteral("IBM Plex Mono"), 7));
            painter.setPen(muted);
            painter.drawText(profile.adjusted(5, 3, -5, -3), Qt::AlignLeft | Qt::AlignBottom,
                             QStringLiteral("VZ %1 м/с").arg(verticalSpeed_, 0, 'f', 2));
        }
        else
        {
            painter.setPen(muted);
            painter.setFont(QFont(QStringLiteral("Roboto Condensed"), 8));
            painter.drawText(profile, Qt::AlignCenter, QStringLiteral("НЕТ ДАННЫХ DVL"));
        }

        const QPointF center = sonar.center();
        const qreal radius = qMin(sonar.width(), sonar.height()) * 0.46;
        painter.setPen(QPen(grid, 1));
        for (int ring = 1; ring <= 4; ++ring)
            painter.drawEllipse(center, radius * ring / 4.0, radius * ring / 4.0);
        for (int ray = 0; ray < 8; ++ray)
        {
            const qreal angle = ray * M_PI / 4.0;
            painter.drawLine(center, center + QPointF(std::cos(angle), std::sin(angle)) * radius);
        }

        painter.setPen(muted);
        painter.setFont(QFont(QStringLiteral("Roboto Condensed"), 9));
        painter.drawText(sonar, Qt::AlignCenter, QStringLiteral("СОНАР\nНЕТ ДАННЫХ"));
    }

private:
    QVector<double> depthHistory_;
    double depth_ = 0.0;
    double verticalSpeed_ = 0.0;
    bool hasPose_ = false;
};

static QIcon makeSunIcon()
{
    QPixmap pm(24, 24);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const QPointF c(12, 12);
    p.setPen(QPen(QColor("#f5a623"), 2, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 8; ++i)
    { // лучи
        const qreal a = i * M_PI / 4.0;
        p.drawLine(QPointF(c.x() + 8 * std::cos(a), c.y() + 8 * std::sin(a)),
                   QPointF(c.x() + 11 * std::cos(a), c.y() + 11 * std::sin(a)));
    }
    p.setPen(QPen(QColor("#f5a623"), 1.5)); // диск
    p.setBrush(QColor("#ffd54f"));
    p.drawEllipse(c, 5.5, 5.5);
    p.end();
    return QIcon(pm);
}

static QIcon makeMoonIcon()
{
    QPixmap pm(24, 24);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath moon;
    moon.addEllipse(QRectF(3, 3, 18, 18));
    QPainterPath cut; // вырезаем серп
    cut.addEllipse(QRectF(9, 1, 16, 16));
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#cfd8dc"));
    p.drawPath(moon - cut);
    p.end();
    return QIcon(pm);
}

static const QString LIGHT_THEME =
    "QMainWindow { background-color: #dcecea; color: #16383d; font-family: 'Inter', 'Segoe UI', "
    "sans-serif; font-size: 13px; }"
    "QWidget#centralwidget { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #f5fbfa, "
    "stop:0.55 #eaf5f3, stop:1 #dcecea); color: #16383d; font-family: 'Inter', 'Segoe UI', "
    "sans-serif; font-size: 13px; }"
    "QWidget { outline: none; }"
    "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, stop:1 "
    "#dceae8); border: 1px solid #91b9b7; border-radius: 8px; padding: 6px 13px; color: #16383d; "
    "min-height: 22px; font-size: 12px; font-weight: 500; }"
    "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f7fbfa, "
    "stop:1 #cce8e4); border: 1px solid #159d93; color: #075e58; }"
    "QPushButton:pressed { background: #c5dfdc; border: 1px solid #0f8f87; color: #064d49; "
    "padding: 6px 13px; }"
    "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #62ddd2, "
    "stop:1 #19aaa0); border: 1px solid #07847c; color: #052f2c; font-weight: 700; }"
    "QPushButton:disabled { background-color: #e4eeed; color: #829c9d; border-color: #bfd1d0; }"
    "QTabWidget::pane { border: 1px solid #b5d2d0; background-color: #edf5f4; border-radius: 9px; "
    "top: -1px; }"
    "QTabBar::tab { background-color: transparent; color: #557b7d; padding: 8px 16px; "
    "margin-right: 3px; border: none; border-bottom: 2px solid transparent; font-weight: 600; }"
    "QTabBar::tab:selected { color: #087f77; border-bottom-color: #15a99f; }"
    "QTabBar::tab:hover:!selected { color: #075e58; background-color: #dcefed; }"
    "QLineEdit, QSpinBox, QComboBox, QDoubleSpinBox { background-color: #f2f8f7; border: 1px solid "
    "#a8c9c7; border-radius: 6px; padding: 5px 8px; color: #16383d; selection-background-color: "
    "#42c6bc; selection-color: #052f2c; }"
    "QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDoubleSpinBox:focus { border: 1px solid "
    "#159d93; background-color: #f7fbfa; }"
    "QComboBox QAbstractItemView { background-color: #f2f8f7; border: 1px solid #91b9b7; color: "
    "#16383d; selection-background-color: #bde9e5; selection-color: #064d49; }"
    "QGroupBox { border: 1px solid #b5d2d0; border-radius: 9px; margin-top: 12px; padding-top: "
    "15px; background-color: #edf5f4; color: #087f77; font-weight: 600; }"
    "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 7px; color: #087f77; }"
    "QRadioButton, QCheckBox { color: #23474c; spacing: 7px; }"
    "QRadioButton::indicator, QCheckBox::indicator { width: 15px; height: 15px; border: 1px solid "
    "#78aaa7; background-color: #f2f8f7; }"
    "QRadioButton::indicator { border-radius: 8px; } QCheckBox::indicator { border-radius: 4px; }"
    "QRadioButton::indicator:checked, QCheckBox::indicator:checked { background-color: #1ab2a7; "
    "border: 2px solid #087f77; }"
    "QTextEdit { background-color: #f0f7f6; color: #12665f; border: 1px solid #b5d2d0; "
    "border-radius: 7px; padding: 6px; font-family: 'JetBrains Mono', 'Consolas', monospace; }"
    "QLCDNumber { background-color: #eef8f7; color: #087f77; border: 1px solid #b5d2d0; "
    "border-radius: 6px; }"
    "QLabel { color: #23474c; background: transparent; }"
    "QFrame#frame_telemetry { background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: "
    "10px; padding: 5px; }"
    "QFrame#frame_quickActions, QFrame#dashboardNavigation { background-color: #edf5f4; border: "
    "1px solid #b5d2d0; border-radius: 9px; }"
    "QPushButton[dashboardNav='true'] { text-align: left; padding: 11px 13px; border: none; "
    "border-left: 3px solid transparent; background: transparent; color: #557b7d; }"
    "QPushButton[dashboardNav='true']:checked { color: #087f77; background-color: #dcefed; "
    "border-left-color: #15a99f; font-weight: 700; }"
    "QLabel[hudValue='true'], QPushButton[hudValue='true'] { background-color: #e0f2f0; color: "
    "#087f77; border: 1px solid #abd2cf; border-radius: 7px; padding: 4px 9px; font-weight: 600; }"
    "QPushButton#btn_killswitch_status { min-height: 0px; padding: 4px 10px; margin: 0px; }"
    "QPushButton#btn_killswitch_status:disabled { color: #8f1830; background-color: #ffdce2; "
    "border: 1px solid #ef7185; }"
    "QLabel[controlValue='true'] { background-color: #e0f2f0; color: #087f77; border: 1px solid "
    "#abd2cf; border-radius: 5px; padding: 3px 7px; min-width: 24px; font-family: 'JetBrains "
    "Mono', 'Consolas', monospace; font-weight: 600; }"
    "QStackedWidget { background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: 9px; }"
    "QTableView, QTableWidget, QListView, QTreeView { background-color: #f0f7f6; "
    "alternate-background-color: #e5f0ee; color: #16383d; border: 1px solid #b5d2d0; "
    "border-radius: 6px; gridline-color: #c8dcda; selection-background-color: #bde9e5; "
    "selection-color: #064d49; }"
    "QHeaderView::section { background-color: #dcefed; color: #087f77; border: none; border-right: "
    "1px solid #b5d2d0; border-bottom: 1px solid #b5d2d0; padding: 6px; font-weight: 700; }"
    "QProgressBar { background-color: #e1efed; color: #16383d; border: 1px solid #b5d2d0; "
    "border-radius: 6px; text-align: center; } QProgressBar::chunk { background-color: #1ab2a7; "
    "border-radius: 5px; }"
    "QSlider::groove:horizontal { background: #c8dcda; height: 5px; border-radius: 2px; } "
    "QSlider::sub-page:horizontal { background: #1ab2a7; border-radius: 2px; }"
    "QSlider::handle:horizontal { background: #f2f8f7; border: 2px solid #159d93; width: 15px; "
    "margin: -6px 0; border-radius: 8px; }"
    "QScrollBar:vertical { background: #e6f1ef; width: 9px; margin: 0; } "
    "QScrollBar::handle:vertical { background: #8bb9b6; min-height: 24px; border-radius: 4px; } "
    "QScrollBar::handle:vertical:hover { background: #159d93; }"
    "QMenuBar { background-color: #eef8f7; border: none; } QStatusBar { background-color: #eef8f7; "
    "color: #557b7d; border: none; }"
    "QLabel#headerConnection { color: #087f77; font-weight: 700; padding: 5px 9px; }"
    "QLabel#headerClock { color: #16383d; font-family: 'JetBrains Mono', 'Consolas', monospace; "
    "font-weight: 600; padding: 3px 8px; }"
    "QToolTip { background-color: #f2f8f7; color: #16383d; border: 1px solid #159d93; padding: "
    "5px; border-radius: 4px; }";

static const QString DARK_THEME =
    "QMainWindow { background-color: #000c17; color: #e3f6ff; font-family: 'Roboto Condensed', "
    "'Arial Narrow', 'Segoe UI', sans-serif; font-size: 13px; }"
    "QWidget#centralwidget { background: qradialgradient(cx:0.5, cy:0.35, radius:0.9, stop:0 "
    "#031726, stop:0.65 #000c17, stop:1 #000a14); color: #e3f6ff; font-family: 'Roboto "
    "Condensed', 'Arial Narrow', 'Segoe UI', sans-serif; font-size: 13px; }"
    "QWidget { outline: none; }"
    "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, stop:1 "
    "#0a303a); border: 1px solid #236574; border-radius: 8px; padding: 6px 13px; color: #d7f3f1; "
    "min-height: 22px; font-size: 12px; font-weight: 500; }"
    "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #176173, "
    "stop:1 #0d414d); border: 1px solid #74eee3; color: #ffffff; }"
    "QPushButton:pressed { background: #0b2a32; border: 1px solid #20c7b8; color: #8af5ec; "
    "padding: 6px 13px; }"
    "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #74eee3, "
    "stop:1 #20b9ad); border: 1px solid #a7fff7; color: #031416; font-weight: 700; }"
    "QPushButton:disabled { background-color: #0a252d; color: #55747a; border-color: #123b45; }"
    "QTabWidget::pane { border: 1px solid #174a56; background-color: #09262e; border-radius: 8px; "
    "top: -1px; }"
    "QTabBar::tab { background-color: transparent; color: #7faaa9; padding: 8px 16px; "
    "margin-right: 3px; border: none; border-bottom: 2px solid transparent; font-weight: 600; }"
    "QTabBar::tab:selected { color: #74eee3; border-bottom-color: #20c7b8; }"
    "QTabBar::tab:hover:!selected { color: #c4e9e6; background-color: #0d3540; }"
    "QLineEdit, QSpinBox, QComboBox, QDoubleSpinBox { background-color: #071f27; border: 1px solid "
    "#185464; border-radius: 6px; padding: 5px 8px; color: #e7fbf9; selection-background-color: "
    "#20c7b8; selection-color: #031416; }"
    "QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDoubleSpinBox:focus { border: 1px solid "
    "#5de2d7; background-color: #0a2932; }"
    "QComboBox QAbstractItemView { background-color: #09262e; border: 1px solid #247080; color: "
    "#d7f3f1; selection-background-color: #19b8aa; selection-color: #031416; }"
    "QGroupBox { border: 1px solid #174a56; border-radius: 9px; margin-top: 12px; padding-top: "
    "15px; background-color: #09262e; color: #66e2d7; font-weight: 600; }"
    "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 7px; color: #66e2d7; }"
    "QRadioButton, QCheckBox { color: #c9e7e5; spacing: 7px; }"
    "QRadioButton::indicator, QCheckBox::indicator { width: 15px; height: 15px; border: 1px solid "
    "#247080; background-color: #071f27; }"
    "QRadioButton::indicator { border-radius: 8px; }"
    "QCheckBox::indicator { border-radius: 4px; }"
    "QRadioButton::indicator:checked, QCheckBox::indicator:checked { background-color: #20c7b8; "
    "border: 2px solid #8af5ec; }"
    "QTextEdit { background-color: #04161c; color: #7de5d5; border: 1px solid #174a56; "
    "border-radius: 7px; padding: 6px; font-family: 'JetBrains Mono', 'Consolas', monospace; }"
    "QLCDNumber { background-color: #04161c; color: #5de2d7; border: 1px solid #174a56; "
    "border-radius: 6px; }"
    "QLabel { color: #c9e7e5; background: transparent; }"
    "QFrame#frame_telemetry { background-color: #09262e; border: 1px solid #174a56; border-radius: "
    "10px; padding: 5px; }"
    "QFrame#frame_quickActions, QFrame#dashboardNavigation { background-color: #071f27; border: "
    "1px solid #174a56; border-radius: 9px; }"
    "HudPanel[telemetryCard='true'] { background: transparent; }"
    "QLabel[telemetryIcon='true'] { color: #55f5f0; background-color: #021825; border: 1px solid "
    "#07516b; border-radius: 5px; font-family: 'IBM Plex Mono', monospace; font-size: 10px; "
    "font-weight: 600; padding: 3px; }"
    "QLabel[telemetryTitle='true'] { color: #91bbd0; font-size: 11px; font-weight: 500; }"
    "QLabel[telemetryValue='true'] { color: #e3f6ff; font-family: 'Exo 2', 'Segoe UI', sans-serif; "
    "font-size: 20px; font-weight: 600; background: transparent; border: none; padding: 0; }"
    "QLabel[telemetryValue='true'][telemetryStale='true'], "
    "QPushButton[telemetryValue='true'][telemetryStale='true'] { color: #668a9b; }"
    "QLabel[orientationMetric='true'][telemetryStale='true'] { color: #668a9b; "
    "border-color: #123b45; }"
    "HudPanel[commandUnavailable='true'] { border-color: #6f5428; }"
    "QLabel[dashboardPanelTitle='true'] { color: #55e9ed; font-size: 12px; font-weight: 700; "
    "letter-spacing: 0.5px; padding: 0 0 2px 1px; }"
    "QLabel[emptyInstrument='true'] { color: #668a9b; border: 1px solid #0c5269; "
    "background-color: #021522; font-family: 'IBM Plex Mono', monospace; font-size: 10px; }"
    "QWidget[manipulatorMetric='true'] { border-bottom: 1px solid #164456; background: "
    "transparent; }"
    "QLabel[metricLabel='true'], QLabel[coordinateLabel='true'] { color: #779dad; font-size: 10px; "
    "}"
    "QLabel[metricValue='true'] { color: #e3f6ff; font-size: 13px; font-weight: 600; }"
    "QProgressBar[mechanismState='true'] { min-height: 0; max-height: 7px; border-radius: 3px; "
    "background-color: #082738; border-color: #0b526b; }"
    "QProgressBar[mechanismState='true']::chunk { background-color: #20d9d4; border-radius: 3px; }"
    "QWidget#bathymetryMap { border: 1px solid #07516b; border-radius: 6px; }"
    "HudPanel#positionPanel QLineEdit { padding: 2px 4px; min-height: 18px; font-size: 10px; }"
    "HudPanel#positionPanel QPushButton { padding: 2px 6px; min-height: 20px; font-size: 10px; }"
    "QWidget#dashboardBottomRow, QWidget#dashboardInstrumentColumn { background: transparent; }"
    "HudPanel#movementControlPanel QStackedWidget { background: transparent; border: none; }"
    "QLabel[controlSectionTitle='true'] { color: #779dad; font-size: 10px; font-weight: 600; }"
    "QPushButton[primaryMode='true'] { padding: 3px 5px; min-height: 25px; font-size: 11px; }"
    "QPushButton[speedChoice='true'] { padding: 2px 5px; min-height: 22px; font-size: 10px; }"
    "QPushButton[motionPad='true'] { padding: 2px; min-width: 0; min-height: 0; border-radius: "
    "7px; "
    "font-size: 13px; font-weight: 700; }"
    "QPushButton[axisMode='true'] { padding: 2px 3px; min-height: 23px; min-width: 0; "
    "font-size: 9px; }"
    "QLabel[depthCommand='true'] { color: #e3f6ff; font-family: 'IBM Plex Mono', monospace; "
    "font-size: 13px; font-weight: 600; }"
    "QComboBox[eventLogFilter='true'] { min-height: 20px; padding: 2px 7px; font-size: 10px; "
    "background-color: #031b29; border: 1px solid #0b5c78; border-radius: 4px; }"
    "QComboBox[eventLogFilter='true']::drop-down { width: 24px; border-left: 1px solid #0b5c78; }"
    "HudPanel#eventLogPanel QTextEdit { background-color: #000c17; border: none; padding: 5px 7px; "
    "font-family: 'IBM Plex Mono', 'Consolas', monospace; font-size: 10px; }"
    "HudPanel#eventLogPanel QScrollBar:vertical { width: 7px; background: #000c17; }"
    "HudPanel#eventLogPanel QScrollBar::handle:vertical { background: #073149; border-radius: 3px; "
    "}"
    "QWidget#cameraToolbar { background-color: #02131f; border-bottom: 1px solid #07516b; }"
    "QLabel#cameraTitle { color: #00d9e6; font-size: 11px; font-weight: 700; }"
    "QLabel#cameraType, QLabel[streamBadge='true'] { color: #91bbd0; background-color: #011421; "
    "border: 1px solid #10445a; border-radius: 4px; padding: 3px 7px; font-size: 9px; }"
    "QLabel#cameraLiveDot { color: #f03d59; font-size: 11px; }"
    "QLabel#cameraLiveLabel { color: #f5fbff; font-size: 10px; font-weight: 700; }"
    "QLabel#cameraBitrate { color: #91bbd0; font-size: 9px; }"
    "QPushButton[cameraAction='true'] { min-width: 0; min-height: 0; padding: 0; border-radius: "
    "5px; "
    "background-color: #032236; border: 1px solid #0a6380; color: #55e9ed; font-size: 14px; }"
    "QPushButton[cameraThumbnail='true'] { text-align: left bottom; padding: 5px 7px; min-width: "
    "0; "
    "min-height: 0; color: #7f9cab; background-color: rgba(0, 15, 26, 220); "
    "border: 1px solid #19485d; border-radius: 6px; font-size: 10px; }"
    "QPushButton[cameraThumbnail='true'][activeCamera='true'] { color: #ffffff; "
    "border: 2px solid #00d9e6; background-color: rgba(3, 53, 65, 220); }"
    "QFrame#frame_quickActions QPushButton { padding: 3px 5px; min-width: 0; font-size: 10px; }"
    "QLabel[orientationMetric='true'] { color: #dceff7; background-color: #031722; "
    "border: 1px solid #0b526b; border-radius: 5px; padding: 5px 6px; "
    "font-family: 'IBM Plex Mono', monospace; font-size: 10px; font-weight: 600; }"
    "QWidget#centralwidget[compactMode='true'] QLabel[telemetryTitle='true'] { font-size: 9px; }"
    "QWidget#centralwidget[compactMode='true'] QLabel[telemetryValue='true'] { font-size: 17px; }"
    "QWidget#centralwidget[compactMode='true'] QPushButton[axisMode='true'] { font-size: 8px; "
    "padding-left: 1px; padding-right: 1px; }"
    "QPushButton[dashboardNav='true'] { text-align: left; padding: 11px 13px; border: none; "
    "border-left: 3px solid transparent; background: transparent; color: #7faaa9; }"
    "QPushButton[dashboardNav='true']:hover { color: #c4e9e6; background-color: #0d3540; }"
    "QPushButton[dashboardNav='true']:checked { color: #74eee3; background-color: #0d3540; "
    "border-left-color: #20c7b8; font-weight: 700; }"
    "QLabel[hudValue='true'], QPushButton[hudValue='true'] { background-color: #0b3039; color: "
    "#74eee3; border: 1px solid #1d5965; border-radius: 7px; padding: 4px 9px; font-weight: 600; }"
    "QPushButton#btn_killswitch_status { min-height: 0px; padding: 4px 10px; margin: 0px; }"
    "QPushButton#btn_killswitch_status:disabled { color: #ffe8ec; background-color: #a3263d; "
    "border: 1px solid #f05268; }"
    "QWidget[commandUnavailable='true'] QPushButton:disabled { color: #46616b; "
    "background: #071b24; border-color: #163846; }"
    "QWidget[commandUnavailable='true'] QLabel[dashboardPanelTitle='true'] { color: #9b7841; }"
    "QWidget[commandUnavailable='true'] QLabel[controlValue='true'], "
    "QWidget[commandUnavailable='true'] QLabel[metricValue='true'] { color: #46616b; "
    "border-color: #163846; }"
    "QWidget[commandUnavailable='true'] QProgressBar { border-color: #163846; "
    "background-color: #071b24; }"
    "QLabel[controlValue='true'] { background-color: #041a20; color: #74eee3; border: 1px solid "
    "#174a56; border-radius: 5px; padding: 3px 7px; min-width: 24px; font-family: 'JetBrains "
    "Mono', 'Consolas', monospace; font-weight: 600; }"
    "QStackedWidget { background-color: #09262e; border: 1px solid #174a56; border-radius: 9px; }"
    "QTableView, QTableWidget, QListView, QTreeView { background-color: #071f27; "
    "alternate-background-color: #0a2932; color: #d7f3f1; border: 1px solid #174a56; "
    "border-radius: 6px; gridline-color: #174a56; selection-background-color: #12665f; "
    "selection-color: #efffff; }"
    "QHeaderView::section { background-color: #0d3540; color: #74eee3; border: none; border-right: "
    "1px solid #174a56; border-bottom: 1px solid #174a56; padding: 6px; font-weight: 600; }"
    "QProgressBar { background-color: #071f27; color: #d7f3f1; border: 1px solid #174a56; "
    "border-radius: 6px; text-align: center; }"
    "QProgressBar::chunk { background-color: #20c7b8; border-radius: 5px; }"
    "QFrame[frameShape='4'], QFrame[frameShape='5'] { color: #174a56; max-height: 1px; }"
    "QSlider::groove:horizontal { background: #123b45; height: 5px; border-radius: 2px; }"
    "QSlider::sub-page:horizontal { background: #20c7b8; border-radius: 2px; }"
    "QSlider::handle:horizontal { background: #8af5ec; border: 2px solid #0d3540; width: 15px; "
    "margin: -6px 0; border-radius: 8px; }"
    "QScrollBar:vertical { background: #071f27; width: 9px; margin: 0; }"
    "QScrollBar::handle:vertical { background: #226674; min-height: 24px; border-radius: 4px; }"
    "QScrollBar::handle:vertical:hover { background: #20c7b8; }"
    "QMenuBar { background-color: #04161c; border: none; }"
    "QLabel#headerConnection { color: #74eee3; font-weight: 700; padding: 5px 9px; }"
    "QLabel#headerConnection[connectionState='waiting'] { color: #f0b44d; }"
    "QLabel#headerConnection[connectionState='online'] { color: #17f49a; }"
    "QLabel#headerConnection[connectionState='stale'] { color: #ff8a2a; }"
    "QLabel#headerConnection[connectionState='offline'] { color: #f05268; }"
    "QLabel#headerClock { color: #c9e7e5; font-family: 'JetBrains Mono', 'Consolas', monospace; "
    "font-weight: 600; padding: 3px 8px; }"
    "QStatusBar { background-color: #04161c; color: #7faaa9; border: none; }"
    "QToolTip { background-color: #0d3540; color: #efffff; border: 1px solid #5de2d7; padding: "
    "5px; border-radius: 4px; }";

static void setupDashboardNavigation(MainWindow *window)
{
    auto *tabs = window->findChild<QTabWidget *>(QStringLiteral("tabWidget"));
    auto *contentLayout = window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_6"));
    auto *workspaceLayout = window->findChild<QVBoxLayout *>(QStringLiteral("verticalLayout_4"));
    auto *quickActions = window->findChild<QFrame *>(QStringLiteral("frame_quickActions"));
    if (!tabs || !contentLayout || !workspaceLayout || !quickActions)
        return;

    tabs->tabBar()->hide();
    tabs->setDocumentMode(true);

    auto *navigation = new HudPanel(window);
    navigation->setObjectName(QStringLiteral("dashboardNavigation"));
    navigation->setMinimumWidth(150);
    navigation->setMaximumWidth(180);

    auto *navigationLayout = new QVBoxLayout(navigation);
    navigationLayout->setContentsMargins(0, 8, 0, 8);
    navigationLayout->setSpacing(2);

    auto *navigationGroup = new QButtonGroup(navigation);
    navigationGroup->setExclusive(true);
    const QStringList labels = {QStringLiteral("▣  Камера"),     QStringLiteral("◉  БСО"),
                                QStringLiteral("▤  Сообщения"),  QStringLiteral("▥  Питание"),
                                QStringLiteral("⌘  Устройства"), QStringLiteral("◇  Отладка")};
    for (int index = 0; index < tabs->count() && index < labels.size(); ++index)
    {
        auto *button = new QPushButton(labels.at(index), navigation);
        button->setProperty("dashboardNav", true);
        button->setCheckable(true);
        button->setChecked(index == tabs->currentIndex());
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        navigationGroup->addButton(button, index);
        navigationLayout->addWidget(button);
    }
    navigationLayout->addStretch(1);

    QObject::connect(navigationGroup, QOverload<int>::of(&QButtonGroup::idClicked), tabs,
                     &QTabWidget::setCurrentIndex);
    QObject::connect(tabs, &QTabWidget::currentChanged, navigation,
                     [navigationGroup](int index)
                     {
                         if (QAbstractButton *button = navigationGroup->button(index))
                             button->setChecked(true);
                     });

    contentLayout->insertWidget(0, navigation);
    workspaceLayout->removeWidget(quickActions);
    workspaceLayout->addWidget(quickActions);
}

static void setupTelemetryCards(MainWindow *window)
{
    auto *telemetryLayout =
        window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_telemetry"));
    if (!telemetryLayout)
        return;

    struct TelemetryDescriptor
    {
        const char *titleName;
        const char *valueName;
        const char *iconText;
        int width;
    };

    const TelemetryDescriptor descriptors[] = {
        {"lbl_depth_title", "lbl_depth_value", "DEP", 160},
        {"lbl_bottom_title", "lbl_bottom_value", "ALT", 136},
        {"lbl_speed_title", "lbl_speed_value", "SPD", 175},
        {"lbl_temp_title", "lbl_temp_value", "TMP", 137},
        {"lbl_leak_title", "lbl_leak_value", "LEK", 141},
        {"lbl_voltage_title", "lbl_voltage_value", "B1", 129},
        {"lbl_voltage2_title", "lbl_voltage2_value", "B2", 130},
        {"lbl_kill_title", "btn_killswitch_status", "KIL", 163},
        {"lbl_ping_title", "lbl_ping_value", "PNG", 140},
    };

    while (QLayoutItem *item = telemetryLayout->takeAt(0))
    {
        if (QLayout *nestedLayout = item->layout())
        {
            while (QLayoutItem *nestedItem = nestedLayout->takeAt(0))
                delete nestedItem;
            delete nestedLayout;
        }
        else
        {
            delete item;
        }
    }

    telemetryLayout->setContentsMargins(0, 0, 0, 0);
    telemetryLayout->setSpacing(9);

    for (const TelemetryDescriptor &descriptor : descriptors)
    {
        auto *title = window->findChild<QLabel *>(QString::fromLatin1(descriptor.titleName));
        auto *value = window->findChild<QWidget *>(QString::fromLatin1(descriptor.valueName));
        if (!title || !value)
            continue;

        auto *card = new HudPanel(window);
        card->setProperty("telemetryCard", true);
        card->setAccentVisible(false);
        card->setFixedSize(descriptor.width, 64);

        auto *cardLayout = new QHBoxLayout(card);
        cardLayout->setContentsMargins(11, 8, 11, 8);
        cardLayout->setSpacing(9);

        auto *icon = new QLabel(QString::fromLatin1(descriptor.iconText), card);
        icon->setProperty("telemetryIcon", true);
        icon->setAlignment(Qt::AlignCenter);
        icon->setFixedSize(31, 31);

        auto *textLayout = new QVBoxLayout();
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(1);
        title->setProperty("telemetryTitle", true);
        value->setProperty("telemetryValue", true);
        value->setProperty("telemetryStale", true);
        title->setStyleSheet(QString());
        value->setStyleSheet(QString());
        textLayout->addWidget(title);
        textLayout->addWidget(value);

        cardLayout->addWidget(icon);
        cardLayout->addLayout(textLayout, 1);
        telemetryLayout->addWidget(card);
    }
    telemetryLayout->addStretch(1);
}

static QLabel *makePanelTitle(const QString &text, QWidget *parent)
{
    auto *title = new QLabel(text, parent);
    title->setProperty("dashboardPanelTitle", true);
    title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    return title;
}

static HudPanel *makeDashboardPanel(const QString &name, const QString &titleText,
                                    QVBoxLayout **contentLayout, QWidget *parent)
{
    auto *panel = new HudPanel(parent);
    panel->setObjectName(name);
    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(13, 10, 13, 12);
    layout->setSpacing(7);
    layout->addWidget(makePanelTitle(titleText, panel));
    *contentLayout = layout;
    return panel;
}

static QPushButton *makeMotionButton(const QString &text, int key, QWidget *window, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setProperty("motionPad", true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setFixedSize(38, 34);
    QObject::connect(button, &QPushButton::pressed, window,
                     [window, key]()
                     {
                         QKeyEvent event(QEvent::KeyPress, key, Qt::NoModifier);
                         QApplication::sendEvent(window, &event);
                     });
    QObject::connect(button, &QPushButton::released, window,
                     [window, key]()
                     {
                         QKeyEvent event(QEvent::KeyRelease, key, Qt::NoModifier);
                         QApplication::sendEvent(window, &event);
                     });
    return button;
}

static void setupDashboardGrid(MainWindow *window)
{
    auto *rootLayout = window->findChild<QVBoxLayout *>(QStringLiteral("mainWorkspaceLayout"));
    auto *centerLayout = window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_2"));
    auto *workspaceLayout = window->findChild<QVBoxLayout *>(QStringLiteral("verticalLayout_4"));
    auto *contentColumn = window->findChild<QVBoxLayout *>(QStringLiteral("verticalLayout_5"));
    auto *legacyBottomLayout =
        window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_4"));
    auto *controlStack = window->findChild<QStackedWidget *>(QStringLiteral("stackedWidget_mode"));
    auto *orientation = window->findChild<QGroupBox *>(QStringLiteral("groupBox_info"));
    auto *quickActions = window->findChild<QFrame *>(QStringLiteral("frame_quickActions"));
    auto *console = window->findChild<QTextEdit *>(QStringLiteral("textEdit_console"));
    if (!rootLayout || !centerLayout || !workspaceLayout || !contentColumn || !legacyBottomLayout ||
        !controlStack || !orientation || !quickActions || !console)
        return;

    contentColumn->removeItem(legacyBottomLayout);
    for (const QString &legacyLabel :
         {QStringLiteral("label_6"), QStringLiteral("label_8"), QStringLiteral("label_9")})
    {
        if (QWidget *widget = window->findChild<QWidget *>(legacyLabel))
            widget->hide();
    }
    workspaceLayout->removeWidget(quickActions);
    centerLayout->removeWidget(controlStack);
    if (orientation->parentWidget() && orientation->parentWidget()->layout())
        orientation->parentWidget()->layout()->removeWidget(orientation);

    auto *instrumentColumn = new QWidget(window);
    instrumentColumn->setObjectName(QStringLiteral("dashboardInstrumentColumn"));
    instrumentColumn->setMinimumWidth(430);
    auto *instrumentLayout = new QVBoxLayout(instrumentColumn);
    instrumentLayout->setContentsMargins(0, 0, 0, 0);
    instrumentLayout->setSpacing(10);

    QVBoxLayout *orientationLayout = nullptr;
    HudPanel *orientationPanel = makeDashboardPanel(QStringLiteral("orientationPanel"),
                                                    QStringLiteral("ОРИЕНТАЦИЯ И ПОЛОЖЕНИЕ"),
                                                    &orientationLayout, instrumentColumn);
    orientation->setTitle(QString());
    orientation->setStyleSheet(QStringLiteral(
        "QGroupBox { border: none; margin: 0; padding: 0; background: transparent; }"));
    if (auto *orientationContent =
            window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_info")))
    {
        auto *compass = window->findChild<QWidget *>(QStringLiteral("compass"));
        auto *attitude = window->findChild<QWidget *>(QStringLiteral("attitudeIndicator"));
        if (compass && attitude)
        {
            orientationContent->removeWidget(compass);
            orientationContent->removeWidget(attitude);
            auto *vehicle = new VehicleOrientationPreview(orientationPanel);
            orientationContent->insertWidget(0, vehicle, 3);
            orientationContent->addWidget(compass, 3);
            if (auto *debugLayout =
                    window->findChild<QVBoxLayout *>(QStringLiteral("verticalLayout_10")))
                debugLayout->addWidget(attitude);
        }
    }
    for (const QString &labelName :
         {QStringLiteral("lbl_heading"), QStringLiteral("lbl_pitch"), QStringLiteral("lbl_roll")})
    {
        if (auto *label = window->findChild<QLabel *>(labelName))
        {
            if (labelName == QStringLiteral("lbl_heading"))
                label->setText(QStringLiteral("HDG  —"));
            else if (labelName == QStringLiteral("lbl_pitch"))
                label->setText(QStringLiteral("PITCH  —"));
            else
                label->setText(QStringLiteral("ROLL  —"));
            label->setProperty("orientationMetric", true);
            label->setStyleSheet(QString());
            label->setAlignment(Qt::AlignCenter);
            label->setMinimumWidth(88);
        }
    }
    orientationLayout->addWidget(orientation, 1);

    QVBoxLayout *depthLayout = nullptr;
    HudPanel *depthPanel =
        makeDashboardPanel(QStringLiteral("depthSonarPanel"),
                           QStringLiteral("ГЛУБИНА И РЕЛЬЕФ ДНА"), &depthLayout, instrumentColumn);
    depthLayout->addWidget(new DepthSonarPreview(depthPanel), 1);

    instrumentLayout->addWidget(orientationPanel, 1);
    instrumentLayout->addWidget(depthPanel, 1);
    centerLayout->addWidget(instrumentColumn, 35);
    centerLayout->setStretch(0, 65);
    centerLayout->setStretch(1, 35);

    auto *bottomRow = new QWidget(window);
    bottomRow->setObjectName(QStringLiteral("dashboardBottomRow"));
    bottomRow->setMinimumHeight(245);
    bottomRow->setMaximumHeight(310);
    auto *bottomLayout = new QHBoxLayout(bottomRow);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->setSpacing(10);

    quickActions->setMinimumWidth(330);
    quickActions->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (auto *manipulatorLayout =
            window->findChild<QHBoxLayout *>(QStringLiteral("horizontalLayout_manipulator")))
    {
        auto *manipulatorTitle = window->findChild<QLabel *>(QStringLiteral("lbl_manip_title"));
        const QList<QPushButton *> manipulatorButtons = {
            window->findChild<QPushButton *>(QStringLiteral("btn_manip_compress")),
            window->findChild<QPushButton *>(QStringLiteral("btn_manip_release")),
            window->findChild<QPushButton *>(QStringLiteral("btn_manip_rotate_ccw")),
            window->findChild<QPushButton *>(QStringLiteral("btn_manip_rotate_cw")),
            window->findChild<QPushButton *>(QStringLiteral("btn_manip_stop"))};
        while (QLayoutItem *item = manipulatorLayout->takeAt(0))
            delete item;
        manipulatorLayout->setDirection(QBoxLayout::TopToBottom);
        manipulatorLayout->setContentsMargins(13, 10, 13, 12);
        manipulatorLayout->setSpacing(7);

        manipulatorTitle->setText(QStringLiteral("МАНИПУЛЯТОР"));
        manipulatorTitle->setProperty("dashboardPanelTitle", true);
        manipulatorTitle->setStyleSheet(QString());
        manipulatorLayout->addWidget(manipulatorTitle);

        auto *buttonRow = new QHBoxLayout();
        buttonRow->setSpacing(6);
        for (QPushButton *button : manipulatorButtons)
        {
            if (!button)
                continue;
            button->setMinimumSize(0, 34);
            button->setMaximumWidth(QWIDGETSIZE_MAX);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            buttonRow->addWidget(button);
        }
        manipulatorLayout->addLayout(buttonRow);

        auto *body = new QHBoxLayout();
        body->setSpacing(10);
        body->addWidget(new ManipulatorPreview(quickActions), 2);
        auto *metrics = new QVBoxLayout();
        metrics->setSpacing(3);
        const auto addMetric = [metrics, quickActions](const QString &name)
        {
            auto *row = new QWidget(quickActions);
            row->setProperty("manipulatorMetric", true);
            auto *rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(5, 3, 5, 3);
            auto *label = new QLabel(name, row);
            label->setProperty("metricLabel", true);
            auto *value = new QLabel(QStringLiteral("—"), row);
            value->setProperty("metricValue", true);
            rowLayout->addWidget(label);
            rowLayout->addStretch();
            rowLayout->addWidget(value);
            metrics->addWidget(row);
        };
        addMetric(QStringLiteral("Угол"));
        addMetric(QStringLiteral("Вращение"));
        addMetric(QStringLiteral("Раскрытие"));
        for (int index = 0; index < 2; ++index)
        {
            auto *state = new QProgressBar(quickActions);
            state->setRange(0, 100);
            state->setValue(0);
            state->setTextVisible(false);
            state->setFixedHeight(7);
            state->setProperty("mechanismState", true);
            metrics->addWidget(state);
        }
        metrics->addStretch();
        body->addLayout(metrics, 1);
        manipulatorLayout->addLayout(body, 1);
    }

    QVBoxLayout *controlLayout = nullptr;
    HudPanel *controlPanel =
        makeDashboardPanel(QStringLiteral("movementControlPanel"),
                           QStringLiteral("УПРАВЛЕНИЕ ДВИЖЕНИЕМ"), &controlLayout, bottomRow);

    auto *manualMode = window->findChild<QPushButton *>(QStringLiteral("pushButton_modeManual"));
    auto *stabilizationMode =
        window->findChild<QPushButton *>(QStringLiteral("pushButton_modeAutomated"));
    auto *automaticMode =
        window->findChild<QPushButton *>(QStringLiteral("pushButton_modeAutomatic"));
    auto *modeRow = new QHBoxLayout();
    modeRow->setSpacing(7);
    if (manualMode && stabilizationMode && automaticMode)
    {
        manualMode->setText(QStringLiteral("Ручной"));
        stabilizationMode->setText(QStringLiteral("Стабилизация"));
        automaticMode->setText(QStringLiteral("Автоматический"));
        for (QPushButton *button : {manualMode, stabilizationMode, automaticMode})
        {
            button->setProperty("primaryMode", true);
            button->setMinimumSize(0, 32);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            modeRow->addWidget(button);
        }
    }
    controlLayout->addLayout(modeRow);

    auto *movementBody = new QHBoxLayout();
    movementBody->setSpacing(10);
    auto *speedColumn = new QVBoxLayout();
    speedColumn->setSpacing(3);
    auto *speedTitle = new QLabel(QStringLiteral("СКОРОСТЬ"), controlPanel);
    speedTitle->setProperty("controlSectionTitle", true);
    speedColumn->addWidget(speedTitle);
    const QList<QPair<QString, QString>> speedButtons = {
        {QStringLiteral("pushButton_speedSlow"), QStringLiteral("Медленно")},
        {QStringLiteral("pushButton_speedMedium"), QStringLiteral("Средняя")},
        {QStringLiteral("pushButton_speedFast"), QStringLiteral("Быстро")}};
    for (const auto &descriptor : speedButtons)
    {
        if (auto *button = window->findChild<QPushButton *>(descriptor.first))
        {
            button->setText(descriptor.second);
            button->setProperty("speedChoice", true);
            button->setMinimumSize(86, 27);
            speedColumn->addWidget(button);
        }
    }
    speedColumn->addStretch();
    movementBody->addLayout(speedColumn, 25);

    auto *motionPad = new QGridLayout();
    motionPad->setSpacing(3);
    auto *forward = makeMotionButton(QStringLiteral("▲"), Qt::Key_O, window, controlPanel);
    auto *backward = makeMotionButton(QStringLiteral("▼"), Qt::Key_L, window, controlPanel);
    auto *left = makeMotionButton(QStringLiteral("◀"), Qt::Key_A, window, controlPanel);
    auto *right = makeMotionButton(QStringLiteral("▶"), Qt::Key_D, window, controlPanel);
    auto *stop = new QPushButton(QStringLiteral("■"), controlPanel);
    stop->setProperty("motionPad", true);
    stop->setFocusPolicy(Qt::NoFocus);
    stop->setFixedSize(38, 34);
    QObject::connect(stop, &QPushButton::clicked, window,
                     [window]()
                     {
                         for (int key : {Qt::Key_O, Qt::Key_L, Qt::Key_A, Qt::Key_D})
                         {
                             QKeyEvent event(QEvent::KeyRelease, key, Qt::NoModifier);
                             QApplication::sendEvent(window, &event);
                         }
                     });
    motionPad->addWidget(forward, 0, 1);
    motionPad->addWidget(left, 1, 0);
    motionPad->addWidget(stop, 1, 1);
    motionPad->addWidget(right, 1, 2);
    motionPad->addWidget(backward, 2, 1);
    movementBody->addLayout(motionPad, 35);

    auto *depthColumn = new QVBoxLayout();
    depthColumn->setSpacing(4);
    auto *depthTitle = new QLabel(QStringLiteral("ГЛУБИНА"), controlPanel);
    depthTitle->setProperty("controlSectionTitle", true);
    depthColumn->addWidget(depthTitle);
    auto *up = makeMotionButton(QStringLiteral("Вверх  ▲"), Qt::Key_V, window, controlPanel);
    auto *down = makeMotionButton(QStringLiteral("Вниз  ▼"), Qt::Key_C, window, controlPanel);
    for (QPushButton *button : {up, down})
    {
        button->setFixedHeight(35);
        button->setMaximumWidth(QWIDGETSIZE_MAX);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        depthColumn->addWidget(button);
    }
    if (auto *depthValue = window->findChild<QLabel *>(QStringLiteral("label_controlDepth")))
    {
        depthValue->setProperty("depthCommand", true);
        depthValue->setAlignment(Qt::AlignCenter);
        depthColumn->addWidget(depthValue);
    }
    depthColumn->addStretch();
    movementBody->addLayout(depthColumn, 28);
    controlLayout->addLayout(movementBody, 1);

    auto *axisRow = new QHBoxLayout();
    axisRow->setSpacing(4);
    const QStringList axisNames = {QStringLiteral("pushButton_modeAutomated_surge"),
                                   QStringLiteral("pushButton_modeAutomated_sway"),
                                   QStringLiteral("pushButton_modeAutomated_depth"),
                                   QStringLiteral("pushButton_modeAutomated_yaw"),
                                   QStringLiteral("pushButton_modeAutomated_pitch"),
                                   QStringLiteral("pushButton_modeAutomated_roll")};
    for (const QString &name : axisNames)
    {
        if (auto *button = window->findChild<QPushButton *>(name))
        {
            button->setProperty("axisMode", true);
            button->setMinimumSize(0, 29);
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            axisRow->addWidget(button);
        }
    }
    controlLayout->addLayout(axisRow);

    controlStack->setMinimumWidth(0);
    controlStack->setMaximumWidth(QWIDGETSIZE_MAX);
    controlStack->setStyleSheet(QString());
    if (auto *debugLayout = window->findChild<QVBoxLayout *>(QStringLiteral("verticalLayout_10")))
        debugLayout->addWidget(controlStack);

    QVBoxLayout *eventsLayout = nullptr;
    HudPanel *eventsPanel =
        makeDashboardPanel(QStringLiteral("eventLogPanel"), QStringLiteral("ЖУРНАЛ СОБЫТИЙ"),
                           &eventsLayout, bottomRow);
    if (QLayoutItem *titleItem = eventsLayout->takeAt(0))
    {
        auto *header = new QHBoxLayout();
        header->setContentsMargins(0, 0, 0, 0);
        header->setSpacing(7);
        if (QWidget *title = titleItem->widget())
            header->addWidget(title);
        header->addStretch();

        auto *filter = new QComboBox(eventsPanel);
        filter->setObjectName(QStringLiteral("eventLogFilter"));
        filter->setProperty("eventLogFilter", true);
        filter->addItem(QStringLiteral("Все сообщения"), 0);
        filter->addItem(QStringLiteral("Информация"), EventInfo);
        filter->addItem(QStringLiteral("Успех"), EventSuccess);
        filter->addItem(QStringLiteral("Предупреждения"), EventWarning);
        filter->addItem(QStringLiteral("Ошибки"), EventError);
        filter->setMinimumWidth(112);
        header->addWidget(filter);
        eventsLayout->insertLayout(0, header);
        delete titleItem;

        QObject::connect(filter, QOverload<int>::of(&QComboBox::currentIndexChanged), console,
                         [filter, console]()
                         {
                             const int selectedCategory = filter->currentData().toInt();
                             QTextBlock block = console->document()->firstBlock();
                             while (block.isValid())
                             {
                                 block.setVisible(selectedCategory == 0 ||
                                                  block.userState() == selectedCategory);
                                 block = block.next();
                             }
                             console->document()->markContentsDirty(
                                 0, console->document()->characterCount());
                             console->viewport()->update();
                         });
    }
    console->setMaximumHeight(QWIDGETSIZE_MAX);
    console->setLineWrapMode(QTextEdit::NoWrap);
    eventsLayout->addWidget(console, 1);

    QVBoxLayout *mapLayout = nullptr;
    HudPanel *mapPanel = makeDashboardPanel(
        QStringLiteral("positionPanel"), QStringLiteral("КАРТА И ПОЗИЦИЯ"), &mapLayout, bottomRow);
    mapLayout->addWidget(new BathymetryMapPreview(mapPanel), 1);
    auto *coordinateRow = new QGridLayout();
    coordinateRow->setHorizontalSpacing(5);
    coordinateRow->setVerticalSpacing(2);
    auto *latitude = window->findChild<QLineEdit *>(QStringLiteral("lineEdit_reper_latitude"));
    auto *longitude = window->findChild<QLineEdit *>(QStringLiteral("lineEdit_reper_longitude"));
    if (latitude && longitude)
    {
        latitude->setText(QString::number(latitude->text().toDouble(), 'f', 6));
        longitude->setText(QString::number(longitude->text().toDouble(), 'f', 6));
        auto *latitudeLabel = new QLabel(QStringLiteral("Широта"), mapPanel);
        auto *longitudeLabel = new QLabel(QStringLiteral("Долгота"), mapPanel);
        latitudeLabel->setProperty("coordinateLabel", true);
        longitudeLabel->setProperty("coordinateLabel", true);
        coordinateRow->addWidget(latitudeLabel, 0, 0);
        coordinateRow->addWidget(longitudeLabel, 0, 1);
        coordinateRow->addWidget(latitude, 1, 0);
        coordinateRow->addWidget(longitude, 1, 1);
    }
    mapLayout->addLayout(coordinateRow);

    auto *mapActions = new QHBoxLayout();
    mapActions->setSpacing(5);
    if (auto *pick =
            window->findChild<QPushButton *>(QStringLiteral("pushButton_sendReper_map_onoff")))
    {
        pick->setText(QStringLiteral("Выбрать"));
        mapActions->addWidget(pick);
    }
    if (auto *send = window->findChild<QPushButton *>(QStringLiteral("pushButton_sendReper")))
    {
        send->setText(QStringLiteral("Установить"));
        mapActions->addWidget(send);
    }
    mapLayout->addLayout(mapActions);

    bottomLayout->addWidget(quickActions, 27);
    bottomLayout->addWidget(controlPanel, 29);
    bottomLayout->addWidget(eventsPanel, 25);
    bottomLayout->addWidget(mapPanel, 19);
    rootLayout->addWidget(bottomRow);
    rootLayout->setStretch(1, 5);
    rootLayout->setStretch(2, 3);
}

static void setDynamicProperty(QWidget *widget, const char *name, const QVariant &value)
{
    if (!widget || widget->property(name) == value)
        return;
    widget->setProperty(name, value);
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("Hydronautics • UMAS Control Station"));
    harmonizeLegacyStyles();
    setupBrandHeader();
    setupDashboardNavigation(this);
    setupTelemetryCards(this);
    setupDashboardGrid(this);
    ui->btn_theme_toggle->hide();

    qRegisterMetaType<UVState::ImuData>("UVState::ImuData");
    qRegisterMetaType<UVState::Pose>("UVState::Pose");
    qRegisterMetaType<UVState::BatteryData>("UVState::BatteryData");

    uvState = new UVState(this);
    if (auto *mapPreview = findChild<BathymetryMapPreview *>(QStringLiteral("bathymetryMap")))
    {
        connect(uvState, &UVState::poseUpdated, mapPreview,
                [mapPreview](const UVState::Pose &pose)
                { mapPreview->setPosition(pose.x, pose.y); });
    }
    if (auto *depthPreview = findChild<DepthSonarPreview *>(QStringLiteral("depthSonarPreview")))
    {
        connect(uvState, &UVState::poseUpdated, depthPreview,
                [this, depthPreview](const UVState::Pose &pose)
                {
                    depthPreview->setPose(pose.z, pose.velocity_z);
                    const double speed = std::sqrt(pose.velocity_x * pose.velocity_x +
                                                   pose.velocity_y * pose.velocity_y +
                                                   pose.velocity_z * pose.velocity_z);
                    ui->lbl_depth_value->setText(
                        QStringLiteral("%1 м").arg(std::abs(pose.z), 0, 'f', 1));
                    ui->lbl_speed_value->setText(QStringLiteral("%1 м/с").arg(speed, 0, 'f', 2));
                });
    }
    if (auto *vehicle =
            findChild<VehicleOrientationPreview *>(QStringLiteral("vehicleOrientationPreview")))
    {
        connect(uvState, &UVState::imuUpdated, vehicle,
                [vehicle](const UVState::ImuData &imu)
                { vehicle->setAttitude(imu.yaw, imu.pitch, imu.roll); });
    }

    rosBridge = new RosBridge(this);

    struct ConnectionFreshness
    {
        bool rosReady = false;
        qint64 poseMs = 0;
        qint64 imuMs = 0;
        qint64 batteryMs = 0;
        QString reportedState;
    };
    const auto freshness = std::make_shared<ConnectionFreshness>();
    auto *headerConnection = findChild<QLabel *>(QStringLiteral("headerConnection"));
    connect(rosBridge, &RosBridge::readyChanged, this,
            [freshness](bool ready) { freshness->rosReady = ready; });
    connect(rosBridge, &RosBridge::poseReceived, this,
            [freshness](const UVState::Pose &)
            { freshness->poseMs = QDateTime::currentMSecsSinceEpoch(); });
    connect(rosBridge, &RosBridge::imuReceived, this,
            [freshness](const UVState::ImuData &)
            { freshness->imuMs = QDateTime::currentMSecsSinceEpoch(); });
    connect(rosBridge, &RosBridge::batteryReceived, this,
            [freshness](const UVState::BatteryData &)
            { freshness->batteryMs = QDateTime::currentMSecsSinceEpoch(); });

    auto *connectionWatchdog = new QTimer(this);
    connectionWatchdog->setInterval(500);
    connect(
        connectionWatchdog, &QTimer::timeout, this,
        [this, freshness, headerConnection]()
        {
            constexpr qint64 staleAfterMs = 2500;
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const qint64 newestTelemetry =
                qMax(freshness->poseMs, qMax(freshness->imuMs, freshness->batteryMs));
            QString state;
            QString text;
            if (!freshness->rosReady)
            {
                state = QStringLiteral("offline");
                text = QStringLiteral("ROS OFFLINE");
            }
            else if (newestTelemetry == 0)
            {
                state = QStringLiteral("waiting");
                text = QStringLiteral("НЕТ ТЕЛЕМЕТРИИ");
            }
            else if (now - newestTelemetry > staleAfterMs)
            {
                state = QStringLiteral("stale");
                text = QStringLiteral("ДАННЫЕ · %1с").arg((now - newestTelemetry) / 1000);
            }
            else
            {
                state = QStringLiteral("online");
                text = QStringLiteral("ROS ONLINE");
            }
            if (headerConnection)
            {
                headerConnection->setText(text);
                setDynamicProperty(headerConnection, "connectionState", state);

                const auto ageText = [now](qint64 timestamp)
                {
                    if (timestamp == 0)
                        return QStringLiteral("нет данных");
                    return QStringLiteral("%1 с назад")
                        .arg(static_cast<double>(now - timestamp) / 1000.0, 0, 'f', 1);
                };
                headerConnection->setToolTip(
                    QStringLiteral("Состояние ROS 2 · DOMAIN %1\nПоложение: %2\nIMU: %3\n"
                                   "Питание: %4")
                        .arg(qEnvironmentVariable("ROS_DOMAIN_ID", "—"), ageText(freshness->poseMs),
                             ageText(freshness->imuMs), ageText(freshness->batteryMs)));
            }

            if (freshness->reportedState != state)
            {
                freshness->reportedState = state;
                setCommandLinkAvailable(state == QStringLiteral("online"));
                if (state == QStringLiteral("online"))
                    displayText(QStringLiteral("Соединение с аппаратом установлено"));
                else if (state == QStringLiteral("stale"))
                    displayText(QStringLiteral("Предупреждение: телеметрия аппарата устарела"));
                else if (state == QStringLiteral("waiting"))
                    displayText(QStringLiteral("ROS 2 готов, ожидание телеметрии аппарата"));
                else
                    displayText(QStringLiteral("Ошибка: соединение с ROS 2 недоступно"));
            }

            const bool poseStale = freshness->poseMs == 0 || now - freshness->poseMs > staleAfterMs;
            setDynamicProperty(ui->lbl_depth_value, "telemetryStale", poseStale);
            setDynamicProperty(ui->lbl_speed_value, "telemetryStale", poseStale);
            const bool imuStale = freshness->imuMs == 0 || now - freshness->imuMs > staleAfterMs;
            setDynamicProperty(ui->lbl_heading, "telemetryStale", imuStale);
            setDynamicProperty(ui->lbl_pitch, "telemetryStale", imuStale);
            setDynamicProperty(ui->lbl_roll, "telemetryStale", imuStale);
            const bool batteryStale =
                freshness->batteryMs == 0 || now - freshness->batteryMs > staleAfterMs;
            setDynamicProperty(ui->lbl_voltage_value, "telemetryStale", batteryStale);
            setDynamicProperty(ui->lbl_voltage2_value, "telemetryStale", batteryStale);
        });
    connectionWatchdog->start();

    connect(this, &MainWindow::publishTwistRequested, rosBridge, &RosBridge::publishTwistInternal,
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::poseReceived, uvState,
            static_cast<void (UVState::*)(const UVState::Pose &)>(&UVState::setPose),
            Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::imuReceived, uvState, &UVState::setImu, Qt::QueuedConnection);

    connect(rosBridge, &RosBridge::batteryReceived, uvState, &UVState::setBattery,
            Qt::QueuedConnection);
    connect(
        uvState, &UVState::batteryUpdated, this,
        [this](const UVState::BatteryData &battery)
        {
            const auto percentageText = [](float percentage, float voltage, std::uint64_t timestamp)
            {
                if (timestamp == 0)
                    return QStringLiteral("—");
                if (percentage >= 0.0f)
                {
                    const float normalized = percentage <= 1.0f ? percentage * 100.0f : percentage;
                    return QStringLiteral("%1%").arg(qBound(0, qRound(normalized), 100));
                }
                if (voltage > 0.0f)
                    return QStringLiteral("%1 В").arg(voltage, 0, 'f', 1);
                return QStringLiteral("—");
            };
            ui->lbl_voltage_value->setText(
                percentageText(battery.percentage_1, battery.voltage_1, battery.timestamp_1_ms));
            ui->lbl_voltage2_value->setText(
                percentageText(battery.percentage_2, battery.voltage_2, battery.timestamp_2_ms));
        });

    connect(
        rosBridge, &RosBridge::dvlOriginResetFinished, this,
        [this](bool success, const QString &message)
        {
            ui->pushButton_resetDvlOrigin->setEnabled(true);
            displayText((success ? "DVL: " : "Ошибка DVL: ") + message);
        },
        Qt::QueuedConnection);

    connect(uvState, &UVState::imuUpdated, this,
            [this](const UVState::ImuData &imu)
            {
                ui->compass->setYaw(imu.yaw);
                ui->attitudeIndicator->setAttitude(imu.pitch, imu.roll);
                ui->lbl_heading->setText(QStringLiteral("HDG  %1°")
                                             .arg(static_cast<int>(qRound(imu.yaw + 360.0f)) % 360,
                                                  3, 10, QLatin1Char('0')));
                ui->lbl_pitch->setText(QStringLiteral("PITCH  %1°").arg(imu.pitch, 0, 'f', 1));
                ui->lbl_roll->setText(QStringLiteral("ROLL  %1°").arg(imu.roll, 0, 'f', 1));
            });

    connect(rosBridge, &RosBridge::controlFlagsPublished, uvState, &UVState::setControlFlags,
            Qt::QueuedConnection);

    rosBridge->start();

    setWidget();
    setConsole();
    setTimer_updateImpact(10);
    setBottom();

    darkStyle_ = DARK_THEME;

    ui->btn_theme_toggle->setText(QString());
    ui->btn_theme_toggle->setIconSize(QSize(22, 22));

    connect(ui->btn_theme_toggle, &QPushButton::clicked, this, &MainWindow::toggleTheme);

    setTab();
    setupActuators();

    brightnessTimer_ = new QTimer(this);
    brightnessTimer_->setInterval(100); // каждые 100 мс
    connect(brightnessTimer_, &QTimer::timeout, this,
            [this]()
            {
                if (brightnessStep_ == 0)
                    return;
                int current = ui->slider_brightness->value();
                int newValue = qBound(0, current + brightnessStep_, 500);
                ui->slider_brightness->setValue(newValue);
            });

    setUpdateUI();

    gainSpinBoxes = {ui->spinBox_gain_surge, ui->spinBox_gain_sway,  ui->spinBox_gain_depth,
                     ui->spinBox_gain_yaw,   ui->spinBox_gain_pitch, ui->spinBox_gain_roll};
    qApp->installEventFilter(this);

    loadSettings();
    setSpinBoxValuesForCurrentMode();

    for (auto spinBox : gainSpinBoxes)
    {
        connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
                [this]() { saveCurrentModeGains(); });
    }

    setSpeedMode(SpeedMode::Medium);

    QButtonGroup *inputGroup = new QButtonGroup(this);
    inputGroup->addButton(ui->radioButton_useJoyStick);
    inputGroup->addButton(ui->radioButton_useKeyBoard);
    inputGroup->addButton(ui->gamepad_btn);
    inputGroup->setExclusive(true);

    // Применяем тему после создания всех групп и настройки виджетов. Иначе
    // радиокнопки выбора источника управления впервые получают актуальный
    // стиль только после ручного переключения темы.
    applyTheme(isDark_);

    connect(ui->full_screen, &QPushButton::toggled, this,
            [this](bool checked)
            {
                if (checked)
                {
                    if (!fullscreenWindow_ && videoPlayer_)
                    {
                        //  полноэкранное окно
                        fullscreenWindow_ = new FullscreenVideoWindow(uvState, this);

                        //  плеер в полноэкранное окно
                        videoPlayer_->setParent(fullscreenWindow_);
                        fullscreenWindow_->setVideoWidget(videoPlayer_);
                        videoPlayer_->show();

                        //сигнал закрытия окна
                        connect(fullscreenWindow_, &FullscreenVideoWindow::windowClosed, this,
                                [this]()
                                {
                                    FullscreenVideoWindow *closedWindow = fullscreenWindow_;

                                    ui->full_screen->blockSignals(true);
                                    ui->full_screen->setChecked(false);
                                    ui->full_screen->blockSignals(false);

                                    //  плеер на место
                                    if (videoPlayer_)
                                    {
                                        videoPlayer_->setParent(ui->tab_video);
                                        ui->tab_video->layout()->addWidget(videoPlayer_);
                                        videoPlayer_->show();
                                        // videoPlayer_->updateVideoWindow();
                                    }

                                    fullscreenWindow_ = nullptr;
                                    if (closedWindow)
                                        closedWindow->deleteLater();
                                });

                        fullscreenWindow_->showFullScreen();
                    }
                }
                else
                {
                    if (fullscreenWindow_)
                    {
                        fullscreenWindow_->close();
                    }
                }
            });

    // Запуск видео-потока напрямую через GStreamer
    QTimer::singleShot(500, this,
                       [this]()
                       {
                           if (videoPlayer_)
                           {
                               videoPlayer_->startStream("5000");
                           }
                       });
}

void MainWindow::setupBrandHeader()
{
    menuBar()->setFixedHeight(58);

    auto *brand = new QWidget(menuBar());
    brand->setObjectName(QStringLiteral("brandHeader"));
    auto *layout = new QHBoxLayout(brand);
    layout->setContentsMargins(14, 6, 18, 6);
    layout->setSpacing(11);

    auto *logo = new QLabel(brand);
    logo->setObjectName(QStringLiteral("brandLogo"));
    logo->setFixedSize(44, 44);
    logo->setAlignment(Qt::AlignCenter);
    QPixmap brandPixmap(QStringLiteral(":/branding/hydronautics_logo.png"));
    if (brandPixmap.isNull())
    {
        brandPixmap = QPixmap(40, 40);
        brandPixmap.fill(Qt::transparent);
        QPainter painter(&brandPixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(QColor(QStringLiteral("#20d9e6")), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRectF(5, 8, 30, 22), 205 * 16, 130 * 16);
        painter.drawLine(QPointF(10, 25), QPointF(18, 32));
        painter.drawLine(QPointF(30, 25), QPointF(22, 32));
        painter.drawEllipse(QPointF(20, 17), 4, 4);
    }
    logo->setPixmap(brandPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setStyleSheet(QStringLiteral("background: transparent; border: none;"));

    auto *title = new QLabel(
        QStringLiteral(
            "<span style='font-size:16px; font-weight:700; letter-spacing:1px; "
            "color:#74eee3;'>HYDRONAUTICS</span>"
            "<br><span style='font-size:10px; color:#7faaa9;'>UMAS CONTROL STATION</span>"),
        brand);
    title->setObjectName(QStringLiteral("brandTitle"));
    title->setTextFormat(Qt::RichText);

    layout->addWidget(logo);
    layout->addWidget(title);
    menuBar()->setCornerWidget(brand, Qt::TopLeftCorner);

    ui->horizontalLayout_telemetry->removeWidget(ui->btn_theme_toggle);
    ui->horizontalLayout_manipulator->removeWidget(ui->full_screen);

    auto *headerActions = new QWidget(menuBar());
    auto *actionsLayout = new QHBoxLayout(headerActions);
    actionsLayout->setContentsMargins(8, 5, 14, 5);
    actionsLayout->setSpacing(10);

    auto *connection = new QLabel(QStringLiteral("ROS ЗАПУСКАЕТСЯ"), headerActions);
    connection->setObjectName(QStringLiteral("headerConnection"));
    connection->setProperty("connectionState", QStringLiteral("waiting"));
    connection->setToolTip(QStringLiteral("Состояние ROS 2 · DOMAIN %1")
                               .arg(qEnvironmentVariable("ROS_DOMAIN_ID", "—")));

    auto *clock = new QLabel(headerActions);
    clock->setObjectName(QStringLiteral("headerClock"));
    clock->setAlignment(Qt::AlignCenter);
    auto updateClock = [clock]()
    {
        clock->setText(QDateTime::currentDateTime()
                           .toString(QStringLiteral("dd MMM yyyy\nHH:mm:ss"))
                           .toUpper());
    };
    updateClock();
    auto *clockTimer = new QTimer(headerActions);
    connect(clockTimer, &QTimer::timeout, headerActions, updateClock);
    clockTimer->start(1000);

    actionsLayout->addWidget(connection);
    actionsLayout->addWidget(ui->btn_theme_toggle);
    ui->full_screen->setFixedWidth(120);
    actionsLayout->addWidget(ui->full_screen);
    actionsLayout->addWidget(clock);
    menuBar()->setCornerWidget(headerActions, Qt::TopRightCorner);
}

void MainWindow::harmonizeLegacyStyles()
{
    // Приводим оставшиеся стили из .ui к общей палитре, пока старые формы
    // постепенно переводятся на централизованную тему.
    const QList<QPair<QString, QString>> palette = {
        {"#0d1b2a", "#061a21"}, {"#151b23", "#09262e"}, {"#1e2a38", "#0d3540"},
        {"#2d4052", "#185464"}, {"#00bcd4", "#20c7b8"}, {"#00ff88", "#74eee3"},
        {"#0f1419", "#031416"}, {"#8a99a8", "#7faaa9"}, {"#e0e6ed", "#d7f3f1"},
        {"#0a0e13", "#04161c"}, {"#4ade80", "#7de5d5"}, {"#4a5568", "#55747a"},
        {"#ff4444", "#a3263d"}};

    const auto widgets = findChildren<QWidget *>();
    for (QWidget *widget : widgets)
    {
        QString style = widget->styleSheet();
        if (!style.isEmpty())
        {
            for (const auto &colors : palette)
                style.replace(colors.first, colors.second, Qt::CaseInsensitive);
            widget->setStyleSheet(style);
        }
    }

    for (QWidget *widget :
         {static_cast<QWidget *>(ui->lbl_depth_value), static_cast<QWidget *>(ui->lbl_bottom_value),
          static_cast<QWidget *>(ui->lbl_speed_value), static_cast<QWidget *>(ui->lbl_temp_value),
          static_cast<QWidget *>(ui->lbl_leak_value), static_cast<QWidget *>(ui->lbl_voltage_value),
          static_cast<QWidget *>(ui->lbl_voltage2_value),
          static_cast<QWidget *>(ui->btn_killswitch_status),
          static_cast<QWidget *>(ui->lbl_ping_value)})
        widget->setProperty("hudValue", true);

    for (QLabel *label : {ui->label_controlMarch, ui->label_controlYaw, ui->label_controlLag,
                          ui->label_controlDif, ui->label_controlDepth, ui->label_controlKren})
        label->setProperty("controlValue", true);

    // Панели и интервалы уже формируют визуальную иерархию — дополнительные
    // горизонтальные линии только дробят компактный операторский интерфейс.
    for (QFrame *separator : {ui->line_separator1, ui->line_separator_light, ui->line_separator2})
        separator->hide();

    ui->horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
    ui->horizontalLayout_2->setSpacing(12);
    ui->horizontalLayout_telemetry->setSpacing(8);
    ui->verticalLayout_4->setSpacing(8);
    ui->horizontalLayout_manipulator->setSpacing(8);
    ui->textEdit_console->setPlaceholderText(QStringLiteral("Журнал событий пульта"));
}

void MainWindow::ensureInteractiveTextFits()
{
    const auto buttons = findChildren<QPushButton *>();
    for (QPushButton *button : buttons)
    {
        if (button->text().isEmpty() || button == ui->btn_theme_toggle ||
            button == ui->btn_killswitch_status)
            continue;

        const QFontMetrics metrics(button->font());
        const int textWidth = metrics.horizontalAdvance(button->text());
        const int safeWidth = qMin(260, textWidth + 34);
        const int safeHeight = metrics.height() + 18;
        button->setMinimumWidth(qMax(button->minimumWidth(), safeWidth));
        button->setMinimumHeight(qMax(button->minimumHeight(), safeHeight));
    }

    const QFontMetrics fullScreenMetrics(ui->full_screen->font());
    ui->full_screen->setMinimumWidth(fullScreenMetrics.horizontalAdvance(ui->full_screen->text()) +
                                     54);
    ui->full_screen->setMinimumHeight(fullScreenMetrics.height() + 18);

    QTabBar *tabs = ui->tabWidget->tabBar();
    QFont tabFont = tabs->font();
    tabFont.setWeight(QFont::DemiBold);
    tabs->setFont(tabFont);
    tabs->setExpanding(false);
    tabs->setUsesScrollButtons(true);
    const QFontMetrics tabMetrics(tabFont);
    int requiredTabWidth = 0;
    for (int index = 0; index < tabs->count(); ++index)
        requiredTabWidth += tabMetrics.horizontalAdvance(tabs->tabText(index)) + 42;
    tabs->setMinimumWidth(requiredTabWidth);
}

void MainWindow::setWidget()
{
    videoPlayer_ = new VideoPlayerWidget(this);
    connect(videoPlayer_, &VideoPlayerWidget::fullscreenRequested, ui->full_screen,
            [this]() { ui->full_screen->setChecked(true); });

    if (!ui->tab_video->layout())
    {
        ui->tab_video->setLayout(new QVBoxLayout(ui->tab_video));
    }

    ui->tab_video->layout()->addWidget(videoPlayer_);
}

void MainWindow::setConsole()
{
    displayText("Приложение работает");
    displayText("Установите соединение для работы с агентом");
}

void MainWindow::displayText(QString str)
{
    const QString currentTime = QTime::currentTime().toString(QStringLiteral("HH:mm:ss"));
    const EventCategory category = classifyEventMessage(str);
    qInfo() << currentTime << str;

    QTextCursor cursor(ui->textEdit_console->document());
    cursor.movePosition(QTextCursor::End);
    if (!ui->textEdit_console->document()->isEmpty())
        cursor.insertBlock();
    const int firstPosition = cursor.position();

    QTextCharFormat timeFormat;
    timeFormat.setForeground(QColor(QStringLiteral("#6fb7d2")));
    timeFormat.setFontFamily(QStringLiteral("IBM Plex Mono"));
    timeFormat.setFontFixedPitch(true);
    cursor.insertText(currentTime + QStringLiteral("  "), timeFormat);

    QTextCharFormat messageFormat;
    messageFormat.setForeground(eventCategoryColor(category));
    cursor.insertText(str, messageFormat);

    QTextBlock block = ui->textEdit_console->document()->findBlock(firstPosition);
    const int lastPosition = cursor.position();
    while (block.isValid() && block.position() <= lastPosition)
    {
        block.setUserState(category);
        block = block.next();
    }

    if (auto *filter = findChild<QComboBox *>(QStringLiteral("eventLogFilter")))
    {
        const int selectedCategory = filter->currentData().toInt();
        if (selectedCategory != 0 && selectedCategory != category)
        {
            block = ui->textEdit_console->document()->findBlock(firstPosition);
            while (block.isValid() && block.position() <= lastPosition)
            {
                block.setVisible(false);
                block = block.next();
            }
            ui->textEdit_console->document()->markContentsDirty(firstPosition,
                                                                lastPosition - firstPosition + 1);
        }
    }
    ui->textEdit_console->moveCursor(QTextCursor::End);
    ui->textEdit_console->ensureCursorVisible();
}

void MainWindow::setTimer_updateImpact(int periodUpdateMsec)
{
    joyStick = std::make_unique<JoyStick>();
    keyBoard = std::make_unique<KeyBoard>();
    if (ui->radioButton_useKeyBoard->isChecked())
    {
        useKeyBoard();
    }
    else if (ui->radioButton_useJoyStick->isChecked())
    {
        useJoyStick();
    }
    else if (ui->gamepad_btn->isChecked())
    {
        useGamepad();
    }

    connect(ui->radioButton_useJoyStick, &QRadioButton::clicked, this, &MainWindow::useJoyStick);
    connect(ui->radioButton_useKeyBoard, &QRadioButton::clicked, this, &MainWindow::useKeyBoard);
    connect(ui->gamepad_btn, &QRadioButton::clicked, this, &MainWindow::useGamepad);

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateUi_fromControl()));
    updateTimer->start(periodUpdateMsec);
    displayText("Таймер обновления джойстика запущен");
}

void MainWindow::useKeyBoard()
{
    activeInput = nullptr;
    gamepadInput.reset();
    if (gamepad)
    {
        delete gamepad;
        gamepad = nullptr;
    }

    if (!keyBoard)
    {
        keyBoard = std::make_unique<KeyBoard>();
    }

    ui->radioButton_useKeyBoard->setChecked(true);
    status_keyboard = true;
    activeInput = keyBoard.get();
    ui->radioButton_useKeyBoard->setToolTip(
        QStringLiteral("O/L — марш; W/S — дифферент; A/D — курс; C/V — глубина; "
                       "Q/E — крен; K/; — лаг"));
    displayText(QStringLiteral("Источник управления: клавиатура"));
}

void MainWindow::useJoyStick()
{
    activeInput = nullptr;
    gamepadInput.reset();
    if (gamepad)
    {
        delete gamepad;
        gamepad = nullptr;
    }

    if (!joyStick)
    {
        joyStick = std::make_unique<JoyStick>();
    }

    if (!joyStick->isAvailable())
    {
        displayText("Джойстик не обнаружен. Переключение на клавиатуру.");
        useKeyBoard();
        return;
    }

    activeInput = joyStick.get();
}

void MainWindow::useGamepad()
{
    Gamepad *newGamepad = new Gamepad(0, this);
    if (!newGamepad->isConnected())
    {
        displayText("Геймпад не обнаружен! Проверьте подключение.");
        delete newGamepad;
        newGamepad = nullptr;
        useKeyBoard();
        return;
    }

    activeInput = nullptr;
    joyStick.reset();
    keyBoard.reset();
    gamepadInput.reset();
    if (gamepad)
    {
        delete gamepad;
        gamepad = nullptr;
    }
    gamepad = newGamepad;

    // connect(gamepad, &Gamepad::backButtonPressed,
    //         this, &MainWindow::useKeyBoard);

    displayText("Геймпад подключен. Режим управления с геймпада активирован.");
    gamepadInput = std::make_unique<GamepadInputSource>(gamepad, this);
    activeInput = gamepadInput.get();

    // D-Pad вправо — увеличить яркость (пока зажата)
    connect(gamepad, &Gamepad::dPadRightPressed, this,
            [this]()
            {
                brightnessStep_ = +20;
                brightnessTimer_->start();
                // Сразу применить один шаг
                int current = ui->slider_brightness->value();
                ui->slider_brightness->setValue(qMin(current + 20, 500));
            });
    connect(gamepad, &Gamepad::dPadRightReleased, this,
            [this]()
            {
                brightnessStep_ = 0;
                brightnessTimer_->stop();
            });

    // D-Pad влево — уменьшить яркость (пока зажата)
    connect(gamepad, &Gamepad::dPadLeftPressed, this,
            [this]()
            {
                brightnessStep_ = -20;
                brightnessTimer_->start();
                int current = ui->slider_brightness->value();
                ui->slider_brightness->setValue(qMax(current - 20, 0));
            });
    connect(gamepad, &Gamepad::dPadLeftReleased, this,
            [this]()
            {
                brightnessStep_ = 0;
                brightnessTimer_->stop();
            });

    // L1 (number 4) — поворот влево (раньше было на L2)
    connect(gamepad, &Gamepad::L1Pressed, this, [this]() { onLeftTriggerMoved(100.0f); });
    connect(gamepad, &Gamepad::L1Released, this, [this]() { onLeftTriggerMoved(0.0f); });

    // R1 (number 5) — поворот вправо (раньше было на R2)
    connect(gamepad, &Gamepad::R1Pressed, this, [this]() { onRightTriggerMoved(100.0f); });
    connect(gamepad, &Gamepad::R1Released, this, [this]() { onRightTriggerMoved(0.0f); });

    // Y — «Разжать»
    connect(gamepad, &Gamepad::buttonYPressed, this, [this]() { setGripState("open"); });
    connect(gamepad, &Gamepad::buttonYReleased, this, [this]() { setGripState("stop"); });

    // A — «Сжать»
    connect(gamepad, &Gamepad::buttonAPressed, this, [this]() { setGripState("close"); });
    connect(gamepad, &Gamepad::buttonAReleased, this, [this]() { setGripState("stop"); });

    // Кнопка 11 — toggle: замкнуть/разомкнуть 3 контура
    connect(gamepad, &Gamepad::button11Pressed, this,
            [this]()
            {
                bool currentlyClosed = ui->pushButton_modeAutomated_yaw->isChecked() &&
                                       ui->pushButton_modeAutomated_pitch->isChecked() &&
                                       ui->pushButton_modeAutomated_roll->isChecked();

                if (currentlyClosed)
                {
                    // Размыкаем
                    ui->pushButton_modeAutomated_yaw->setChecked(false);
                    ui->pushButton_modeAutomated_pitch->setChecked(false);
                    ui->pushButton_modeAutomated_roll->setChecked(false);
                    displayText("Геймпад: контуры Курс/Крен/Дифферент разомкнуты");
                }
                else
                {
                    // Замыкаем (включаем автоматизированный режим если нужно)
                    ui->pushButton_modeAutomated->setChecked(true);
                    ui->pushButton_modeAutomated_yaw->setChecked(true);
                    ui->pushButton_modeAutomated_pitch->setChecked(true);
                    ui->pushButton_modeAutomated_roll->setChecked(true);
                    displayText("Геймпад: контуры Курс/Крен/Дифферент замкнуты");
                }
            });

    // Кнопка 12 — круговая смена режима скоростей
    connect(gamepad, &Gamepad::button12Pressed, this,
            [this]()
            {
                int current = static_cast<int>(currentMode);
                int next = (current + 1) % 3;
                setSpeedMode(static_cast<SpeedMode>(next));
                displayText(QString("Геймпад: режим скорости → %1")
                                .arg(next == 0   ? "Слабый"
                                     : next == 1 ? "Средний"
                                                 : "Быстрый"));
            });
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (activeInput == keyBoard.get() && keyBoard)
    {
        keyBoard->keyPressEvent(event);
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (activeInput == keyBoard.get() && keyBoard)
    {
        keyBoard->keyReleaseEvent(event);
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::updateUi_fromControl()
{
    if (!commandLinkAvailable_)
    {
        const umas::input::ControlCommand stopped;
        updateControlLabels(stopped);
        emit publishTwistRequested(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
        return;
    }

    if (activeInput != nullptr)
    {
        const auto command = activeInput->poll();
        if (command.has_value())
        {
            controlService.apply(command.value());
        }
    }

    const auto scaled = applyGains(controlService.snapshot());
    updateControlLabels(scaled);

    emit publishTwistRequested(scaled.march, scaled.lag, scaled.depth, scaled.roll, scaled.pitch,
                               scaled.yaw);
}

void MainWindow::setCommandLinkAvailable(bool available)
{
    commandLinkAvailable_ = available;
    const QString tooltip =
        available ? QString() : QStringLiteral("Управление заблокировано: нет свежей телеметрии");
    for (const QString &name :
         {QStringLiteral("movementControlPanel"), QStringLiteral("frame_quickActions")})
    {
        if (QWidget *panel = findChild<QWidget *>(name))
        {
            panel->setEnabled(available);
            panel->setToolTip(tooltip);
            setDynamicProperty(panel, "commandUnavailable", !available);
        }
    }

    if (!available)
    {
        controlService.reset();
        if (rosBridge)
        {
            rosBridge->setTurnState(QStringLiteral("stop"));
            rosBridge->setGripState(QStringLiteral("stop"));
        }
    }
}

umas::input::ControlCommand MainWindow::applyGains(const umas::input::ControlCommand &raw) const
{
    umas::input::ControlCommand scaled = raw;
    scaled.yaw *= ui->spinBox_gain_yaw->value();
    scaled.march *= ui->spinBox_gain_surge->value();
    scaled.pitch *= ui->spinBox_gain_pitch->value();
    scaled.lag *= ui->spinBox_gain_sway->value();
    scaled.depth *= ui->spinBox_gain_depth->value();
    scaled.roll *= ui->spinBox_gain_roll->value();
    return scaled;
}

void MainWindow::updateControlLabels(const umas::input::ControlCommand &scaled)
{
    ui->label_controlYaw->setNum(scaled.yaw);
    ui->label_controlMarch->setNum(scaled.march);
    ui->label_controlDif->setNum(scaled.pitch);
    ui->label_controlLag->setNum(scaled.lag);
    ui->label_controlDepth->setNum(scaled.depth);
    ui->label_controlKren->setNum(scaled.roll);
}

void MainWindow::setBottom()
{
    setBottom_mode();

    // Кнопка временно отключается на время запроса. Если она принимает фокус,
    // Qt передаёт его следующему виджету — полю коэффициента марша.
    ui->pushButton_resetDvlOrigin->setFocusPolicy(Qt::NoFocus);

    if (ui->pushButton_speedFast)
    {
        connect(ui->pushButton_speedFast, &QPushButton::clicked, this,
                [this]() { setSpeedMode(SpeedMode::Fast); });
    }

    if (ui->pushButton_speedMedium)
    {
        connect(ui->pushButton_speedMedium, &QPushButton::clicked, this,
                [this]() { setSpeedMode(SpeedMode::Medium); });
    }

    if (ui->pushButton_speedSlow)
    {
        connect(ui->pushButton_speedSlow, &QPushButton::clicked, this,
                [this]() { setSpeedMode(SpeedMode::Slow); });
    }

    QButtonGroup *speedGroup = new QButtonGroup(this);
    speedGroup->addButton(ui->pushButton_speedFast);
    speedGroup->addButton(ui->pushButton_speedMedium);
    speedGroup->addButton(ui->pushButton_speedSlow);
    speedGroup->setExclusive(true);

    connect(ui->pushButton_zeroYaw, &QPushButton::clicked, rosBridge, &RosBridge::zeroYawInternal,
            Qt::QueuedConnection);
    connect(ui->pushButton_resetDvlOrigin, &QPushButton::clicked, this,
            [this]()
            {
                ui->pushButton_resetDvlOrigin->setEnabled(false);
                displayText("Запрошен сброс положения DVL...");
                QMetaObject::invokeMethod(rosBridge, "resetDvlOriginInternal",
                                          Qt::QueuedConnection);
            });
}

void MainWindow::setBottom_mode()
{
    ui->pushButton_modeManual->setCheckable(true);
    ui->pushButton_modeAutomated->setCheckable(true);
    ui->pushButton_modeAutomatic->setCheckable(false);
    QButtonGroup *mode = new QButtonGroup(this);
    mode->addButton(ui->pushButton_modeManual);
    mode->addButton(ui->pushButton_modeAutomated);
    mode->addButton(ui->pushButton_modeAutomatic);
    mode->setExclusive(true);

    ui->pushButton_modeManual->setChecked(true);

    ui->pushButton_modeAutomated_surge->setCheckable(true);
    ui->pushButton_modeAutomated_sway->setCheckable(true);
    ui->pushButton_modeAutomated_depth->setCheckable(true);
    ui->pushButton_modeAutomated_yaw->setCheckable(true);
    ui->pushButton_modeAutomated_pitch->setCheckable(true);
    ui->pushButton_modeAutomated_roll->setCheckable(true);
    QButtonGroup *modeAutomated = new QButtonGroup(this);
    modeAutomated->addButton(ui->pushButton_modeAutomated_surge);
    modeAutomated->addButton(ui->pushButton_modeAutomated_sway);
    modeAutomated->addButton(ui->pushButton_modeAutomated_depth);
    modeAutomated->addButton(ui->pushButton_modeAutomated_yaw);
    modeAutomated->addButton(ui->pushButton_modeAutomated_pitch);
    modeAutomated->addButton(ui->pushButton_modeAutomated_roll);
    modeAutomated->setExclusive(false);

    connect(ui->pushButton_modeAutomated_surge, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(0, checked); });
    connect(ui->pushButton_modeAutomated_sway, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(1, checked); });
    connect(ui->pushButton_modeAutomated_depth, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(2, checked); });
    connect(ui->pushButton_modeAutomated_yaw, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(3, checked); });
    connect(ui->pushButton_modeAutomated_pitch, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(4, checked); });
    connect(ui->pushButton_modeAutomated_roll, &QPushButton::toggled, this,
            [this](bool checked) { emit controlFlagRequested(5, checked); });

    connect(this, &MainWindow::controlFlagRequested, rosBridge, &RosBridge::setControlFlagInternal,
            Qt::QueuedConnection);
}

void MainWindow::setTab()
{
    ui->tabWidget->setTabText(0, "Камера");
    ui->tabWidget->setTabText(1, "БСО");
    ui->tabWidget->setTabText(2, "Контроль сообщений");
    ui->tabWidget->setTabText(3, "Режимы питания");
    ui->tabWidget->setTabText(4, "Исполнительные устройства");
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::setupActuators()
{
    // === Кнопки режимов света ===
    QButtonGroup *lightModeGroup = new QButtonGroup(this);
    lightModeGroup->addButton(ui->btn_light_off, 0);
    lightModeGroup->addButton(ui->btn_light_mode1, 1);
    lightModeGroup->addButton(ui->btn_light_mode2, 2);
    lightModeGroup->addButton(ui->btn_light_mode3, 3);
    lightModeGroup->setExclusive(true);

    connect(lightModeGroup, &QButtonGroup::idClicked, this,
            [this](int id) { onLightModeChanged(static_cast<unsigned>(id)); });

    // === Слайдер яркости ===
    connect(ui->slider_brightness, &QSlider::valueChanged, this, &MainWindow::onBrightnessChanged);

    ui->slider_brightness->setEnabled(false);

    // === Кнопки манипулятора ===
    connect(ui->btn_manip_compress, &QPushButton::clicked, this,
            [this]() { setGripState("close"); });
    connect(ui->btn_manip_release, &QPushButton::clicked, this, [this]() { setGripState("open"); });
    connect(ui->btn_manip_rotate_cw, &QPushButton::clicked, this,
            [this]() { setTurnState("right"); });
    connect(ui->btn_manip_rotate_ccw, &QPushButton::clicked, this,
            [this]() { setTurnState("left"); });
    connect(ui->btn_manip_stop, &QPushButton::clicked, this,
            [this]()
            {
                setTurnState("stop");
                setGripState("stop");
            });
}

void MainWindow::onLightModeChanged(unsigned mode)
{
    rosBridge->publishLightsMode(mode);
    // Слайдер активен только когда включён хотя бы один режим (mode > 0)
    ui->slider_brightness->setEnabled(mode > 0);

    QString modeText;
    switch (mode)
    {
    case 0:
        modeText = "Свет выключен";
        break;
    case 1:
        modeText = "Свет: Режим 1";
        break;
    case 2:
        modeText = "Свет: Режим 2";
        break;
    case 3:
        modeText = "Свет: Режим 3";
        break;
    default:
        modeText = "Свет: Неизвестный режим";
        break;
    }
    displayText(modeText);
}

void MainWindow::onBrightnessChanged(int value)
{
    ui->lbl_brightness_value->setText(QString::number(value));
    rosBridge->publishLightsBrightness(static_cast<unsigned>(value));
}

void MainWindow::setUpdateUI()
{
    connect(this, SIGNAL(updateCompass(float)), this, SLOT(updateUi_Compass(float)));
}

void MainWindow::loadSettings()
{
    QSettings settings("/UMAS_GUI/umas_settings.ini", QSettings::IniFormat);

    for (int mode = 0; mode < 3; ++mode)
    {
        settings.beginGroup(QString("SpeedMode_%1").arg(mode));

        QMap<QString, double> gains;
        for (int i = 0; i < gainNames.size(); ++i)
        {
            double defaultValue = 0;
            if (mode == static_cast<int>(SpeedMode::Slow))
                defaultValue = 10;
            else if (mode == static_cast<int>(SpeedMode::Medium))
                defaultValue = 50;
            else if (mode == static_cast<int>(SpeedMode::Fast))
                defaultValue = 100;

            gains[gainNames[i]] = settings.value(gainNames[i], defaultValue).toDouble();
        }

        speedModeGains[mode] = gains;
        settings.endGroup();
    }

    int savedMode = settings.value("CurrentSpeedMode", static_cast<int>(SpeedMode::Medium)).toInt();
    currentMode = static_cast<SpeedMode>(savedMode);
}

void MainWindow::saveSettings()
{
    QSettings settings("/UMAS_GUI/umas_settings.ini", QSettings::IniFormat);

    for (int mode = 0; mode < 3; ++mode)
    {
        settings.beginGroup(QString("SpeedMode_%1").arg(mode));

        for (int i = 0; i < gainNames.size(); ++i)
        {
            settings.setValue(gainNames[i], speedModeGains[mode][gainNames[i]]);
        }

        settings.endGroup();
    }

    settings.setValue("CurrentSpeedMode", static_cast<int>(currentMode));
}

void MainWindow::saveCurrentModeGains()
{
    for (int i = 0; i < gainSpinBoxes.size(); ++i)
    {
        speedModeGains[static_cast<int>(currentMode)][gainNames[i]] = gainSpinBoxes[i]->value();
    }
    saveSettings();
}

void MainWindow::setSpinBoxValuesForCurrentMode()
{
    for (auto spinBox : gainSpinBoxes)
    {
        spinBox->blockSignals(true);
    }

    for (int i = 0; i < gainSpinBoxes.size(); ++i)
    {
        gainSpinBoxes[i]->setValue(speedModeGains[static_cast<int>(currentMode)][gainNames[i]]);
    }

    for (auto spinBox : gainSpinBoxes)
    {
        spinBox->blockSignals(false);
    }
}

void MainWindow::setSpeedMode(SpeedMode mode)
{
    saveCurrentModeGains();
    currentMode = mode;
    setSpinBoxValuesForCurrentMode();

    // Тема сама покрасит :checked в зелёный #00ff88
    ui->pushButton_speedFast->setChecked(mode == SpeedMode::Fast);
    ui->pushButton_speedMedium->setChecked(mode == SpeedMode::Medium);
    ui->pushButton_speedSlow->setChecked(mode == SpeedMode::Slow);

    saveSettings();

    saveSettings();
}

void MainWindow::updateUi_Compass(float yaw) { ui->compass->setYaw(yaw); }

void MainWindow::resetTelemetryToDefault()
{
    isConnected = false;

    // Красный фон для индикации отсутствия связи
    QString noConnectionStyle =
        "background-color: #a3263d; color: #ffe8ec; font-weight: 600; border-radius: 7px; border: "
        "1px solid #f05268; font-size: 15px; padding: 4px 10px;";

    ui->lbl_depth_value->setStyleSheet(noConnectionStyle);
    ui->lbl_depth_value->setText("N/A");

    ui->lbl_bottom_value->setStyleSheet(noConnectionStyle);
    ui->lbl_bottom_value->setText("N/A");

    ui->lbl_voltage_value->setStyleSheet(noConnectionStyle);
    ui->lbl_voltage_value->setText("N/A");

    ui->lbl_voltage2_value->setStyleSheet(noConnectionStyle);
    ui->lbl_voltage2_value->setText("N/A");

    ui->lbl_speed_value->setStyleSheet(noConnectionStyle);
    ui->lbl_speed_value->setText("N/A");

    ui->btn_killswitch_status->setStyleSheet(noConnectionStyle);
    ui->btn_killswitch_status->setText("N/A");

    ui->lbl_ping_value->setStyleSheet(noConnectionStyle);
    ui->lbl_ping_value->setText("N/A");
}

void MainWindow::updateTelemetryFromState()
{
    isConnected = true;

    // Зелёный фон для нормальных данных
    QString connectedStyle =
        "background-color: #12665f; color: #dffffb; font-weight: 600; border-radius: 7px; border: "
        "1px solid #20c7b8; font-size: 15px; padding: 4px 10px;";

    ui->lbl_depth_value->setStyleSheet(connectedStyle);
    ui->lbl_bottom_value->setStyleSheet(connectedStyle);
    ui->lbl_voltage_value->setStyleSheet(connectedStyle);
    ui->lbl_voltage2_value->setStyleSheet(connectedStyle);
    ui->lbl_speed_value->setStyleSheet(connectedStyle);
    ui->btn_killswitch_status->setStyleSheet(connectedStyle);
    ui->lbl_ping_value->setStyleSheet(connectedStyle);

    // Здесь обновляй реальные значения из uvState
    // ui->lbl_depth_value->setText(QString::number(uvState->getDepth(), 'f', 2) + " м");
    // и т.д.
}

void MainWindow::toggleTheme()
{
    isDark_ = !isDark_;
    applyTheme(isDark_);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (!ui || !ui->centralwidget)
        return;

    const bool compact = event->size().width() < 1500 || event->size().height() < 850;
    setDynamicProperty(ui->centralwidget, "compactMode", compact);

    if (auto *navigation = findChild<QWidget *>(QStringLiteral("dashboardNavigation")))
    {
        navigation->setMinimumWidth(compact ? 126 : 150);
        navigation->setMaximumWidth(compact ? 132 : 180);
    }
    if (auto *instruments = findChild<QWidget *>(QStringLiteral("dashboardInstrumentColumn")))
        instruments->setMinimumWidth(compact ? 350 : 430);

    const QList<QPair<QString, QStringList>> axisLabels = {
        {QStringLiteral("pushButton_modeAutomated_surge"),
         {QStringLiteral("Марш"), QStringLiteral("Марш")}},
        {QStringLiteral("pushButton_modeAutomated_sway"),
         {QStringLiteral("Лаг"), QStringLiteral("Лаг")}},
        {QStringLiteral("pushButton_modeAutomated_depth"),
         {QStringLiteral("Глубина"), QStringLiteral("Глуб.")}},
        {QStringLiteral("pushButton_modeAutomated_yaw"),
         {QStringLiteral("Курс"), QStringLiteral("Курс")}},
        {QStringLiteral("pushButton_modeAutomated_pitch"),
         {QStringLiteral("Дифферент"), QStringLiteral("Дифф.")}},
        {QStringLiteral("pushButton_modeAutomated_roll"),
         {QStringLiteral("Крен"), QStringLiteral("Крен")}}};
    for (const auto &axis : axisLabels)
    {
        if (auto *button = findChild<QPushButton *>(axis.first))
            button->setText(axis.second.at(compact ? 1 : 0));
    }

    if (auto *filter = findChild<QComboBox *>(QStringLiteral("eventLogFilter")))
    {
        filter->setItemText(0, compact ? QStringLiteral("Все") : QStringLiteral("Все сообщения"));
        filter->setMinimumWidth(compact ? 88 : 112);
        filter->setMaximumWidth(compact ? 100 : QWIDGETSIZE_MAX);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QWidget *focusedWidget = QApplication::focusWidget();
        QWidget *clickedWidget = qobject_cast<QWidget *>(watched);

        for (QSpinBox *spinBox : gainSpinBoxes)
        {
            const bool ownsFocus =
                focusedWidget && (focusedWidget == spinBox || spinBox->isAncestorOf(focusedWidget));
            const bool clickedInside =
                clickedWidget && (clickedWidget == spinBox || spinBox->isAncestorOf(clickedWidget));

            if (ownsFocus && !clickedInside)
            {
                spinBox->clearFocus();
                break;
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::applyTheme(bool dark)
{
    setStyleSheet(dark ? DARK_THEME : LIGHT_THEME);

    ui->stackedWidget_mode->setStyleSheet(
        dark ? "background-color: #09262e; border: 1px solid #174a56; border-radius: 9px;"
             : "background-color: #edf5f4; border: 1px solid #b5d2d0; border-radius: 9px;");

    const QString titleStyle = dark ? "color: #66e2d7; font-weight: 600; font-size: 15px;"
                                    : "color: #087f77; font-weight: 600; font-size: 15px;";
    for (QLabel *l : {ui->lbl_depth_title, ui->lbl_bottom_title, ui->lbl_speed_title,
                      ui->lbl_temp_title, ui->lbl_leak_title, ui->lbl_voltage_title,
                      ui->lbl_voltage2_title, ui->lbl_kill_title, ui->lbl_ping_title})
        l->setStyleSheet(titleStyle);

    ui->lbl_heading->setStyleSheet(dark ? "font-size: 14px; font-weight: 600; color: #74eee3;"
                                        : "font-size: 14px; font-weight: 600; color: #087f77;");

    setupButtonStyles(dark);

    // Локальный стиль исключает зависимость переключателей источника
    // управления от момента полировки глобальной таблицы стилей Qt.
    const QString inputSelectorStyle =
        dark ? "QRadioButton { color: #c9e7e5; spacing: 8px; font-weight: 500; }"
               "QRadioButton::indicator { width: 16px; height: 16px; border-radius: 9px; border: "
               "1px solid #397886; background-color: #071f27; }"
               "QRadioButton::indicator:hover { border-color: #69ddd3; background-color: #0b3039; }"
               "QRadioButton::indicator:checked { background-color: #69ddd3; border: 4px solid "
               "#0b3039; }"
             : "QRadioButton { color: #23474c; spacing: 8px; font-weight: 500; }"
               "QRadioButton::indicator { width: 16px; height: 16px; border-radius: 9px; border: "
               "1px solid #789fa0; background-color: #f2f8f7; }"
               "QRadioButton::indicator:hover { border-color: #087e79; background-color: #e0f2f0; }"
               "QRadioButton::indicator:checked { background-color: #087e79; border: 4px solid "
               "#dcecea; }";
    for (QRadioButton *selector :
         {ui->radioButton_useJoyStick, ui->gamepad_btn, ui->radioButton_useKeyBoard})
    {
        selector->setStyleSheet(inputSelectorStyle);
        selector->update();
    }

    ui->btn_theme_toggle->setStyleSheet(
        dark ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, "
               "stop:1 #0a303a); border: 1px solid #236574; border-radius: 9px; }"
               "QPushButton:hover { background-color: #176173; border-color: #74eee3; }"
               "QPushButton:pressed { background-color: #0b2a32; border-color: #20c7b8; }"
             : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, "
               "stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 9px; }"
               "QPushButton:hover { background-color: #d7ece9; border-color: #159d93; }"
               "QPushButton:pressed { background-color: #c5dfdc; border-color: #0f8f87; }");

    ui->full_screen->setText(QStringLiteral("Полный экран"));
    ui->full_screen->setStyleSheet(
        dark ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, "
               "stop:1 #0a303a); color: #d7f3f1; border: 1px solid #236574; border-radius: 9px; "
               "padding: 7px 14px; font-weight: 600; }"
               "QPushButton:hover { border-color: #74eee3; color: #ffffff; }"
               "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 "
               "#176173, stop:1 #0d414d); border-color: #74eee3; color: #8af5ec; }"
             : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f4faf9, "
               "stop:1 #dcebe9); color: #16383d; border: 1px solid #91b9b7; border-radius: 9px; "
               "padding: 7px 14px; font-weight: 600; }"
               "QPushButton:hover { border-color: #159d93; color: #075e58; }"
               "QPushButton:checked { background-color: #ccebe7; border-color: #0f8f87; color: "
               "#064d49; }");

    ui->compass->setLightTheme(!dark);
    ui->attitudeIndicator->setLightTheme(!dark);

    if (QLabel *brandTitle = findChild<QLabel *>(QStringLiteral("brandTitle")))
    {
        brandTitle->setText(
            dark ? QStringLiteral(
                       "<span style='font-size:16px; font-weight:700; letter-spacing:1px; "
                       "color:#74eee3;'>HYDRONAUTICS</span><br><span style='font-size:10px; "
                       "color:#7faaa9;'>UMAS CONTROL STATION</span>")
                 : QStringLiteral(
                       "<span style='font-size:16px; font-weight:700; letter-spacing:1px; "
                       "color:#087f77;'>HYDRONAUTICS</span><br><span style='font-size:10px; "
                       "color:#557b7d;'>UMAS CONTROL STATION</span>"));
    }

    ui->btn_theme_toggle->setIcon(dark ? makeMoonIcon() : makeSunIcon());
    ensureInteractiveTextFits();
}

void MainWindow::setupButtonStyles(bool dark)
{
    const QString speedStyle =
        dark ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, "
               "stop:1 #0a303a); border: 1px solid #236574; border-radius: 8px;"
               " color: #d7f3f1; min-height: 28px; font-size: 13px; }"
               "QPushButton:hover { background-color: #176173; border: 1px solid #74eee3; color: "
               "#ffffff; }"
               "QPushButton:pressed { background-color: #0b2a32; color: #8af5ec; }"
               "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 "
               "#74eee3, stop:1 #20b9ad); color: #031416;"
               " border: 1px solid #a7fff7; font-weight: 700; }"
             : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, "
               "stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 8px;"
               " color: #16383d; min-height: 28px; font-size: 13px; }"
               "QPushButton:hover { background-color: #d7ece9; border: 1px solid #159d93; color: "
               "#075e58; }"
               "QPushButton:pressed { background-color: #c5dfdc; color: #064d49; }"
               "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 "
               "#62ddd2, stop:1 #19aaa0); color: #052f2c;"
               " border: 1px solid #07847c; font-weight: 700; }";
    for (auto *b : {ui->pushButton_speedFast, ui->pushButton_speedMedium, ui->pushButton_speedSlow})
        b->setStyleSheet(speedStyle);

    const QString modeStyle =
        dark ? "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #124653, "
               "stop:1 #0a303a); border: 1px solid #236574; border-radius: 8px;"
               " color: #d7f3f1; min-height: 24px; }"
               "QPushButton:hover { background-color: #176173; border: 1px solid #74eee3; color: "
               "#ffffff; }"
               "QPushButton:pressed { background-color: #0b2a32; color: #8af5ec; }"
               "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 "
               "#74eee3, stop:1 #20b9ad); color: #031416; border: 1px solid #a7fff7; font-weight: "
               "700; }"
             : "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #f3f9f8, "
               "stop:1 #dceae8); border: 1px solid #91b9b7; border-radius: 8px;"
               " color: #16383d; min-height: 24px; }"
               "QPushButton:hover { background-color: #d7ece9; border: 1px solid #159d93; color: "
               "#075e58; }"
               "QPushButton:pressed { background-color: #c5dfdc; color: #064d49; }"
               "QPushButton:checked { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 "
               "#62ddd2, stop:1 #19aaa0); color: #052f2c; border: 1px solid #07847c; font-weight: "
               "700; }";
    for (auto *b :
         {ui->pushButton_modeManual, ui->pushButton_modeAutomated, ui->pushButton_modeAutomatic,
          ui->pushButton_modeAutomated_surge, ui->pushButton_modeAutomated_sway,
          ui->pushButton_modeAutomated_depth, ui->pushButton_modeAutomated_yaw,
          ui->pushButton_modeAutomated_pitch, ui->pushButton_modeAutomated_roll})
        b->setStyleSheet(modeStyle);
    for (auto *b :
         {ui->btn_light_off, ui->btn_light_mode1, ui->btn_light_mode2, ui->btn_light_mode3})
        b->setStyleSheet(speedStyle);

    if (dark)
    {
        ui->btn_manip_stop->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #c93a52, "
            "stop:1 #861d31);"
            " color: #fff1f3; border: 1px solid #f05268; border-radius: 8px; font-weight: 700; }"
            "QPushButton:hover { background-color: #dc4760; border-color: #ff91a1; }"
            "QPushButton:pressed { background-color: #711426; color: #ffdce2; }");
    }
    else
    {
        ui->btn_manip_stop->setStyleSheet(
            "QPushButton { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #fff4f6, "
            "stop:1 #ffdce2);"
            " color: #8f1830; border: 1px solid #ef7185; border-radius: 8px; font-weight: 700; }"
            "QPushButton:hover { background-color: #ffcbd4; border-color: #d9435c; }"
            "QPushButton:pressed { background-color: #f4b9c3; color: #711426; }");
    }
}

// ============================================================
// Поворот (курс) — L1/R1
// ============================================================

void MainWindow::onLeftTriggerMoved(float v)
{
    const bool pressed = (v > TRIGGER_THRESHOLD);
    if (pressed == m_l1Pressed)
        return;
    m_l1Pressed = pressed;

    setTurnState(pressed ? "left" : "stop");
}

void MainWindow::onRightTriggerMoved(float v)
{
    const bool pressed = (v > TRIGGER_THRESHOLD);
    if (pressed == m_r1Pressed)
        return;
    m_r1Pressed = pressed;

    setTurnState(pressed ? "right" : "stop");
}

void MainWindow::setTurnState(const QString &state)
{
    rosBridge->setTurnState(state);
    displayText(QString("Поворот: %1").arg(state));
}

// ============================================================
// Манипулятор (клешня) — кнопки в UI
// ============================================================

void MainWindow::setGripState(const QString &state)
{
    rosBridge->setGripState(state);
    displayText(QString("Манипулятор: %1").arg(state));
}

MainWindow::~MainWindow() { delete ui; }
