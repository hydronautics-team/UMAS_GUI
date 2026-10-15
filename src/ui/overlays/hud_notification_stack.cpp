#include "hud_notification_stack.h"

#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>

HudNotificationStack::HudNotificationStack(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);

    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(8);
    layout_->setAlignment(Qt::AlignTop);
}

void HudNotificationStack::setCondition(const QString &id, bool active, const QString &text,
                                        Severity severity)
{
    if (!active)
    {
        removeNotification(id);
        return;
    }

    QLabel *notification = notifications_.value(id, nullptr);
    if (!notification)
        notification = createNotification(id, text, severity);
    else
    {
        notification->setText(text);
        notification->setStyleSheet(styleFor(severity));
    }
}

void HudNotificationStack::showInfo(const QString &id, const QString &text, int durationMs)
{
    if (notifications_.contains(id))
        return;

    createNotification(id, text, Severity::Info);
    QTimer::singleShot(durationMs, this, [this, id]() { removeNotification(id); });
}

QLabel *HudNotificationStack::createNotification(const QString &id, const QString &text,
                                                 Severity severity)
{
    auto *notification = new QLabel(text, this);
    notification->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    notification->setWordWrap(true);
    notification->setMinimumHeight(48);
    notification->setStyleSheet(styleFor(severity));
    layout_->addWidget(notification);
    notifications_.insert(id, notification);
    return notification;
}

void HudNotificationStack::removeNotification(const QString &id)
{
    QLabel *notification = notifications_.take(id);
    if (!notification)
        return;

    layout_->removeWidget(notification);
    notification->deleteLater();
}

QString HudNotificationStack::styleFor(Severity severity)
{
    switch (severity)
    {
    case Severity::Error:
        return QStringLiteral(
            "color: #fff0f3; font-size: 16px; font-weight: 700;"
            "background-color: rgba(122, 18, 38, 220);"
            "border-left: 5px solid #f05268; border-radius: 7px; padding: 9px 12px;");
    case Severity::Warning:
        return QStringLiteral(
            "color: #fff4df; font-size: 16px; font-weight: 650;"
            "background-color: rgba(105, 62, 15, 215);"
            "border-left: 5px solid #efb766; border-radius: 7px; padding: 9px 12px;");
    case Severity::Info:
        return QStringLiteral(
            "color: #dffffb; font-size: 16px; font-weight: 600;"
            "background-color: rgba(8, 57, 60, 205);"
            "border-left: 5px solid #20c7b8; border-radius: 7px; padding: 9px 12px;");
    }

    return {};
}
