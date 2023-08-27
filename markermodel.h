#ifndef MARKERMODEL_H
#define MARKERMODEL_H

#include <QList>
#include <QPointF>
#include <QMetaType>
#include <QDateTime>
#include <QGeoRoute>
#include <QGeoCoordinate>
#include <QAbstractListModel>

struct MarkerItem{

    MarkerItem(const QPointF& pos, const QDateTime& when, const QString& label);

    const QPointF& position() const;
    QGeoCoordinate coordinate() const;
    const QString& label() const;
    const QDateTime& when() const;

private:
    QPointF      _position;
    QString      _label;
    QDateTime    _when;
};

Q_DECLARE_METATYPE(MarkerItem)

class MarkerModel : public QAbstractListModel{
    Q_OBJECT
public:
    enum MarkerRoles {
        PositionRole = Qt::UserRole + 1,
        LabelRole
    };
    explicit MarkerModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public:
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void removeMarker(int index);

private:
    QList<MarkerItem*> _markers;
public:
    void addMarker(MarkerItem* marker);

public slots:
    void removeOldItems();
};

#endif // MARKERMODEL_H
