#include <QtQml>
#include "markermodel.h"
#include <QMetaType>

const int item_lifetime = 30; //sec

MarkerItem::MarkerItem(const QPointF &pos, const QDateTime &when, const QString &label): _position(pos), _label(label), _when(when){
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


const QDateTime& MarkerItem::when() const{
    return _when;
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
    }else if(role == int(LabelRole)){
        return _markers.at(index.row())->label();
    }
    return QVariant();
}

void MarkerModel::removeMarker(int index) {

    if (index >= 0 && index < _markers.count()) {
        beginRemoveRows(QModelIndex(), index, index);
        delete _markers.takeAt(index);
        endRemoveRows();
    }
}


QHash<int, QByteArray> MarkerModel::roleNames() const{
    QHash<int, QByteArray> roles;
    roles[PositionRole] = "position";
    roles[LabelRole]    = "label";
    return roles;
}

void MarkerModel::addMarker(MarkerItem *marker){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    _markers.push_back(marker);
    qWarning() << rowCount();
    endInsertRows();
}


void MarkerModel::removeOldItems() {
    int removedCount = 0;

    QDateTime currentTime = QDateTime::currentDateTime();
    QDateTime thresholdTime = currentTime.addSecs(-item_lifetime);

    for (int i = 0; i < _markers.size(); ++i) {
        MarkerItem *marker = _markers[i];
        if (marker->when() < thresholdTime) {
            beginRemoveRows(QModelIndex(), i - removedCount, i - removedCount);
            _markers.removeAt(i);
            delete marker;
            endRemoveRows();
            ++removedCount;
        }
    }
}

