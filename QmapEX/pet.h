#ifndef PET_H
#define PET_H

#include <QObject>

class Pet : public QObject
{
    Q_OBJECT

    int age;
    QString name;
public:
    explicit Pet(QObject *parent = nullptr);

    int getAge() const;
    void setAge(int newAge);

    QString getName() const;
    void setName(const QString &newName);

signals:

};

#endif // PET_H
