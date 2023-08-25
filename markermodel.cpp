//#include "markermodel.h"

//MarkerModel::MarkerModel(QObject *parent)
//    : QAbstractListModel(parent),
//    m_maxMarkers(0)
//{
//}

//void MarkerModel::moveMarker(const NamedCoordinate &coordinate)
//{
//    NamedCoordinate last = m_current;
//    m_current = coordinate;
//    Q_EMIT currentChanged();
//    if(!last.coord.isValid())
//        return;
//    if(m_maxMarkers == 0){
//        insert(0, last);
//        return;
//    }
//    if(rowCount() >= m_maxMarkers){
//        while (rowCount() >= m_maxMarkers)
//            removeLastMarker();
//        removeLastMarker();
//    }
//    insert(0, last);
//}
//int MarkerModel::maxMarkers() const
//{
//    return m_maxMarkers;
//}

//void MarkerModel::setMaxMarkers(int maxMarkers)
//{
//    m_maxMarkers = maxMarkers > 1 ? maxMarkers: 0;
//}

//NamedCoordinate MarkerModel::current() const
//{
//    return m_current;
//}

//int MarkerModel::rowCount(const QModelIndex &parent) const
//{
//    if (parent.isValid())
//        return 0;
//    return m_markers.count();
//}

//QVariant MarkerModel::data(const QModelIndex &index, int role) const
//{
//    if (!index.isValid())
//        return QVariant();

//    if(role == PositionRole)
//        return QVariant::fromValue(m_markers[index.row()]);
//    if(role == NameRole)
//        return QVariant::fromValue(m_markers[index.row()]);

//    return QVariant();
//}

//QHash<int, QByteArray> MarkerModel::roleNames() const
//{
//    QHash<int, QByteArray> roles;
//    roles[PositionRole] = "position";
//    roles[NameRole] = "name";
//    return roles;
//}

//void MarkerModel::insert(int row, const NamedCoordinate & coordinate)
//{
//    beginInsertRows(QModelIndex(), row, row);
//    m_markers.insert(row, coordinate);
//    endInsertRows();
//}

//void MarkerModel::removeLastMarker()
//{
//    if(m_markers.isEmpty())
//        return;
//    beginRemoveRows(QModelIndex(), rowCount()-1, rowCount()-1);
//    m_markers.removeLast();
//    endRemoveRows();
//}


#include <QtQml>
#include "markermodel.h"
#include <QMetaType>

MarkerItem::MarkerItem(const QPointF &pos, MarkerItem::marker_state state, const QDateTime &when, const QString &label): _position(pos), _state(state), _label(label), _when(when){
    //    qmlRegisterType<QGeoRoute*>();
}

const QPointF &MarkerItem::position() const{
    return _position;
}


QGeoCoordinate MarkerItem::coordinate() const{
    return QGeoCoordinate(_position.x(), _position.y());
}

const QString &MarkerItem::label() const{
    return _label;
}

MarkerItem::marker_state MarkerItem::state() const{
    return _state;
}

void MarkerItem::change_state(MarkerItem::marker_state state){
    _state = state;
}

MarkerModel::MarkerModel(QObject *parent): QAbstractListModel(parent){

}

int MarkerModel::rowCount(const QModelIndex &parent) const{
    if (parent.isValid())
        return 0;

    return _markers.count();
}

QVariant MarkerModel::data(const QModelIndex &index, int role) const{
    if (!index.isValid())
        return QVariant();

    if(role == int(PositionRole)){
        return _markers.at(index.row())->position();
    }else if(role == int(StateRole)){
        switch(_markers.at(index.row())->state()){
        case MarkerItem::marker_important:
            return "important";
        case MarkerItem::marker_observation:
            return "observation";
        case MarkerItem::marker_redundant:
            return "redundant";
        case MarkerItem::marker_deleted:
            return "deleted";
        }
    }else if(role == int(LabelRole)){
        return _markers.at(index.row())->label();
    }
    return QVariant();
}

QHash<int, QByteArray> MarkerModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[PositionRole] = "position";
    roles[StateRole]    = "status";
    roles[LabelRole]    = "label";
    return roles;
}

void MarkerModel::addMarker(MarkerItem *marker){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _markers.push_back(marker);
    qWarning() << rowCount();
    endInsertRows();
}

QGeoRoute *MarkerModel::route() const{
    QGeoRoute* geo_route = new QGeoRoute;
    QList<QGeoCoordinate> coordinates;
    foreach(MarkerItem* marker, _markers){
        coordinates.push_back(marker->coordinate());
    }
    geo_route->setPath(coordinates);
    return geo_route;
}
