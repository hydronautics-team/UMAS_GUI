#ifndef HUD_NOTIFICATION_STACK_H
#define HUD_NOTIFICATION_STACK_H

#include <QHash>
#include <QString>
#include <QWidget>

class QLabel;
class QVBoxLayout;

class HudNotificationStack : public QWidget
{
public:
    enum class Severity
    {
        Info,
        Warning,
        Error
    };

    explicit HudNotificationStack(QWidget *parent = nullptr);

    void setCondition(const QString &id, bool active, const QString &text, Severity severity);
    void showInfo(const QString &id, const QString &text, int durationMs = 4000);

private:
    QLabel *createNotification(const QString &id, const QString &text, Severity severity);
    void removeNotification(const QString &id);
    static QString styleFor(Severity severity);

    QVBoxLayout *layout_;
    QHash<QString, QLabel *> notifications_;
};

#endif // HUD_NOTIFICATION_STACK_H
