#pragma once

#include <QObject>
#include <QVector>

// TODO: window name

class WindowElement : public QObject
{
    Q_OBJECT

public:
    explicit WindowElement(const QString &id, QObject *parent = nullptr);
    ~WindowElement() = default;

    Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)
    inline bool isActive() { return m_isActive; }

    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    inline QString icon() { return m_icon; }

    inline QString id() { return m_id; }

    Q_INVOKABLE void requestActiveChanged();

public slots:
    void deleteSelf();
    void setIcon(const QString &icon);

signals:
    void activeChanged();
    void requestGroupIconChanged(const QString &);
    void iconChanged();
    void selfDestory(WindowElement *);

private:
    QString m_id;
    bool m_isActive = true;
    // INFO: base 64
    QString m_icon;
};

class WindowElementGroup : public QObject
{
    Q_OBJECT

public:
    explicit WindowElementGroup(bool isPin, const QString &id, QObject *parent = nullptr);

    Q_PROPERTY(QVector<WindowElement *> elements READ elements NOTIFY elementsChanged)
    inline QVector<WindowElement *> elements() { return m_elements; }

    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    inline QString icon() { return m_icon; }

    Q_PROPERTY(bool isPin READ isPin NOTIFY isPinChanged)
    inline bool isPin() { return m_isPin; }

    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    inline QString id() { return m_id; }

    Q_INVOKABLE void requestActiveChanged();

public slots:
    void appendWindowElement(WindowElement *element);
    void removeWindowElement(WindowElement *element);
    void setIcon(const QString &icon);

signals:
    void elementsChanged();
    void activeChanged();
    void iconChanged();
    void idChanged();
    void isPinChanged();
    void deleteSelf(WindowElementGroup *);

private:
    bool m_isPin;
    QString m_id;
    QVector<WindowElement *> m_elements;
    // INFO: base 64, the actived icon
    QString m_icon;
};
