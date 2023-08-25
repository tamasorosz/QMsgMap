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
    enum marker_state{
        marker_observation = 0,
        marker_important,
        marker_redundant,
        marker_deleted
    };


    MarkerItem(const QPointF& pos, marker_state state, const QDateTime& when, const QString& label);

    const QPointF& position() const;
    QGeoCoordinate coordinate() const;
    const QString& label() const;
    marker_state state() const;

    void change_state(marker_state state);

private:
    QPointF      _position;
    marker_state _state;
    QString      _label;
    QDateTime    _when;
};

Q_DECLARE_METATYPE(MarkerItem)

class MarkerModel : public QAbstractListModel{
    Q_OBJECT
    Q_PROPERTY(QGeoRoute* route READ route NOTIFY routeChanged)
public:
    enum MarkerRoles {
        PositionRole = Qt::UserRole + 1,
        StateRole,
        LabelRole
    };
    explicit MarkerModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public:
    QHash<int, QByteArray> roleNames() const;
    Q_INVOKABLE void removeMarker(int index);

private:
    QList<MarkerItem*> _markers;
public:
    void addMarker(MarkerItem* marker);
public:
    QGeoRoute* route() const;
signals:
    void routeChanged();
};

#endif // MARKERMODEL_H
